dnl Alpha mpn_mod_1s_4p

dnl  Contributed to the GNU project by Torbjorn Granlund.

dnl  Copyright 2009 Free Software Foundation, Inc.

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

C TODO:
C  * Optimise.  2.75 c/l should be possible.
C  * Write a proper mpn_mod_1s_4p_cps.  The code below was compiler generated.
C  * Make mpn_mod_1s_4p_cps work for ev4-ev5.
C  * Optimise feed-in code, starting the sw pipeline in switch code.
C  * Use fewer registers.  Use r28 and r27.
C  * If we cannot reduce register usage, write perhaps small-n basecase.
C  * Does it work for PIC?

C      cycles/limb
C EV4:     ?
C EV5:    23
C EV6:     3

define(`ap',     `r16')
define(`n',      `r17')
define(`pl',     `r24')
define(`ph',     `r25')
define(`rl',     `r6')
define(`rh',     `r7')
define(`B1modb', `r1')
define(`B2modb', `r2')
define(`B3modb', `r3')
define(`B4modb', `r4')
define(`B5modb', `r5')

ASM_START()
	.arch	ev56

PROLOGUE(mpn_mod_1s_4p)
	lda	r30, -80(r30)
	stq	r9, 8(r30)
	stq	r10, 16(r30)
	stq	r11, 24(r30)
	stq	r12, 32(r30)
	stq	r13, 40(r30)
	stq	r14, 48(r30)
	stq	r15, 56(r30)
	s8addq	n, ap, ap		C point ap at vector end

	ldq	B1modb, 16(r19)
	ldq	B2modb, 24(r19)
	ldq	B3modb, 32(r19)
	ldq	B4modb, 40(r19)
	ldq	B5modb, 48(r19)

	and	n, 3, r0
	lda	n, -4(n)
	beq	r0, L(b0)
	lda	r6, -2(r0)
	blt	r6, L(b1)
	beq	r6, L(b2)

L(b3):	ldq	r21, -16(ap)
	ldq	r22, -8(ap)
	ldq	r20, -24(ap)
	mulq	r21, B1modb, r8
	umulh	r21, B1modb, r12
	mulq	r22, B2modb, r9
	umulh	r22, B2modb, r13
	addq	r8, r20, pl
	cmpult	pl, r8, r0
	addq	r0, r12, ph
	addq	r9, pl, rl
	cmpult	rl, r9, r0
	addq	r13, ph, ph
	addq	r0, ph, rh
	lda	ap, -56(ap)
	br	L(com)

L(b0):	ldq	r21, -24(ap)
	ldq	r22, -16(ap)
	ldq	r23, -8(ap)
	ldq	r20, -32(ap)
	mulq	r21, B1modb, r8
	umulh	r21, B1modb, r12
	mulq	r22, B2modb, r9
	umulh	r22, B2modb, r13
	mulq	r23, B3modb, r10
	umulh	r23, B3modb, r14
	addq	r8, r20, pl
	cmpult	pl, r8, r0
	addq	r0, r12, ph
	addq	r9, pl, pl
	cmpult	pl, r9, r0
	addq	r13, ph, ph
	addq	r0, ph, ph
	addq	r10, pl, rl
	cmpult	rl, r10, r0
	addq	r14, ph, ph
	addq	r0, ph, rh
	lda	ap, -64(ap)
	br	L(com)

L(b1):	bis	r31, r31, rh
	ldq	rl, -8(ap)
	lda	ap, -40(ap)
	br	L(com)

L(b2):	ldq	r21, -8(ap)
	ldq	r20, -16(ap)
	mulq	r21, B1modb, r8
	umulh	r21, B1modb, r12
	addq	r8, r20, rl
	cmpult	rl, r8, r0
	addq	r0, r12, rh
	lda	ap, -48(ap)

L(com):	ble	n, L(ed3)
	ldq	r21, 8(ap)
	ldq	r22, 16(ap)
	ldq	r23, 24(ap)
	ldq	r20, 0(ap)
	lda	n, -4(n)
	lda	ap, -32(ap)
	mulq	r21, B1modb, r8
	umulh	r21, B1modb, r12
	mulq	r22, B2modb, r9
	umulh	r22, B2modb, r13
	mulq	r23, B3modb, r10
	umulh	r23, B3modb, r14
	mulq	rl, B4modb, r11
	umulh	rl, B4modb, r15
	ble	n, L(ed2)

	ALIGN(16)
L(top):	ldq	r21, 8(ap)
	mulq	rh, B5modb, rl
	addq	r8, r20, pl
	ldq	r22, 16(ap)
	cmpult	pl, r8, r0
	umulh	rh, B5modb, rh
	ldq	r23, 24(ap)
	addq	r0, r12, ph
	addq	r9, pl, pl
	mulq	r21, B1modb, r8
	cmpult	pl, r9, r0
	addq	r13, ph, ph
	umulh	r21, B1modb, r12
	lda	ap, -32(ap)
	addq	r0, ph, ph
	addq	r10, pl, pl
	mulq	r22, B2modb, r9
	cmpult	pl, r10, r0
	addq	r14, ph, ph
	addq	r11, pl, pl
	umulh	r22, B2modb, r13
	addq	r0, ph, ph
	cmpult	pl, r11, r0
	addq	r15, ph, ph
	mulq	r23, B3modb, r10
	ldq	r20, 32(ap)
	addq	pl, rl, rl
	umulh	r23, B3modb, r14
	addq	r0, ph, ph
	cmpult	rl, pl, r0
	mulq	rl, B4modb, r11
	addq	ph, rh, rh
	umulh	rl, B4modb, r15
	addq	r0, rh, rh
	lda	n, -4(n)
	bgt	n, L(top)

L(ed2):	mulq	rh, B5modb, rl
	addq	r8, r20, pl
	umulh	rh, B5modb, rh
	cmpult	pl, r8, r0
	addq	r0, r12, ph
	addq	r9, pl, pl
	cmpult	pl, r9, r0
	addq	r13, ph, ph
	addq	r0, ph, ph
	addq	r10, pl, pl
	cmpult	pl, r10, r0
	addq	r14, ph, ph
	addq	r11, pl, pl
	addq	r0, ph, ph
	cmpult	pl, r11, r0
	addq	r15, ph, ph
	addq	pl, rl, rl
	addq	r0, ph, ph
	cmpult	rl, pl, r0
	addq	ph, rh, rh
	addq	r0, rh, rh

L(ed3):	mulq	rh, B1modb, r8
	umulh	rh, B1modb, rh
	addq	r8, rl, rl
	cmpult	rl, r8, r0
	addq	r0, rh, rh

	ldq	r24, 8(r19)		C cnt
	sll	rh, r24, rh
	subq	r31, r24, r25
	srl	rl, r25, r2
	sll	rl, r24, rl
	or	r2, rh, rh

	ldq	r23, 0(r19)		C bi
	mulq	rh, r23, r8
	umulh	rh, r23, r9
	addq	rh, 1, r7
	addq	r8, rl, r8		C ql
	cmpult	r8, rl, r0
	addq	r9, r7, r9
	addq	r0, r9, r9		C qh
	mulq	r9, r18, r21		C qh * b
	subq	rl, r21, rl
	cmpult	r8, rl, r0		C rl > ql
	negq	r0, r0
	and	r0, r18, r0
	addq	rl, r0, rl
	cmpule	r18, rl, r0		C rl >= b
	negq	r0, r0
	and	r0, r18, r0
	subq	rl, r0, rl

	srl	rl, r24, r0

	ldq	r9, 8(r30)
	ldq	r10, 16(r30)
	ldq	r11, 24(r30)
	ldq	r12, 32(r30)
	ldq	r13, 40(r30)
	ldq	r14, 48(r30)
	ldq	r15, 56(r30)
	lda	r30, 80(r30)
	ret	r31, (r26), 1
EPILOGUE()

PROLOGUE(mpn_mod_1s_4p_cps,gp)
	ldgp	r29,	0(r27)
	LEA(	r28,	__clz_tab)
	lda	r30,	-32(r30)
	lda	r5,	65(r31)
	cmpbge	r31,	r17,	r8
	stq	r26,	0(r30)
	stq	r10,	16(r30)
	srl	r8,	1,	r7
	xor	r7,	127,	r6
	stq	r11,	24(r30)
	stq	r9,	8(r30)
	bis	r31,	r16,	r11
	addq	r6,	r28,	r4
	ldbu	r2,	0(r4)
	s8subq	r2,	7,	r3
	srl	r17,	r3,	r27
	subq	r5,	r3,	r26
	addq	r27,	r28,	r10
	ldbu	r9,	0(r10)
	subq	r26,	r9,	r10
	sll	r17,	r10,	r9
	bis	r31,	r9,	r16
	jsr	r26,	mpn_invert_limb
	ldgp	r29,	0(r26)
	stq	r10,	8(r11)
	subq	r31,	r10,	r25
	lda	r24,	1(r31)
	subq	r31,	r9,	r20
	stq	r0,	0(r11)
	srl	r0,	r25,	r22
	sll	r24,	r10,	r23
	bis	r22,	r23,	r21
	mulq	r20,	r21,	r1
	umulh	r1,	r0,	r18
	srl	r1,	r10,	r19
	mulq	r1,	r0,	r8
	stq	r19,	16(r11)
	addq	r18,	r1,	r17
	ornot	r31,	r17,	r16
	mulq	r16,	r9,	r2
	cmpule	r2,	r8,	r7
	addq	r2,	r9,	r6
	cmoveq	r7,	r6,	r2
	umulh	r2,	r0,	r4
	srl	r2,	r10,	r5
	mulq	r2,	r0,	r27
	stq	r5,	24(r11)
	addq	r4,	r2,	r3
	ornot	r31,	r3,	r28
	mulq	r28,	r9,	r23
	cmpule	r23,	r27,	r26
	addq	r23,	r9,	r25
	cmoveq	r26,	r25,	r23
	ldq	r26,	0(r30)
	umulh	r23,	r0,	r22
	srl	r23,	r10,	r24
	mulq	r23,	r0,	r19
	stq	r24,	32(r11)
	addq	r22,	r23,	r21
	ornot	r31,	r21,	r20
	mulq	r20,	r9,	r1
	addq	r1,	r9,	r17
	cmpule	r1,	r19,	r18
	cmoveq	r18,	r17,	r1
	umulh	r1,	r0,	r8
	srl	r1,	r10,	r16
	mulq	r1,	r0,	r5
	stq	r16,	40(r11)
	addq	r8,	r1,	r7
	ornot	r31,	r7,	r6
	mulq	r6,	r9,	r2
	addq	r2,	r9,	r3
	cmpule	r2,	r5,	r4
	ldq	r9,	8(r30)
	cmoveq	r4,	r3,	r2
	srl	r2,	r10,	r0
	ldq	r10,	16(r30)
	stq	r0,	48(r11)
	ldq	r11,	24(r30)
	lda	r30,	32(r30)
	ret	r31,	(r26),	1
EPILOGUE()
