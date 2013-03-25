dnl  ARM mpn_mul_1 optimised for A15.

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
C Cortex-A9	 5.25			3.25
C Cortex-A15	 2.25			this


C This runs well on A15 but very poorly on A9.  By scheduling loads and adds
C it is possible to get good A9 performance as well, but at the cost of using
C many more (callee-saves) registers.

C This is armv5 code, optimized for the armv7a cpu A15.  Its location in the
C GMP file structure might be misleading.


define(`rp', `r0')
define(`up', `r1')
define(`n',  `r2')
define(`v0', `r3')

ASM_START()
PROLOGUE(mpn_mul_1c)
	ldr	r12, [sp]
	b	L(ent)
EPILOGUE()
PROLOGUE(mpn_mul_1)
	mov	r12, #0
L(ent):	push	{r4-r7}

	ldr	r6, [up], #4
	tst	n, #1
	beq	L(bx0)

L(bx1):	umull	r4, r7, r6, v0
	adds	r4, r4, r12
	tst	n, #2
	beq	L(lo1)
	b	L(lo3)

L(bx0):	umull	r4, r5, r6, v0
	adds	r4, r4, r12
	tst	n, #2
	beq	L(lo0)
	b	L(lo2)

L(top):	ldr	r6, [up], #4
	str	r4, [rp], #4
	umull	r4, r5, r6, v0
	adds	r4, r4, r7
L(lo0):	ldr	r6, [up], #4
	str	r4, [rp], #4
	umull	r4, r7, r6, v0
	adcs	r4, r4, r5
L(lo3):	ldr	r6, [up], #4
	str	r4, [rp], #4
	umull	r4, r5, r6, v0
	adcs	r4, r4, r7
L(lo2):	ldr	r6, [up], #4
	str	r4, [rp], #4
	umull	r4, r7, r6, v0
	adcs	r4, r4, r5
L(lo1):	adc	r7, r7, #0
	subs	n, n, #4
	bgt	L(top)

	str	r4, [rp]
	mov	r0, r7
	pop	{r4-r7}
	bx	lr
EPILOGUE()
