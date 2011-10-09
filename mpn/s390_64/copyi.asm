dnl  S/390-64 mpn_copyi

dnl  Copyright 2011 Free Software Foundation, Inc.

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

C            cycles/limb
C z990           1		fluctuates somewhat

C FIXME
C  * Optimise.  GNU memcpy beats us at 0.75 c/l.

C INPUT PARAMETERS
define(`rp',	`%r2')
define(`up',	`%r3')
define(`n',	`%r4')

ASM_START()
PROLOGUE(mpn_copyi)
	lgr	%r0, %r3
	sllg	%r3, %r4, 3
	sllg	%r1, %r4, 3

L(top):	mvcle	%r2, %r0, 0(0)
	jne	L(top)

	br	%r14
EPILOGUE()
