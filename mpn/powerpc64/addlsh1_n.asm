dnl  PowerPC-64 mpn_addlsh1_n -- rp[] = up[] + (vp[] << 1)

dnl  Copyright 2003 Free Software Foundation, Inc.

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
C POWER3/PPC630:     2.25	(1.5 c/l should be possible)
C POWER4/PPC970:     ?		(2.0 c/l should be possible)

C INPUT PARAMETERS
C rp	r3
C up	r4
C vp	r5
C n	r6

ASM_START()
PROLOGUE(mpn_addlsh1_n)
	mtctr	r6		C copy size into CTR
	addic	r0,r0,0		C clear cy
	li	r10, 0		C clear r10
	ld	r8,0(r4)	C load least significant u limb
	ld	r0,0(r5)	C load least significant v limb
	addi	r3,r3,-8	C offset res_ptr, it's updated before it's used
	bdz	.Lend		C If done, skip loop
.Loop:
	srdi	r12, r10, 63
	ld	r9,8(r4)	C load u limb
	rldimi	r12, r0, 1, 0
	ld	r10,8(r5)	C load v limb
	adde	r7,r12,r8	C add limbs with cy, set cy
	std	r7,8(r3)	C store result limb
	bdz	.Lexit		C decrement CTR and exit if done
	srdi	r11, r0, 63
	ldu	r8,16(r4)	C load u limb and update up
	rldimi	r11, r10, 1, 0
	ldu	r0,16(r5)	C load v limb and update vp
	adde	r7,r11,r9	C add limbs with cy, set cy
	stdu	r7,16(r3)	C store result limb and update rp
	bdnz	.Loop		C decrement CTR and loop back

.Lend:	srdi	r12, r10, 63
	rldimi	r12, r0, 1, 0
	adde	r7,r12,r8
	std	r7,8(r3)	C store ultimate result limb
	srdi	r3, r0, 63
	addze	r3,r3		C ... return value register
	blr
.Lexit:	srdi	r11, r0, 63
	rldimi	r11, r10, 1, 0
	adde	r7,r11,r9
	std	r7,16(r3)
	srdi	r3, r10, 63
	addze	r3,r3		C ... return value register
	blr
EPILOGUE()
