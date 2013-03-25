dnl  ARM Neon mpn_tabselect

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
C StrongARM	 -
C XScale	 -
C Cortex-A7	 ?
C Cortex-A8	 ?
C Cortex-A9	 2.25
C Cortex-A15	 0.95

C This is a basic implementation using 64-bit Neon, with shallow software
C pipelining and no unrolling.  It is probably close to optimal for A9, while
C 128-bit Neon code runs significantly faster on A15.

define(`rp',    `r0')
define(`tabp',  `r1')
define(`n',     `r2')
define(`nents', `r3')
C      which  on stack

define(`mask',  `r6')
define(`maskv', `d6')
C
ASM_START()
PROLOGUE(mpn_tabselect)
	push	{r4-r7}
	ldr	r7, [sp, #16]
	sub	r7, r7, nents
L(outer):
	add	mask, r7, nents
	subs	mask, mask, #1
	sbc	mask, mask, mask
	mov	r4, rp
	mov	r5, rp
	mov	r12, n

	vdup.32	maskv, mask

	tst	rp, #4			C Is rp 64-bit aligned?
	beq	L(ali)			C Yes, skip!
	vld1.32	{d4[0]}, [tabp]!	C Else perform one 32-bit...
	vld1.32	{d0[0]}, [r4]!		C ...operation in...
	vbit	d0, d4, maskv		C ...order to make...
	subs	r12, r12, #1		C ...aligned operations...
	vst1.32	{d0[0]}, [r5]!		C ...on rp[] in loop.

L(ali):	subs	r12, r12, #4
	blt	L(ed1)
	subs	r12, r12, #4
	vld1.32	{d4,d5}, [tabp]!
	b	L(mid)

L(top):	subs	r12, r12, #4
	vld1.32	{d4,d5}, [tabp]!
	vst1.32	{d0,d1}, [r5:64]!
L(mid):	vld1.32	{d0,d1}, [r4:64]!
	vbit	d0, d4, maskv
	vbit	d1, d5, maskv
	bge	L(top)

L(end):	vst1.32	{d0,d1}, [r5:64]!

L(ed1):	tst	r12, #2			C 2 or 3 more limbs to go?
	beq	L(ed2)			C No, skip!
	vld1.32	{d4}, [tabp]!		C Handle 2 limbs
	vld1.32	{d0}, [r4:64]!
	vbit	d0, d4, maskv
	vst1.32	{d0}, [r5:64]!

L(ed2):	tst	r12, #1			C One more limb to go?
	beq	L(ed3)			C No, skip!
	vld1.32	{d4[0]}, [tabp]!	C Handle last limb
	vld1.32	{d0[0]}, [r4]
	vbit	d0, d4, maskv
	vst1.32	{d0[0]}, [r5]

L(ed3):	subs	nents, nents, #1
	bne	L(outer)
	pop	{r4-r7}
	bx	lr
EPILOGUE()
