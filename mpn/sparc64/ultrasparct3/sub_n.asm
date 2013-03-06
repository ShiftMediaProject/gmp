dnl  SPARC v9 mpn_sub_n for T3/T4.

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
C UltraSPARC T3:	 10
C UltraSPARC T4:	 4

C INPUT PARAMETERS
define(`rp', `%o0')
define(`up', `%o1')
define(`vp', `%o2')
define(`n',  `%o3')
define(`cy', `%o4')

ASM_START()
	REGISTER(%g2,#scratch)
	REGISTER(%g3,#scratch)
PROLOGUE(mpn_sub_nc)
	ba,pt	%xcc, L(ent)
	 xor	cy, 1, cy
EPILOGUE()
PROLOGUE(mpn_sub_n)
	mov	1, cy
L(ent):
	subcc	n, 1, n
	be	L(final_one)
	 cmp	%g0, cy
L(top):
	ldx	[vp+0], %o5
	add	vp, 16, vp
	ldx	[up+0], %o4
	add	up, 16, up
	ldx	[vp-8], %g2
	sub	n, 2, n
	ldx	[up-8], %g1
	add	rp, 16, rp
	xnor	%o5, %g0, %o5
	addxccc	%o4, %o5, %g3
	stx	%g3, [rp-16]
	xnor	%g2, %g0, %g2
	addxccc	%g1, %g2, %g2
	brgz	n, L(top)
	 stx	%g2, [rp-8]

	brlz,pt	n, L(done)
	 nop

L(final_one):
	ldx	[up+0], %o4
	ldx	[vp+0], %o5
	xnor	%o5, %g0, %o5
	addxccc	%o4, %o5, %g3
	stx	%g3, [rp+0]

L(done):
	clr	%o0
	retl
	 movcc	%xcc, 1, %o0
EPILOGUE()
