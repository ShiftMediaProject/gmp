# PowerPC-64 mpn_sqr_diagonal.

# Copyright 2001, 2002 Free Software Foundation, Inc.

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

include(`../config.m4')


# ppc630: 18.0 cycles/limb


# INPUT PARAMETERS
# res_ptr	r3
# s1_ptr	r4
# size		r5
# s2_limb	r6

ASM_START()
PROLOGUE(mpn_sqr_diagonal)
	mtctr	r5
	cal	r3,-8(r3)
	cal	r4,-8(r4)

.Loop:	ldu	r0,8(r4)
	mulld	r7,r0,r0
	mulhdu	r9,r0,r0
	std	r7,8(r3)
	stdu	r9,16(r3)
	bdnz	.Loop

	blr
EPILOGUE(mpn_sqr_diagonal)
