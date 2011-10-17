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

C            cycles/limb
C z900		 6.5
C z990		 3.5
C z9		 ?
C z10		 ?
C z196		 ?

C TODO
C  * Optimise for small n
C  * Use r0 and save/restore one less register

C INPUT PARAMETERS
define(`rp',	`%r2')
define(`up',	`%r3')
define(`vp',	`%r4')
define(`n',	`%r5')

ASM_START()
PROLOGUE(mpn_add_n)
	stmg	%r6, %r12, 48(%r15)

	la	%r1, 3(n)
	lghi	%r7, 3
	srlg	%r1, %r1, 2
	ngr	%r7, n			C n mod 4
	je	L(top)			C The C flag is clear
	cghi	%r7, 2
	jl	L(b1)
	je	L(b2)

L(b3):	lmg	%r5, %r7, 0(up)
	la	up, 24(up)
	lmg	%r9, %r11, 0(vp)
	la	vp, 24(vp)
	algr	%r5, %r9
	alcgr	%r6, %r10
	alcgr	%r7, %r11
	stmg	%r5, %r7, 0(rp)
	la	rp, 24(rp)
	brctg	%r1, L(top)
	j	L(end)

L(b1):	lg	%r5, 0(up)
	la	up, 8(up)
	lg	%r9, 0(vp)
	la	vp, 8(vp)
	algr	%r5, %r9
	stg	%r5, 0(rp)
	la	rp, 8(rp)
	brctg	%r1, L(top)
	j	L(end)

L(b2):	lmg	%r5, %r6, 0(up)
	la	up, 16(up)
	lmg	%r9, %r10, 0(vp)
	la	vp, 16(vp)
	algr	%r5, %r9
	alcgr	%r6, %r10
	stmg	%r5, %r6, 0(rp)
	la	rp, 16(rp)
	brctg	%r1, L(top)
	j	L(end)

L(top):	lmg	%r5, %r8, 0(up)
	la	up, 32(up)
	lmg	%r9, %r12, 0(vp)
	la	vp, 32(vp)
	alcgr	%r5, %r9
	alcgr	%r6, %r10
	alcgr	%r7, %r11
	alcgr	%r8, %r12
	stmg	%r5, %r8, 0(rp)
	la	rp, 32(rp)
	brctg	%r1, L(top)

L(end):	lghi	%r2, 0
	alcgr	%r2, %r2

	lmg	%r6, %r12, 48(%r15)
	br	%r14
EPILOGUE()
