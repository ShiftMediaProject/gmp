dnl  Alpha ev67 mpn_hamdist -- mpn hamming distance.

dnl  Copyright 2003 Free Software Foundation, Inc.

dnl  This file is part of the GNU MP Library.
dnl
dnl  The GNU MP Library is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU Lesser General Public License as
dnl  published by the Free Software Foundation; either version 2.1 of the
dnl  License, or (at your option) any later version.
dnl
dnl  The GNU MP Library is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl  Lesser General Public License for more details.
dnl
dnl  You should have received a copy of the GNU Lesser General Public
dnl  License along with the GNU MP Library; see the file COPYING.LIB.  If
dnl  not, write to the Free Software Foundation, Inc., 59 Temple Place -
dnl  Suite 330, Boston, MA 02111-1307, USA.

include(`../config.m4')


C ev67: 2.4 cycles/limb


C unsigned long mpn_hamdist (mp_srcptr xp, mp_srcptr yp, mp_size_t size);
C
C The hope was for 2.0 c/l here, but that isn't achieved.  Not sure what the
C limiting factor is.
C
C The main loop processes two limbs from each operand on each iteration.  An
C odd size is handled by processing xp[0]^yp[0] at the start.  If the size
C is even that result is discarded, and is repeated by the main loop.
C

ASM_START()
PROLOGUE(mpn_hamdist)

	C r16	xp
	C r17	yp
	C r18	size

	ldq	r1, 0(r16)		C L0  xp[0]
	ldq	r2, 0(r17)		C L1  yp[0]
	and	r18, 1, r8		C u1  1 if size odd
	srl	r18, 1, r18		C U0  size, limb pairs

	clr	r0			C l0  initial total
	s8addq	r8, r17, r17		C u1  yp++ if size odd
	s8addq	r8, r16, r16		C l1  xp++ if size odd
	clr	r6			C u0  dummy initial xor 1

	xor	r1, r2, r5		C l   initial xor 0
	beq	r18, L(one)		C U   if size==1

	cmoveq	r8, r31, r5		C l   discard first limb if size even
	unop				C u


	ALIGN(16)
L(top):
	C r0	total accumulating
	C r7	xor 0
	C r8	xor 1
	C r16	xp, incrementing
	C r17	yp, incrementing
	C r18	size, limb pairs, decrementing

	ldq	r1, 0(r16)		C L
	ldq	r2, 0(r17)		C L
	ctpop	r5, r7			C U0
	lda	r16, 16(r16)		C u

	ldq	r3, -8(r16)		C L
	ldq	r4, 8(r17)		C L
	ctpop	r6, r8			C U0
	lda	r17, 16(r17)		C u

	xor	r1, r2, r5		C l
	xor	r3, r4, r6		C u

	addq	r0, r7, r0		C l
	lda	r18, -1(r18)		C u
	addq	r0, r8, r0		C l
	bne	r18, L(top)		C U


	ctpop	r6, r8			C U0
	addq	r0, r8, r0		C l
L(one):
	ctpop	r5, r7			C U0
	addq	r0, r7, r0		C l

	ret	r31, (r26), 1		C L0

EPILOGUE()
ASM_END()
