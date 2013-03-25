dnl  ARM mpn_addmul_1 optimised for A15.

dnl  Copyright 2012, 2013 Free Software Foundation, Inc.

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

C	     cycles/limb		best
C StrongARM:	 -
C XScale	 ?
C Cortex-A7	 ?
C Cortex-A8	 ?
C Cortex-A9	 6.5			3.25
C Cortex-A15	 3			this


C This runs well on A15 but very poorly on A9.  We have made no effort at
C improving its A9 performance, as doubling the speed seems hard.

C This is armv5 code, optimized for the armv7a cpu A15.  Its location in the
C GMP file structure might be misleading.


define(`rp', `r0')
define(`up', `r1')
define(`n',  `r2')
define(`v0', `r3')

ASM_START()
PROLOGUE(mpn_addmul_1)
	push	{r4-r8}

	adds	r0, r0, #0	C clear carry

	tst	n, #1
	beq	L(bx0)

L(bx1):	mov	r5, #0
	ldr	r8, [up], #4
	tst	n, #2
	beq	L(lo1)
	b	L(lo3)

L(bx0):	mov	r7, #0
	ldr	r8, [up], #4
	adds	r0, r0, #0
	tst	n, #2
	beq	L(lo0)
	b	L(lo2)

L(top):	ldr	r8, [up], #4
	str	r6, [rp, #-4]
L(lo0):	ldr	r4, [rp], #4
	mov	r5, #0
	umlal	r4, r5, r8, v0
	adds	r4, r4, r7
	ldr	r8, [up], #4
	str	r4, [rp, #-4]
L(lo3):	ldr	r6, [rp], #4
	mov	r7, #0
	umlal	r6, r7, r8, v0
	adcs	r6, r6, r5
	ldr	r8, [up], #4
	str	r6, [rp, #-4]
L(lo2):	ldr	r4, [rp], #4
	mov	r5, #0
	umlal	r4, r5, r8, v0
	adcs	r4, r4, r7
	ldr	r8, [up], #4
	str	r4, [rp, #-4]
L(lo1):	ldr	r6, [rp], #4
	mov	r7, #0
	umlal	r6, r7, r8, v0
	adcs	r6, r6, r5
	adc	r7, r7, #0
	subs	n, n, #4
	bgt	L(top)

	str	r6, [rp, #-4]
	mov	r0, r7
	pop	{r4-r8}
	bx	lr
EPILOGUE()
