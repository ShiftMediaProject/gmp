dnl  Alpha auxiliary for longlong.h's count_leading_zeros

dnl  Copyright 1997, 2000 Free Software Foundation, Inc.

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

ASM_START()
PROLOGUE_GP(mpn_count_leading_zeros)
	.set at		C need the `at' register for expanding ldbu on ev4/ev5
	cmpbge	r31,  r16, r1
	lda	r3,   __clz_tab
	sra	r1,   1,   r1
	xor	r1,   127, r1
	srl	r16,  1,   r16
	addq	r1,   r3,  r1
	ldbu	r0,   0(r1)
	lda	r2,   64
	s8subl	r0,   8,    r0
	srl	r16,  r0,   r16
	addq	r16,  r3,   r16
	ldbu	r1,   0(r16)
	subq	r2,   r1,   r2
	subq	r2,   r0,   r0
	ret	r31,  (r26),1
EPILOGUE(mpn_count_leading_zeros)
ASM_END()

dnl This is an alternative sequence that might be faster, but probably is
dnl one cycle slower:
dnl
dnl	cmpbge	r31,  r16, r1
dnl	lda	r3,   MPN(clz_tab)
dnl	sra	r1,   1,   r1
dnl	xor	r1,   127, r1
dnl	addq	r1,   r3,  r1
dnl	ldbu	r2,   0(r1)
dnl	subl	r2,   1,    r2
dnl	extbl	r16,  r2,   r16
dnl	srl	r16,  1,    r16
dnl	addq	r16,  r3,   r16
dnl	ldbu	r1,   0(r16)
dnl	lda	r0,   64
dnl	s8addq	r2,   r1,   r2
dnl	subl	r0,   r2,   r0
