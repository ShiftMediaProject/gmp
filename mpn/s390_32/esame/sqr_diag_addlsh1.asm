dnl  S/390-32 mpn_sqr_diag_addlsh1

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
C z990           8

C INPUT PARAMETERS
define(`rp',	`%r2')
define(`tp',	`%r3')
define(`up',	`%r4')
define(`n',	`%r5')

ASM_START()
PROLOGUE(mpn_sqr_diag_addlsh1)
	stm	%r6, %r11, 24(%r15)

	lhi	%r11, -1		C save non-carry state

	l	%r1, 0(up)
	mlr	%r0, %r1
	ahi	n, -1
	l	%r7, 0(tp)
	alr	%r7, %r7
	lhi	%r6, 0
	j	L(mid)

L(top):	lm	%r6, %r7, 4(tp)
	la	tp, 8(tp)
	alcr	%r6, %r6
	alcr	%r7, %r7
L(mid):	slbr	%r10, %r10		C save carry
	ahi	%r11, 1			C restore old carry
	alcr	%r6, %r1
	alcr	%r7, %r0
	stm	%r6, %r7, 0(rp)
	la	rp, 8(rp)
	l	%r1, 4(up)
	la	up, 4(up)
	lr	%r11, %r10		C copy carry save register
	mlr	%r0, %r1
	brct	n, L(top)

L(end):	l	%r6, 4(tp)
	alcr	%r6, %r6
	slbr	%r10, %r10		C save carry
	ahi	%r11, 1			C restore old carry
	lhi	%r7, 1
	alcr	%r6, %r1
	alcr	%r7, %r0
	alr	%r7, %r10
	stm	%r6, %r7, 0(rp)

	lm	%r6, %r11, 24(%r15)
	br	%r14
EPILOGUE()
