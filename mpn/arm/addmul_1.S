@ ARM mpn_mul_1 -- Multiply a limb vector with a limb and add the result to a
@ second limb vector.
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
	.global	__mpn_addmul_1
	.type	__mpn_addmul_1,%function
__mpn_addmul_1:
	stmfd	sp!, { r7, r8, r9, lr }
	mov	lr, #0
	movs	n, n, lsr #1
	bcc	Lskip1
	ldmia	a!, { lr }
	umull	ip, lr, w, lr
	ldmia	p, { r9 }
	adds	r9, r9, ip
	adc	lr, lr, #0
	stmia	p!, { r9 }
Lskip1:
	movs	n, n, lsr #1
	bcc	Lskip2
	mov	r7, lr
	ldmia	a!, { ip, lr }
	umull	r8, r9, w, ip
	umull	ip, lr, w, lr
	adds	r7, r7, r8
	adcs	r8, r9, ip
	adc	lr, lr, #0
	ldmia	p, { r9, ip }
	adds	r7, r7, r9
	adcs	r8, r8, ip
	adc	lr, lr, #0
	stmia	p!, { r7, r8 }
Lskip2:
	teq	n, #0
	beq	Lreturn
	stmfd	sp!, { r5, r6 }
LLoop:
	mov	r5, lr
	ldmia	a!, { r8, r9, ip, lr }
	umull	r6, r7, w, r8
	umull	r8, r9, w, r9
	adds	r5, r5, r6
	adcs	r6, r7, r8
	adc	r7, r9, #0
	umull	r8, r9, w, ip
	umull	ip, lr, w, lr
	adds	r7, r7, r8
	adcs	r8, r9, ip
	adc	lr, lr, #0
	ldmia	p!, { r9, ip }
	adds	r5, r5, r9
	adcs	r6, r6, ip
	ldmia	p!, { r9, ip }
	adcs	r7, r7, r9
	adcs	r8, r8, ip
	adc	lr, lr, #0
	stmdb	p, { r5, r6, r7, r8 }
	subs	n, n, #1
	bne	LLoop
	ldmfd	sp!, { r5, r6 }
Lreturn:
	mov	r0, lr
	ldmfd	sp!, { r7, r8, r9, pc }^
Lend:
	.size	__mpn_addmul_1, Lend - __mpn_addmul_1
