dnl  AMD64 mpn_sublsh1_n -- rp[] = up[] - (vp[] << 1)

dnl  Copyright 2003, 2005, 2006 Free Software Foundation, Inc.

dnl  This file is part of the GNU MP Library.

dnl  The GNU MP Library is free software; you can redistribute it and/or modify
dnl  it under the terms of the GNU Lesser General Public License as published
dnl  by the Free Software Foundation; either version 2.1 of the License, or (at
dnl  your option) any later version.

dnl  The GNU MP Library is distributed in the hope that it will be useful, but
dnl  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
dnl  License for more details.

dnl  You should have received a copy of the GNU Lesser General Public License
dnl  along with the GNU MP Library; see the file COPYING.LIB.  If not, write
dnl  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
dnl  Boston, MA 02110-1301, USA.

include(`../config.m4')


C		    cycles/limb
C Hammer:		2.21
C Prescott/Nocona:	?


C Sometimes speed degenerates, supposedly related to that some operand
C alignments cause cache conflicts.

C The speed is limited by decoding/issue bandwidth.  There are 26 instructions
C in the loop, which corresponds to 26/3/4 = 2.167 c/l.

C Each limb needs 5 instructions, which corresponds to 1.67 c/l.  8-way
C unrolling could approach 1.917 c/l.

C INPUT PARAMETERS
define(`rp',`%rdi')
define(`up',`%rsi')
define(`vp',`%rdx')
define(`n',`%rcx')

ASM_START()
	TEXT
	ALIGN(16)
PROLOGUE(mpn_sublsh1_n)
	pushq	%rbx
	pushq	%rbp

	movq	(vp), %r8
	movl	%ecx, %eax
	leaq	(rp,n,8), rp
	leaq	(up,n,8), up
	leaq	(vp,n,8), vp
	negq	n
	xorl	%ebp, %ebp
	andl	$3, %eax
	je	.Lb00
	cmpl	$2, %eax
	jc	.Lb01
	je	.Lb10

.Lb11:	addq	%r8, %r8
	movq	8(vp,n,8), %r9
	adcq	%r9, %r9
	movq	16(vp,n,8), %r10
	adcq	%r10, %r10
	sbbl	%eax, %eax		C save scy
	movq	(up,n,8), %rbp
	movq	8(up,n,8), %rbx
	subq	%r8, %rbp
	sbbq	%r9, %rbx
	movq	%rbp, (rp,n,8)
	movq	%rbx, 8(rp,n,8)
	movq	16(up,n,8), %rbp
	sbbq	%r10, %rbp
	movq	%rbp, 16(rp,n,8)
	sbbl	%ebp, %ebp		C save acy
	addq	$3, n
	jmp	.LL

.Lb10:	addq	%r8, %r8
	movq	8(vp,n,8), %r9
	adcq	%r9, %r9
	sbbl	%eax, %eax		C save scy
	movq	(up,n,8), %rbp
	movq	8(up,n,8), %rbx
	subq	%r8, %rbp
	sbbq	%r9, %rbx
	movq	%rbp, (rp,n,8)
	movq	%rbx, 8(rp,n,8)
	sbbl	%ebp, %ebp		C save acy
	addq	$2, n
	jmp	.LL

.Lb01:	addq	%r8, %r8
	sbbl	%eax, %eax		C save scy
	movq	(up,n,8), %rbp
	subq	%r8, %rbp
	movq	%rbp, (rp,n,8)
	sbbl	%ebp, %ebp		C save acy
	incq	n
.LL:	jns,pn	.Lend

	ALIGN(16)
.Loop:	addl	%eax, %eax		C restore scy

	movq	(vp,n,8), %r8
.Lb00:	adcq	%r8, %r8
	movq	8(vp,n,8), %r9
	adcq	%r9, %r9
	movq	16(vp,n,8), %r10
	adcq	%r10, %r10
	movq	24(vp,n,8), %r11
	adcq	%r11, %r11

	sbbl	%eax, %eax		C save scy
	addl	%ebp, %ebp		C restore acy

	movq	(up,n,8), %rbp
	movq	8(up,n,8), %rbx
	sbbq	%r8, %rbp
	sbbq	%r9, %rbx
	movq	%rbp, (rp,n,8)
	movq	%rbx, 8(rp,n,8)
	movq	16(up,n,8), %rbp
	movq	24(up,n,8), %rbx
	sbbq	%r10, %rbp
	sbbq	%r11, %rbx
	movq	%rbp, 16(rp,n,8)
	movq	%rbx, 24(rp,n,8)

	sbbl	%ebp, %ebp		C save acy
	addq	$4, n
	js,pt	.Loop

.Lend:	addl	%ebp, %eax
	negl	%eax

	popq	%rbp
	popq	%rbx
	ret
EPILOGUE()
