dnl  AMD64 mpn_add_n -- Add two limb vectors of the same length > 0 and store
dnl  sum in a third limb vector.

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
C Hammer:     1.75


C INPUT PARAMETERS
C rp	rdi
C up	rsi
C vp	rdx
C n	rcx

	TEXT
	ALIGN(16)
ASM_START()
PROLOGUE(mpn_add_n)
	pushq	%rbx			C				1
	movq	%rcx, %r8		C				3
	shrq	$2, %rcx		C				4
	je	.Lend			C				2
	andl	$3, %r8d		C				4
	.byte	0x66, 0x90		C				2

C Main loop.  16-byte aligned.  Blocks between blank lines take one cycle.
.Loop:	movq	(%rsi), %rax		C				3
	movq	8(%rsi), %rbx		C				4
	leaq	32(%rsi), %rsi		C				4

	adcq	(%rdx), %rax		C				3
	movq	%rax, (%rdi)		C				3

	adcq	8(%rdx), %rbx		C				4
	movq	%rbx, 8(%rdi)		C				4

	movq	-16(%rsi), %rax		C				4
	movq	-8(%rsi), %rbx		C				4

	adcq	16(%rdx), %rax		C				4
	movq	%rax, 16(%rdi)		C				4

	adcq	24(%rdx), %rbx		C				4
	movq	%rbx, 24(%rdi)		C				4
	decq	%rcx			C				3

	leaq	32(%rdx), %rdx		C				4
	leaq	32(%rdi), %rdi		C				4
	jne	.Loop			C				2


	incl	%r8d			C				3
	decl	%r8d			C				3
	jne	.L0			C				2

	sbbl	%eax,%eax		C				2
	negl	%eax			C				2
	popq	%rbx			C				1
	ret				C				1

.Lend:	testl	%eax, %eax		C clear cy			2
.L0:	decl	%r8d			C				3
	jne	.L1			C				2

	movq	(%rsi), %rax		C				3
	adcq	(%rdx), %rax		C				3
	movq	%rax, (%rdi)		C				3
	sbbl	%eax,%eax		C				2
	negl	%eax			C				2
	popq	%rbx			C				1
	ret				C				1

.L1:	decl	%r8d			C				3
	jne	.L2			C				2

	movq	(%rsi), %rax		C				3
	movq	8(%rsi), %rbx		C				4
	adcq	(%rdx), %rax		C				3
	movq	%rax, (%rdi)		C				3
	adcq	8(%rdx), %rbx		C				4
	movq	%rbx, 8(%rdi)		C				4
	sbbl	%eax,%eax		C				2
	negl	%eax			C				2
	popq	%rbx			C				1
	ret				C				1

.L2:
	movq	(%rsi), %rax		C				3
	movq	8(%rsi), %rbx		C				4
	adcq	(%rdx), %rax		C				3
	movq	%rax, (%rdi)		C				3
	adcq	8(%rdx), %rbx		C				4
	movq	%rbx, 8(%rdi)		C				4
	movq	16(%rsi), %rax		C				4
	adcq	16(%rdx), %rax		C				4
	movq	%rax, 16(%rdi)		C				4
	sbbl	%eax,%eax		C				2
	negl	%eax			C				2
	popq	%rbx			C				1
	ret				C				1
EPILOGUE()
