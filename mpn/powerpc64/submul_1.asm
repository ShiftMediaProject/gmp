# PowerPC-64 mpn_submul_1 -- Multiply a limb vector with a limb and subtract
# the result from a second limb vector.

# Copyright 1999, 2000 Free Software Foundation, Inc.

# This file is part of the GNU MP Library.

# The GNU MP Library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 2.1 of the License, or (at your
# option) any later version.

# The GNU MP Library is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
# License for more details.

# You should have received a copy of the GNU Lesser General Public License
# along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
# MA 02111-1307, USA.


# INPUT PARAMETERS
# res_ptr	r3
# s1_ptr	r4
# size		r5
# s2_limb	r6

include(`../config.m4')

ASM_START()
PROLOGUE(mpn_submul_1)
	mtctr	r5
	li	r9,0		# cy_limb = 0
	addic	r0,r0,0
	cal	r3,-8(r3)
	cal	r4,-8(r4)
.Loop:
	ldu	r0,8(r4)
	ld	r10,8(r3)
	mulld	r7,r0,r6
	adde	r7,r7,r9
	mulhdu	r9,r0,r6
	addze	r9,r9
	subfc	r7,r7,r10
	stdu	r7,8(r3)
	subfe	r11,r11,r11	# invert ...
	addic	r11,r11,1		# ... carry
	bdnz	.Loop

	addze	r3,r9
	blr
EPILOGUE(mpn_submul_1)
