dnl  AMD64 mpn_mod_34lsub1 -- remainder modulo 2^24-1.

dnl  Copyright 2000, 2001, 2002, 2004, 2005 Free Software Foundation, Inc.
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


C          cycles/limb
C Athlon64:    1.0
C Pentium4:    3.7

C INPUT PARAMETERS
C up	rdi
C n	rsi

C mp_limb_t mpn_mod_34lsub1 (mp_srcptr up, mp_size_t n)

C TODO
C  * Apply the movzwl tricks to the x86/k7 code
C  * Review feed-in and wind-down code.  In particular, try to avoid adcq and
C    sbbq to placate Pentium4.
C  * More unrolling and/or index addressing could bring time to under 1 c/l
C    for Athlon64, approaching 0.67 c/l seems possible.
C  * There are recurrencies on the carry registers (r8, r9, r10) that might
C    be the limiting factor for the Pentium4 speed.  Splitting these into 6
C    registers would help.
C  * For ultimate Athlon64 performance, a sequence like this might be best.
C    It should reach 0.5 c/l (limited by L1 cache bandwidth).
C
C	addq	(%rdi), %rax
C	adcq	8(%rdi), %rcx
C	adcq	16(%rdi), %rdx
C	adcq	$0, %r8
C	addq	24(%rdi), %rax
C	adcq	32(%rdi), %rcx
C	adcq	40(%rdi), %rdx
C	adcq	$0, %r8
C	...



	TEXT
	ALIGN(32)
PROLOGUE(mpn_mod_34lsub1)

	movq	$0x0000FFFFFFFFFFFF, %r11

	subq	$2, %rsi
	ja	L(gt2)

	movq	(%rdi), %rax
	jb	L(1)

	movq	8(%rdi), %rsi
	movq	%rax, %rdx
	shrq	$48, %rax		C src[0] low

	andq	%r11, %rdx		C src[0] high
	addq	%rdx, %rax
	movl	%esi, %edx

	shrq	$32, %rsi		C src[1] high
	addq	%rsi, %rax

	shlq	$16, %rdx		C src[1] low
	addq	%rdx, %rax

L(1):	ret


L(gt2):	xorl	%eax, %eax
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	xorq	%r8, %r8
	xorq	%r9, %r9
	xorq	%r10, %r10

L(top):	addq	(%rdi), %rax
	adcq	$0, %r10
	addq	8(%rdi), %rcx
	adcq	$0, %r8
	addq	16(%rdi), %rdx
	adcq	$0, %r9

	leaq	48(%rdi), %rdi

	subq	$3,%rsi
	jng	L(end)

	addq	-24(%rdi), %rax
	adcq	$0, %r10
	addq	-16(%rdi), %rcx
	adcq	$0, %r8
	addq	-8(%rdi), %rdx
	adcq	$0, %r9

	subq	$3,%rsi
	jg	L(top)

	addq	$24, %rdi


L(end):	addq	%r9, %rax
	adcq	%r10, %rcx
	adcq	%r8, %rdx

	incq	%rsi
	movq	$0xFFFFFFFFFFFFFFFF, %r10
	js	L(combine)

	movq	$0xFFFFFFFFFFFF0000, %r10
	adcq	-24(%rdi), %rax
	decq	%rsi
	js	L(combine)

	adcq	-16(%rdi), %rcx
	movq	$0xFFFFFFFF00000000, %r10

L(combine):
	sbbq	%rsi, %rsi		C carry
	movq	%rax, %rdi		C 0mod3
	shrq	$48, %rax		C 0mod3 high

	andq	%r10, %rsi		C carry masked
	andq	%r11, %rdi		C 0mod3 low
	movl	%ecx, %r10d		C 1mod3

	subq	%rsi, %rax		C apply carry
	shrq	$32, %rcx		C 1mod3 high

	addq	%rdi, %rax		C apply 0mod3 low
	movzwl	%dx, %edi		C 2mod3
	shlq	$16, %r10		C 1mod3 low

	addq	%rcx, %rax		C apply 1mod3 high
	shrq	$16, %rdx		C 2mod3 high

	addq	%r10, %rax		C apply 1mod3 low
	shlq	$32, %rdi		C 2mod3 low

	addq	%rdx, %rax		C apply 2mod3 high
	addq	%rdi, %rax		C apply 2mod3 low

	ret
EPILOGUE()
