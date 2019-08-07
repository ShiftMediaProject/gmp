dnl  AMD64 mpn_gcd_11 optimised for Intel C2, NHM, SBR and AMD K10, BD1.

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
C AMD K8,K9	 8.50
C AMD K10	 4.30
C AMD bd1	 5.00
C AMD bd2	 ?
C AMD bd3	 ?
C AMD bd4	 ?
C AMD bt1	10.0
C AMD bt2	 ?
C AMD zn1	 ?
C AMD zn2	 ?
C Intel P4	18.6
C Intel core2	 3.83
C Intel NHM	 5.17
C Intel SBR	 4.69
C Intel IBR	 ?
C Intel HWL	 ?
C Intel BWL	 ?
C Intel SKL	 ?
C Intel atom	17.0
C Intel SLM	 ?
C Intel GLM	 ?
C Intel GLM+	 ?
C VIA nano	 5.44
C Numbers measured with: speed -CD -s16-64 -t48 mpn_gcd_1

define(`u0',    `%rdi')
define(`v0',    `%rsi')

ABI_SUPPORT(DOS64)
ABI_SUPPORT(STD64)

ASM_START()
	TEXT
	ALIGN(16)
PROLOGUE(mpn_gcd_11)
	FUNC_ENTRY(2)
	mov	v0, %r10	C
	sub	u0, %r10	C
	jz	L(end)		C

	ALIGN(16)		C              K10 BD1 CNR NHM SBR
L(top):	bsf	%r10, %rcx	C              3   3   6   5   5
	mov	u0, %r9		C              2   2   3   3   4
	sub	v0, u0		C              2   2   4   3   4
	cmovc	%r10, u0	C if x-y < 0   0,3 0,3 0,6 0,5 0,5
	cmovc	%r9, v0		C use x,y-x    0,3 0,3 2,8 1,7 1,7
	shr	R8(%rcx), u0	C              1,7 1,6 2,8 2,8 2,8
	mov	v0, %r10	C              1   1   4   3   3
	sub	u0, %r10	C              2   2   5   4   4
	jnz	L(top)		C

L(end):	mov	v0, %rax
	FUNC_EXIT()
	ret
EPILOGUE()
