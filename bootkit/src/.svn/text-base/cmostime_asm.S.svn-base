/**
 *  cmostime.S
 *  get system time from CMOS
 */

#include "cmostime.h"

.globl get_cmos_time

get_cmos_time:
	pushl	%ebp
	movl	%esp, %ebp

check:
	movb	$CMOS_DISABLE_INT, %al
	outb	%al, $CMOS_TIME_INPORT
	inb		$CMOS_TIME_OUTPORT, %al
	test	$CMOS_DISABLE_INT, %al
	jnz		check
	movb	8(%ebp), %al
	outb	%al, $CMOS_TIME_INPORT
	inb		$CMOS_TIME_OUTPORT, %al

	movl	%ebp, %esp
	popl	%ebp
	ret

