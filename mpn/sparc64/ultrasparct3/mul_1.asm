dnl  SPARC v9 mpn_mul_1 for T3/T4.

dnl  Copyright 2013 Free Software Foundation, Inc.

dnl  This file is part of the GNU MP Library.

dnl  The GNU MP Library is free software; you can redistribute it and/or modify
dnl  it under the terms of the GNU Lesser General Public License as published
dnl  by the Free Software Foundation; either version 3 of the License, or (at
dnl  your option) any later version.

dnl  The GNU MP Library is distributed in the hope that it will be useful, but
dnl  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
dnl  License for more details.

dnl  You should have received a copy of the GNU Lesser General Public License
dnl  along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.

include(`../config.m4')

C		   cycles/limb
C UltraSPARC T3:	 25.5
C UltraSPARC T4:	 3.8

C INPUT PARAMETERS
define(`rp', `%o0')
define(`up', `%o1')
define(`n',  `%o2')
define(`v0', `%o3')

ASM_START()
	REGISTER(%g2,#scratch)
	REGISTER(%g3,#scratch)
	ALIGN(32)
PROLOGUE(mpn_mul_1)
	subcc	n, 1, n
	be	L(final_one)
	 subcc	%g0, %g0, %o5

L(top):
	ldx	[up+0], %g1
	sub	n, 2, n
	ldx	[up+8], %o4
	mulx	%g1, v0, %g3
	add	up, 16, up
	umulxhi	%g1, v0, %g2
	mulx	%o4, v0, %g1
	add	rp, 16, rp
	addxccc	%g3, %o5, %g3
	umulxhi	%o4, v0, %o5
	stx	%g3, [rp-16]
	addxccc	%g1, %g2, %g1
	brgz	n, L(top)
	 stx	%g1, [rp-8]

	brlz,pt	n, L(done)
	 nop

L(final_one):
	ldx	[up+0], %g1
	mulx	%g1, v0, %g3
	addxccc	%g3, %o5, %g3
	umulxhi	%g1, v0, %o5
	stx	%g3, [rp+0]

L(done):
	retl
	 addxc	%g0, %o5, %o0
EPILOGUE()
