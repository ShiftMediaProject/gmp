# PowerPC-32  __mpn_sub_n -- Subtract two limb vectors of the same length > 0
# and store difference in a third limb vector.

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
# s2_ptr	r5
# size		r6

	.toc
	.extern __mpn_sub_n[DS]
	.extern .__mpn_sub_n
.csect [PR]
	.align 2
	.globl __mpn_sub_n
	.globl .__mpn_sub_n
	.csect __mpn_sub_n[DS]
__mpn_sub_n:
	.long .__mpn_sub_n, TOC[tc0], 0
	.csect [PR]
.__mpn_sub_n:
	mtctr	6		# copy size into CTR
	addic	0,6,-1		# set cy
	lwz	8,0(4)		# load least significant s1 limb
	lwz	0,0(5)		# load least significant s2 limb
	addi	3,3,-4		# offset res_ptr, it's updated before it's used
	bdz	Lend		# If done, skip loop
Loop:	lwz	9,4(4)		# load s1 limb
	lwz	10,4(5)		# load s2 limb
	subfe	7,0,8		# subtract limbs with cy, set cy
	stw	7,4(3)		# store result limb
	bdz	Lexit		# decrement CTR and exit if done
	lwzu	8,8(4)		# load s1 limb and update s1_ptr
	lwzu	0,8(5)		# load s2 limb and update s2_ptr
	subfe	7,10,9		# subtract limbs with cy, set cy
	stwu	7,8(3)		# store result limb and update res_ptr
	bdnz	Loop		# decrement CTR and loop back

Lend:	subfe	7,0,8
	stw	7,4(3)		# store ultimate result limb
	subfe	3,0,0		# load !cy into ...
	subfic	3,3,0		# ... return value register
	blr
Lexit:	subfe	7,10,9
	stw	7,8(3)
	subfe	3,0,0		# load !cy into ...
	subfic	3,3,0		# ... return value register
	blr
