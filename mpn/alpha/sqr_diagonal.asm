dnl  Alpha mpn_sqr_diagonal.

dnl  Copyright 2001 Free Software Foundation, Inc.

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

dnl  INPUT PARAMETERS
dnl  res_ptr	r16
dnl  s1_ptr	r17
dnl  size	r18

dnl  This code runs at 42 cycles/limb on EV4, 18 cycles/limb on EV5, and 4
dnl  cycles/limb on EV6.

ASM_START()
PROLOGUE(mpn_sqr_diagonal)
	ldq	r2,0(r17)	C r2 = s1_limb
	lda	r18,-2(r18)	C size -= 2
	mulq	r2,r2,r3	C r3 = prod_low
	umulh	r2,r2,r4	C r4 = prod_high
	blt	r18,$Lend1	C jump if size was == 1
	ldq	r2,8(r17)	C r2 = s1_limb
	beq	r18,$Lend2	C jump if size was == 2

	ALIGN(8)
$Loop:	stq	r3,0(r16)
	mulq	r2,r2,r3	C r3 = prod_low
	lda	r18,-1(r18)	C size--
	stq	r4,8(r16)
	umulh	r2,r2,r4	C r4 = cy_limb
	ldq	r2,16(r17)	C r2 = s1_limb
	lda	r17,8(r17)	C s1_ptr++
	lda	r16,16(r16)	C res_ptr++
	bne	r18,$Loop

$Lend2:	stq	r3,0(r16)
	mulq	r2,r2,r3	C r3 = prod_low
	stq	r4,8(r16)
	umulh	r2,r2,r4	C r4 = cy_limb
	stq	r3,16(r16)
	stq	r4,24(r16)
	ret	r31,(r26),1
$Lend1:	stq	r3,0(r16)
	stq	r4,8(r16)
	ret	r31,(r26),1
EPILOGUE(mpn_sqr_diagonal)
ASM_END()
