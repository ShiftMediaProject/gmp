dnl  PowerPC-64 mpn_rshift -- Shift a number right.

dnl  Copyright 1999, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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

C		cycles/limb
C POWER3/PPC630:     2.25
C POWER4/PPC970:     ?

C INPUT PARAMETERS
C res_ptr	r3
C s1_ptr	r4
C size		r5
C cnt		r6

ASM_START()
PROLOGUE(mpn_rshift)
	mtctr	r5		C copy size into CTR
	addi	r7,r3,-8	C move adjusted res_ptr to free return reg
	subfic	r8,r6,64
	ld	r11,0(r4)	C load first s1 limb
	sld	r3,r11,r8	C compute function return value
	bdz	.Lend1

.Loop:	ldu	r10,8(r4)
	srd	r9,r11,r6
	sld	r12,r10,r8
	or	r9,r9,r12
	stdu	r9,8(r7)
	bdz	.Lend2
	ldu	r11,8(r4)
	srd	r9,r10,r6
	sld	r12,r11,r8
	or	r9,r9,r12
	stdu	r9,8(r7)
	bdnz	.Loop

.Lend1:	srd	r0,r11,r6
	std	r0,8(r7)
	blr

.Lend2:	srd	r0,r10,r6
	std	r0,8(r7)
	blr
EPILOGUE(mpn_rshift)
