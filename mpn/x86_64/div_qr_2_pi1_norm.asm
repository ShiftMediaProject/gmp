dnl  x86-64 mpn_divrem_2 -- Divide an mpn number by a normalized 2-limb number.

dnl  Copyright 2011 Free Software Foundation, Inc.

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


C		c/l
C INPUT PARAMETERS
define(`qp',		`%rdi')
define(`up',		`%rsi')
define(`un_param',	`%rdx')
define(`d1',		`%rcx')
define(`d0',		`%r8')
define(`di', 		`%r9')

define(`un',		`%r10')
define(`u2',		`%r11')
define(`u1',		`%r12')
define(`t1',		`%r13')
define(`t0',		`%r14')
	
ASM_START()
	TEXT
	ALIGN(16)
PROLOGUE(mpn_div_qr_2_pi1_norm)
	push	%r14
	push	%r13
	push	%r12

	mov	-16(up, un_param, 8), u1
	mov	-8(up, un_param, 8), u2

	mov	u1, t0
	mov	u2, t1
	sub	d0, t0
	sbb	d1, t1
	cmovnc  t0, u1
	cmovnc	t1, u2
	C push qh which is !carry
	sbb	%rax, %rax
	inc	%rax
	push	%rax
	lea	-2(un_param), un
	jmp	L(next)

	ALIGN(16)
L(loop):
	C udiv_qr_3by2 (q,u2,u1,u2,u1,n0, d1,d0,di)
	mov	di, %rax
	mul	u2
	mov	u2, t1		C q in t1
	mov	u1, u2		C Form new u2
	mov	u1, t0		C q0 in t0
	add	%rax, t0
	adc	%rdx, t1
	mov	d1, %rax
	imul	t1, %rax
	sub	%rax, u2
	mov	(up, un, 8), u1
	sub	d0, u1
	sbb	d1, u2
	mov	d0, %rax
	mul	t1
	add	$1, t1
	sub	%rax, u1
	sbb	%rdx, u2

	C Adjustment, mask = - (u2 >= q0) */
	stc	C Need to subtract u2 + 1
	sbb	u2, t0
	sbb	t0, t0
	add	t0, t1
	mov	d1, %rax
	and	t0, %rax
	and	d0, t0
	add	t0, u1
	adc	%rax, u2
	cmp	d1, u2
	jnc	L(fix)
L(ok):
	mov	t1, (qp, un, 8)
	
L(next):
	sub	$1, un
	jnc	L(loop)
L(end):
	mov	u2, 8(up)
	mov	u1, (up)

	C qh on stack
	pop	%rax

	pop	%r12
	pop	%r13
	pop	%r14
	ret

L(fix):	C Unlikely update. u2 >= d1	
	jne	L(adjust)	C strict inequality
	C Now u2 == d1
	cmp	d0, u1
	jc	L(ok)
L(adjust):
	add	$1, t1
	sub	d0, u1
	sbb	d1, u2
	jmp 	L(ok)
EPILOGUE()
