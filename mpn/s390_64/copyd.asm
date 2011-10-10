dnl  S/390-64 mpn_copyd

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
C z990           1.5

C FIXME:
C  * Avoid saving/restoring callee-saves registers for n < 3.  This could be
C    done by setting rp=r1, up=r2, i=r0 and r3,r4,r5 for clock regs.
C    We coould then use r3...r10 in main loop.

C INPUT PARAMETERS
define(`rp_param',	`%r2')
define(`up_param',	`%r3')
define(`n',		`%r4')

define(`rp',	`%r8')
define(`up',	`%r9')

ASM_START()
PROLOGUE(mpn_copyd)
	stmg	%r6, %r10, 48(%r15)

	sllg	%r1, n, 3
	la	%r10, 7(n)
	srlg	%r10, %r10, 3

	lay	rp, -64(%r1,rp_param)
	lay	up, -64(%r1,up_param)

	lghi	%r7, 7
	ngr	%r7, n			C n mod 8
	cghi	%r7, 2
	jh	L(b34567)
	cghi	%r7, 1
	je	L(b1)
	jh	L(b2)

L(b0):	j	L(top)

L(b1):	lg	%r0, 56(up)
	lay	up, -8(up)
	stg	%r0, 56(rp)
	lay	rp, -8(rp)
	brctg	%r10, L(top)
	j	L(end)

L(b2):	lmg	%r0, %r1, 48(up)
	lay	up, -16(up)
	stmg	%r0, %r1, 48(rp)
	lay	rp, -16(rp)
	brctg	%r10, L(top)
	j	L(end)

L(b34567):
	cghi	%r7, 4
	jl	L(b3)
	je	L(b4)
	cghi	%r7, 6
	je	L(b6)
	jh	L(b7)

L(b5):	lmg	%r0, %r4, 24(up)
	lay	up, -40(up)
	stmg	%r0, %r4, 24(rp)
	lay	rp, -40(rp)
	brctg	%r10, L(top)
	j	L(end)

L(b3):	lmg	%r0, %r2, 40(up)
	lay	up, -24(up)
	stmg	%r0, %r2, 40(rp)
	lay	rp, -24(rp)
	brctg	%r10, L(top)
	j	L(end)

L(b4):	lmg	%r0, %r3, 32(up)
	lay	up, -32(up)
	stmg	%r0, %r3, 32(rp)
	lay	rp, -32(rp)
	brctg	%r10, L(top)
	j	L(end)

L(b6):	lmg	%r0, %r5, 16(up)
	lay	up, -48(up)
	stmg	%r0, %r5, 16(rp)
	lay	rp, -48(rp)
	brctg	%r10, L(top)
	j	L(end)

L(b7):	lmg	%r0, %r6, 8(up)
	lay	up, -56(up)
	stmg	%r0, %r6, 8(rp)
	lay	rp, -56(rp)
	brctg	%r10, L(top)
	j	L(end)

L(top):	lmg	%r0, %r7, 0(up)
	lay	up, -64(up)
	stmg	%r0, %r7, 0(rp)
	lay	rp, -64(rp)
	brctg	%r10, L(top)

L(end):	lmg	%r6, %r10, 48(%r15)
	br	%r14
EPILOGUE()
