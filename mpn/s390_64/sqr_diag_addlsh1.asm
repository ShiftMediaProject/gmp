dnl  S/390-64 mpn_sqr_diag_addlsh1

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
C z900		 ?
C z990           14.5
C z9		 ?
C z10		 ?
C z196		 ?

C INPUT PARAMETERS
define(`rp',	`%r2')
define(`tp',	`%r3')
define(`up',	`%r4')
define(`n',	`%r5')

ASM_START()
PROLOGUE(mpn_sqr_diag_addlsh1)
	stmg	%r6, %r9, 48(%r15)

	lghi	%r9, -1			C save non-carry state

	lg	%r1, 0(up)
	mlgr	%r0, %r1
	aghi	n, -1
	lg	%r7, 0(tp)
	algr	%r7, %r7
	lghi	%r6, 0
	j	L(mid)

L(top):	lmg	%r6, %r7, 8(tp)
	la	tp, 16(tp)
	alcgr	%r6, %r6
	alcgr	%r7, %r7
L(mid):	slbgr	%r8, %r8		C save carry
	aghi	%r9, 1			C restore old carry
	alcgr	%r6, %r1
	alcgr	%r7, %r0
	stmg	%r6, %r7, 0(rp)
	la	rp, 16(rp)
	lg	%r1, 8(up)
	la	up, 8(up)
	lgr	%r9, %r8		C copy carry save register
	mlgr	%r0, %r1
	brctg	n, L(top)

L(end):	lg	%r6, 8(tp)
	alcgr	%r6, %r6
	slbgr	%r8, %r8		C save carry
	aghi	%r9, 1			C restore old carry
	lghi	%r7, 1
	alcgr	%r6, %r1
	alcgr	%r7, %r0
	algr	%r7, %r8
	stmg	%r6, %r7, 0(rp)

	lmg	%r6, %r9, 48(%r15)
	br	%r14
EPILOGUE()
