dnl  ARM mpn_mul_1.

dnl  Copyright 2012 Free Software Foundation, Inc.

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

C	     cycles/limb
C StrongARM:	 -
C XScale	 -
C Cortex-A8	 ?
C Cortex-A9	 3.25
C Cortex-A15	 ?

C TODO
C  * Micro-optimise feed-in code.
C  * Optimise for n=1,2 by delaying register saving.
C  * Try using ldm/stm.
C  * Performance degenerates to 4.49 c/l for some alignments.

define(`rp',`r0')
define(`up',`r1')
define(`n', `r2')
define(`v0',`r3')

ASM_START()
PROLOGUE(mpn_mul_1)
	stmfd	sp!, { r4, r5, r6, r7, r8 }

	ands	r12, n, #3
	beq	L(fi0)
	cmp	r12, #2
	bcc	L(fi1)
	beq	L(fi2)

L(fi3):	ldr	r4, [up], #4
	ldr	r5, [up], #4
	umull	r8, r12, r4, v0
	b	L(lo3)

L(fi0):	ldr	r5, [up], #4
	ldr	r4, [up], #4
	umull	r7, r8, r5, v0
	add	rp, rp, #4
	b	L(lo0)

L(fi1):	ldr	r4, [up], #4
	subs	n, n, #1
	beq	L(1)
	ldr	r5, [up], #4
	umull	r6, r7, r4, v0
	add	rp, rp, #8
	b	L(lo1)

L(fi2):	ldr	r5, [up], #4
	ldr	r4, [up], #4
	umull	r12, r6, r5, v0
	subs	n, n, #2
	add	rp, rp, #12
	beq	L(end)

	ALIGN(16)
L(top):	ldr	r5, [up], #4
	mov	r7, #0
	umaal	r6, r7, r4, v0
	str	r12, [rp, #-12]
L(lo1):	ldr	r4, [up], #4
	mov	r8, #0
	umaal	r7, r8, r5, v0
	str	r6, [rp, #-8]
L(lo0):	ldr	r5, [up], #4
	mov	r12, #0
	umaal	r8, r12, r4, v0
	str	r7, [rp, #-4]
L(lo3):	subs	n, n, #4
	ldr	r4, [up], #4
	mov	r6, #0
	umaal	r12, r6, r5, v0
	str	r8, [rp], #16
	bhi	L(top)

L(end):	mov	r7, #0
	umaal	r6, r7, r4, v0
	str	r12, [rp, #-12]
	str	r6, [rp, #-8]
	mov	r0, r7
	ldmfd	sp!, { r4, r5, r6, r7, r8 }
	bx	lr

L(1):	umull	r6, r7, r4, v0
	str	r6, [rp, #0]
	mov	r0, r7
	ldmfd	sp!, { r4, r5, r6, r7, r8 }
	bx	lr
EPILOGUE()
