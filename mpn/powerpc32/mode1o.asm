dnl  PowerPC-32 mpn_modexact_1_odd -- mpn by limb exact remainder.

dnl  Copyright 2002 Free Software Foundation, Inc.
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
C 604e:   6.0
C 750:    6.25-12.5 depending on divisor


C mp_limb_t mpn_modexact_1_odd (mp_srcptr src, mp_size_t size,
C                               mp_limb_t divisor);
C mp_limb_t mpn_modexact_1c_odd (mp_srcptr src, mp_size_t size,
C                                mp_limb_t divisor, mp_limb_t carry);
C
C For PIC, the inverse is established arithmetically since it measures about
C 5 cycles faster than the nonsense needed to access modlimb_invert_table in
C SVR4 or Darwin style PIC.  AIX might be better, since it avoids bl/mflr to
C get at the GOT/TOC/whatever.
C
C Using divwu for size==1 measured about 10 cycles slower on 604e, or about
C 3-5 cycles faster on 750.  For now it doesn't seem worth bothering with.

ASM_START()
PROLOGUE(mpn_modexact_1_odd)

	C r3	src
	C r4	size
	C r5	divisor

	li	r6, 0

PROLOGUE(mpn_modexact_1c_odd)

	C r3	src
	C r4	size
	C r5	divisor
	C r6	carry

	mtctr	r4		C size

ifdef(`PIC',`
	rlwinm	r7, r5, 1,28,28	C (divisor << 1) & 8
	rlwinm	r8, r5, 2,28,28	C (divisor << 2) & 8

	xor	r7, r7, r8	C ((divisor << 1) ^ (divisor << 2)) & 8
	rlwinm	r4, r5, 0,28,31	C divisor low 4 bits, speedup mullw

	xor	r4, r4, r7	C inverse, 4 bits

	mullw	r7, r4, r4	C i*i
	slwi	r4, r4, 1	C 2*i

	rlwinm	r8, r5, 0,24,31	C divisor low 8 bits, speedup mullw

	mullw	r7, r7, r8	C i*i*d

	C

	sub	r4, r4, r7	C inverse, 8 bits
',`
	dnl  non-PIC

	lis	r7, modlimb_invert_table@ha
	rlwinm	r4, r5, 31,25,31 C (divisor/2) & 0x7F

	la	r7, modlimb_invert_table@l(r7)

	lbzx	r4, r4,r7	C inverse, 8 bits
')

	mullw	r7, r4, r4	C i*i
	slwi	r4, r4, 1	C 2*i

	rlwinm	r8, r5, 0,16,31	C divisor low 16 bits, speedup mullw

	mullw	r7, r7, r8	C i*i*d

	C

	sub	r4, r4, r7	C inverse, 16 bits

	mullw	r7, r4, r4	C i*i
	slwi	r4, r4, 1	C 2*i

	C

	mullw	r7, r7, r5	C i*i*d
	lwz	r0, 0(r3)	C src[0]

	C

	sub	r4, r4, r7	C inverse, 32 bits
	ASSERT(eq,`
	mullw	r10, r4, r5
	cmpwi	cr0, r10, 1
	')
	subfc	r7, r6, r0	C l = src[0] - carry

	mullw	r7, r7, r4	C q = l * inverse
	bdz	L(one)

	lwzu	r0, 4(r3)	C src[1]

	C

	mulhwu	r6, r7, r5	C carry = high(q*divisor)

	C

	subfe	r7, r6, r0	C l = src[1] - carry
	bdz	L(two)


L(top):
	C r0
	C r3	src, incrementing
	C r4	inverse
	C r5	divisor
	C r6
	C r7	l

	mullw	r7, r7, r4	C q = l * inverse
	lwzu	r0, 4(r3)	C src[i]

	C

	mulhwu	r6, r7, r5	C carry = high(q*divisor)

	C

	subfe	r7, r6, r0	C l = src[i] - carry
	bdnz	L(top)


L(two):
	mullw	r7, r7, r4	C q = l * inverse

L(one):
	subfe	r3, r3, r3	C ca 0 or -1
	mulhwu	r6, r7, r5	C carry = high(q*divisor)

	subf	r3, r3, r6	C carry + ca
	blr

EPILOGUE(mpn_modexact_1c_odd)
EPILOGUE(mpn_modexact_1_odd)
