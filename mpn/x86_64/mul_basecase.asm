dnl  AMD64 mpn_mul_basecase -- multiply two mpn numbers.

dnl  Copyright 2003, 2004, 2005 Free Software Foundation, Inc.
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
dnl  not, write to the Free Software Foundation, Inc., 51 Franklin Street,
dnl  Fifth Floor, Boston, MA 02110-1301, USA.

include(`../config.m4')


C K8: 3.25 cycles/limb (3.62 cycles/limb at 20x20 limbs).

C TODO
C  * Microoptimize outside of loops for speed and better ALIGN-ment
C  * Perhaps reorg .Lc code snippets.
C  * Consider changing offsets, thus avoiding the initial leaq's

C INPUT PARAMETERS
define(`rp',	`%rdi')
define(`up',	`%rsi')
define(`un',	`%rdx')
define(`vp',	`%rcx')
define(`vn',	`%r8')

ASM_START()
PROLOGUE(mpn_mul_basecase)
	pushq	%rbp
	pushq	%r12
	movq	(vp), %r12		C load vp[0]
	addq	$8, vp			C vp++
	leaq	(up,un,8), up		C
	leaq	(rp,un,8), rp		C
	movq	un, %rbp
	xorl	%r10d, %r10d		C keep r10 zero
	negq	%rbp
	andl	$3, %edx
	je	.Lb00
	cmpl	$1, %edx
	je	.Lb01
	cmpl	$2, %edx
	je	.Lb10

.Lb11:	leaq	2(%rbp), %r11		C inner-loop count
	movq	(up,%rbp,8), %rax
	mulq	%r12
	movq	%rax, (rp,%rbp,8)
	movq	%rdx, %r9
	jmp	.Lm11

	ALIGN(32)
.Llm11:	movq	-24(up,%r11,8), %rax
	mulq	%r12
	nop
	nop
	addq	%r9, %rax
	movq	%rax, -24(rp,%r11,8)
	movq	%r10, %r9
	adcq	%rdx, %r9
	movq	-16(up,%r11,8), %rax
	mulq	%r12
	nop
	nop
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, -16(rp,%r11,8)
	adcq	%rdx, %r9
.Lm11:	movq	-8(up,%r11,8), %rax
	mulq	%r12
	nop
	nop
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, -8(rp,%r11,8)
	adcq	%rdx, %r9
	movq	(up,%r11,8), %rax
	mulq	%r12
	nop
	nop
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, (rp,%r11,8)
	adcq	%rdx, %r9
	addq	$4, %r11
	jae	.Llm11
	jmp	.Lc11

.Lo11:	movq	(%rcx), %r12
	leaq	2(%rbp), %r11		C inner-loop count
	addq	$8, %rcx
	movq	(up,%rbp,8), %rax
	mulq	%r12
	addq	(rp,%rbp,8), %rax
	adcq	%r10, %rdx
	movq	%rax, (rp,%rbp,8)
	movq	%rdx, %r9
	jmp	.Ln11

	ALIGN(8)
.Li11:	movq	-24(up,%r11,8), %rax
	mulq	%r12
	addq	-24(rp,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%rax, -24(rp,%r11,8)
	movq	%r10, %r9
	adcq	%rdx, %r9
	movq	-16(up,%r11,8), %rax
	mulq	%r12
	addq	-16(rp,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, -16(rp,%r11,8)
	adcq	%rdx, %r9
.Ln11:	movq	-8(up,%r11,8), %rax
	mulq	%r12
	addq	-8(rp,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, -8(rp,%r11,8)
	adcq	%rdx, %r9
	movq	(up,%r11,8), %rax
	mulq	%r12
	addq	(rp,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, (rp,%r11,8)
	adcq	%rdx, %r9
	addq	$4, %r11
	jae	.Li11

.Lc11:	movq	%r9, (rp)
	addq	$8, rp
	decq	%r8
	jne	.Lo11

	popq	%r12
	popq	%rbp
	ret


	ALIGN(8)
.Lb01:	leaq	4(%rbp), %r11		C inner-loop count
	movq	(up,%rbp,8), %rax
	mulq	%r12
	movq	%rax, (rp,%rbp,8)
	movq	%rdx, %r9

	testq	%r11, %r11
	jns	.Lc01			C FIXME: Special exit for un=vn=1?
	jmp	.Llm01			C avoid executing alignment nops

	ALIGN(32)
.Llm01:	movq	-24(up,%r11,8), %rax
	mulq	%r12
	nop
	nop
	addq	%r9, %rax
	movq	%rax, -24(rp,%r11,8)
	movq	%r10, %r9
	adcq	%rdx, %r9
	movq	-16(up,%r11,8), %rax
	mulq	%r12
	nop
	nop
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, -16(rp,%r11,8)
	adcq	%rdx, %r9
	movq	-8(up,%r11,8), %rax
	mulq	%r12
	nop
	nop
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, -8(rp,%r11,8)
	adcq	%rdx, %r9
	movq	(up,%r11,8), %rax
	mulq	%r12
	nop
	nop
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, (rp,%r11,8)
	adcq	%rdx, %r9
	addq	$4, %r11
	jae	.Llm01
	jmp	.Lc01

	ALIGN(8)
.Lo01:	movq	(%rcx), %r12
	leaq	4(%rbp), %r11		C inner-loop count
	addq	$8, %rcx
	movq	(up,%rbp,8), %rax
	mulq	%r12
	addq	(rp,%rbp,8), %rax
	adcq	%r10, %rdx
	movq	%rax, (rp,%rbp,8)
	movq	%rdx, %r9

C	testq	%r11, %r11
C	jns	.Lc01			C FIXME: jumps to false condition

	ALIGN(8)
.Li01:	movq	-24(up,%r11,8), %rax
	mulq	%r12
	addq	-24(rp,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%rax, -24(rp,%r11,8)
	movq	%r10, %r9
	adcq	%rdx, %r9
	movq	-16(up,%r11,8), %rax
	mulq	%r12
	addq	-16(rp,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, -16(rp,%r11,8)
	adcq	%rdx, %r9
	movq	-8(up,%r11,8), %rax
	mulq	%r12
	addq	-8(rp,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, -8(rp,%r11,8)
	adcq	%rdx, %r9
	movq	(up,%r11,8), %rax
	mulq	%r12
	addq	(rp,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, (rp,%r11,8)
	adcq	%rdx, %r9
	addq	$4, %r11
	jae	.Li01

.Lc01:	movq	%r9, (rp)
	addq	$8, rp
	decq	%r8
	jne	.Lo01

	popq	%r12
	popq	%rbp
	ret


	ALIGN(8)
.Lb10:	leaq	1(%rbp), %r11		C inner-loop count
	movq	(up,%rbp,8), %rax
	mulq	%r12
	movq	%rax, (rp,%rbp,8)
	movq	%rdx, %r9
	jmp	.Lm10

	ALIGN(32)
.Llm10:	movq	-24(up,%r11,8), %rax
	mulq	%r12
	nop
	nop
	addq	%r9, %rax
	movq	%rax, -24(rp,%r11,8)
	movq	%r10, %r9
	adcq	%rdx, %r9
	movq	-16(up,%r11,8), %rax
	mulq	%r12
	nop
	nop
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, -16(rp,%r11,8)
	adcq	%rdx, %r9
	movq	-8(up,%r11,8), %rax
	mulq	%r12
	nop
	nop
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, -8(rp,%r11,8)
	adcq	%rdx, %r9
.Lm10:	movq	(up,%r11,8), %rax
	mulq	%r12
	nop
	nop
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, (rp,%r11,8)
	adcq	%rdx, %r9
	addq	$4, %r11
	jae	.Llm10
	jmp	.Lc10

.Lo10:	movq	(%rcx), %r12
	leaq	1(%rbp), %r11		C inner-loop count
	addq	$8, %rcx
	movq	(up,%rbp,8), %rax
	mulq	%r12
	addq	(rp,%rbp,8), %rax
	adcq	%r10, %rdx
	movq	%rax, (rp,%rbp,8)
	movq	%rdx, %r9
	jmp	.Ln10

	ALIGN(8)
.Li10:	movq	-24(up,%r11,8), %rax
	mulq	%r12
	addq	-24(rp,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%rax, -24(rp,%r11,8)
	movq	%r10, %r9
	adcq	%rdx, %r9
	movq	-16(up,%r11,8), %rax
	mulq	%r12
	addq	-16(rp,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, -16(rp,%r11,8)
	adcq	%rdx, %r9
	movq	-8(up,%r11,8), %rax
	mulq	%r12
	addq	-8(rp,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, -8(rp,%r11,8)
	adcq	%rdx, %r9
.Ln10:	movq	(up,%r11,8), %rax
	mulq	%r12
	addq	(rp,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, (rp,%r11,8)
	adcq	%rdx, %r9
	addq	$4, %r11
	jae	.Li10

.Lc10:	movq	%r9, (rp)
	addq	$8, rp
	decq	%r8
	jne	.Lo10

	popq	%r12
	popq	%rbp
	ret


	ALIGN(8)
.Lb00:	leaq	3(%rbp), %r11		C inner-loop count
	xorl	%r9d, %r9d		C clear carry limb
	jmp	.Llm00			C avoid executing alignment nops

	ALIGN(32)
.Llm00:	movq	-24(up,%r11,8), %rax
	mulq	%r12
	nop
	nop
	addq	%r9, %rax
	movq	%rax, -24(rp,%r11,8)
	movq	%r10, %r9
	adcq	%rdx, %r9
	movq	-16(up,%r11,8), %rax
	mulq	%r12
	nop
	nop
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, -16(rp,%r11,8)
	adcq	%rdx, %r9
	movq	-8(up,%r11,8), %rax
	mulq	%r12
	nop
	nop
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, -8(rp,%r11,8)
	adcq	%rdx, %r9
	movq	(up,%r11,8), %rax
	mulq	%r12
	nop
	nop
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, (rp,%r11,8)
	adcq	%rdx, %r9
	addq	$4, %r11
	jae	.Llm00
	jmp	.Lc00

.Lo00:	movq	(%rcx), %r12
	leaq	3(%rbp), %r11		C inner-loop count
	addq	$8, %rcx
	xorl	%r9d, %r9d		C clear carry limb

	ALIGN(8)
.Li00:	movq	-24(up,%r11,8), %rax
	mulq	%r12
	addq	-24(rp,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%rax, -24(rp,%r11,8)
	movq	%r10, %r9
	adcq	%rdx, %r9
	movq	-16(up,%r11,8), %rax
	mulq	%r12
	addq	-16(rp,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, -16(rp,%r11,8)
	adcq	%rdx, %r9
	movq	-8(up,%r11,8), %rax
	mulq	%r12
	addq	-8(rp,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, -8(rp,%r11,8)
	adcq	%rdx, %r9
	movq	(up,%r11,8), %rax
	mulq	%r12
	addq	(rp,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, (rp,%r11,8)
	adcq	%rdx, %r9
	addq	$4, %r11
	jae	.Li00

.Lc00:	movq	%r9, (rp)
	addq	$8, rp
	decq	%r8
	jne	.Lo00

	popq	%r12
	popq	%rbp
	ret
EPILOGUE()
