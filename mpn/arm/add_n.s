@ ARM __mpn_add -- Add two limb vectors of the same length > 0 and store
@ sum in a third limb vector.

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
@ RES_ptr	r0
@ S1_ptr	r1
@ S2_ptr	r2
@ SIZE		r3

@ NOT TESTED CODE

	.text
	.align	0
	.global	___mpn_add_n
___mpn_add_n:
	stmfd	sp!, {r4, r5, r6, r7, r8, r9, lr}
	subs	r3, r3, #3
	mov	r12, #-1		@ init carry save register
	blt	Ltail			@ branch if SIZE < 3

Loop:	rsbs	r12, r12, #0		@ restore carry
	ldmia	r1!, {r4, r5, r6}	@ load next 3 limbs from S1
	ldmia	r2!, {r7, r8, r9}	@ load next 3 limbs from S2
	adcs	r4, r4, r7
	adcs	r5, r5, r8
	adcs	r6, r6, r9
	stmia	r0!, {r4, r5, r6}	@ store 3 result limbs to RES
	sbc	r12, r12, r12		@ save negated carry
	subs	r3, r3, #3
	bge	Loop			@ loop back while remaining count >= 4

Ltail:	adds	r3, r3, #3
	beq	Lend			@ done if SIZE mod 4 = 0

Loop2:	rscs	r12, r12, #0		@ restore carry
	ldr	r4, [r1], #4		@ load next limb from S1
	ldr	r7, [r2], #4		@ load next limb from S2
	adcs	r4, r4, r7
	str	r4, [r0], #4		@ store result limb
	sbc	r12, r12, r12		@ save negated carry
	subs	r3, r3, #1
	bgt	Loop2

Lend:	mov	r0, r12
	ldmfd	sp!, {r4, r5, r6, r7, r8, r9, pc}^
