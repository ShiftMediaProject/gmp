dnl  S/390-64 mpn_add_n

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

C INPUT PARAMETERS
define(`rp',	`%r2')
define(`up',	`%r3')
define(`vp',	`%r4')
define(`n',	`%r5')

ASM_START()
PROLOGUE(mpn_add_n)
	stg	%r12, 96(%r15)
	lghi	%r12, 0			C zero index register
	aghi	%r12, 0			C clear C flag

L(top):	lg	%r0, 0(%r12,up)
	lg	%r1, 0(%r12,vp)
	alcgr	%r0, %r1
	stg	%r0, 0(%r12,rp)
	la	%r12, 8(%r12)
	brctg	n, L(top)

	lghi	%r2, 0
	alcgr	%r2, %r2

	lg	%r12, 96(%r15)
	br	%r14
EPILOGUE()
