target remote localhost:1234
define rem
target remote localhost:1234
end
display/i $eip
b *(&__log_buf)
b sysrq_handle_test_reboot
b panic
b die
c
