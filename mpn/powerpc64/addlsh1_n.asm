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
C POWER3/PPC630:     2		(1.5 c/l should be possible)
C POWER4/PPC970:     ?		(2.0 c/l should be possible)

C INPUT PARAMETERS
C rp	r3
C up	r4
C vp	r5
C n	r6

define(`rp',`r3')
define(`up',`r4')
define(`vp',`r5')

define(`s0',`r6')
define(`s1',`r7')
define(`u0',`r8')
define(`v0',`r10')
define(`v1',`r11')

ASM_START()
PROLOGUE(mpn_addlsh1_n)
	mtctr	r6		C copy n in ctr
	addic	r31, r31, 0	C clear cy

	ld	v0, 0(vp)	C load v limb
	ld	u0, 0(up)	C load u limb
	addi	up, up, -8	C update up
	addi	rp, rp, -8	C update rp
	sldi	s1, v0, 1
	bdz	.Lend		C If done, skip loop

.Loop:	ld	v1, 8(vp)	C load v limb
	adde	s1, s1, u0	C add limbs with cy, set cy
	std	s1, 8(rp)	C store result limb
	srdi	s0, v0, 63	C shift down previous v limb
	ldu	u0, 16(up)	C load u limb and update up
	rldimi	s0, v1, 1, 0	C lest shift v limb and merge with prev v limb

	bdz	.Lexit		C decrement ctr and exit if done

	ldu	v0, 16(vp)	C load v limb and update vp
	adde	s0, s0, u0	C add limbs with cy, set cy
	stdu	s0, 16(rp)	C store result limb and update rp
	srdi	s1, v1, 63	C shift down previous v limb
	ld	u0, 8(up)	C load u limb
	rldimi	s1, v0, 1, 0	C lest shift v limb and merge with prev v limb

	bdnz	.Loop		C decrement ctr and loop back

.Lend:	adde	r7, s1, u0
	std	r7, 8(rp)	C store last result limb
	srdi	r3, v0, 63
	addze	r3, r3
	blr
.Lexit:	adde	r7, s0, u0
	std	r7, 16(rp)	C store last result limb
	srdi	r3, v1, 63
	addze	r3, r3
	blr
EPILOGUE()
