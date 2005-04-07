dnl  AMD64 mpn_divexact_by3 -- mpn division by 3, expecting no remainder.

dnl  Copyright 2000, 2002, 2004, 2005 Free Software Foundation, Inc.
dnl
dnl  This file is part of the GNU MP Library.
dnl
dnl  The GNU MP Library is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU Lesser General Public License as
dnl  published by the Free Software Foundation; either version 2.1 of the
dnl  License, or (at your option) any later version.
dnl
dnl  The GNU MP Library is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl  Lesser General Public License for more details.
dnl
dnl  You should have received a copy of the GNU Lesser General Public
dnl  License along with the GNU MP Library; see the file COPYING.LIB.  If
dnl  not, write to the Free Software Foundation, Inc., 59 Temple Place -
dnl  Suite 330, Boston, MA 02111-1307, USA.

include(`../config.m4')


C		    cycles/limb
C Hammer:		7
C Prescott/Nocona:	24

C Based on 32-bit Athlon code.
C Not carefully optimized, but it already runs quite well.

C INPUT PARAMETERS
define(`rp',	`%rdi')
define(`up',	`%rsi')
define(`n',	`%rdx')
define(`cy',	`%rcx')

ASM_START()
PROLOGUE(mpn_divexact_by3c)

	movq	(up), %rax		C up low limb

	movq	$0x5555555555555556, %r11
	leaq	-1(%r11,%r11), %r10
	leaq	-2(%r11,%r11), %r8
	leaq	(up,n,8), up		C &up[n-1]

	subq	cy, %rax

	setc	%cl
	decq	n
	jz	.Lend

	leaq	(rp,n,8), rp		C &rp[n-1]
	negq	n

.Loop:	imulq	%r10, %rax
	movq	%rax, (rp,n,8)
	cmpq	%r11, %rax
	sbbq	$-1, cy			C +1 if result>=ceil(b/3)
	cmpq	%rax, %r8
	movq	(up,n,8), %rax		C next up limb
	sbbq	cy, %rax		C and further 1 if result>=ceil(b*2/3)
	incq	n
	setc	%cl			C new carry
	jnz	.Loop

.Lend:	imulq	%r10, %rax
	movq	%rax, (rp)
	cmpq	%r11, %rax
	sbbq	$-1, cy			C +1 if eax>=ceil(b/3)
	cmpq	%rax, %r8
	movq	n, %rax			C n is zero here
	adcq	cy, %rax		C further +1 if eax>=ceil(b*2/3)
	ret
EPILOGUE()
