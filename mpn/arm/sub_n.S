@ ARM mpn_sub -- Subtract two limb vectors of the same length > 0 and store
@ difference in a third limb vector.
@ Contributed by Robert Harley.

@ Copyright (C) 1997, 2000 Free Software Foundation, Inc.

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
        
#define d r0
#define a r1
#define b r2
#define n r3

#define sl r10 /* Do not use, reserved for PIC. */
#define fp r11
#define ip r12
#define sp r13
#define lr r14
#define pc r15

.text
	.align	0
	.global	__mpn_sub_n
	.type	__mpn_sub_n,%function
__mpn_sub_n:
	stmfd	sp!, { r8, r9, lr }
	subs	ip, ip, ip
	tst	n, #1
	beq	Lskip1
	ldr	ip, [a], #4
	ldr	lr, [b], #4
	subs	ip, ip, lr
	str	ip, [d], #4
Lskip1:
	tst	n, #2
	beq	Lskip2
	ldmia	a!, { r8, r9 }
	ldmia	b!, { ip, lr }
	sbcs	r8, r8, ip
	sbcs	r9, r9, lr
	stmia	d!, { r8, r9 }
Lskip2:
	bics	n, n, #3
	beq	Lreturn
	stmfd	sp!, { r4, r5, r6, r7 }
LLoop:
	ldmia	a!, { r4, r5, r6, r7 }
	ldmia	b!, { r8, r9, ip, lr }
	sbcs	r4, r4, r8
	sbcs	r5, r5, r9
	sbcs	r6, r6, ip
	sbcs	r7, r7, lr
	stmia	d!, { r4, r5, r6, r7 }
	sub	n, n, #4
	teq	n, #0
	bne	LLoop
	ldmfd	sp!, { r4, r5, r6, r7 }
Lreturn:
	sbc	r0, r0, r0
	and	r0, r0, #1
	ldmfd	sp!, { r8, r9, pc }^
Lend:
	.size	__mpn_sub_n, Lend - __mpn_sub_n
