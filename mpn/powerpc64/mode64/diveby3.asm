dnl  PowerPC-64 mpn_divexact_by3 -- mpn by 3 exact division

dnl  Copyright 2002, 2003, 2005, 2006 Free Software Foundation, Inc.

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

C		cycles/limb
C POWER3/PPC630:     13
C POWER4/PPC970:     16
C POWER5:	     16

C INPUT PARAMETERS
define(`rp', `r3')
define(`up', `r4')
define(`n', `r5')
define(`cy', `r6')

C void mpn_divexact_by3 (mp_ptr rp, mp_srcptr up, mp_size_t n);
C
C mulld has the up[] limb in the second operand, since there's at least a
C chance of it giving an early-out on ppc630, which the inverse 0xAA..AB
C will never give.
C
C mulhdu has the "3" multiplier in the second operand, which is an early-out
C for ppc630.

ASM_START()
PROLOGUE(mpn_divexact_by3c)

	mtctr	n		C size
	ld	r7, 0(up)	C up[0]

	li	r5, -0x5556	C 0xFFFFFFFFFFFFAAAA
	rldimi	r5, r5, 16, 32	C 0xFFFFFFFFAAAAAAAA
	rldimi	r5, r5, 32, 63	C 0xAAAAAAAAAAAAAAAB = 1/3

	subi	r3, r3, 8	C adjust rp for first stdu

	li	r0, 3		C multiplier 3

	subfc	r7, cy, r7	C l = up[0] - carry
	bdz	L(one)

	ALIGN(8)
L(top):	mulld	r8, r5, r7	C q = l * inverse
	nop
	ldu	r7, 8(up)	C up[i]

	mulhdu	r6, r8, r0	C c = high(3*q)
	stdu	r8, 8(rp)	C rp[i-1] = q

	subfe	r7, r6, r7	C l = s - carry
	bdnz	L(top)

L(one):	subfe	r4, r4, r4	C 0 or -1

	mulld	r8, r7, r5	C q = l * inverse

	mulhdu	r6, r8, r0	C c = high(3*q)
	std	r8, 8(rp)	C rp[i] = q

	subf	r3, r4, r6	C carry + ca

	blr

EPILOGUE()
