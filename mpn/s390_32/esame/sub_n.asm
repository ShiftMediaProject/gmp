dnl  S/390-32 mpn_sub_n

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
PROLOGUE(mpn_sub_n)
	stm	%r6, %r12, 24(%r15)

	la	%r1, 3(n)
	lhi	%r7, 3
	srl	%r1, 2
	nr	%r7, n			C n mod 4
	je	L(b0)
	chi	%r7, 2
	jl	L(b1)
	je	L(b2)

L(b3):	lm	%r5, %r7, 0(up)
	la	up, 12(up)
	lm	%r9, %r11, 0(vp)
	la	vp, 12(vp)
	slr	%r5, %r9
	slbr	%r6, %r10
	slbr	%r7, %r11
	stm	%r5, %r7, 0(rp)
	la	rp, 12(rp)
	brct	%r1, L(top)
	j	L(end)

L(b0):	slr	%r5, %r5		C set C flag
	j	L(top)
	
L(b1):	l	%r5, 0(up)
	la	up, 4(up)
	l	%r9, 0(vp)
	la	vp, 4(vp)
	slr	%r5, %r9
	st	%r5, 0(rp)
	la	rp, 4(rp)
	brct	%r1, L(top)
	j	L(end)

L(b2):	lm	%r5, %r6, 0(up)
	la	up, 8(up)
	lm	%r9, %r10, 0(vp)
	la	vp, 8(vp)
	slr	%r5, %r9
	slbr	%r6, %r10
	stm	%r5, %r6, 0(rp)
	la	rp, 8(rp)
	brct	%r1, L(top)
	j	L(end)

L(top):	lm	%r5, %r8, 0(up)
	la	up, 16(up)
	lm	%r9, %r12, 0(vp)
	la	vp, 16(vp)
	slbr	%r5, %r9
	slbr	%r6, %r10
	slbr	%r7, %r11
	slbr	%r8, %r12
	stm	%r5, %r8, 0(rp)
	la	rp, 16(rp)
	brct	%r1, L(top)

L(end):	slbr	%r2, %r2
	lcr	%r2, %r2

	lm	%r6, %r12, 24(%r15)
	br	%r14
EPILOGUE()
