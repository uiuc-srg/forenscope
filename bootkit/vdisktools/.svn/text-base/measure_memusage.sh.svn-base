#! /bin/sh

sync
echo 3 > /proc/sys/vm/drop_caches

$@&

while true
do
	date >> /tmp/memusage.log
	cat /proc/meminfo >> /tmp/memusage.log
	echo --------- >> /tmp/memusage.log
	echo "."
	sleep 1
done
	

