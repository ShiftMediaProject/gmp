dnl  AMD64 mpn_mul_1 -- Multiply a limb vector with a limb and store the
dnl  result in a second limb vector.

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
C Hammer:     3.25


C INPUT PARAMETERS
C rp	rdi
C up	rsi
C n	rdx
C vl	rcx

	TEXT
	ALIGN(16)
	.byte	0x66, 0x90, 0x66, 0x90, 0x66, 0x90, 0x66, 0x90
ASM_START()
PROLOGUE(mpn_mul_1)
	pushq	%r12			C				      2
	pushq	%rbx			C				      1

	movq	%rdx, %rbx		C				      3
	xorq	%r8, %r8		C clear carry limb		      3
	xorq	%r12, %r12		C maintain r12 = 0 FIXME: don't!      3
	subq	$4, %rbx		C				      4
	jb	.Lend			C				      2
	.byte	0x66, 0x90, 0x66, 0x90, 0x66, 0x90			      6
.Loop:
	movq	(%rsi), %rax		C				      3
	mulq	%rcx			C				      3
	movq	%r12, %r9		C				      3
	addq	%rax, %r8		C				      3
	adcq	%rdx, %r9		C				      3

	movq	8(%rsi), %rax		C				      4
	mulq	%rcx			C				      3
	movq	%r12, %r10		C				      3
	addq	%rax, %r9		C				      3
	adcq	%rdx, %r10		C				      3

	movq	16(%rsi), %rax		C				      4
	mulq	%rcx			C				      3
	movq	%r12, %r11		C				      3
	addq	%rax, %r10		C				      3
	adcq	%rdx, %r11		C				      3

	movq	%r8, (%rdi)		C				      3
	movq	%r9, 8(%rdi)		C				      4

	movq	24(%rsi), %rax		C				      4
	mulq	%rcx			C				      3
	movq	%r12, %r8		C				      3
	addq	%rax, %r11		C				      3
	adcq	%rdx, %r8		C				      3

	movq	%r10, 16(%rdi)		C				      4
	movq	%r11, 24(%rdi)		C				      4

	leaq	32(%rsi), %rsi		C				      4
	leaq	32(%rdi), %rdi		C				      4
	subq	$4, %rbx		C				      4
	jae	.Loop			C				      2

	cmpl	$-4, %ebx		C				      3
	jne	.Lend			C				      2

	movq	%r8, %rax		C				      3
	popq	%rbx			C				      1
	popq	%r12			C				      2
	ret				C				      1

.Lend:	movq	(%rsi), %rax		C				      3
	mulq	%rcx			C				      3
	movq	%r12, %r9		C				      3
	addq	%rax, %r8		C				      3
	adcq	%rdx, %r9		C				      3

	cmpl	$-3, %ebx		C				      3
	jne	.L1			C				      2

	movq	%r8, (%rdi)		C				      3
	movq	%r9, %rax		C				      3
	popq	%rbx			C				      1
	popq	%r12			C				      2
	ret				C				      1

.L1:	movq	8(%rsi), %rax		C				      4
	mulq	%rcx			C				      3
	movq	%r12, %r10		C				      3
	addq	%rax, %r9		C				      3
	adcq	%rdx, %r10		C				      3

	cmpl	$-2, %ebx		C				      3
	jne	.L2			C				      2

	movq	%r8, (%rdi)		C				      3
	movq	%r9, 8(%rdi)		C				      4
	movq	%r10, %rax		C				      3
	popq	%rbx			C				      1
	popq	%r12			C				      2
	ret				C				      1

.L2:	movq	16(%rsi), %rax		C				      4
	mulq	%rcx			C				      3
	movq	%r12, %r11		C				      3
	addq	%rax, %r10		C				      3
	adcq	%rdx, %r11		C				      3

	movq	%r8, (%rdi)		C				      3
	movq	%r9, 8(%rdi)		C				      4
	movq	%r10, 16(%rdi)		C				      4
	movq	%r11, %rax		C				      3
	popq	%rbx			C				      1
	popq	%r12			C				      2
	ret				C				      1
EPILOGUE()
