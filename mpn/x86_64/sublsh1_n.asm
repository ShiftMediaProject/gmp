dnl  AMD64 mpn_sublsh1_n -- rp[] = up[] + (vp[] << 1)

dnl  Copyright 2003 Free Software Foundation, Inc.

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
dnl  along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
dnl  the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
dnl  MA 02111-1307, USA.

include(`../config.m4')


C         cycles/limb
C Hammer:     2.45	(mpn_lshift + mpn_sub_n need 4.125)

C Sometimes speed degenerates to 2.58 c/l, supposedly related to that some
C operand alignments cause conflicts in the blocked cache.

C The speed is limited by decoding/issue bandwith.  There are 29 instructions
C in the loop, which corresponds to 29/3/4 = 2.417 c/l.  Better speed could be
C achieved by using indexed addressing (and thereby saving pointer updates) and
C by using more unrolling.  Since we save and restore carry using two registers
C (ebp and eax), carry is irrelevant between these instructions, and that could
C be used for loop counter update using addq, as required for proper indexed
C addressing.

C It should be possible to get a little under 2 c/l for this operation, to
C about 46/8/3 = 1.917 c/l.

C FIXME: Use technique of rsh1add_n.asm to handle unrolling.  That will save
C a register (rbx) and allow for a noticable constant speedup.

C INPUT PARAMETERS
define(`rp',`%rdi')
define(`up',`%rsi')
define(`vp',`%rdx')
define(`n',`%rcx')

ASM_START()
	TEXT
	ALIGN(16)
	.byte	0,0,0,0,0,0,0,0
PROLOGUE(mpn_sublsh1_n)
	pushq	%rbx			C				1
	pushq	%rbp			C				1
	pushq	%r13			C				2

	xorl	%eax, %eax		C				2
	movl	%ecx, %ebx		C				2

	shrq	$2, n			C				4
	clc				C				1
	je	.Lend			C				2
	andl	$3, %ebx		C				3
	.byte	0x66,0x90,0x66,0x90,0x66,0x90
	ALIGN(16)
.Loop:
	sbbl	%ebp, %ebp		C save acy
	addl	%eax, %eax		C restore scy

	leaq	32(up), up
	leaq	32(rp), rp

	movq	(vp), %r8
	adcq	%r8, %r8
	movq	8(vp), %r9
	adcq	%r9, %r9
	movq	16(vp), %r10
	adcq	%r10, %r10
	movq	24(vp), %r11
	adcq	%r11, %r11
	leaq	32(vp), vp

	sbbl	%eax, %eax		C save scy
	addl	%ebp, %ebp		C restore acy

	movq	-32(up), %rbp
	sbbq	%r8, %rbp
	movq	-24(up), %r13
	sbbq	%r9, %r13
	movq	%rbp, -32(rp)
	movq	%r13, -24(rp)
	movq	-16(up), %rbp
	sbbq	%r10, %rbp
	movq	-8(up), %r13
	sbbq	%r11, %r13
	movq	%rbp, -16(rp)
	movq	%r13, -8(rp)

	decq	n
	jne	.Loop
.Lend:
	incl	%ebx
	decl	%ebx
	je	.Lret

	sbbl	%ebp, %ebp		C save acy
	addl	%eax, %eax		C restore scy
	movq	(vp), %r8
	adcq	%r8, %r8
	sbbl	%eax, %eax		C save scy
	addl	%ebp, %ebp		C restore acy
	movq	(up), %rbp
	sbbq	%r8, %rbp
	movq	%rbp, (rp)
	decl	%ebx
	je	.Lret

	sbbl	%ebp, %ebp		C save acy
	addl	%eax, %eax		C restore scy
	movq	8(vp), %r8
	adcq	%r8, %r8
	sbbl	%eax, %eax		C save scy
	addl	%ebp, %ebp		C restore acy
	movq	8(up), %rbp
	sbbq	%r8, %rbp
	movq	%rbp, 8(rp)
	decl	%ebx
	je	.Lret

	sbbl	%ebp, %ebp		C save acy
	addl	%eax, %eax		C restore scy
	movq	16(vp), %r8
	adcq	%r8, %r8
	sbbl	%eax, %eax		C save scy
	addl	%ebp, %ebp		C restore acy
	movq	16(up), %rbp
	sbbq	%r8, %rbp
	movq	%rbp, 16(rp)
	decl	%ebx
	je	.Lret
.Lret:
	sbbl	$0, %eax
	negl	%eax

	popq	%r13			C				2
	popq	%rbp			C				1
	popq	%rbx			C				1
	ret				C				1
EPILOGUE()
