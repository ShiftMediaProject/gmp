# PowerPC-32 __mpn_submul_1 -- Multiply a limb vector with a limb and subtract
# the result from a second limb vector.

# Copyright (C) 1995, 1997, 1998 Free Software Foundation, Inc.

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

# This is optimized for the PPC604.  It has not been tested on PPC601, PPC603
# or PPC750 since I don't have access to any such machines.

#include "asm-syntax.h"

#ifdef AIX_SYNTAX
	.toc
	.csect	.text[PR]
	.align	2
	.globl	__mpn_submul_1
	.globl	.__mpn_submul_1
	.csect	__mpn_submul_1[DS]
__mpn_submul_1:
	.long	.__mpn_submul_1, TOC[tc0], 0
	.csect	.text[PR]
.__mpn_submul_1:
#else
	.text
	.align	4
	.globl	__mpn_submul_1
__mpn_submul_1:
#endif
	cmpi	0,5,9		# more than 9 limbs?
	bgt	0,Lbig		# branch if more than 9 limbs

	mtctr	5
	lwz	0,0(4)
	mullw	7,0,6
	mulhwu	10,0,6
	lwz	9,0(3)
	subfc	8,7,9
	addc	7,7,8		# invert cy (r7 is junk)
	addi	3,3,-4
	bdz	Lend
Loop:	lwzu	0,4(4)
	stwu	8,4(3)
	mullw	8,0,6
	adde	7,8,10
	mulhwu	10,0,6
	lwz	9,4(3)
	addze	10,10
	subfc	8,7,9
	addc	7,7,8		# invert cy (r7 is junk)
	bdnz	Loop
Lend:	stw	8,4(3)
	addze	3,10
	blr

Lbig:	stmw	30,-32(1)
	addi	5,5,-1
	srwi	0,5,2
	mtctr	0

	lwz	7,0(4)
	mullw	8,7,6
	mulhwu	0,7,6
	lwz	7,0(3)
	subfc	7,8,7
	addc	8,7,8
	stw	7,0(3)

LoopU:	lwz	7,4(4)
	lwz	12,8(4)
	lwz	30,12(4)
	lwzu	31,16(4)
	mullw	8,7,6
	mullw	9,12,6
	mullw	10,30,6
	mullw	11,31,6
	adde	8,8,0		# add cy_limb
	mulhwu	0,7,6
	lwz	7,4(3)
	adde	9,9,0
	mulhwu	0,12,6
	lwz	12,8(3)
	adde	10,10,0
	mulhwu	0,30,6
	lwz	30,12(3)
	adde	11,11,0
	mulhwu	0,31,6
	lwz	31,16(3)
	addze	0,0		# new cy_limb
	subfc	7,8,7
	stw	7,4(3)
	subfe	12,9,12
	stw	12,8(3)
	subfe	30,10,30
	stw	30,12(3)
	subfe	31,11,31
	stwu	31,16(3)
	subfe	11,11,11	# invert ...
	addic	11,11,1		# ... carry
	bdnz	LoopU

	andi.	31,5,3
	mtctr	31
	beq	0,Lendx

LoopE:	lwzu	7,4(4)
	mullw	8,7,6
	adde	8,8,0		# add cy_limb
	mulhwu	0,7,6
	lwz	7,4(3)
	addze	0,0		# new cy_limb
	subfc	7,8,7
	addc	8,7,8
	stwu	7,4(3)
	bdnz	LoopE
Lendx:
	addze	3,0
	lmw	30,-32(1)
	blr
