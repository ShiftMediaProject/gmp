@ ARM M series __mpn_mul_1 -- Multiply a limb vector with a single limb and
@ store the product in a second limb vector.

@ Copyright (C) 1997 Free Software Foundation, Inc.

@ This file is part of the GNU MP Library.

@ The GNU MP Library is free software; you can redistribute it and/or modify
@ it under the terms of the GNU Library General Public License as published by
@ the Free Software Foundation; either version 2 of the License, or (at your
@ option) any later version.

@ The GNU MP Library is distributed in the hope that it will be useful, but
@ WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
@ or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
@ License for more details.

@ You should have received a copy of the GNU Library General Public License
@ along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
@ the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
@ MA 02111-1307, USA.


@ INPUT PARAMETERS
@ res_ptr	r0
@ s1_ptr	r1
@ size		r2
@ s2_limb	r3

@ NOT TESTED CODE

@ Possible optimizations:
@ 1. Maybe we should schedule things to avoid latencies?
@ 2. Are the ldm/stm instructions really faster than individual ldr/str?

	.text
	.align	0
	.global	___mpn_mul_1
___mpn_mul_1:
	stmfd	sp!, {r4, r5, r6, r7, r8, r9, lr}
	subs	r2, r2, #4
	mov	r12, #0			@ init cylimb reg
	blt	Ltail			@ branch if SIZE < 4

Loop:	ldmia	r1!, {r4, r5, r6, r7}	@ load next 4 limbs
	umull	r8, r9, r4, r3
	adds	r4, r8, r12
	umull	r8, r12, r5, r3
	adcs	r5, r8, r9
	umull	r8, r9, r6, r3
	adcs	r6, r8, r12
	umull	r8, r12, r7, r3
	adcs	r7, r8, r9
	adc	r12, r12, #0		@ new carry limb
	stmia	r0!, {r4, r5, r6, r7}	@ store 4 result limbs
	subs	r2, r2, #4
	bge	Loop			@ loop back while remaining count >= 4

Ltail:	ands	r2, r2, #(4-1)
	beq	Lend			@ done if SIZE mod 4 = 0

Loop2:	ldr	r4, [r1], #4		@ load next limb
	umull	r8, r9, r4, r3
	adds	r4, r8, r12
	adc	r12, r9, #0		@ new carry limb
	str	r4, [r0], #4		@ store result limb
	subs	r2, r2, #1
	bgt	Loop2

Lend:	mov	r0, r12
	ldmfd	sp!, {r4, r5, r6, r7, r8, r9, pc}^
