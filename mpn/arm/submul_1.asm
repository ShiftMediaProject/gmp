dnl  ARM mpn_submul_1 -- Multiply a limb vector with a limb and subtract the
dnl  result from a second limb vector.
dnl  Based on mpn_addmul_1, which was contributed by Robert Harley.

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

C This runs at 9.75 cycles/limb in the StrongARM.

C Could use some register cleanup.  Some fewer registers might be needed, or
C r11 could be utilized for better speed.  Could avoid saving all registers for
C small (n <= 3) operands.

define(`rp',`r0')
define(`up',`r1')
define(`n',`r2')
define(`v',`r3')

ASM_START()
PROLOGUE(mpn_submul_1)
	stmfd	sp!, { r4-r10, lr }
	mov	r4, #0
	movs	n, n, lsr #1
	bcc	L(skip1)
	ldr	lr, [up], #4
	umull	r4, r12, v, lr
	ldr	r6, [rp]
	subs	r6, r6, r4
	sbc	r4, r0, r0
	sub	r4, r12, r4
	str	r6, [rp], #4
L(skip1):
	movs	n, n, lsr #1
	bcc	L(skip2)
	ldmia	up!, { r9, r10 }
	mov	r5, #0
	umlal	r4, r5, v, r9
	mov	r9, #0
	umlal	r5, r9, v, r10
	ldmia	rp, { r6, r7 }
	subs	r6, r6, r4
	sbcs	r7, r7, r5
	sbc	r4, r0, r0
	sub	r4, r9, r4
	stmia	rp!, { r6, r7 }
L(skip2):
	teq	n, #0
	beq	L(return)

L(submul_loop):
	ldmia	up!, { r9, r10, r12, lr }
	mov	r5, #0
	umlal	r4, r5, v, r9
	mov	r9, #0
	umlal	r5, r9, v, r10
	mov	r10, #0
	umlal	r9, r10, v, r12
	mov	r12, #0
	umlal	r10, r12, v, lr
	ldmia	rp, { r6, r7, r8, lr }
	subs	r6, r6, r4
	sbcs	r7, r7, r5
	sbcs	r8, r8, r9
	sbcs	lr, lr, r10
	sbc	r4, r0, r0
	sub	r4, r12, r4
	subs	n, n, #1
	stmia	rp!, { r6, r7, r8, lr }
	bne	L(submul_loop)
L(return):
	mov	r0, r4
	ldmfd	sp!, { r4-r10, pc }
EPILOGUE(mpn_submul_1)
