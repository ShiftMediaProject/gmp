# I960 __gmpn_mul_1 -- Multiply a limb vector with a limb and store
# the result in a second limb vector.

# Copyright 1995, 2000 Free Software Foundation, Inc.

#  This file is part of the GNU MP Library.
#
#  The GNU MP Library is free software; you can redistribute it and/or modify
#  it under the terms of either:
#
#    * the GNU Lesser General Public License as published by the Free
#      Software Foundation; either version 3 of the License, or (at your
#      option) any later version.
#
#  or
#
#    * the GNU General Public License as published by the Free Software
#      Foundation; either version 2 of the License, or (at your option) any
#      later version.
#
#  or both in parallel, as here.
#
#  The GNU MP Library is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
#  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
#  for more details.
#
#  You should have received copies of the GNU General Public License and the
#  GNU Lesser General Public License along with the GNU MP Library.  If not,
#  see https://www.gnu.org/licenses/.

.text
	.align	4
	.globl	___gmpn_mul_1
___gmpn_mul_1:
	subo	g2,0,g2
	shlo	2,g2,g4
	subo	g4,g1,g1
	subo	g4,g0,g13
	mov	0,g0

	cmpo	1,0		# clear C bit on AC.cc

Loop:	ld	(g1)[g2*4],g5
	emul	g3,g5,g6

	addc	g0,g6,g6	# relies on that C bit is clear
	st	g6,(g13)[g2*4]
	addc	0,g7,g0

	addo	g2,1,g2
	cmpobne	0,g2,Loop	# when branch is taken, clears C bit

	ret
