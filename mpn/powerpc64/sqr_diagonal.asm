dnl  PowerPC-64 mpn_sqr_diagonal.

dnl  Copyright 2001, 2002, 2003 Free Software Foundation, Inc.

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
C POWER3/PPC630:    18
C POWER4/PPC970:     8

C INPUT PARAMETERS
C rp	r3
C up	r4
C n	r5

ASM_START()
PROLOGUE(mpn_sqr_diagonal)
	mtctr	r5
.Loop:
	ld	r0, 0(r4)
	nop
	mulld	r5, r0, r0
	mulhdu	r6, r0, r0
	std	r5, 0(r3)
	addi	r4, r4, 8
	std	r6, 8(r3)
	addi	r3, r3, 16
	bdnz	.Loop

	blr
EPILOGUE()
