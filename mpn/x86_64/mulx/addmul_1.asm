dnl  AMD64 mpn_addmul_1 for CPUs with mulx but without adx.

dnl  Copyright 2012, 2013 Free Software Foundation, Inc.

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

C	     cycles/limb
C AMD K8,K9	 -
C AMD K10	 -
C AMD bd1	 -
C AMD bobcat	 -
C Intel P4	 -
C Intel PNR	 -
C Intel NHM	 -
C Intel SBR	 -
C Intel HWL	 ?
C Intel BWL	 ?
C Intel atom	 -
C VIA nano	 -

define(`rp',      `%rdi')   C rcx
define(`up',      `%rsi')   C rdx
define(`n_param', `%rdx')   C r8
define(`v0_param',`%rcx')   C r9

define(`n',       `%rcx')
define(`v0',      `%rdx')

IFDOS(`	define(`up', ``%rsi'')	') dnl
IFDOS(`	define(`rp', ``%rcx'')	') dnl
IFDOS(`	define(`v0', ``%r9'')	') dnl
IFDOS(`	define(`r9', ``rdi'')	') dnl
IFDOS(`	define(`n',  ``%r8'')	') dnl
IFDOS(`	define(`r8', ``r11'')	') dnl

ASM_START()
	TEXT
	ALIGN(16)
PROLOGUE(mpn_addmul_1)
	mov	(up), %r8

	push	%rbx
	push	%r12
	push	%r13
	push	%r14

	lea	(up,n_param,8), up
	lea	-32(rp,n_param,8), rp
	mov	R32(n_param), R32(%rax)
	xchg	v0_param, v0		C FIXME: is this insn fast?

	neg	n

	and	$3, R8(%rax)
	jz	L(b0)
	cmp	$2, R8(%rax)
	jz	L(b2)
	jg	L(b3)

L(b1):	mulx	%r8, %rbx, %rax
	sub	$-1, n
	jz	L(wd1)
	mulx	(up,n,8), %r9, %r8
	mulx	8(up,n,8), %r11, %r10
	and	%rax, %rax	 		C clear cy
	jmp	L(lo1)

L(b0):	mulx	%r8, %r9, %r8
	mulx	8(up,n,8), %r11, %r10
	mulx	16(up,n,8), %r13, %r12
	xor	R32(%rax), R32(%rax)
	jmp	L(lo0)

L(b3):	mulx	%r8, %r11, %r10
	mulx	8(up,n,8), %r13, %r12
	mulx	16(up,n,8), %rbx, %rax
	add	%r10, %r13
	adc	%r12, %rbx
	adc	$0, %rax
	sub	$-3, n
	jz	L(wd3)
	and	%rax, %rax	 		C clear cy
	jmp	L(lo3)

L(b2):	mulx	%r8, %r13, %r12
	mulx	8(up,n,8), %rbx, %rax
	add	%r12, %rbx
	adc	$0, %rax
	sub	$-2, n
	jz	L(wd2)
	mulx	(up,n,8), %r9, %r8
	and	%rax, %rax	 		C clear cy
	jmp	L(lo2)

L(top):	add	%r9, (rp,n,8)
L(lo3):	mulx	(up,n,8), %r9, %r8
	adc	%r11, 8(rp,n,8)
L(lo2):	mulx	8(up,n,8), %r11, %r10
	adc	%r13, 16(rp,n,8)
L(lo1):	mulx	16(up,n,8), %r13, %r12
	adc	%rbx, 24(rp,n,8)
L(lo0):	mulx	24(up,n,8), %rbx, %r14
	adc	%rax, %r9
	adc	%r8, %r11
	adc	%r10, %r13
	adc	%r12, %rbx
	mov	$0, R32(%rax)
	adc	%r14, %rax		C rax = carry limb
	add	$4, n
	js	L(top)

L(end):	add	%r9, (rp)
L(wd3):	adc	%r11, 8(rp)
L(wd2):	adc	%r13, 16(rp)
L(wd1):	adc	%rbx, 24(rp)
	adc	n, %rax
	pop	%r14
	pop	%r13
	pop	%r12
	pop	%rbx
	ret
EPILOGUE()
ASM_END()
