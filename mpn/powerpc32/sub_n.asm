dnl  PowerPC 750 mpn_sub_n -- subtract limb vectors.

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


C       cycles/limb
C 604e:     4.0
C 750:      4.0


C mp_limb_t mpn_sub_n (mp_ptr dst, mp_srcptr src1, mp_srcptr src2,
C                      mp_size_t size);
C
C Same style as mpn_add_n.

ASM_START()
PROLOGUE(mpn_sub_n)

	C r3	wp
	C r4	xp
	C r5	yp
	C r6	size

	mtctr	r6		C size
	lwz	r6, 0(r4)	C xp[0]

	lwz	r7, 0(r5)	C yp[0]
	sub	r4, r4, r3	C xp-wp

	sub	r5, r5, r3	C yp-wp
	addi	r4, r4, 4	C xp-wp-4

	addi	r5, r5, 4	C yp-wp-4
	subfc	r6, r7, r6

	stw	r6, 0(r3)	C wp[0]
	bdz	L(done)


L(top):
	C r3	wp, incrementing
	C r4	xp-wp+4
	C r5	yp-wp+4
	C r6	scratch
	C r7	scratch

	lwzx	r6, r4,r3	C xp[i]

	lwzx	r7, r5,r3	C yp[i]

	C serialize
	subfe	r6, r7, r6

	stwu	r6, 4(r3)	C wp[i]
	bdnz	L(top)


L(done):
	subfe	r3, r0, r0	C 0 or -1

	subfic	r3, r3, 0	C 0 or 1
	blr

EPILOGUE()
