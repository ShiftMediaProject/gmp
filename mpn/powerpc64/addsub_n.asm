dnl  PowerPC-64 mpn_addsub_n -- Simultaneous add and sub.

dnl  Copyright 1999, 2000, 2001, 2003 Free Software Foundation, Inc.

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

C INPUT PARAMETERS
C res_ptr	r3
C s1_ptr	r4
C s2_ptr	r5
C size		r6

include(`../config.m4')

define(SAVE_BORROW_RESTORE_CARRY,
	`sldi $1,$1,63
	adde $1,$1,$1')
define(SAVE_CARRY_RESTORE_BORROW,
	`sldi $1,$1,63
	adde $1,$1,$1')

C 19991117

C This is just crafted for testing some ideas, and verifying that we can make
C it run fast.  It runs at 2.55 cycles/limb on the 630, which is very good.
C We should play a little with the schedule.  No time has been spent on that.

C To finish this, the loop warm up and cool down code needs to be written,
C and the result need to be tested.  Also, the proper calling sequence should
C be used.

C             r1p r2p s1p s2p n
C Use reg r0, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12

ASM_START()
PROLOGUE(mpn_addsub_n)
	std	r14,-64(r1)
	std	r15,-56(r1)
	std	r16,-48(r1)
	std	r17,-40(r1)
	std	r18,-32(r1)
	std	r19,-24(r1)

	srdi	r7,r7,2
	mtctr	r7		C copy size into CTR
	addic	r0,r0,0		C clear cy
	addi	r3,r3,-8	C offset res_ptr, it's updated before it's used
	addi	r4,r4,-8	C offset res_ptr, it's updated before it's used

.Loop:
	adde	r12,r8,r9
	std	r12,8(r3)
	adde	r12,r10,r11
	std	r12,16(r3)

	SAVE_CARRY_RESTORE_BORROW(r0)

	subfe	r12,r8,r9
	std	r12,8(r4)
	ld	r8,8(r5)	C s1 L 1
	ld	r9,8(r6)	C s2 L 1
	subfe	r12,r10,r11
	std	r12,16(r4)
	ld	r10,16(r5)	C s1 L 2
	ld	r11,16(r6)	C s2 L 2
C pair -------------------------
	subfe	r12,r14,r15
	std	r12,24(r4)
	subfe	r12,r16,r17
	stdu	r12,32(r4)

	SAVE_BORROW_RESTORE_CARRY(r0)

	adde	r12,r14,r15
	std	r12,24(r3)
	ld	r14,24(r5)	C s1 L 3
	ld	r15,24(r6)	C s2 L 3
	adde	r12,r16,r17
	stdu	r12,32(r3)
	ldu	r16,32(r5)	C s1 L 4
	ldu	r17,32(r6)	C s2 L 4
	bdnz	.Loop

	ld	r14,-64(r1)
	ld	r15,-56(r1)
	ld	r16,-48(r1)
	ld	r17,-40(r1)
	ld	r18,-32(r1)
	ld	r19,-24(r1)
	blr
EPILOGUE(mpn_addsub_n)
