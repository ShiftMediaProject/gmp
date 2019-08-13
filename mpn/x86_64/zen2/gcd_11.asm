dnl  AMD64 mpn_gcd_11 optimised for AMD ZN2.

dnl  Based on the K7 gcd_1.asm, by Kevin Ryde.  Rehacked for AMD64 by Torbjorn
dnl  Granlund.

dnl  Copyright 2000-2002, 2005, 2009, 2011, 2012, 2017, 2019 Free Software
dnl  Foundation, Inc.

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


C	     cycles/bit (approx)
C AMD K8,K9	 -
C AMD K10	 -
C AMD bd1	 -
C AMD bd2	 -
C AMD bd3	 -
C AMD bd4	 3.07
C AMD bt1	 -
C AMD bt2	 -
C AMD zn1	 3.10
C AMD zn2	 3.17  *
C Intel P4	 -
C Intel CNR	 -
C Intel PNR	 -
C Intel NHM	 -
C Intel WSM	 -
C Intel SBR	 -
C Intel IBR	 -
C Intel HWL	 ?
C Intel BWL	 ?
C Intel SKL	 ?
C Intel atom	 -
C Intel SLM	 -
C Intel GLM	 -
C Intel GLM+	 -
C VIA nano	 -

define(`u0',    `%rdi')
define(`v0',    `%rsi')

ABI_SUPPORT(DOS64)
ABI_SUPPORT(STD64)

ASM_START()
	TEXT
	ALIGN(16)
PROLOGUE(mpn_gcd_11)
	FUNC_ENTRY(2)
	mov	u0, %r9
	jmp	L(ent)		C

	ALIGN(16)		C
L(top):	cmovc	%rax, u0	C u = |v - u|
	cmovc	%r9, v0		C v = min(u,v)
	rep;bsf	%rax, %rcx	C
	shrx(	%rcx, u0, %r9)	C
	shrx(	%rcx, u0, u0)	C
L(ent):	mov	v0, %rax	C
	sub	u0, v0		C v - u
	sub	%rax, u0	C u - v
	jz	L(end)		C

	cmovc	v0, u0		C u = |v - u|
	cmovc	%r9, %rax	C v = min(u,v)
	rep;bsf	v0, %rcx	C
	shrx(	%rcx, u0, %r9)	C
	shrx(	%rcx, u0, u0)	C
	mov	%rax, v0	C
	sub	u0, %rax	C v - u
	sub	v0, u0		C u - v
	jnz	L(top)		C

	mov	v0, %rax
L(end):	FUNC_EXIT()
	ret
EPILOGUE()
