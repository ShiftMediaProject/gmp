dnl  PowerPC-64 mpn_copyd

dnl  Copyright 2004 Free Software Foundation, Inc.

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
C POWER3/PPC630:     1
C POWER4/PPC970:     1

C INPUT PARAMETERS
C rp	r3
C up	r4
C n	r5

ASM_START()
PROLOGUE(mpn_copyd)
	rldicl.	r0, r5, 3, 59	C r0 = (r5 & 3) << 3; cr0 = (n == 4t)?
	cmpldi	cr6, r0, 16	C cr6 = (n cmp 4t + 2)?

	sldi	r6, r5, 3

	addi	r5, r5, 4	C compute...
	srdi	r5, r5, 2	C ...branch count
	mtctr	r5

	add	r4, r4, r6
	add	r3, r3, r6
	sub	r4, r4, r0	C offset up
	sub	r3, r3, r0	C offset rp

	beq	cr0, .LL00
	blt	cr6, .LL01
	beq	cr6, .LL10
	b	.LL11

	ALIGN(8)
.Loop:	ld	r6, 24(r4)
	std	r6, 24(r3)
.LL11:	ld	r6, 16(r4)
	std	r6, 16(r3)
.LL10:	ld	r6, 8(r4)
	std	r6, 8(r3)
.LL01:	ld	r6, 0(r4)
	std	r6, 0(r3)
.LL00:	addi	r4, r4, -32
	addi	r3, r3, -32

	bdnz	.Loop

	blr
EPILOGUE()
