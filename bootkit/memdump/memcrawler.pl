#! /usr/bin/env perl 
use strict;
use Getopt::Long;
use Glib qw(TRUE FALSE);
use Gtk2 '-init';
use Gtk2::SimpleList;
use Gtk2::GladeXML;
use Graph;
use Graph::Directed;
use GraphViz;
use Time::localtime;
use Digest::MD5  qw(md5 md5_hex md5_base64);
use Algorithm::ClusterPoints;

package MemNode;
use Class::Struct;
# Graph Node Attributes
# - headaddr: The starting address of this node.  Should also be the name of the vertex.
# - printname: The label name of this node.
# - size: The number of bytes that this node extends.
# - type: The type that this node is currently classified as.   
# - inptrs: Array of addresses point to this node
# - outptrs: Array of addresses that this node points to
# - original: Hash of original src->dest pointers that this node represents
struct( MemNode => {
		headaddr => '$',
		printname => '$',
		size => '$',
		type => '$',
		inptrs => '@',
		outptrs => '@',
		original => '%'
	});

# END package MemNode

package TypeClass;
use Class::Struct;
# Type-Class Attributes
# - id: Some identifier for this typeclass. Seems necessary?
# - size: Length of the struct type in bytes.
# - words: Hash of offset->wordtype in this struct type
# - nodes: Array of nodes classified as this type.
struct( TypeClass => {
		id => '$',
		size => '$',
		words => '%',
		nodes => '@'
	});

# END package TypeClass

package main;

my %settings = ();
my %graphset = ();
my %nodesset = ();
my @valid_tol;

init_vars();
my $number_of_cpus = get_num_cpus();

# Read options
my $progname;
GetOptions("n=s"=>\$progname);

#test_clustering();
gui();

#---------------- Nodeset Subroutines ---------------

# function newPtrNode - Make new node in nodeset
sub newPtrNode {
	my $addr = shift;
	my $tol = shift;
	my $label = sprintf "x%08x", $addr;
	%{$nodesset{$tol}}->{"$addr"} = new MemNode;
	%{$nodesset{$tol}}->{"$addr"}->headaddr($addr);
	%{$nodesset{$tol}}->{"$addr"}->printname($label);
	return $addr;
}

# function addPtrOut - Add a dest address to a node in nodeset
sub addPtrOut {
	my $node = shift;
	my $dest = shift;
	my $tol = shift;
	push(@{%{$nodesset{$tol}}->{"$node"}->outptrs}, $dest); 
			# what if there are duplicates in outptrs?
}

# function addPtrIn - Add a incoming address src to a node
sub addPtrIn {
	my $node = shift;
	my $src = shift;
	my $tol = shift;
	push(@{%{$nodesset{$tol}}->{"$node"}->inptrs}, $src); 
			# what if there are duplicates in outptrs?
}

# function addPtrRecord - Add the original src->dest record to a node
sub addPtrRecord {
	my $node = shift;
	my $src = shift;
	my $dest = shift;
	my $tol = shift;
	%{$nodesset{$tol}}->{"$node"}->original($src, $dest);
}

# function snapNode - Tries to snap an address to an existing one
# arg0:	$addr - input address
# arg1: $tol - cluster tolerance size (user knob)
# arg2: $dir - search direction (0=backwards, 1=both)
# return: $snapaddr - address snapped to, or 0 if none
sub snapNode {
	my $orig_addr = shift;
	my $tol = shift;
	my $dir = shift;
	my $snaplimit = $orig_addr - $tol;
	my $addr = $orig_addr; 

	while ($addr >= $snaplimit) {
		if (exists %{$nodesset{$tol}}->{"$addr"}) {
			# found node to snap to
			return $addr;	
		}
		$addr = $addr - 4  # assuming everything is 4 byte aligned
	}
	if ($dir) {
		$snaplimit = $orig_addr + $tol;
		$addr = $orig_addr;
		while ($addr <= $snaplimit) {
			if (exists %{$nodesset{$tol}}->{"$addr"}) {
				# found node to snap to
				return $addr;	
			}
			$addr = $addr + 4  # assuming everything is 4 byte aligned
		}
	}

	# no node to snap to
	return 0;
}

# function addGraphPtr - Adds a pointer to the node set.
# arg0: $addr - source address of pointer
# arg1: $dest - destination address of pointer
# arg2: $tol - cluster tolerance size (user knob)
# return: $newaddr - address of source node that was snapped to
sub addGraphPtr {
	my $addr = shift; # pop first arg
	my $dest = shift;
	my $tol = shift;
	my @destset;
	my $attr_name = "outptrs";

	if (exists %{$nodesset{$tol}}->{"$dest"}) {
		addPtrIn($dest, $addr, $tol);
		addPtrRecord($dest, $addr, $dest, $tol);
	}
	else {
		newPtrNode($dest, $tol);
		addPtrIn($dest, $addr, $tol);
		addPtrRecord($dest, $addr, $dest, $tol);
	}

	return $dest;
}

# function groupPtrDest - Goes through existing pointers and groups the outptrs
sub groupPtrDest {
	my $tol = shift;
	my $node;
	my $dest;
	my @newoutptrs;


	for $node ( keys %{$nodesset{$tol}} ) {
		@newoutptrs = ();	
		for $dest ( @{%{$nodesset{$tol}}->{"$node"}->outptrs} ) {
			my $snapaddr = snapNode($dest, $tol, 1);
			if ($snapaddr) {
				#printf "snapped destination:%08x\n", $snapaddr;
				push(@newoutptrs, $snapaddr);
			}
			else {
				#printf "orig destination:%08x\n", $dest;
				push(@newoutptrs, $dest);
				newPtrNode($dest, $tol);
				# addPtrIn($dest, $node); #redundant?
			}
		}

		#	%{$nodesset{$settings{'curr_tol'}}}->{"$node"}->outptrs(@newoutptrs);
		# awful efficiency here
		$#{%{$nodesset{$tol}}->{"$node"}->outptrs} = -1;
		for $dest (@newoutptrs) {
			addPtrOut($node, $dest, $tol);
			addPtrIn($dest, $node, $tol);
		}					
	}
}

# function groupPtrSrc - Goes through existing pointer and groups the inptrs
sub groupPtrSrc {
	my $tol = shift;
	my $node;
	my $src;
	my @newinptrs;


	for $node ( keys %{$nodesset{$tol}} ) {
		@newinptrs = ();	
		for $src ( @{%{$nodesset{$tol}}->{"$node"}->inptrs} ) {
			my $snapaddr = snapNode($src, $tol, 0);
			if ($snapaddr) {
				#printf "snapped destination:%08x\n", $snapaddr;
				push(@newinptrs, $snapaddr);
			}
			else {
				#printf "orig destination:%08x\n", $dest;
				push(@newinptrs, $src);
				newPtrNode($src, $tol);
				# addPtrIn($dest, $node); #redundant?
			}
		}

		#	%{$nodesset{$settings{'curr_tol'}}}->{"$node"}->outptrs(@newoutptrs);
		# awful efficiency here
		$#{%{$nodesset{$tol}}->{"$node"}->inptrs} = -1;
		for $src (@newinptrs) {
			addPtrOut($src, $node, $tol);
			addPtrIn($node, $src, $tol);
		}					
	}
}

sub hexNameToAddr {
	my $nodename = shift;
	return hex(substr $nodename, 1);
}

#---------------- Analysis --------------
sub stat_sum {
	my ($arrayref) = @_;
	my $result;
	foreach(@$arrayref) { $result+= $_; }
	return $result;
}

sub stat_mean {
	my ($arrayref) = @_;
	my $result;
	foreach (@$arrayref) { $result += $_ }
	return $result / @$arrayref;
}


sub stat_variance {
	my ($arrayref)= @_;
	my $mean = stat_mean(@_);
	my @a = map ( ($_ - $mean)**2, @$arrayref);
	return stat_sum(\@a) / scalar @$arrayref;
}

# "degree" button callback
# calculates indegree and outdegree mean and variance for each wcc
sub find_degrees {
	my %nodeset = %{$nodesset{$settings{'curr_tol'}}};

	printf "current tol: %d\n", $settings{'curr_tol'};

	for my $cc (@{$settings{'components'}}) {
		my @indegrees = ();
		my @outdegrees = ();

		for my $v (@$cc)
		{
			push(@indegrees, scalar @{$nodeset{ hexNameToAddr($v) }->inptrs});
			push(@outdegrees, scalar @{$nodeset{ hexNameToAddr($v) }->outptrs});
		}

		print "indegrees: @indegrees\n";
		print "outdegrees: @outdegrees\n";

		my $inmean = stat_mean(\@indegrees);
		my $invar = stat_variance(\@indegrees);
		my $outmean = stat_mean(\@outdegrees);
		my $outvar = stat_variance(\@outdegrees);
		print "cc = $cc\n";
		print "indegree - mean: $inmean, var: $invar\n";
		print "outdegree - mean: $outmean, var: $outvar\n";
	}
}

# function wordType - returns a guessed type of a word
# arg0: word - the numberical value of the 4 byte word
# return: a string with the type name from the set {"pointer", "small_int"}
# return: 0 if unknown or no good guess
sub wordType {
	my $word = shift;

	if ($word < 0xff) {
		return "small_int";
	}
	elsif ( !($word % 4) && $word > 0x01000000 ) {
		return "pointer";
	}

	return 0;
}

sub accessMem {
	my $addr = shift;
	my $size = shift;
	my @memblock = ();
	my $pid = $settings{'pid'};

	my $mline;
	my @MapLines = @{$settings{'MapLines'}};
	for $mline (@MapLines)
	{
		#print "Line $mline\n";
		if($mline =~ /MEM ([0-9a-f]{8})-([0-9a-f]{8}) ([rwxp\-]+) ([0-9a-f]{8}) ([0-9]{2}:[0-9]{2}) ([0-9]+) (.*)/)
		{
			my $startmem = "$1"; my $endmem = "$2"; my $perms = "$3";
			my $offset = "$4"; my $dev = "$5"; my $inode = "$6";
			my $pathname = "$7";
			my $dumpfile = "dump-$pid/$startmem-$endmem.dump";

			unless (-e "$dumpfile") {
				dumpmem($pid, $startmem, $endmem, $dumpfile);
			}

			$startmem = hex $startmem;
			$endmem = hex $endmem;
			if ($addr >= $startmem && $addr+$size <= $endmem) {
				open(DUMP_FH, $dumpfile) or die "Failed to open dumpfile $dumpfile\n";
				binmode(DUMP_FH);
				my $file_offset = $addr-$startmem;
				seek(DUMP_FH, $addr-$startmem, 0) or die "Failed to seek to offset $addr-$startmem\n";
				print "getting $size bytes of memory from $addr\n";
				print "offset into file: $file_offset\n";
				my $fbuf = 0;
				my $n;
				my $totalread = 0;
				#printf "Set startmem to %x for %s\n", $memaddr + (tell FH), $dumpfile;
				while(($n = read(DUMP_FH, $fbuf, 4)) != 0 && $totalread < $size)
				{
					my $printbuf = sprintf "x%08x", unpack('l32', $fbuf);
					print "pushing $printbuf to memblock\n";
					push(@memblock, unpack('l32', $fbuf));
					$totalread += 4;
				}
				close(DUMP_FH);
			}
		}
	}

	return @memblock;
}

#---------------- Subroutines -------------------
sub get_num_cpus
{
	open(FH, "cat /proc/cpuinfo | grep processor | wc -l |") || die "Can't determine number of CPUs";
	my $cpus;
	while(<FH>) { $cpus = $_; }
	chomp $cpus;
	print "Found $cpus CPUs\n";
	close(FH);
	return $cpus;
}

sub pokemem {
	my $addr = $_[0];
	my $pid = $settings{'pid'};
	#printf "Poking addr %x...\n", $addr;
	my $mapline = find_mapline($addr);
	$mapline || die "No such mapline\n";
	if($mapline =~ /MEM ([0-9a-f]{8})-([0-9a-f]{8}) ([rwxp\-]+) ([0-9a-f]{8}) ([0-9]{2}:[0-9]{2}) ([0-9]+) (.*)/)
	{
		my $fbuf = 0;
		#printf "Poking addr %x... found mapline %s\n", $addr, $mapline;
		my $startmem = "$1"; my $endmem = "$2"; my $perms = "$3";
		my $dumpfile = "dump-$pid/$startmem-$endmem.dump";
		my $off = $addr - hex $startmem;
		open(FH, $dumpfile) or die "Failed to open dumpfile\n";
		binmode(FH);	
		seek(FH, $off, 0) or die "Can't seek address";
		my $data = read(FH, $fbuf, 4);
		die "pokemem can't read memory $addr" if($data == 0);
		my $val = unpack('l32', $fbuf);
		#printf "Dump: read address($data) %x at file offset %08x as %08x %d\n", $addr, $off, $val, $val;
		close(FH);
		return $val;	
	}
	die "No such address $addr";
}

sub dumpmem {
	my $pid = $settings{'pid'};
	my $startmem = $_[1];
	my $endmem = $_[2];
	my $dumpfile = $_[3];
	print "Dumping process memory of PID $pid\n";
	my $dumpdir = "dump-$pid";
	unlink("$dumpdir");
	mkdir "$dumpdir" or die "Can't make $dumpdir";
	my $dumpcmd = "attach $pid\n dump binary memory $dumpfile 0x$startmem 0x$endmem\nquit\n";
	open(GDBCMD, ">$dumpdir/gdb.cmd");
	print GDBCMD "$dumpcmd";
	close(GDBCMD);
	system("gdb -x $dumpdir/gdb.cmd -batch -q > /dev/null"); 
}

sub find_mapline
{
	my $addr = $_[1];

	foreach(@{$settings{'MapLines'}}) {
		my $line = $_;
		if($line =~ /... ([0-9a-f]{1,8})-([0-9a-f]{1,8})/)
		{
			my $start = hex "$1";
			my $end = hex "$2";
			my $line = $_;
			if(($addr > $start) && ($addr < $end)) {
				if($line =~ /stack/)
				{
					#print "[stack]... ignored\n";
				} else {
					return $line;
				}
			}
		}
	}
	return 0;
}

sub read_maps {
	my $pid = $settings{'pid'};
	my $mapsfile = "/proc/$pid/maps";
	my $cnt = 0;

	printf "read_maps: Opening maps file $mapsfile\n";
	open(MYINPUTFILE, "$mapsfile") or die "Can't open maps $mapsfile\n";
	while(<MYINPUTFILE>)
	{
		# Good practice to store $_ value because
		# subsequent operations may change it.
		my($line) = $_;

		# Good practice to always strip the trailing
		# newline from the line.
		chomp($line);

		#print "$line\n";

		if($line =~ /([0-9a-f]{8})-([0-9a-f]{8}) ([rwxp\-]+) ([0-9a-f]{8}) ([0-9]{2}:[0-9]{2}) ([0-9]+) (.*)/) {
			my $startmem = "$1";
			my $endmem = "$2";
			my $perms = "$3";
			my $offset = "$4";
			my $dev = "$5";
			my $inode = "$6";
			my $pathname = "$7";
			
			if(index($pathname, '/') != -1)
			{
				#print "Skipping mapped file $pathname: $line\n";
			}
			elsif(length "$pathname" == 0)
			{
				#print "Found BSS line: $line\n";
				#$MapLines[$cnt++] = "BSS $line";
			}
			elsif(index($pathname, '[') != -1)
			{
				#print "Found MEM line: $line\n";
				if(index($pathname, "[heap]") != -1)
				{
					@{$settings{'MapLines'}}[$cnt++] = "MEM $line";
				} else {
					print "Ignoring section: $pathname\n";
				}
			}
			else {	
				print "!!!Unmatched line: $startmem $endmem $perms $offset $dev $inode $pathname\n";
			}
		}
	}
	close(MYINPUTFILE);
}

sub parse_mem {
	my $pid = $settings{'pid'};
	my $tolerancedist = $_[0];
	#print "Found the following map lines:\n";
	my $mline;
	my @MapLines = @{$settings{'MapLines'}};
	for $mline (@MapLines)
	{
		#print "Line $mline\n";
		if($mline =~ /MEM ([0-9a-f]{8})-([0-9a-f]{8}) ([rwxp\-]+) ([0-9a-f]{8}) ([0-9]{2}:[0-9]{2}) ([0-9]+) (.*)/)
		{
			my $startmem = "$1"; my $endmem = "$2"; my $perms = "$3";
			my $offset = "$4"; my $dev = "$5"; my $inode = "$6";
			my $pathname = "$7";
			my $dumpfile = "dump-$pid/$startmem-$endmem.dump";

			unless (-e "$dumpfile") {
				dumpmem($pid, $startmem, $endmem, $dumpfile);
			}

			open(FH, $dumpfile) or die "Failed to open dumpfile $dumpfile\n";
			binmode(FH);	
			my $fbuf = 0;
			my $n;
			my $startmem = hex $startmem;
			my $memaddr = $startmem;
			my $startcluster = 0;
			#printf "Set startmem to %x for %s\n", $memaddr + (tell FH), $dumpfile;
			while(($n = read(FH, $fbuf, 4)) != 0)
			{
				# Walk through all of process (section) memory
				$memaddr = (tell FH) + $startmem - 4;
				my $dest = unpack('l32', $fbuf);

				my $mapline = find_mapline(@MapLines, $dest); # see if it points to the heap

				if($mapline)
				{
					$startcluster = addGraphPtr($memaddr, $dest, $tolerancedist);
				}
			}
			close(FH);
		}
	}
}

sub graphoutput 
{
	my $dotgraph = $_[0];
	my $graphviz = $_[1];
	my $selectednode = $_[2];

	my $pid = $settings{'pid'};
	my $randnum = rand();
	my $fname = "dump-$pid/$pid-$randnum.png";
	for my $e ($dotgraph->edges())
	{
		my ($u, $v) = @$e;
		$graphviz->add_edge($u => $v);
	}

	# Output vertices
	@{$settings{'components'}} =  $dotgraph->weakly_connected_components();
	my $i = 0;
	my @colors = ( "blue", "green", "orange", "pink" );
	for my $cc (@{$settings{'components'}})
	{
		++$i;
		for my $v (@$cc)
		{
			#print "vertex $v color=$colors[0]\n";
			$graphviz->add_node($v, shape => "record", color => $colors[$i % (scalar @colors)]);
		}
	}

	# Print original pointer list
	my $nodeaddr;
	my $orig_src;
	for $nodeaddr ( keys %{$nodesset{$settings{'curr_tol'}}} ) {
		my $node = %{$nodesset{$settings{'curr_tol'}}}->{"$nodeaddr"};
		my $label = $node->printname;

		for $orig_src ( keys %{$node->original} ) {
			my $src = sprintf "x%08x", $orig_src; 
			my $val = sprintf "x%08x", $node->original($orig_src);
			$label = $label."\n$src->$val";
		}
		# Print memory contents at node
		my @memory = accessMem($nodeaddr, 16);
		for (my $offset = 0; $offset < 16; $offset += 4) {
			my $mem = sprintf "%08x", shift @memory;
			$label = $label."\nnode+$offset: $mem";
		}

		$graphviz->add_node($node->printname, label => $label);
	}




	# Color selection red
	$graphviz->add_node($selectednode, color => "red");

	my $debug_dot = 0;
	if($debug_dot == 1) {
		#print $graphviz->as_debug;
		my $dbg = sprintf "%s", $graphviz->as_debug;
		print "Dot hash " . (md5_hex $dbg) . "\n";
		my $tol = get_tolerance();
		open DBG_OUT, ">dump-$pid/$pid-$tol.dot" or die "Can't open dot dump file";
		print DBG_OUT "$dbg";
		close DBG_OUT;	
	}

	$graphviz->as_png($fname);
	return $fname;
}

sub create_output {
	my $tol = $_[0];
	my $dotgraph = $_[1];
	my $nodeaddr;
	my $dest;
	my $key;
	my $is_overlap = 0;

	for $nodeaddr ( keys %{$nodesset{$tol}} ) {
		my $node = %{$nodesset{$tol}}->{"$nodeaddr"};
		my $x1 = $node->printname;


		for $dest ( @{$node->outptrs} ) {
			my $x2 = sprintf "x%08x", $dest;
			$is_overlap = 1 if($x1 eq $x2);
			$dotgraph->add_edge($x1, $x2);
		}

		$dotgraph->add_vertex($x1);
	}
	if($is_overlap == 1){
		print "\n Tolerance $tol is invalid \n";
	}
	#else{
		#print "\n tolerance is valid.. pushing it";
		push(@valid_tol,$tol);
	#}
}

sub gui
{
	Gtk2::Glade->set_custom_handler( \&makeobjlist );
	$settings{'gladexml'} = Gtk2::GladeXML->new('memcrawler.glade');
	my $gladexml = $settings{'gladexml'};
	$gladexml->signal_autoconnect_from_package('main');
	my $quitbtn = $gladexml->get_widget('Quit'); 
	my $window = $gladexml->get_widget('MemCrawlerWindow');
	$window->signal_connect (destroy => sub { Gtk2->main_quit; });

	#my $dotdatamap = $gladexml->get_widget('DotDatamap');
	#$dotdatamap->set_from_file("dump-$pid/$pid.png");
	update_components();
	my $prognamebox = $gladexml->get_widget('prognamebox')->set_text($progname); 

	$window->maximize();
	Gtk2->main;
}

sub makeobjlist {
	my $pid = $settings{'pid'};
	my ($xml,       # The Gtk2::GladeXML object
            # the remaining arguments are as specified in the glade file:
            $func_name, # The function name
            $name,      # the name of the widget to be created
            $str1,      # the string1 property
            $str2,      # the string2 property
            $int1,      # the int1 property
            $int2,      # the int2 property
            $userdata   # the data passed to set_custom_handler
           ) = @_;

	#print "Making Custom widget $name\n";
	if($name eq "ObjList") {
		#print "Building MemCrawler tab\n";
		$settings{'objlist'} = Gtk2::SimpleList->new("Address" => "text");
		my $objlist = $settings{'objlist'};
		$objlist->get_selection->signal_connect (changed => sub {
			my ($selection) = @_;
			my ($model, $iter) = $selection->get_selected;
			my $selected = $model->get ($iter);
			$settings{'selected_node'} = $selected;
			print "new selection is $selected\n";
			my $objbox = $settings{'gladexml'}->get_widget('ObjInfo'); 
			$objbox->set_text(sprintf(" Current node: %s\nDegree: %d",$settings{'selected_node'},$graphset{$settings{'curr_tol'}}->in_degree($settings{'selected_node'})));
			#graphit("button");
		});

		$objlist->columns_autosize();
		$objlist->show();
		return $objlist;
	} elsif ($name eq "StructList") {
		my $structlist = Gtk2::SimpleList->new("Struct" => "text");
		#update_components();
		$structlist->get_selection->signal_connect (changed => sub {
			my ($selection) = @_;
			my ($model, $iter) = $selection->get_selected;
			my $selected = $model->get ($iter);
			#print "new selection is $selected\n";
                      
			update_nodes($selected);
		});

		$structlist->set_size_request(-1, -1);
		$structlist->show();
		return $structlist;
	} elsif ($name eq "DataStructureList") {
		#print "Building Variations tab\n";
		my $objlist = Gtk2::SimpleList->new("Data structure" => "pixbuf");
		my $line;
		push @{$objlist->{data}}, [Gtk2::Gdk::Pixbuf->new_from_file ("icon.png")];
		push @{$objlist->{data}}, [Gtk2::Gdk::Pixbuf->new_from_file ("icon.png")];
		push @{$objlist->{data}}, [Gtk2::Gdk::Pixbuf->new_from_file ("icon.png")];

		$objlist->show();
		return $objlist;
	} else {
		return Gtk2::SimpleList->new("Error" => "text");
	}
}

sub open_evince
{
	my $pid = $settings{'pid'};
	my $fname = $settings{'main_disp_fname'};
	system "evince $fname&";
}

sub graphfit
{
	graphit("fitbutton");
}

sub graphit_button
{
	graphit("button");
}

sub find_pid
{
	my $pid = shift;
	print "Looking up PID for program $pid\n";
	open(PROGNAME, "pidof -s $pid |") || die "No such program $pid $!\n";
	while(<PROGNAME>) { $pid = $_; }
	close(PROGNAME);
	chomp $pid;
	return $pid;
}

sub get_tolerance
{
	my $gladexml = $settings{'gladexml'};
	my $tolbox = $gladexml->get_widget('tolerance_box'); 
	my $tol = (split $tolbox->get_text)[0];
	return $tol;
}

sub graphit 
{
	my $source = $_[0];
	my $pid = $settings{'pid'};
	my $gladexml = $settings{'gladexml'};
	my $prognamebox = $gladexml->get_widget('prognamebox'); 
	$pid = $prognamebox->get_text; 

	chomp $pid;
	if($pid eq "") { 
		$prognamebox->set_text("<No such prog>"); 
		return; 
	}

	$pid = $settings{pid} = find_pid($pid);

	my $tolbox = $gladexml->get_widget('tolerance_box'); 
	my $tol = $tolbox->get_text; 
	if(!$tol) {
		$tolbox->set_text("<Bad tolerance>"); 
		return; 
	}

	print "Analyzing PID $pid with tolerance $tol\n";

	#init_vars();	
	my $graphviz;
	if($source eq "fitbutton")
	{
		$graphviz = GraphViz->new(width => 6, height => 6);
	} else {
		$graphviz = GraphViz->new;
	}

	$settings{'curr_tol'} = $tol;

	nodes_for_tol($tol);

	$settings{'components'} = [];
	my $fname = graphoutput($graphset{$settings{'curr_tol'}}, $graphviz, $settings{'selected_node'});

	$settings{'main_disp_fname'} = $fname;
	if(($source eq "button") || ($source eq "fitbutton")) {
		update_components();
		update_nodes();
	}
	my $dotdatamap = $gladexml->get_widget('DotDatamap');
	$dotdatamap->set_from_file($fname);
}

sub update_nodes
{
	my $dotgraph = $graphset{$settings{'curr_tol'}};
	my $selected = $_[0];
	my $wc_ind = $dotgraph->weakly_connected_component_by_vertex($selected);
	@{$settings{'components'}} = $dotgraph->weakly_connected_components();
	@{$settings{'objlist'}->{data}} = ();
	my $cnt = 0;
	for my $cc (@{$settings{'components'}}[$wc_ind])
	{
		#print "component is $cc\n";
		for my $v (@$cc)
		{
			$cnt++; 
			push @{$settings{'objlist'}->{data}}, ["$v"];
		}
	}
	my $nodelabel = $settings{'gladexml'}->get_widget('nodelabel'); 
	$nodelabel->set_text(sprintf("Nodes (%d)", $cnt));
}

sub update_components
{
	my $dotgraph = $graphset{$settings{'curr_tol'}};
	my $gladexml = $settings{'gladexml'};
	my $structlist = $gladexml->get_widget('StructList'); 
	if ($dotgraph) {
		@{$settings{'components'}} = $dotgraph->weakly_connected_components();
	}
	@{$structlist->{data}} = ();
	for my $cc (@{$settings{'components'}})
	{
		push @{$structlist->{data}}, ["@$cc[0]"];
	}
	my $structlabel = $gladexml->get_widget('structlabel'); 
	$structlabel->set_text(sprintf("Structures (%d)", scalar @{$settings{'components'}}));
}            

sub init_vars
{
	$settings{'components'} = [];
	$settings{'MapLines'} = [];
	$settings{'selected_node'} = "none";
}

sub dialog_box
{
	my $message = $_[0];
	my $dialog = Gtk2::Dialog->new('Tolerance', $settings{'gladexml'}->get_widget('MemCrawlerWindow'), 'destroy-with-parent', 'OK' => 'ok');
	my $label = Gtk2::Label->new ($message);
	$dialog->vbox->add ($label);
	$dialog->signal_connect (response => sub { $_[0]->destroy });

	$dialog->show_all;
}

sub find_tol_sens
{
	my $timeout = 10;
	my @valid_tol;
	my @tolerances;
	
	if((scalar @{$settings{'MapLines'}} eq 0)) {
		dialog_box("Please create graph first");
		return;
	}

	my $tolbox = $settings{'gladexml'}->get_widget('tolerance_box');
	my $tol_combo = $settings{'gladexml'}->get_widget('tolerance_combo');
	my $tol = $tolbox->get_text; 
	
	my $time = localtime->sec(); 
	my $cur = $tol;
	my $cur2;

	my $lim = 100;
	while($tol_combo->get_active && ($lim-- > 0))
	{
		$tol_combo->remove_text(0);
	}

	# Quick threading hack here... since Winston's system caches
	# graphs, just pre-generate the graphs here...

	for(my $counter = 1; $counter <= 25; $counter++)
	{
		# Quit after s seconds
		((localtime->sec() - $time) > $timeout) && print "Timeout!\n";
		((localtime->sec() - $time) > $timeout) && last;

		printf "..%d", $counter*4;

		push(@tolerances, $counter);
	}

	foreach my $counter (@tolerances)
	{
		$cur2 =	$tol + (4*$counter);

		my $t1 = nodes_for_tol($cur);
		my $t2 = nodes_for_tol($cur2);

		if($t1 eq $t2) {
			print  "t$cur2 is equal to t$cur\n";
			$tol_combo->append_text("$cur2 stable");
# TODO: make sure "stable" label doesn't get considered in cached graph lookup

		} else{
			#print "New sensitive point $cur $cur2\n";
			$tol_combo->append_text("$cur2");
			$cur = $cur2;
		}
	}
	
	return @valid_tol;
}

sub nodes_for_tol
{
	my $tol = shift;

	print "NODES FOR TOL called on $tol\n";

	if (!exists $graphset{$tol}) {
		print "making a new nodes_for_tol for $tol\n";
		$graphset{$tol} = Graph::Directed->new;
		$nodesset{$tol} = ();
		if((scalar @{$settings{'MapLines'}} eq 0)) {
			read_maps();
		}
		parse_mem($tol);
		groupPtrSrc($tol);
		create_output($tol, $graphset{$tol});
	}
	#print "\n in node_for_tol and the tolerance is: @out1 \n";
	return ($graphset{$tol});
}

sub union {
	my @all = @_;
	my %union = ();
	for my $e (@all) {
		$union{$e} = 1;
	}
	return keys %union;
}

sub go_button
{
	print "Go!\n";
	
	my @nodes_union = ();
	#my @valid_tolerances = find_tol_sens();
	
	# For each tolerance, generate node list and union them together	
	foreach(@valid_tol)
	{
		my $tol = $_;
		# Generate nodes per tolerance in $t
		#my $t = nodes_for_tol($tol);
		my @nodes_tol = keys %{$nodesset{$tol}};
		print "\n @nodes_tol";
		@nodes_union = union(@nodes_union,@nodes_tol);
		
	}
	
	# Generate the Matrix... rows = nodes, columns = tolerance, one run per node... so this is actually 3d

	# Example (V node = 3)
	# 	Tolerance	4	8	12	16	P
	# Node
	# 1			1	1	0	0	0.5
	# 2			0	0	0	0	0.0
	# 3 			1	1	1	1	1.0
	print "\n********** @nodes_union";
	my @matrix = generate_matrix(@nodes_union);
	print "\n The matrix is: $#nodes_union x $#valid_tol \n";
	my $s1 = $#nodes_union + 1;
	my $s2 = $#valid_tol + 1;
	open(MATOUT, ">matrix.txt");
	print "Printing to file\n";
	for (my$k = 0; $k < $s1;$k++){
		print MATOUT "Adjacency Matrix for Node $k:\n";
		for (my $i=0;$i < $s2;$i++) {
        		for (my $j=0;$j < $s1;$j++) {
        	    		print MATOUT $matrix[$i][$j][$k]," ";
        		}
 		print MATOUT "\n";
		}
	}
	close(MATOUT);
	print "Matrix generation complete\n";
	# Cluster the points based on the probability
	cluster_components($s1,$s2,@matrix);

	# Analyze degrees of components for uniformity
	#analyze_component_degrees(@matrix);

	# ???
	# Profit!	

	#find_tol_sens();
}

sub generate_matrix
{
	my @n_union = @_;
	my @the_matrix = ();
	my $tol_count = 0;
	my $node_count = 0;
	#my $comp_node = sprintf "x%08x", @n_union[3];
	foreach my $tol (@valid_tol){
		my @matrix2d;
		my $t = nodes_for_tol($tol);
		my $wcc_set_size = scalar $t->weakly_connected_components();
		print "number of wcc's at tol $tol: $wcc_set_size\n";
		$tol_count++;		
		foreach my $c1 (@n_union){
			my @adj_list;		
			my $curr_node = sprintf "x%08x", $c1;
 			# my @a1 = ($curr_node);
			my @n1 = $t->neighbors($curr_node);
			#print "\n Neighbours of $curr_node are @n1 \n";
			my $is_neighbour= 0;
			foreach my $n2 (@n_union){
				my $comp_node = sprintf "x%08x", $n2;		
				foreach my $c2 (@n1){
					my $curr_n = sprintf "%s", $c2; 
					#print "\n cur N is: $curr_n";
					if($comp_node eq $curr_n){
						$is_neighbour = 1;
						# last; #winston's edit???
						#print "Node $node_count is a neighbour to the given node... \n";
					}
				}
				push(@adj_list,$is_neighbour);
				$is_neighbour = 0; #winston's edit???
			}
			push(@matrix2d, [@adj_list]);
		}
		push(@the_matrix,[@matrix2d]);
	}
	print "\n $tol_count , $node_count";
	return @the_matrix;
}

sub cluster_components
{
	
	my $s1 = scalar shift;
	my $s2 = scalar shift;
	my @matrix = @_;
	print " s1 is $s1 and s2 is $s2\n";
	open(PROBOUT, ">probabilities.txt");
	for (my$k = 0; $k < $s1;$k++){
		for (my $i=0;$i < $s1;$i++) {
			my $prob = 0;
        		for (my $j=0;$j < $s2;$j++) {
				#printf "Calc probability matrix %d %d %d\n", $j,$i,$k;        	    		
				$prob = $prob + $matrix[$j][$i][$k];
        		}
			# $prob = $prob/$s1;
 			print PROBOUT "$prob\t\t";
		}
		print PROBOUT "\n";
	}
	close(PROBOUT);
	print "Done generating probabilities\n";
}

sub test_clustering
{
	# Test code for Perl's clustering algorithm
	# http://search.cpan.org/~salva/Algorithm-ClusterPoints-0.08/lib/Algorithm/ClusterPoints.pm
	my $clp = Algorithm::ClusterPoints->new( dimension => 3,
			radius => 1.0,
			minimum_size => 2,
			ordered => 1 );
	my @points;
	for my $p (@points) {
		$clp->add_point($p->{x}, $p->{y}, $p->{z});
	}
	my @clusters = $clp->clusters_ix;
	for my $i (0..$#clusters) {
		print( join( ' ',
					"cluster $i:",
					map {
					my ($x, $y, $z) = $clp->point_coords($_);
					"($_: $x, $y, $z)"
					} @{$clusters[$i]}
			   ), "\n"
		     );
	}
}
