# PowerPC-32 __mpn_lshift -- Shift a number left.

# Copyright (C) 1995, 1998 Free Software Foundation, Inc.

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
# cnt		r6

#include "asm-syntax.h"

#ifdef AIX_SYNTAX
	.toc
	.csect	.text[PR]
	.align	2
	.globl	__mpn_lshift
	.globl	.__mpn_lshift
	.csect	__mpn_lshift[DS]
__mpn_lshift:
	.long	.__mpn_lshift, TOC[tc0], 0
	.csect	.text[PR]
.__mpn_lshift:
#else
	.text
	.align 4
	.globl __mpn_lshift
__mpn_lshift:
#endif
	cmpi	0,5,12		# more than 12 limbs?
	slwi	0,5,2
	add	4,4,0		# make r4 point at end of s1
	add	7,3,0		# make r7 point at end of res
	bgt	.LBIG		# branch if more than 12 limbs

	mtctr	5		# copy size into CTR
	subfic	8,6,32
	lwzu	11,-4(4)	# load first s1 limb
	srw	3,11,8		# compute function return value
	bdz	.Lend1

.Loop:	lwzu	10,-4(4)
	slw	9,11,6
	srw	12,10,8
	or	9,9,12
	stwu	9,-4(7)
	bdz	.Lend2
	lwzu	11,-4(4)
	slw	9,10,6
	srw	12,11,8
	or	9,9,12
	stwu	9,-4(7)
	bdnz	.Loop

.Lend1:	slw	0,11,6
	stw	0,-4(7)
	blr
.Lend2:	slw	0,10,6
	stw	0,-4(7)
	blr

.LBIG:
	stmw	24,-32(1)	# save registers we are supposed to preserve
	lwzu	9,-4(4)
	subfic	8,6,32
	srw	3,9,8		# compute function return value
	slw	0,9,6
	addi	5,5,-1

	andi.	10,5,3		# count for spill loop
	beq	.Le
	mtctr	10
	lwzu	28,-4(4)
	bdz	.Lxe0

.Loop0:	slw	12,28,6
	srw	24,28,8
	lwzu	28,-4(4)
	or	24,0,24
	stwu	24,-4(7)
	mr	0,12
	bdnz	.Loop0		# taken at most once!

.Lxe0:	slw	12,28,6
	srw	24,28,8
	or	24,0,24
	stwu	24,-4(7)
	mr	0,12

.Le:	srwi	5,5,2		# count for unrolled loop
	addi	5,5,-1
	mtctr	5
	lwz	28,-4(4)
	lwz	29,-8(4)
	lwz	30,-12(4)
	lwzu	31,-16(4)

.LoopU:	slw	9,28,6
	srw	24,28,8
	lwz	28,-4(4)
	slw	10,29,6
	srw	25,29,8
	lwz	29,-8(4)
	slw	11,30,6
	srw	26,30,8
	lwz	30,-12(4)
	slw	12,31,6
	srw	27,31,8
	lwzu	31,-16(4)
	or	24,0,24
	stw	24,-4(7)
	or	25,9,25
	stw	25,-8(7)
	or	26,10,26
	stw	26,-12(7)
	or	27,11,27
	stwu	27,-16(7)
	mr	0,12
	bdnz	.LoopU

	slw	9,28,6
	srw	24,28,8
	slw	10,29,6
	srw	25,29,8
	slw	11,30,6
	srw	26,30,8
	slw	12,31,6
	srw	27,31,8
	or	24,0,24
	stw	24,-4(7)
	or	25,9,25
	stw	25,-8(7)
	or	26,10,26
	stw	26,-12(7)
	or	27,11,27
	stwu	27,-16(7)
	mr	0,12

	stw	0,-4(7)
	lmw	24,-32(1)	# restore registers
	blr
