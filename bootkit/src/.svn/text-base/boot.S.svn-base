/* boot.S - bootstrap the kernel */
/* Copyright (C) 1999, 2001  Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
 
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
 
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#define ASM	1
#include "multiboot.h"
	
	.text

	.globl	start, _start
start:
_start:
	jmp	multiboot_entry

	/* Align 32 bits boundary.  */
	.align	4
	
	/* Multiboot header.  */
multiboot_header:
	/* magic */
	.long	MULTIBOOT_HEADER_MAGIC
	/* flags */
	.long	MULTIBOOT_HEADER_FLAGS
	/* checksum */
	.long	-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
#ifndef __ELF__
	/* header_addr */
	.long	multiboot_header
	/* load_addr */
	.long	_start
	/* load_end_addr */
	.long	_edata
	/* bss_end_addr */
	.long	_end
	/* entry_addr */
	.long	multiboot_entry
#endif /* ! __ELF__ */

multiboot_entry:
	/* Initialize the stack pointer.  */
	movl	$(stack + STACK_SIZE), %esp

	/* Reset EFLAGS.  */
	pushl	$0
	popf

	/* Push the pointer to the Multiboot information structure.  */
	pushl	%ebx

	/* Push the magic value.  */
	pushl	%eax

	/* Now enter the C main function...  */
	call	EXT_C(cmain)

	/* Halt.  */
	/*
	pushl	$halt_message
	call	EXT_C(printf)
	*/
	
	/* Execute the payload */
	call	EXT_C(payload) 

	/* Enable protected mode */
	nop
	call	resuscitate
	nop

	/* Not reached */

loop:	hlt
	jmp	loop

resuscitate:
	/* Push stack */
	push	%ebp
	mov	%esp,%ebp

	/* Inform user */
	pushl	$resuscitate_message
	call	EXT_C(printf)

	/* Enable protected mode */
	cli /* disable interrupts */

	jmp asm_setup

.globl finish_setup
finish_setup:
	/* Finish, not reached */
	pushl	$resuscitate_message_done
	call	EXT_C(printf)

	leave
	ret


halt_message:
	.asciz	"Halted.\0"

resuscitate_message:
	.asciz	"Resuscitating system...\n\0"

resuscitate_message_done:
	.asciz	"Resuscitating system... done\n\0"

/* Our stack area.  */
.comm	stack, STACK_SIZE

