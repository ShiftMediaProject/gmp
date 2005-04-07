dnl  AMD64 mpn_mul_1 -- Multiply a limb vector with a limb and store the
dnl  result in a second limb vector.

dnl  Copyright 2003, 2004, 2005 Free Software Foundation, Inc.

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


C		    cycles/limb
C Hammer:		 3
C Prescott/Nocona:	13

C TODO
C  * Perhaps make this use the algorithm of addmul_1.asm.


C INPUT PARAMETERS
C rp		rdi
C up		rsi
C n		rdx
C vl		rcx
C cylimb	r8

	TEXT
	ALIGN(16)
	.byte	0,0,0,0,0,0,0,0,0	C this aligns the loop		      9
ASM_START()
PROLOGUE(mpn_mul_1)
	xorl	%r8d, %r8d		C clear carry limb		      3
PROLOGUE(mpn_mul_1c)
	movq	%rdx, %r11		C				      3
	leaq	(%rsi,%rdx,8), %rsi	C				      4
	leaq	(%rdi,%rdx,8), %rdi	C				      4
	negq	%r11			C				      3
	addq	$3, %r11		C				      4
	jb	.Ltail			C jump for n = 1, 2, 3		      2

.Loop:	movq	-24(%rsi,%r11,8), %rax	C				      5
	mulq	%rcx			C				      3
	xorq	%r9, %r9		C				      3
	addq	%rax, %r8		C				      3
	adcq	%rdx, %r9		C				      3

	movq	-16(%rsi,%r11,8), %rax	C				      5
	mulq	%rcx			C				      3
	xorq	%r10, %r10		C				      3
	addq	%rax, %r9		C				      3
	adcq	%rdx, %r10		C				      3

	movq	%r8, -24(%rdi,%r11,8)	C				      5
	movq	%r9, -16(%rdi,%r11,8)	C				      5

	movq	-8(%rsi,%r11,8), %rax	C				      5
	mulq	%rcx			C				      3
	xorq	%r9, %r9		C				      3
	addq	%rax, %r10		C				      3
	adcq	%rdx, %r9		C				      3

	movq	(%rsi,%r11,8), %rax	C				      4
	mulq	%rcx			C				      3
	xorq	%r8, %r8		C				      3
	addq	%rax, %r9		C				      3
	adcq	%rdx, %r8		C				      3

	movq	%r10, -8(%rdi,%r11,8)	C				      5
	movq	%r9, (%rdi,%r11,8)	C				      4

	addq	$4, %r11		C				      4
	jae	.Loop			C				      2

	cmpl	$3, %r11d		C				      4
	jne	.Ltail			C				      2

	movq	%r8, %rax		C				      3
	ret				C				      1

.Ltail:	movq	-24(%rsi,%r11,8), %rax	C				      5
	mulq	%rcx			C				      3
	xorq	%r9, %r9		C				      3
	addq	%rax, %r8		C				      3
	adcq	%rdx, %r9		C				      3

	cmpl	$2, %r11d		C				      4
	jne	.L1			C				      2

	movq	%r8, -8(%rdi)		C				      4
	movq	%r9, %rax		C				      3
	ret				C				      1

.L1:	movq	-16(%rsi,%r11,8), %rax	C				      5
	mulq	%rcx			C				      3
	xorq	%r10, %r10		C				      3
	addq	%rax, %r9		C				      3
	adcq	%rdx, %r10		C				      3

	cmpl	$1, %r11d		C				      4
	jne	.L2			C				      2

	movq	%r8, -16(%rdi)		C				      4
	movq	%r9, -8(%rdi)		C				      4
	movq	%r10, %rax		C				      3
	ret				C				      1

.L2:	movq	-8(%rsi), %rax		C				      4
	mulq	%rcx			C				      3
	xorq	%r11, %r11		C				      3
	addq	%rax, %r10		C				      3
	adcq	%rdx, %r11		C				      3

	movq	%r8, -24(%rdi)		C				      4
	movq	%r9, -16(%rdi)		C				      4
	movq	%r10, -8(%rdi)		C				      4
	movq	%r11, %rax		C				      3
	ret				C				      1
EPILOGUE(mpn_mul_1)
EPILOGUE(mpn_mul_1c)

