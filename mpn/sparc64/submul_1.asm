dnl  SPARC v9 64-bit mpn_submul_1 -- Multiply a limb vector with a limb and
dnl  subtract the result from a second limb vector.

dnl  Copyright 2001, 2002 Free Software Foundation, Inc.

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

C INPUT PARAMETERS
C rp	i0
C up	i1
C n	i2
C v	i3

ASM_START()
	REGISTER(%g2,#scratch)

PROLOGUE(mpn_submul_1)
	save	%sp,-176,%sp

	sllx	%i2, 3, %g2
	or	%g0, %i1, %o1
	add	%g2, 15, %o0
	or	%g0, %i2, %o2
	and	%o0, -16, %o0
	sub	%sp, %o0, %sp
	add	%sp, 2223, %o0
	or	%g0, %o0, %l0
	call	mpn_mul_1
	or	%g0, %i3, %o3
	or	%g0, %o0, %l1		C preserve carry value from mpn_mul_1
	or	%g0, %i0, %o0
	or	%g0, %i0, %o1
	or	%g0, %l0, %o2
	call	mpn_sub_n
	or	%g0, %i2, %o3
	ret
	restore	%l1, %o0, %o0		C sum carry values
EPILOGUE(mpn_submul_1)
