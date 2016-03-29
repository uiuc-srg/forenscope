#!/bin/bash
#
# Copyright (C) 2007,
#   Geoff Buchan	<geoffrey.buchan@gmail.com>
# Based on the script cvsvimdiff, written by
#   Stefano Zacchiroli	<zack@cs.unibo.it>
#   Enrico Tassi	<tassi@cs.unibo.it>
#
# This is free software, you can redistribute it and/or modify it under the
# terms of the GNU General Public License version 2 as published by the Free
# Software Foundation.
#

vimdiff="vimdiff"
suffix="vimsvndiff"
if [[ $1 == "-g" ]] ; then
  vimdiff="gvimdiff -f"
  shift 1
fi
if [[ $# < 0 || $1 == "--help" || $1 == "-h" ]] ; then
    echo "svnvimdiff - script to show svn diff in vimdiff format"
    echo ""
    echo "svnvimdiff [options] file"
    echo ""
    echo "Option:"
    echo "-g    Use gvimdiff (graphical mode) instead of vimdiff"
    echo "Other options are passed to svn diff"
    echo ""
    echo "If file is omitted it will cycle through all changed files in"
    echo "the current directory."
    exit 1
fi

# Assume the last argument is the filename.
# Save everything to pass to svn diff
if (( $# > 0 )) ; then
   shift_args=$(($# - 1))
else
   shift_args=$#
fi
args=$*
shift $shift_args
files="$1"
patch=`tempfile -p $suffix`
orig=`tempfile -p $suffix`
tempfiles="$patch $orig"
if [ -z $files ] || ! [ -f $files ] ; then
    # No file given, so loop over all files svn st says have changed
    files=$(svn st 2> /dev/null | grep -e "^[MU]" | cut -c 3-)
    for f in $files; do
      if ! [ -f $f ]; then break; fi
      filename=`basename $f`
	  ending=${filename/*./}
	  orig=$orig"."$ending 
      cp "$f" $orig
      svn diff $args $f > $patch
      if ! [ $? -eq 0 ]; then break; fi
      patch -R -p0 $orig $patch
      $vimdiff $orig $f
      tempfiles+=" $orig"
      trap "rm -f $tempfiles" EXIT
    done
else
    # file given, so just work with that one
    filename=`basename $files`
    ending=${filename/*./}
    orig=$orig"."$ending
    cp $files $orig
    svn diff $args > $patch
    if ! [ $? -eq 0 ]; then break; fi
    patch -R -p0 $orig $patch
    $vimdiff $orig $files
    tempfiles+=" $orig"
    trap "rm -f $tempfiles" EXIT
fi
