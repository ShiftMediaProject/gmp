dnl  PowerPC 750 mpn_divexact_by3 -- mpn by 3 exact division

dnl  Copyright 2002, 2003 Free Software Foundation, Inc.
dnl 
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


C      cycles/limb
C 750:    10.0
C 7400:   10.0


C void mpn_divexact_by3 (mp_ptr dst, mp_srcptr src, mp_size_t size);
C
C Scheduling the load back a bit gets us down from 11 c/l to 10 c/l.
C
C The mulhwu has the "3" multiplier in the second operand, this lets 750 and
C 7400 use an early-out.  The other way around costs an extra 3.5 c/l or so.

ASM_START()
PROLOGUE(mpn_divexact_by3c)

	C r3	dst
	C r4	src
	C r5	size
	C r6	carry

	mtctr	r5		C size
	lwz	r7, 0(r4)	C src[0]

	lis	r5, 0xAAAA	C inverse high
	subi	r3, r3, 4	C adjust dst for first stwu

	li	r0, 3		C multiplier 3
	ori	r5, r5, 0xAAAB	C inverse low

	subfc	r7, r6, r7	C l = src[0] - carry
	bdz	L(one)

L(top):
	C r0	3
	C r3	dst, incrementing
	C r4	src, incrementing
	C r5	inverse
	C r6	carry
	C r7	l

	mullw	r8, r7, r5	C q = l * inverse
	lwzu	r7, 4(r4)	C src[i]

	C

	mulhwu	r6, r8, r0	C c = high(3*q)
	stwu	r8, 4(r3)	C dst[i-1] = q

	C serialize
	subfe	r7, r6, r7 	C l = s - carry
	bdnz	L(top)


L(one):
	subfe	r4, r4, r4	C ca 0 or -1

	mullw	r8, r7, r5	C q = l * inverse

	mulhwu	r6, r8, r0	C c = high(3*q)
	stwu	r8, 4(r3)	C dst[i] = q

	subf	r3, r4, r6	C carry + ca

	blr

EPILOGUE()
