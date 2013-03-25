dnl  SPARC v9 mpn_hamdist for T3/T4.

dnl  Contributed to the GNU project by David Miller.

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
C UltraSPARC T3:	 ?
C UltraSPARC T4:	 3.5

C INPUT PARAMETERS
define(`up',   `%o0')
define(`vp',   `%o1')
define(`n',    `%o2')
define(`pcnt', `%o5')

ASM_START()
	REGISTER(%g2,#scratch)
	REGISTER(%g3,#scratch)
PROLOGUE(mpn_hamdist)
	subcc	n, 1, n
	be	L(final_one)
	 clr	pcnt
L(top):
	ldx	[up + 0], %g1
	ldx	[vp + 0], %g2
	ldx	[up + 8], %o4
	ldx	[vp + 8], %g3
	sub	n, 2, n
	xor	%g1, %g2, %g1
	add	up, 16, up
	popc	%g1, %g2
	add	vp, 16, vp
	xor	%o4, %g3, %o4
	add	pcnt, %g2, pcnt
	popc	%o4, %g3
	brgz	n, L(top)
	 add	pcnt, %g3, pcnt
	brlz,pt	n, L(done)
	 nop
L(final_one):
	ldx	[up + 0], %g1
	ldx	[vp + 0], %g2
	xor	%g1,%g2, %g1
	popc	%g1, %g2
	add	pcnt, %g2, pcnt
L(done):
	retl
	 mov	pcnt, %o0
EPILOGUE()
