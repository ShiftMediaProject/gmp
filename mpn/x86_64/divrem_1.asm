dnl  AMD64 mpn_divrem_1 -- mpn by limb division.

dnl  Copyright 2004 Free Software Foundation, Inc.

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

C         integer part  fraction part
C         cycles/limb   cycles/limb
C Hammer:     17            16


C mp_limb_t mpn_divrem_1 (mp_ptr qp, mp_size_t fn,
C                         mp_srcptr np, mp_size_t nn, mp_limb_t d);

C TODO
C  * Implement mpn_preinv_divrem_1 entry point
C  * Address FIXME below about leading quotient limbs optimization
C  * Optimize by doing some software pipelining and local scheduling
C  * Optimize by using mmx instructions for shifting


C INPUT PARAMETERS
define(`qp',`%rdi')
define(`fn',`%rsi')
define(`np_param',`%rdx')
define(`nn_param',`%rcx')
define(`d',`%r8')

define(`dinv',`%r10')
define(`np',`%r13')
define(`nn',`%r9')
define(`mask',`%r11')
define(`rem',`%rbp')


	TEXT
	ALIGN(16)
ASM_START()
PROLOGUE(mpn_divrem_1)

C	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbp
	pushq	%rbx

C Form mask for suppressing invalid right shift when msb of divisor is set
	movq	d, mask
	sarq	$63, mask
	notq	mask

	movq	np_param, np
	movq	nn_param, nn
	movl	$0, %ebp		C clear remainder
	leaq	(fn,%rcx), %rax
	testq	%rax, %rax
	je	L(ret)			C we're done if both nn and fn are zero

	leaq	(qp,fn,8), qp		C point qp at least sign. integral limb

	bsrq	d,%rcx
	xorl	$63, %ecx		C %ecx = count_leading_zeros (divisor)

	salq	%cl, d			C d = normalized divisor

C dinv = invert_limb (d)
	leaq	(d,d), %rax
	movq	$-1, dinv
	testq	%rax, %rax
	je	L(1)
	movl	$0, %eax
	movq	d, %rdx
	negq	%rdx
	divq	d
	movq	%rax, dinv
L(1):

	testq	nn, nn
	je	L(frac)			C no integer quotient limbs
	movq	-8(np,nn,8), %rax
	negl	%ecx
	movq	%rax, rem
	shrq	%cl, rem
C FIXME: Should compare rem and d here, and conditionally generate a quotient
C FIXME: limb for the right outcome
	andq	mask, rem
	negl	%ecx
	subq	$2, nn
	js	L(intlast)

L(intloop):
	movq	(np,nn,8), %r14
	salq	%cl, %rax
	negl	%ecx
	shrq	%cl, %r14
	andq	mask, %r14		C conditionally zero %r14
	leaq	(%r14,%rax), %r14
	negl	%ecx
	shrq	$63, %rax
	leaq	(d,%r14), %r12
	cmovz	%r14, %r12
	addq	rem, %rax

	mulq	dinv

	addq	%r12, %rax
	movq	d, %rax
	adcq	rem, %rdx
	notq	%rdx
	movq	%rdx, %r12

	mulq	%rdx

	addq	%r14, %rax
	adcq	rem, %rdx
	subq	d, %rdx			C 00..00 or 11.11
	leaq	(d,%rax), rem		C remainder
	cmovz	%rax, rem		C remainder
	subq	%r12, %rdx		C quotient
	movq	%rdx, 8(qp,nn,8)
	movq	(np,nn,8), %rax
	decq	nn
	jns	L(intloop)
L(intlast):
	salq	%cl, %rax
	movq	%rax, %r14
	shrq	$63, %rax
	leaq	(d,%r14), %r12
	cmovz	%r14, %r12
	addq	rem, %rax

	mulq	dinv

	addq	%r12, %rax
	movq	d, %rax
	adcq	rem, %rdx
	notq	%rdx
	movq	%rdx, %r12

	mulq	%rdx

	addq	%r14, %rax
	adcq	rem, %rdx
	subq	d, %rdx
	leaq	(d,%rax), rem		C remainder
	cmovz	%rax, rem		C remainder
	subq	%r12, %rdx
	movq	%rdx, (qp)

L(frac):
	leaq	0(,fn,8), %rax
	subq	%rax, qp
	decq	fn
	js	L(fracskip)
L(fracloop):
	movq	rem, %rax

	mulq	dinv

	movq	d, %rax
	addq	rem, %rdx
	notq	%rdx
	movq	%rdx, %r12

	mulq	%rdx

	addq	rem, %rdx
	subq	d, %rdx
	leaq	(d,%rax), rem		C remainder
	cmovz	%rax, rem		C remainder
	subq	%r12, %rdx
	movq	%rdx, (qp,fn,8)
	decq	fn
	jns	L(fracloop)
L(fracskip):
	shrq	%cl, rem
L(ret):
	movq	rem, %rax
	popq	%rbx
	popq	%rbp
	popq	%r12
	popq	%r13
	popq	%r14
C	popq	%r15
	ret
EPILOGUE()
