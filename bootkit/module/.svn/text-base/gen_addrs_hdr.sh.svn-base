#! /bin/sh
EXEC="$1"
SYMBOLS="$2"
OUTFILE="$3"

. ${SYMBOLS}

echo > ${OUTFILE}

for i in $symbols
do
	echo "Looking for symbol <$i>"
	line=`nm ${EXEC} | grep ". $i\$" | grep -v __k`
	echo $line
	addr=`echo $line | cut -f 1 -d ' '`
	echo "Found symbol $i at $addr"
	[ -n "$addr" ] || { echo "Cannot find symbol $i"; exit 1; }

	echo "#define kk_$i\t\t0x$addr" >> ${OUTFILE}
done
