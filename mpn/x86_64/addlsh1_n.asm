dnl  AMD64 mpn_addlsh1_n -- rp[] = up[] + (vp[] << 1)

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
C Hammer:     2.5	(mpn_lshift + mpn_add_n need 4.125)

C The speed is constrained by the recurrency on carry and eax.  There are 10
C instructions on that chain, therefore 10/4 = 2.5 c/l.  Try breaking the
C recurrency by keeping acy and scy in different registers.  (Hammer performs
C renaming on the flags register.)  We might be able to reach 2 c/l.

C INPUT PARAMETERS
define(`rp',`%rdi')
define(`up',`%rsi')
define(`vp',`%rdx')
define(`n',`%rcx')

ASM_START()
PROLOGUE(mpn_addlsh1_n)
	pushq	%rbx			C				1

	xorl   %eax, %eax		C				2
	movl	%ecx, %ebx		C				2

	shrq   $2, n			C				4
	clc				C				1
	je     .Lend			C				2
	andl	$3, %ebx		C				3
	ALIGN(16)
.Loop:
	rcll	%eax			C save acy, restore scy

	leaq	32(up), up
	leaq	32(rp), rp

	movq	0(vp), %r8
	adcq	%r8, %r8
	movq	8(vp), %r9
	adcq	%r9, %r9
	movq	16(vp), %r10
	adcq	%r10, %r10
	movq	24(vp), %r11
	adcq	%r11, %r11
	leaq	32(vp), vp

	rcrl	%eax			C save scy, restore acy

	adcq	-32(up), %r8
	movq	%r8, -32(rp)
	adcq	-24(up), %r9
	movq	%r9, -24(rp)
	adcq	-16(up), %r10
	movq	%r10, -16(rp)
	adcq	-8(up), %r11
	movq	%r11, -8(rp)

	decq	n
	jne	.Loop
.Lend:
	incl	%ebx
	decl	%ebx
	je	.Lret

	rcll	%eax			C save acy, restore scy
	movq	(vp), %r8
	adcq	%r8, %r8
	rcrl	%eax			C save scy, restore acy
	adcq	(up), %r8
	movq	%r8, (rp)
	decl	%ebx
	je	.Lret

	rcll	%eax			C save acy, restore scy
	movq	8(vp), %r8
	adcq	%r8, %r8
	rcrl	%eax			C save scy, restore acy
	adcq	8(up), %r8
	movq	%r8, 8(rp)
	decl	%ebx
	je	.Lret

	rcll	%eax			C save acy, restore scy
	movq	16(vp), %r8
	adcq	%r8, %r8
	rcrl	%eax			C save scy, restore acy
	adcq	16(up), %r8
	movq	%r8, 16(rp)
	decl	%ebx
	je	.Lret
.Lret:
	rcll	%eax
	adcl	$0, %eax

	popq	%rbx			C				1
	ret				C				1
EPILOGUE()
