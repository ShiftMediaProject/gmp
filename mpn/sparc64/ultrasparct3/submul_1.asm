dnl  SPARC v9 mpn_submul_1 for T3/T4.

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
C UltraSPARC T3:	 29.5
C UltraSPARC T4:	 6.5

C INPUT PARAMETERS
define(`rp', `%i0')
define(`up', `%i1')
define(`n',  `%i2')
define(`v0', `%i3')

ASM_START()
	REGISTER(%g2,#scratch)
	REGISTER(%g3,#scratch)
	ALIGN(32)
PROLOGUE(mpn_submul_1)
	save	%sp, -176, %sp
	subcc	n, 1, n
	be	L(final_one)
	 clr	%o5

L(top):
	ldx	[up+0], %l0
	ldx	[rp+0], %l2
	ldx	[up+8], %l1
	ldx	[rp+8], %l3
	mulx	%l0, v0, %o0
	add	up, 16, up
	umulxhi	%l0, v0, %o1
	add	rp, 16, rp
	mulx	%l1, v0, %o2
	sub	n, 2, n
	umulxhi	%l1, v0, %o3
	addcc	%o5, %o0, %o0
	addxc	%g0, %o1, %o5
	subcc	%l2, %o0, %o0
	addxc	%g0, %o5, %o5
	stx	%o0, [rp-16]
	addcc	%o5, %o2, %o2
	addxc	%g0, %o3, %o5
	subcc	%l3, %o2, %o2
	addxc	%g0, %o5, %o5
	brgz	n, L(top)
	 stx	%o2, [rp-8]

	brlz,pt	n, L(done)
	 nop

L(final_one):
	ldx	[up+0], %l0
	ldx	[rp+0], %l2
	mulx	%l0, v0, %o0
	umulxhi	%l0, v0, %o1
	addcc	%o5, %o0, %o0
	addxc	%g0, %o1, %o5
	subcc	%l2, %o0, %o0
	addxc	%g0, %o5, %o5
	stx	%o0, [rp+0]

L(done):
	ret
	 restore %o5, 0, %o0
EPILOGUE()
