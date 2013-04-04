dnl  SPARC T3/T4/T5 mpn_modexact_1c_odd.

dnl  Contributed to the GNU project by Torbjörn Granlund.

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

C                  cycles/limb
C UltraSPARC T3:	 ?
C UltraSPARC T4/T5:	 ?

C INPUT PARAMETERS
define(`ap',  `%o0')
define(`n',   `%o1')
define(`d',   `%o2')
define(`cy',  `%o3')

define(`dinv',`%o5')
define(`a0',  `%g1')

ASM_START()
        REGISTER(%g2,#scratch)
        REGISTER(%g3,#scratch)
PROLOGUE(mpn_modexact_1c_odd)
	srlx	d, 1, %g1
	and	%g1, 127, %g1

ifdef(`PIC',`
	save	%sp, -192, %sp
	sethi	%hi(_GLOBAL_OFFSET_TABLE_-4), %l7
	call	L(GETPC0)
	add	%l7, %lo(_GLOBAL_OFFSET_TABLE_+4), %l7
	sethi	%hi(binvert_limb_table), %g2
	or	%g2, %lo(binvert_limb_table), %g2
	ldx	[%l7+%g2], %g2
	restore
',`
	sethi	%hh(binvert_limb_table), %g3
	or	%g3, %hm(binvert_limb_table), %g3
	sllx	%g3, 32, %g3
	sethi	%lm(binvert_limb_table), %g2
	add	%g3, %g2, %g3
	or	%g3, %lo(binvert_limb_table), %g2
')
	ldub	[%g2+%g1], %g1
	add	%g1, %g1, %g2
	mulx	%g1, %g1, %g1
	mulx	%g1, d, %g1
	sub	%g2, %g1, %g2
	add	%g2, %g2, %g1
	mulx	%g2, %g2, %g2
	mulx	%g2, d, %g2
	sub	%g1, %g2, %g1
	add	%g1, %g1, %o5
	mulx	%g1, %g1, %g1
	mulx	%g1, d, %g1
	sub	%o5, %g1, dinv
	add	n, -1, n

L(top):	ldx	[ap], a0
	add	ap, 8, ap
	subcc	a0, cy, %g3
	mulx	%g3, dinv, %g5
	umulxhi(d, %g5, %g5)
	addxc(	%g5, %g0, cy)
	brnz,pt	n, L(top)
	 add	n, -1, n

	retl
	 mov	cy, %o0

ifdef(`PIC',`
L(GETPC0):
	retl
	add	%o7, %l7, %l7
')
EPILOGUE()
