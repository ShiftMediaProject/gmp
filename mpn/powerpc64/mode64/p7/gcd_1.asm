dnl  PowerPC-64 mpn_gcd_1.

dnl  Copyright 2000, 2001, 2002, 2005, 2009, 2011, 2012, 2013 Free Software
dnl  Foundation, Inc.

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

C	 	    cycles/bit (approx)
C POWER3/PPC630		 -
C POWER4/PPC970		 -
C POWER5		 -
C POWER6		 -
C POWER7		 7.6
C Numbers measured with: speed -CD -s16-64 -t48 mpn_gcd_1

C INPUT PARAMETERS
define(`up',    `r3')
define(`n',     `r4')
define(`v0',    `r5')

EXTERN_FUNC(mpn_mod_1)
EXTERN_FUNC(mpn_modexact_1c_odd)

ASM_START()
PROLOGUE(mpn_gcd_1)
	mflr	r0
	std	r30, -16(r1)
	std	r31, -8(r1)
	std	r0, 16(r1)
	stdu	r1, -128(r1)

	ld	r7, 0(up)		C U low limb
	or	r0, r5, r7		C x | y

	neg	r6, r0
	and	r6, r6, r0
	cntlzd	r31, r6			C common twos
	subfic	r31, r31, 63

	neg	r6, r5
	and	r6, r6, r5
	cntlzd	r8, r6
	subfic	r8, r8, 63
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

	neg.	r6, r3
	and	r6, r6, r3
	cntlzd	cnt, r6
	li	r12, 63
	bne	L(mid)
	b	L(end)

	ALIGN(16)
L(top):	isel	r30, r3, r30, 29	C y = min(x,y)
	isel	r3, r10, r11, 29	C x = |y - x|
L(mid):	subf	cnt, cnt, r12		C cnt = 63-cnt
	srd	r3, r3, cnt
	subf	r10, r3, r30		C r10 = y - x
	subf	r11, r30, r3		C r11 = x - y
	cmpld	cr7, r30, r3
	and	r8, r11, r10		C isolate lsb
	cntlzd	cnt, r8
	bne	cr7, L(top)

L(end):	sld	r3, r30, r31

	addi	r1, r1, 128
	ld	r0, 16(r1)
	ld	r30, -16(r1)
	ld	r31, -8(r1)
	mtlr	r0
	blr
EPILOGUE()
