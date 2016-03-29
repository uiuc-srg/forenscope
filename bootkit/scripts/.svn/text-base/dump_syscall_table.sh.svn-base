#! /bin/ksh

syscall_table=$(cat module/linuxaddr.h  | grep sys_call | tr " \t" " " | cut -d " " -f 4)
syscall_table_size=$(cat module/linuxaddr.h  | grep syscall_table_size | tr " \t" " " | cut -d " " -f 4)
#end=$(perl -e "printf \"0x%x\",$syscall_table + $syscall_table_size") 
end=$(perl -e "printf \"%d\", (hex $syscall_table_size)/4") 

echo "Dumping syscall table from $syscall_table size=$syscall_table_size"

#echo "dump memory binary syscall.bin $syscall_table $end" > syscall.gdb
echo "dump binary value syscall.bin *$syscall_table@$syscall_table_size" > syscall.gdb
gdb vmlinux -batch -x syscall.gdb
#objdump -O binary --start-address=$syscall_table --stop-address=$syscall_table_size

