dnl  ARM mpn_mul_1 -- Multiply a limb vector with a limb and store the result
dnl  in a second limb vector.
dnl  Contributed by Robert Harley.

dnl  Copyright 1998, 2000, 2001 Free Software Foundation, Inc.

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

C This runs at 7.75 cycles/limb in the StrongARM.

define(`rp',`r0')
define(`up',`r1')
define(`n',`r2')
define(`v',`r3')


ASM_START()
PROLOGUE(mpn_mul_1)
	stmfd	sp!, { r8, r9, lr }
	ands	ip, n, #1
	beq	L(skip1)
	ldr	lr, [up], #4
	umull	r9, ip, v, lr
	str	r9, [rp], #4
L(skip1):
	tst	n, #2
	beq	L(skip2)
	mov	r8, ip
	ldmia	up!, { ip, lr }
	mov	r9, #0
	umlal	r8, r9, v, ip
	mov	ip, #0
	umlal	r9, ip, v, lr
	stmia	rp!, { r8, r9 }
L(skip2):
	bics	n, n, #3
	beq	L(return)
	stmfd	sp!, { r6, r7 }
L(mul_1_loop):
	mov	r6, ip
	ldmia	up!, { r8, r9, ip, lr }
	ldr	r7, [rp, #12]			C cache allocate
	mov	r7, #0
	umlal	r6, r7, v, r8
	mov	r8, #0
	umlal	r7, r8, v, r9
	mov	r9, #0
	umlal	r8, r9, v, ip
	mov	ip, #0
	umlal	r9, ip, v, lr
	subs	n, n, #4
	stmia	rp!, { r6, r7, r8, r9 }
	bne	L(mul_1_loop)
	ldmfd	sp!, { r6, r7 }
L(return):
	mov	r0, ip
	ldmfd	sp!, { r8, r9, pc }
EPILOGUE(mpn_mul_1)
