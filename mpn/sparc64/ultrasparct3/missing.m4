dnl  SPARC v9-2011 simulation support.

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


dnl Usage addxccc(r1,r2,r3, t1)
dnl  64-bit add with carry-in and carry-out
dnl  FIXME: Uses scratch register g2

define(`addxccc',`dnl
	mov	0, %g2
	movcs	%xcc, -1, %g2
	addcc	%g2, 1, %g0
	addccc	$1, $2, $3
')


dnl Usage addxc(r1,r2,r3, t1,t2)
dnl  64-bit add with carry-in
dnl  FIXME: Uses scratch registers g2, g3

define(`addxc',`dnl
	mov	0, %g2
	rd	%ccr, %g3
	movcs	%xcc, -1, %g2
	addcc	%g2, 1, %g0
	addc	$1, $2, $3
	wr	%g3, 0, %ccr
')


dnl Usage umulxhi(r1,r2,r3)
dnl  64-bit multiply returning upper 64 bits
dnl  Calls __gmpn_umulh using a non-standard calling convention

define(`umulxhi',`dnl
	add	%sp, -512, %sp
	stx	$1, [%sp+2047+256]
	stx	$2, [%sp+2047+256+8]
	stx	%o7, [%sp+2047+256+16]
	call	__gmpn_umulh
	nop
	ldx	[%sp+2047+256+16], %o7
	ldx	[%sp+2047+256], $3
	sub	%sp, -512, %sp
')
