#!/bin/sh
#
# tarball.sh
#
# $Id: tarball.sh 9 2005-05-17 02:30:49Z guillem $
#
# Copyright (C) 2004 Guillem Jover <guillem@debian.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#

set -e

SRCDIR="bochs-snapshot"
export CVSROOT=":pserver:anonymous@cvs.sf.net:/cvsroot/bochs"

case "$1" in
  snapshot)
    echo "-> getting new snapshot."
    cvs login
    cvs co -d $SRCDIR bochs
    ;;
  update)
    echo "-> updating snapshot."
    cvs up -dP $SRCDIR
    ;;
esac

if ! [ -e $SRCDIR/bochs.h ]
then
  echo "error: no bochs source dir available."
  exit 1
fi

echo "-> creating new directory tree."
eval $(grep '^VERSION=' $SRCDIR/configure.in | sed -e 's/\.cvs//' )
SNAPSHOTVERSION="$VERSION+$(date +%Y%m%d)"
TARBALLDIR="bochs-$SNAPSHOTVERSION"
cp -al $SRCDIR $TARBALLDIR

echo "-> cleaning snapshot."
# Clean non-free stuff
rm -f $TARBALLDIR/bios/BIOS*
rm -f $TARBALLDIR/bios/VGABIOS*
# Clean cvs stuff
find $TARBALLDIR -name 'CVS' -o -name '.cvsignore' | xargs rm -rf

echo "-> creating new tarball."
tar czf bochs_$SNAPSHOTVERSION.orig.tar.gz $TARBALLDIR

echo "-> cleaning directory tree."
rm -rf $TARBALLDIR

