# PowerPC-32 mpn_mul_1 -- Multiply a limb vector with a limb and store
# the result in a second limb vector.

# Copyright (C) 1995, 1997, 2000 Free Software Foundation, Inc.

# This file is part of the GNU MP Library.

# The GNU MP Library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Library General Public License as published by
# the Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.

# The GNU MP Library is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
# License for more details.

# You should have received a copy of the GNU Library General Public License
# along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
# MA 02111-1307, USA.


# INPUT PARAMETERS
# res_ptr	r3
# s1_ptr	r4
# size		r5
# s2_limb	r6

# This is optimized for the PPC604 but it runs decently even on PPC601.  It
# has not been tested on a PPC603 since I don't have access to any such
# machines.

include(`../config.m4')

ASM_START()
PROLOGUE(mpn_mul_1)
	mtctr	r5
	addi	r3,r3,-4	# adjust res_ptr, it's offset before it's used
	li	r12,0		# clear upper product reg
	addic	r0,r0,0		# clear cy
# Start software pipeline
	lwz	r8,0(r4)
	bdz	.Lend3
	stmw	r30,-8(r1)	# save registers we are supposed to preserve
	lwzu	r9,4(r4)
	mullw	r11,r8,r6
	mulhwu	r0,r8,r6
	bdz	.Lend1
# Software pipelined main loop
.Loop:	lwz	r8,4(r4)
	mullw	r10,r9,r6
	adde	r30,r11,r12
	mulhwu	r12,r9,r6
	stw	r30,4(r3)
	bdz	.Lend2
	lwzu	r9,8(r4)
	mullw	r11,r8,r6
	adde	r31,r10,r0
	mulhwu	r0,r8,r6
	stwu	r31,8(r3)
	bdnz	.Loop
# Finish software pipeline
.Lend1:	mullw	r10,r9,r6
	adde	r30,r11,r12
	mulhwu	r12,r9,r6
	stw	r30,4(r3)
	adde	r31,r10,r0
	stwu	r31,8(r3)
	addze	r3,r12
	lmw	r30,-8(r1)	# restore registers from stack
	blr
.Lend2:	mullw	r11,r8,r6
	adde	r31,r10,r0
	mulhwu	r0,r8,r6
	stwu	r31,8(r3)
	adde	r30,r11,r12
	stw	r30,4(r3)
	addze	r3,r0
	lmw	r30,-8(r1)	# restore registers from stack
	blr
.Lend3:	mullw	r11,r8,r6
	stw	r11,4(r3)
	mulhwu	r3,r8,r6
	blr
EPILOGUE(mpn_mul_1)
