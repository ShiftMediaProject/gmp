dnl  ARM mpn_tabselect

dnl  Contributed to the GNU project by Torbjörn Granlund.

dnl  Copyright 2013 Free Software Foundation, Inc.

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
C StrongARM	 ?
C XScale	 ?
C Cortex-A8	 ?
C Cortex-A9	 3.33
C Cortex-A15	 3

C This is an OK core register implementation, with 3-way unrolling.  Software
C pipelining might be hard since we run out of registers.

define(`rp',    `r0')
define(`tp',    `r1')
define(`n',     `r2')
define(`nents', `r3')
C      which  on stack

ASM_START()
PROLOGUE(mpn_tabselect)
	push	{r4-r11, r14}
	ldr	r11, [sp, #36]
	sub	r11, r11, r3
L(outer):
	add	r7, r11, r3
	subs	r7, r7, #1
	sbc	r7, r7, r7
	mov	r6, r0
	mov	r14, r2

	subs	r14, r14, #3
	blt	L(end)

L(top):	subs	r14, r14, #3
	ldm	r6, {r4,r5,r8}
	ldmia	r1!, {r9,r10,r12}
	bic	r4, r4, r7
	bic	r5, r5, r7
	bic	r8, r8, r7
	and	r9, r9, r7
	and	r10, r10, r7
	and	r12, r12, r7
	orr	r9, r9, r4
	orr	r10, r10, r5
	orr	r12, r12, r8
	stmia	r6!, {r9,r10,r12}
	bge	L(top)

L(end):	cmp	r14, #-2
	bls	L(1)
	ldm	r6, {r4,r5}
	ldmia	r1!, {r9,r10}
	bic	r4, r4, r7
	bic	r5, r5, r7
	and	r9, r9, r7
	and	r10, r10, r7
	orr	r9, r9, r4
	orr	r10, r10, r5
	stmia	r6!, {r9,r10}
	b	L(2)

L(1):	bne	L(2)
	ldr	r4, [r6]
	ldr	r9, [r1], #4
	bic	r4, r4, r7
	and	r9, r9, r7
	orr	r9, r9, r4
	str	r9, [r6], #4

L(2):	subs	r3, r3, #1
	bne	L(outer)
	pop	{r4-r11, r14}
	bx	lr
EPILOGUE()
