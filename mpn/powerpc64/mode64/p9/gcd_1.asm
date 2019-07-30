dnl  PowerPC-64 mpn_gcd_1.

dnl  Copyright 2000-2002, 2005, 2009, 2011-2013, 2019 Free Software Foundation,
dnl  Inc.

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

C		    cycles/bit (approx)
C POWER3/PPC630		 -
C POWER4/PPC970		 -
C POWER5		 -
C POWER6		 -
C POWER7		 -
C POWER8		 -
C POWER9		 5.75
C Numbers measured with: speed -CD -s16-64 -t48 mpn_gcd_1

C INPUT PARAMETERS
define(`up',    `r3')
define(`n',     `r4')
define(`v0',    `r5')

EXTERN_FUNC(mpn_mod_1)
EXTERN_FUNC(mpn_modexact_1c_odd)

ASM_START()
PROLOGUE(mpn_gcd_1,toc)
	mflr	r0
	std	r30, -16(r1)
	std	r31, -8(r1)
	std	r0, 16(r1)
	stdu	r1, -128(r1)

	ld	r7, 0(up)		C U low limb
	or	r0, r5, r7		C x | y
	cnttzd	r31, r0			C common twos
	cnttzd	r8, r5
	srd	r5, r5, r8
	mr	r30, r5			C v0 saved

	cmpdi	r4, BMOD_1_TO_MOD_1_THRESHOLD
	blt	L(bmod)
	CALL(	mpn_mod_1)
	b	L(reduced)
L(bmod):
	li	r6, 0
	CALL(	mpn_modexact_1c_odd)
L(reduced):

define(`cnt',  `r9')dnl

	cmpdi	r3, 0
	cnttzd	cnt, r3
	bne	L(mid)
	b	L(end)

	ALIGN(16)
L(top):	isel	r30, r3, r30, 29	C y = min(x,y)
	isel	r3, r10, r11, 29	C x = |y - x|
L(mid):	srd	r3, r3, cnt
	subf	r10, r3, r30		C r10 = y - x
	subf	r11, r30, r3		C r11 = x - y
	cmpld	cr7, r30, r3
	cnttzd	cnt, r10
	bne	cr7, L(top)

L(end):	sld	r3, r30, r31

	addi	r1, r1, 128
	ld	r0, 16(r1)
	ld	r30, -16(r1)
	ld	r31, -8(r1)
	mtlr	r0
	blr
EPILOGUE()
