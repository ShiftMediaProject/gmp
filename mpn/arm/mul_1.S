@ ARM mpn_addmul_1 -- Multiply a limb vector with a limb and store the result
@ in a second limb vector.
@ Contributed by Robert Harley.

@ Copyright (C) 1998, 2000 Free Software Foundation, Inc.

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

#define p r0
#define a r1
#define n r2
#define w r3

#define sl r10 /* Do not use, reserved for PIC. */
#define fp r11
#define ip r12
#define sp r13
#define lr r14
#define pc r15

.text
	.align	0
	.global	__gmpn_mul_1
	.type	__gmpn_mul_1,%function
__gmpn_mul_1:
	stmfd	sp!, { r7, r8, r9, lr }
	mov	lr, #0
	movs	n, n, lsr #1
	bcc	Lskip1
	ldmia	a!, { lr }
	umull	ip, lr, w, lr
	stmia	p!, { ip }
Lskip1:
	movs	n, n, lsr #1
	bcc	Lskip2
	ldmia	a!, { r9, ip }
	umull	r7, r8, w, r9
	umull	r9, ip, w, ip
	adds	r7, r7, lr
	adcs	r8, r8, r9
	adc	lr, ip, #0
	stmia	p!, { r7, r8 }
Lskip2:
	teq	n, #0
	beq	Lreturn
	stmfd	sp!, { r5, r6 }
LLoop:
	ldmia	a!, { r7, r8, r9, ip }
	umull	r5, r6, w, r7
	umull	r7, r8, w, r8
	adds	r5, r5, lr
	adcs	r6, r6, r7
	adc	r7, r8, #0
	umull	lr, r8, w, r9
	umull	r9, ip, w, ip
	adds	r7, r7, lr
	adcs	r8, r8, r9
	adc	lr, ip, #0
	stmia	p!, { r5, r6, r7, r8 }
	subs	n, n, #1
	bne	LLoop
	ldmfd	sp!, { r5, r6 }
Lreturn:
	mov	r0, lr
	ldmfd	sp!, { r7, r8, r9, pc }^
Lend:
	.size	__gmpn_mul_1, Lend - __gmpn_mul_1
