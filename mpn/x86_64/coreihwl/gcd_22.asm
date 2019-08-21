dnl  AMD64 mpn_gcd_22.  Assumes useless bsf, useless shrd, useful tzcnt, shlx.

dnl  Copyright 2019 Free Software Foundation, Inc.

dnl  This file is part of the GNU MP Library.
dnl
dnl  The GNU MP Library is free software; you can redistribute it and/or modify
dnl  it under the terms of either:
dnl
dnl    * the GNU Lesser General Public License as published by the Free
dnl      Software Foundation; either version 3 of the License, or (at your
dnl      option) any later version.
dnl
dnl  or
dnl
dnl    * the GNU General Public License as published by the Free Software
dnl      Foundation; either version 2 of the License, or (at your option) any
dnl      later version.
dnl
dnl  or both in parallel, as here.
dnl
dnl  The GNU MP Library is distributed in the hope that it will be useful, but
dnl  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
dnl  for more details.
dnl
dnl  You should have received copies of the GNU General Public License and the
dnl  GNU Lesser General Public License along with the GNU MP Library.  If not,
dnl  see https://www.gnu.org/licenses/.

include(`../config.m4')


C	     cycles/bit
C AMD K8,K9	 -
C AMD K10	 -
C AMD bd1	 -
C AMD bd2	 -
C AMD bd3	 -
C AMD bd4	 6.8
C AMD bt1	 -
C AMD bt2	 -
C AMD zn1	 5.7
C AMD zn2	 5.9
C Intel P4	 -
C Intel CNR	 -
C Intel PNR	 -
C Intel NHM	 -
C Intel WSM	 -
C Intel SBR	 -
C Intel IBR	 -
C Intel HWL	 7.4
C Intel BWL	 6.1
C Intel SKL	 6.3
C Intel atom	 -
C Intel SLM	 -
C Intel GLM	 -
C Intel GLM+	 -
C VIA nano	 -


define(`u1',    `%rdi')
define(`u0',    `%rsi')
define(`v1',    `%rdx')
define(`v0_param', `%rcx')

define(`v0',    `%rax')
define(`cnt',   `%rcx')

define(`s0',    `%r8')
define(`s1',    `%r9')
define(`t0',    `%r10')
define(`t1',    `%r11')

ABI_SUPPORT(DOS64)
ABI_SUPPORT(STD64)

ASM_START()
	TEXT
	ALIGN(16)
PROLOGUE(mpn_gcd_22)
	FUNC_ENTRY(2)
	mov	v0_param, v0

	ALIGN(16)
L(top):	mov	v0, t0
	sub	u0, t0
	jz	L(lowz)		C	jump when low limb result = 0
	mov	v1, t1
	sbb	u1, t1

	mov	u0, s0
	mov	u1, s1

	rep;bsf	t0, cnt		C tzcnt!

	sub	v0, u0
	sbb	v1, u1

L(bck):	cmovc	t0, u0		C u = |u - v|
	cmovc	t1, u1		C u = |u - v|
	cmovc	s0, v0		C v = min(u,v)
	cmovc	s1, v1		C v = min(u,v)

	xor	R32(t0), R32(t0)
	sub	cnt, t0
	shrx(	cnt, u0, u0)
	shlx(	t0, u1, t0)
	or	t0, u0
	shr	R8(cnt), u1
	jnz	L(top)

	test	v1, v1
	jnz	L(top)

L(gcd_11):
	mov	v0, %rdi
	mov	u0, %rsi
	xor	R32(%rdx), R32(%rdx)
	jmp	mpn_gcd_11

L(lowz):C We come here when v0 - u0 = 0
	C 1. If v1 - u1 = 0, then gcd is u = v.
	C 2. Else compute gcd_21({v1,v0}, |u1-v1|)
	mov	v1, t0
	sub	u1, t0
	je	L(end)
	mov	$0, t1

	mov	u0, s0
	mov	u1, s1

	rep;bsf	t0, cnt		C tzcnt!

	mov	u1, u0
	sub	v1, u0
	mov	$0, u1

	jmp	L(bck)

L(end):	C mov	v0, %rax
	C mov	v1, %rdx
	FUNC_EXIT()
	ret
EPILOGUE()
