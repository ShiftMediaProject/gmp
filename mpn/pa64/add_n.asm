dnl  HP-PA 2.0 mpn_add_n -- Add two limb vectors of the same length > 0 and
dnl  store sum in a third limb vector.

dnl  Copyright 1997, 2000, 2002 Free Software Foundation, Inc.

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


dnl  This runs at 2 cycles/limb on PA8000 and 1.875 cycles/limb.  It should be
dnl  possible to do 1.5 cycles/limb for both processors, perhaps with some
dnl  latency scheduling.

include(`../config.m4')

dnl INPUT PARAMETERS
define(`rp',`%r26')
define(`up',`%r25')
define(`vp',`%r24')
define(`n',`%r23')

ifdef(`HAVE_ABI_2_0w',
`       .level  2.0W
',`     .level  2.0N
')
PROLOGUE(mpn_add_n)
	sub		%r0, n, %r22
	depw,z		%r22, 30, 3, %r28	C r28 = 2 * (-n & 7)
	depw,z		%r22, 28, 3, %r22	C r22 = 8 * (-n & 7)
	sub		up, %r22, up		C offset up
	sub		vp, %r22, vp		C offset vp
	sub		rp, %r22, rp		C offset rp
	blr		%r28, %r0		C branch into loop
	add		%r0, %r0, %r0		C reset carry

L(loop)	ldd		0(up), %r20
	ldd		0(vp), %r31
	add,dc		%r20, %r31, %r20
	std		%r20, 0(rp)
L(7)	ldd		8(up), %r21
	ldd		8(vp), %r19
	add,dc		%r21, %r19, %r21
	std		%r21, 8(rp)
L(6)	ldd		16(up), %r20
	ldd		16(vp), %r31
	add,dc		%r20, %r31, %r20
	std		%r20, 16(rp)
L(5)	ldd		24(up), %r21
	ldd		24(vp), %r19
	add,dc		%r21, %r19, %r21
	std		%r21, 24(rp)
L(4)	ldd		32(up), %r20
	ldd		32(vp), %r31
	add,dc		%r20, %r31, %r20
	std		%r20, 32(rp)
L(3)	ldd		40(up), %r21
	ldd		40(vp), %r19
	add,dc		%r21, %r19, %r21
	std		%r21, 40(rp)
L(2)	ldd		48(up), %r20
	ldd		48(vp), %r31
	add,dc		%r20, %r31, %r20
	std		%r20, 48(rp)
L(1)	ldd		56(up), %r21
	ldo		64(up), up
	ldd		56(vp), %r19
	add,dc		%r21, %r19, %r21
	std		%r21, 56(rp)
	ldo		64(vp), vp
	addib,>		-8, n, L(loop)
	ldo		64(rp), rp

	add,dc		%r0, %r0, %r29
	bve		(%r2)
ifdef(`HAVE_ABI_2_0w',
`	copy		%r29, %r28
',`	ldi		0, %r28
')
EPILOGUE(mpn_add_n)
