# PowerPC-32 __mpn_mul_1 -- Multiply a limb vector with a limb and store
# the result in a second limb vector.

# Copyright (C) 1995, 1997 Free Software Foundation, Inc.

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

# This is optimized for the PPC604 but it runs decently even on PPC601.  It has
# not been tested on a PPC603 since I don't have access to any such machines.

	.toc
	.csect .__mpn_mul_1[PR]
	.align 2
	.globl __mpn_mul_1
	.globl .__mpn_mul_1
	.csect __mpn_mul_1[DS]
__mpn_mul_1:
	.long .__mpn_mul_1[PR], TOC[tc0], 0
	.csect .__mpn_mul_1[PR]
.__mpn_mul_1:
	mtctr	5
	addi	3,3,-4		# adjust res_ptr, it's offset before it's used
	li	12,0		# clear upper product reg
	addic	0,0,0		# clear cy
# Start software pipeline
	lwz	8,0(4)
	bdz	Lend3
	stmw	30,-8(1)	# save registers we are supposed to preserve
	lwzu	9,4(4)
	mullw	11,8,6
	mulhwu	0,8,6
	bdz	Lend1
# Software pipelined main loop
Loop:	lwz	8,4(4)
	mullw	10,9,6
	adde	30,11,12
	mulhwu	12,9,6
	stw	30,4(3)
	bdz	Lend2
	lwzu	9,8(4)
	mullw	11,8,6
	adde	31,10,0
	mulhwu	0,8,6
	stwu	31,8(3)
	bdnz	Loop
# Finish software pipeline
Lend1:	mullw	10,9,6
	adde	30,11,12
	mulhwu	12,9,6
	stw	30,4(3)
	adde	31,10,0
	stwu	31,8(3)
	addze	3,12
	lmw	30,-8(1)	# restore registers from stack
	blr
Lend2:	mullw	11,8,6
	adde	31,10,0
	mulhwu	0,8,6
	stwu	31,8(3)
	adde	30,11,12
	stw	30,4(3)
	addze	3,0
	lmw	30,-8(1)	# restore registers from stack
	blr
Lend3:	mullw	11,8,6
	stw	11,4(3)
	mulhwu	3,8,6
	blr
