dnl  Alpha ev6 nails mpn_submul_1.

dnl  Copyright 2002 Free Software Foundation, Inc.
dnl 
dnl  This file is part of the GNU MP Library.
dnl 
dnl  The GNU MP Library is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU Lesser General Public License as
dnl  published by the Free Software Foundation; either version 2.1 of the
dnl  License, or (at your option) any later version.
dnl 
dnl  The GNU MP Library is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl  Lesser General Public License for more details.
dnl 
dnl  You should have received a copy of the GNU Lesser General Public
dnl  License along with the GNU MP Library; see the file COPYING.LIB.  If
dnl  not, write to the Free Software Foundation, Inc., 59 Temple Place -
dnl  Suite 330, Boston, MA 02111-1307, USA.

include(`../config.m4')

dnl  INPUT PARAMETERS
define(`rp',`r16')
define(`up',`r17')
define(`n',`r18')
define(`vl0',`r19')

define(`numb_mask',`r14')

define(`m0a',`r0')
define(`m0b',`r1')
define(`m1a',`r2')
define(`m1b',`r3')
define(`m2a',`r20')
define(`m2b',`r21')
define(`m3a',`r12')
define(`m3b',`r13')

define(`acc0',`r9')
define(`acc1',`r27')

define(`ul0',`r4')
define(`ul1',`r5')
define(`ul2',`r6')
define(`ul3',`r7')

define(`rl0',`r22')
define(`rl1',`r23')
define(`rl2',`r24')
define(`rl3',`r25')

C unused scratch
C unused saved   r10 r11

define(`NAIL_BITS',`GMP_NAIL_BITS')
define(`NUMB_BITS',`GMP_NUMB_BITS')

dnl  This declaration is munged by configure
NAILS_SUPPORT(2-63)

dnl  Runs at 4.5 cycles/limb.  Local scheduling should bring that down to 3.5
dnl  cycles/limb.  It would be possible to reach 3.25 cycles/limb with 8-way
dnl  unrolling.

dnl Register usage:
dnl callee-saves:	r9 r10 r11 r12 r13 r14 r15
dnl scratch: r0 r1 r2 r3 r4 r5 r6 r7 r8
dnl	     r16 r17 r18 r19 r20 r21 r22 r23 r24 r25 r27 r28
dnl return address: 26
dnl global pointer: 29
dnl stack pointer: 30

ASM_START()
PROLOGUE(mpn_submul_1)
	lda	r30,	-240(r30)
	stq	r9,	8(r30)
C	stq	r10,	16(r30)
C	stq	r11,	24(r30)
	stq	r12,	32(r30)
	stq	r13,	40(r30)
	stq	r14,	48(r30)
	stq	r15,	56(r30)

	sll	vl0, NAIL_BITS, vl0
	lda	numb_mask, -1(r31)
	srl	numb_mask, NAIL_BITS, numb_mask

	bic	r31, r31, r15
	bic	r31, r31, m3b

	and	n,	3,	r25
	beq	r25,	L4
Loop0:
	ldq	ul0,	0(up)
	ldq	rl0,	0(rp)
	mulq	vl0,	ul0,	m0a		C U1
	srl	m0a,NAIL_BITS,	r8
	addq	r8,	m3b,	acc0
	subq	rl0,	acc0,	acc0
	addq	r15,	acc0,	acc0
	umulh	vl0,	ul0,	m3b		C U1
	sra	acc0,NUMB_BITS,	r15
	and	acc0,numb_mask,	r28
	stq	r28,	0(rp)
	lda	rp,	8(rp)
	lda	up,	8(up)
	lda	r25,	-1(r25)
	bne	r25,	Loop0

L4:
	lda	n,	-4(n)
	bge	n,	L_4_or_more
L_0_to_3:
	subq	m3b,	r15,	r0
	br	r31,	Lret

L_4_or_more:
	ldq	ul0,	0(up)
	ldq	ul1,	8(up)
	ldq	ul2,	16(up)
	ldq	ul3,	24(up)
	ldq	rl0,	0(rp)
	lda	n,	-4(n)
	lda	up,	32(up)
	bge	n,	L_8_or_more
L_4_to_8:
	mulq	vl0,	ul0,	m0a		C U1
	umulh	vl0,	ul0,	m0b		C U1
	ldq	rl1,	8(rp)
	mulq	vl0,	ul1,	m1a		C U1
	umulh	vl0,	ul1,	m1b		C U1
	ldq	rl2,	16(rp)
	mulq	vl0,	ul2,	m2a		C U1
	umulh	vl0,	ul2,	m2b		C U1
	srl	m0a,NAIL_BITS,	r8
	ldq	rl3,	24(rp)
	mulq	vl0,	ul3,	m3a		C U1
	addq	r8,	m3b,	acc0
	umulh	vl0,	ul3,	m3b		C U1
	subq	rl0,	acc0,	acc0
	srl	m1a,NAIL_BITS,	r8
	addq	r15,	acc0,	acc0

	addq	r8,	m0b,	acc1
	sra	acc0,NUMB_BITS,	r15
	subq	rl1,	acc1,	acc1
	and	acc0,numb_mask,	r28
	srl	m2a,NAIL_BITS,	r8
	addq	r15,	acc1,	acc1
	bis	r31,	r31,	r31		C	nop
	addq	r8,	m1b,	acc0
	sra	acc1,NUMB_BITS,	r15
	stq	r28,	0(rp)
	subq	rl2,	acc0,	acc0
	and	acc1,numb_mask,	r28
	srl	m3a,NAIL_BITS,	r8
	addq	r15,	acc0,	acc0
	bis	r31,	r31,	r31		C	nop
	addq	r8,	m2b,	acc1
	sra	acc0,NUMB_BITS,	r15
	stq	r28,	8(rp)
	subq	rl3,	acc1,	acc1
	and	acc0,numb_mask,	r28
	addq	r15,	acc1,	acc1
	bis	r31,	r31,	r31		C	nop
	sra	acc1,NUMB_BITS,	r15
	stq	r28,	16(rp)
	and	acc1,numb_mask,	r28
	subq	m3b,	r15,	acc0
	stq	r28,	24(rp)
	and	acc0,numb_mask,	r0

	br	r31,	Lret

L_8_or_more:
	mulq	vl0,	ul0,	m0a		C U1
	umulh	vl0,	ul0,	m0b		C U1
	ldq	ul0,	0(up)
	ldq	rl1,	8(rp)
	mulq	vl0,	ul1,	m1a		C U1
	umulh	vl0,	ul1,	m1b		C U1
	ldq	ul1,	8(up)
	ldq	rl2,	16(rp)
	mulq	vl0,	ul2,	m2a		C U1
	umulh	vl0,	ul2,	m2b		C U1
	ldq	ul2,	16(up)
	srl	m0a,NAIL_BITS,	r8
	ldq	rl3,	24(rp)
	mulq	vl0,	ul3,	m3a		C U1
	addq	r8,	m3b,	acc0
	umulh	vl0,	ul3,	m3b		C U1
	ldq	ul3,	24(up)
	subq	rl0,	acc0,	acc0
	srl	m1a,NAIL_BITS,	r8
	ldq	rl0,	32(rp)
	addq	r15,	acc0,	acc0
	lda	n,	-4(n)
	lda	up,	32(up)
	lda	rp,	32(rp)
	bge	n,	L_12_or_more			C U0
L_8_to_11:
	mulq	vl0,	ul0,	m0a		C U1
	addq	r8,	m0b,	acc1
	sra	acc0,NUMB_BITS,	r15
	umulh	vl0,	ul0,	m0b		C U1
	subq	rl1,	acc1,	acc1
	and	acc0,numb_mask,	r28
	srl	m2a,NAIL_BITS,	r8
	ldq	rl1,	8(rp)
	addq	r15,	acc1,	acc1
	bis	r31,	r31,	r31		C	nop
	mulq	vl0,	ul1,	m1a		C U1
	addq	r8,	m1b,	acc0
	sra	acc1,NUMB_BITS,	r15
	stq	r28,	-32(rp)
	umulh	vl0,	ul1,	m1b		C U1
	subq	rl2,	acc0,	acc0
	and	acc1,numb_mask,	r28
	srl	m3a,NAIL_BITS,	r8
	ldq	rl2,	16(rp)
	addq	r15,	acc0,	acc0
	bis	r31,	r31,	r31		C	nop
	mulq	vl0,	ul2,	m2a		C U1
	addq	r8,	m2b,	acc1
	sra	acc0,NUMB_BITS,	r15
	stq	r28,	-24(rp)
	umulh	vl0,	ul2,	m2b		C U1
	subq	rl3,	acc1,	acc1
	and	acc0,numb_mask,	r28
	srl	m0a,NAIL_BITS,	r8
	ldq	rl3,	24(rp)
	addq	r15,	acc1,	acc1
	bis	r31,	r31,	r31		C	nop
	mulq	vl0,	ul3,	m3a		C U1
	addq	r8,	m3b,	acc0
	sra	acc1,NUMB_BITS,	r15
	stq	r28,	-16(rp)
	umulh	vl0,	ul3,	m3b		C U1
	subq	rl0,	acc0,	acc0
	and	acc1,numb_mask,	r28
	srl	m1a,NAIL_BITS,	r8
	addq	r15,	acc0,	acc0

	addq	r8,	m0b,	acc1
	sra	acc0,NUMB_BITS,	r15
	stq	r28,	-8(rp)
	subq	rl1,	acc1,	acc1
	and	acc0,numb_mask,	r28
	srl	m2a,NAIL_BITS,	r8
	addq	r15,	acc1,	acc1
	bis	r31,	r31,	r31		C	nop
	addq	r8,	m1b,	acc0
	sra	acc1,NUMB_BITS,	r15
	stq	r28,	0(rp)
	subq	rl2,	acc0,	acc0
	and	acc1,numb_mask,	r28
	srl	m3a,NAIL_BITS,	r8
	addq	r15,	acc0,	acc0
	bis	r31,	r31,	r31		C	nop
	addq	r8,	m2b,	acc1
	sra	acc0,NUMB_BITS,	r15
	stq	r28,	8(rp)
	subq	rl3,	acc1,	acc1
	and	acc0,numb_mask,	r28
	addq	r15,	acc1,	acc1
	bis	r31,	r31,	r31		C	nop
	sra	acc1,NUMB_BITS,	r15
	stq	r28,	16(rp)
	and	acc1,numb_mask,	r28
	subq	m3b,	r15,	acc0
	stq	r28,	24(rp)
	and	acc0,numb_mask,	r0

	br	r31,	Lret

L_12_or_more:
	mulq	vl0,	ul0,	m0a		C U1
	addq	r8,	m0b,	acc1
	sra	acc0,NUMB_BITS,	r15
	umulh	vl0,	ul0,	m0b		C U1
	ldq	ul0,	0(up)
	subq	rl1,	acc1,	acc1
	and	acc0,numb_mask,	r28
	srl	m2a,NAIL_BITS,	r8
	ldq	rl1,	8(rp)
	addq	r15,	acc1,	acc1
	bis	r31,	r31,	r31		C	nop
	mulq	vl0,	ul1,	m1a		C U1
	addq	r8,	m1b,	acc0
	sra	acc1,NUMB_BITS,	r15
	stq	r28,	-32(rp)
	umulh	vl0,	ul1,	m1b		C U1
	ldq	ul1,	8(up)
	subq	rl2,	acc0,	acc0
	and	acc1,numb_mask,	r28
	srl	m3a,NAIL_BITS,	r8
	ldq	rl2,	16(rp)
	addq	r15,	acc0,	acc0
	bis	r31,	r31,	r31		C	nop
	mulq	vl0,	ul2,	m2a		C U1
	addq	r8,	m2b,	acc1
	sra	acc0,NUMB_BITS,	r15
	stq	r28,	-24(rp)
	umulh	vl0,	ul2,	m2b		C U1
	ldq	ul2,	16(up)
	subq	rl3,	acc1,	acc1
	and	acc0,numb_mask,	r28
	srl	m0a,NAIL_BITS,	r8
	ldq	rl3,	24(rp)
	addq	r15,	acc1,	acc1
	bis	r31,	r31,	r31		C	nop
	mulq	vl0,	ul3,	m3a		C U1
	addq	r8,	m3b,	acc0
	sra	acc1,NUMB_BITS,	r15
	stq	r28,	-16(rp)
	umulh	vl0,	ul3,	m3b		C U1
	ldq	ul3,	24(up)
	subq	rl0,	acc0,	acc0
	and	acc1,numb_mask,	r28
	srl	m1a,NAIL_BITS,	r8
	ldq	rl0,	32(rp)
	addq	r15,	acc0,	acc0
	bis	r31,	r31,	r31		C	nop
	bis	r31,	r31,	r31		C	nop
	bis	r31,	r31,	r31		C	nop
	bis	r31,	r31,	r31		C	nop
	bis	r31,	r31,	r31		C	nop
	lda	n,	-4(n)
	lda	up,	32(up)
	lda	rp,	32(rp)
	blt	n,	L_end			C U0

Loop:
C
	mulq	vl0,	ul0,	m0a		C U1
	addq	r8,	m0b,	acc1
	sra	acc0,NUMB_BITS,	r15
	stq	r28,	-40(rp)
C
	umulh	vl0,	ul0,	m0b		C U1
	ldq	ul0,	0(up)
	subq	rl1,	acc1,	acc1
	and	acc0,numb_mask,	r28
C
	srl	m2a,NAIL_BITS,	r8
	ldq	rl1,	8(rp)
	addq	r15,	acc1,	acc1
	bis	r31,	r31,	r31		C	nop
C
	mulq	vl0,	ul1,	m1a		C U1
	addq	r8,	m1b,	acc0
	sra	acc1,NUMB_BITS,	r15
	stq	r28,	-32(rp)
C
	umulh	vl0,	ul1,	m1b		C U1
	ldq	ul1,	8(up)
	subq	rl2,	acc0,	acc0
	and	acc1,numb_mask,	r28
C
	srl	m3a,NAIL_BITS,	r8
	ldq	rl2,	16(rp)
	addq	r15,	acc0,	acc0
	bis	r31,	r31,	r31		C	nop
C
	mulq	vl0,	ul2,	m2a		C U1
	addq	r8,	m2b,	acc1
	sra	acc0,NUMB_BITS,	r15
	stq	r28,	-24(rp)
C
	umulh	vl0,	ul2,	m2b		C U1
	ldq	ul2,	16(up)
	subq	rl3,	acc1,	acc1
	and	acc0,numb_mask,	r28
C
	srl	m0a,NAIL_BITS,	r8
	ldq	rl3,	24(rp)
	addq	r15,	acc1,	acc1
	bis	r31,	r31,	r31		C	nop
C
	mulq	vl0,	ul3,	m3a		C U1
	addq	r8,	m3b,	acc0
	sra	acc1,NUMB_BITS,	r15
	stq	r28,	-16(rp)
C
	umulh	vl0,	ul3,	m3b		C U1
	ldq	ul3,	24(up)
	subq	rl0,	acc0,	acc0
	and	acc1,numb_mask,	r28
C
	srl	m1a,NAIL_BITS,	r8
	ldq	rl0,	32(rp)
	addq	r15,	acc0,	acc0
	bis	r31,	r31,	r31		C	nop
C
	bis	r31,	r31,	r31		C	nop
	bis	r31,	r31,	r31		C	nop
	bis	r31,	r31,	r31		C	nop
	bis	r31,	r31,	r31		C	nop
C
	lda	n,	-4(n)
	lda	up,	32(up)
	lda	rp,	32(rp)
	bge	n,	Loop			C U0

L_end:
	mulq	vl0,	ul0,	m0a		C U1
	addq	r8,	m0b,	acc1
	sra	acc0,NUMB_BITS,	r15
	stq	r28,	-40(rp)
	umulh	vl0,	ul0,	m0b		C U1
	subq	rl1,	acc1,	acc1
	and	acc0,numb_mask,	r28
	srl	m2a,NAIL_BITS,	r8
	ldq	rl1,	8(rp)
	addq	r15,	acc1,	acc1
	bis	r31,	r31,	r31		C	nop
	mulq	vl0,	ul1,	m1a		C U1
	addq	r8,	m1b,	acc0
	sra	acc1,NUMB_BITS,	r15
	stq	r28,	-32(rp)
	umulh	vl0,	ul1,	m1b		C U1
	subq	rl2,	acc0,	acc0
	and	acc1,numb_mask,	r28
	srl	m3a,NAIL_BITS,	r8
	ldq	rl2,	16(rp)
	addq	r15,	acc0,	acc0
	bis	r31,	r31,	r31		C	nop
	mulq	vl0,	ul2,	m2a		C U1
	addq	r8,	m2b,	acc1
	sra	acc0,NUMB_BITS,	r15
	stq	r28,	-24(rp)
	umulh	vl0,	ul2,	m2b		C U1
	subq	rl3,	acc1,	acc1
	and	acc0,numb_mask,	r28
	srl	m0a,NAIL_BITS,	r8
	ldq	rl3,	24(rp)
	addq	r15,	acc1,	acc1
	bis	r31,	r31,	r31		C	nop
	mulq	vl0,	ul3,	m3a		C U1
	addq	r8,	m3b,	acc0
	sra	acc1,NUMB_BITS,	r15
	stq	r28,	-16(rp)
	umulh	vl0,	ul3,	m3b		C U1
	subq	rl0,	acc0,	acc0
	and	acc1,numb_mask,	r28
	srl	m1a,NAIL_BITS,	r8
	addq	r15,	acc0,	acc0
	bis	r31,	r31,	r31		C	nop
	bis	r31,	r31,	r31		C	nop
	bis	r31,	r31,	r31		C	nop
	bis	r31,	r31,	r31		C	nop
	bis	r31,	r31,	r31		C	nop
	lda	rp,	32(rp)

	addq	r8,	m0b,	acc1
	sra	acc0,NUMB_BITS,	r15
	stq	r28,	-40(rp)
	subq	rl1,	acc1,	acc1
	and	acc0,numb_mask,	r28
	srl	m2a,NAIL_BITS,	r8
	addq	r15,	acc1,	acc1
	bis	r31,	r31,	r31		C	nop
	addq	r8,	m1b,	acc0
	sra	acc1,NUMB_BITS,	r15
	stq	r28,	-32(rp)
	subq	rl2,	acc0,	acc0
	and	acc1,numb_mask,	r28
	srl	m3a,NAIL_BITS,	r8
	addq	r15,	acc0,	acc0
	bis	r31,	r31,	r31		C	nop
	addq	r8,	m2b,	acc1
	sra	acc0,NUMB_BITS,	r15
	stq	r28,	-24(rp)
	subq	rl3,	acc1,	acc1
	and	acc0,numb_mask,	r28
	addq	r15,	acc1,	acc1
	bis	r31,	r31,	r31		C	nop
	sra	acc1,NUMB_BITS,	r15
	stq	r28,	-16(rp)
	and	acc1,numb_mask,	r28
	subq	m3b,	r15,	acc0
	stq	r28,	-8(rp)
	and	acc0,numb_mask,	r0
Lret:
	ldq	r9,	8(r30)
C	ldq	r10,	16(r30)
C	ldq	r11,	24(r30)
	ldq	r12,	32(r30)
	ldq	r13,	40(r30)
	ldq	r14,	48(r30)
	ldq	r15,	56(r30)
	lda	r30,	240(r30)
	ret	r31,	(r26),	1
EPILOGUE(mpn_submul_1)
ASM_END()
