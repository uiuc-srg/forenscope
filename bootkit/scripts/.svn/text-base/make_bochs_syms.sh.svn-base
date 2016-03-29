#! /bin/sh

cat images/System.map | while read i; do echo 0x$i; done | awk '{print $1, $3}' > syms
