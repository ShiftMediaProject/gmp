 # Alpha auxiliary for longlong.h's count_leading_zeros

 # Copyright (C) 1997 Free Software Foundation, Inc.

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

 # DISCUSSION:

 # Other methods have been tried, and using a 128-entry table actually trims
 # about 10% of the execution time (on a 21164) when the table is in the L1
 # cache.  But under non-benchmarking conditions, the table will hardly be in
 # the L1 cache.  Tricky bit-fiddling methods with multiplies and magic tables
 # are also possible, but they require many more instructions than the current
 # code.  (But for count_trailing_zeros, such tricks are beneficial.)
 # Finally, converting to floating-point and extracting the exponent is much
 # slower.

	.set	noreorder
	.set	noat
.text
	.align	3
	.globl	__count_leading_zeros
	.ent	__count_leading_zeros 0
__count_leading_zeros:
	bis	$31,63,$0		# initialize partial result count

	srl	$16,32,$1		# shift down 32 steps -> r1
	cmovne	$1,$1,$16		# select r1 if non-zero
	cmovne	$1,31,$0		# if r1 is nonzero choose smaller count

	srl	$16,16,$1		# shift down 16 steps -> r1
	subq	$0,16,$2		# generate new partial result count
	cmovne	$1,$1,$16		# choose new r1 if non-zero
	cmovne	$1,$2,$0		# choose new count if r1 was non-zero

	srl	$16,8,$1
	subq	$0,8,$2
	cmovne	$1,$1,$16
	cmovne	$1,$2,$0

	srl	$16,4,$1
	subq	$0,4,$2
	cmovne	$1,$1,$16
	cmovne	$1,$2,$0

	srl	$16,2,$1
	subq	$0,2,$2
	cmovne	$1,$1,$16
	cmovne	$1,$2,$0

	srl	$16,1,$1		# extract bit 1
	subq	$0,$1,$0		# subtract it from partial result

	ret	$31,($26),1
	.end	__count_leading_zeros
