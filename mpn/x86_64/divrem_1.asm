dnl  AMD64 mpn_divrem_1 -- mpn by limb division.

dnl  Copyright 2004, 2005 Free Software Foundation, Inc.

dnl  This file is part of the GNU MP Library.

dnl  The GNU MP Library is free software; you can redistribute it and/or modify
dnl  it under the terms of the GNU Lesser General Public License as published
dnl  by the Free Software Foundation; either version 3 of the License, or (at
dnl  your option) any later version.

dnl  The GNU MP Library is distributed in the hope that it will be useful, but
dnl  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
dnl  License for more details.

dnl  You should have received a copy of the GNU Lesser General Public License
dnl  along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.

include(`../config.m4')

C	   integer part    fraction part
C	    cycles/limb     cycles/limb
C K8,K9:	17		15
C K10:		17		15
C P4:		83		57
C P6-15 (Core2):32		21
C P6-28 (Atom):	70		39


C mp_limb_t mpn_divrem_1 (mp_ptr qp, mp_size_t fn,
C                         mp_srcptr np, mp_size_t nn, mp_limb_t d);

C TODO
C  * Address FIXME below about leading quotient limbs optimization
C  * Optimize by doing some software pipelining and local scheduling
C  * Optimize by using mmx instructions for shifting
C  * Perhaps compute the inverse without relying on the 71 cycle divq?  Could
C    either use Newton's method and mulq, or perhaps the faster fdiv.
C  * Perhaps handle nn+fn < 3 using divq directly (if divq is ever to be used).

C mp_limb_t
C mpn_divrem_1 (mp_ptr qp, mp_size_t fn,
C		mp_srcptr np, mp_size_t nn, mp_limb_t d)

C mp_limb_t
C mpn_preinv_divrem_1 (mp_ptr qp, mp_size_t fn,
C		       mp_srcptr np, mp_size_t nn, mp_limb_t d,
C		       mp_limb_t dinv, int shift)

C INPUT PARAMETERS
define(`qp',`%rdi')
define(`fn',`%rsi')
define(`np_param',`%rdx')
define(`nn_param',`%rcx')
define(`d',`%r8')
define(`dinv',`%r9')
C shift %r10 FIXME
define(`np',`%r11')
define(`nn',`%rbp')
define(`rem',`%r10')


ASM_START()
	TEXT
	ALIGN(16)
PROLOGUE(mpn_preinv_divrem_1)
C	pushq	%r15
	pushq	%r14
C	pushq	%r13
	pushq	%r12
	pushq	%rbp
	pushq	%rbx

	movq	np_param, np
	movq	nn_param, nn
	addq	fn, nn_param
	je	L(ret0)			C we're done if both nn and fn are zero

	leaq	(qp,fn,8), qp		C point qp at least sign. integral limb

	movq	40(%rsp), %rcx
	salq	%cl, d			C d = normalized divisor

	jmp	L(comm)
L(ret0):
	xorl	%eax, %eax
	jmp	L(ret)
EPILOGUE()

	ALIGN(16)
PROLOGUE(mpn_divrem_1)

C	pushq	%r15
	pushq	%r14
C	pushq	%r13
	pushq	%r12
	pushq	%rbp
	pushq	%rbx

	movq	np_param, np
	movq	nn_param, nn
	addq	fn, nn_param
	je	L(ret0)			C we're done if both nn and fn are zero

	leaq	(qp,fn,8), qp		C point qp at least sign. integral limb

	bsrq	d,%rcx
	xorl	$63, %ecx		C %ecx = count_leading_zeros (divisor)

	movq	$-1, %rdx
	salq	%cl, d			C d = normalized divisor

	movq	%rdx, %rax
	subq	d, %rdx
	divq	d
	movq	%rax, dinv		C dinv = invert_limb (d)
L(comm):
	xorl	%ebx, %ebx
	xorl	%eax, %eax

	testq	nn, nn
	je	L(frac)			C no integer quotient limbs
	movq	-8(np,nn,8), %r14
	shldq	%cl, %r14, %rax
C FIXME: Should compare rem and d here, and conditionally generate a quotient
C FIXME: limb for the right outcome
	subq	$2, nn
	js	L(intlast)
	ALIGN(64)
L(intloop):
	movq	(np,nn,8), %rdx
	shldq	%cl, %rdx, %r14		C n10
	leaq	(d,%r14), %r12
	btq	$63, %r14
	cmovnc	%r14, %r12
	movq	%rax, rem
	adcq	$0, %rax

	mulq	dinv			C %rax = dinv * n

	addq	%r12, %rax
	movq	d, %rax
	adcq	rem, %rdx
	notq	%rdx
	movq	%rdx, %r12

	mulq	%rdx

	addq	%rax, %r14
	adcq	%rdx, rem
	subq	d, rem			C 00..00 or 11.11
	leaq	(d,%r14), %rax		C remainder
	cmovnc	%r14, %rax		C remainder
	subq	%r12, rem		C quotient
	movq	(np,nn,8), %r14
	movq	rem, 8(qp,nn,8)
	decq	nn
	jns	L(intloop)
L(intlast):
	salq	%cl, %r14
	leaq	(d,%r14), %r12
	btq	$63, %r14
	cmovnc	%r14, %r12
	movq	%rax, rem
	adcq	$0, %rax

	mulq	dinv

	addq	%r12, %rax
	movq	d, %rax
	adcq	rem, %rdx
	notq	%rdx
	movq	%rdx, %r12

	mulq	%rdx

	addq	%rax, %r14
	adcq	%rdx, rem
	subq	d, rem
	leaq	(d,%r14), %rax		C remainder
	cmovnc	%r14, %rax		C remainder
	subq	%r12, rem
	movq	rem, (qp)

L(frac):
	leaq	(,fn,8), %rdx
	subq	%rdx, qp
	decq	fn
	js	L(fracskip)
	ALIGN(4)
L(fracloop):
	movq	%rax, rem

	mulq	dinv

	movq	d, %rax
	addq	rem, %rdx
	notq	%rdx
	movq	%rdx, %r12

	mulq	%rdx

	addq	rem, %rdx
	subq	d, %rdx
	leaq	(d,%rax), %r14
	cmovc	%r14, %rax		C remainder
	subq	%r12, %rdx
	movq	%rdx, (qp,fn,8)
	decq	fn
	jns	L(fracloop)
L(fracskip):
	shrq	%cl, %rax
L(ret):
	popq	%rbx
	popq	%rbp
	popq	%r12
C	popq	%r13
	popq	%r14
C	popq	%r15
	ret
EPILOGUE()
