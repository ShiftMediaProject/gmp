dnl  PowerPC-64 mpn_sqr_diag_addlsh1

dnl  Copyright 2011 Free Software Foundation, Inc.

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

C               cycles/limb
C POWER3/PPC630      10
C POWER4/PPC970       6
C POWER5              5.375
C POWER6              8.5
C POWER7              3.4

C NOTES
C  * This was written for POWER6 and its preferences for adjacent integer
C    multiply insns.  The cost is that we get a large set of live registers,
C    and therefore need to save 9 callee-saves registers.  Except for the
C    multiply insns, the code was not carefully optimised for POWER6 or any
C    other CPU.
C  * Perform some cross-jumping in the feed-in code, into the loop's tail.

C refmpn_sqr_diag_addlsh1 (mp_ptr rp, mp_srcptr tp, mp_srcptr up, mp_size_t n)

C INPUT PARAMETERS
define(`rp',  `r3')
define(`tp',  `r4')
define(`up',  `r5')
define(`n',   `r6')

define(`climb',	`r0')

ASM_START()
PROLOGUE(mpn_sqr_diag_addlsh1)
	std	r31,  -8(r1)
	std	r30, -16(r1)
	std	r29, -24(r1)
	std	r28, -32(r1)
	std	r27, -40(r1)
	std	r26, -48(r1)
	std	r25, -56(r1)
	std	r24, -64(r1)
	std	r23, -72(r1)

	rldicl.	r0, n, 0,62		C r0 = n & 3, set cr0
	cmpdi	cr6, r0, 2
	addi	n, n, 2	 		C compute count...
	srdi	n, n, 2			C ...for ctr
	mtctr	n			C put loop count into ctr
	beq	cr0, L(b0)
	blt	cr6, L(b1)
	beq	cr6, L(b2)

L(b3):	ld	r6,   0(up)
	ld	r7,   8(up)
	ld	r12, 16(up)
	addi	up, up, 24
	mulld	r24, r6, r6
	mulhdu	r25, r6, r6
	mulld	r26, r7, r7
	mulhdu	r27, r7, r7
	mulld	r28, r12, r12
	mulhdu	r29, r12, r12
	ld	r10, 0(tp)
	ld	r11, 8(tp)
	ld	r6, 16(tp)
	ld	r7, 24(tp)
	addi	tp, tp, 32
	addc	r10, r10, r10
	adde	r11, r11, r11
	adde	r6, r6, r6
	adde	r7, r7, r7
	addze	climb, r29
	addc	r10, r10, r25
	adde	r11, r11, r26
	adde	r6, r6, r27
	adde	r7, r7, r28
	std	r24,  0(rp)
	std	r10,  8(rp)
	std	r11, 16(rp)
	std	r6,  24(rp)
	std	r7,  32(rp)
	addi	rp, rp, 40
	bdnz	L(top)
	b	L(end)

L(b2):	ld	r6,  0(up)
	ld	r7,  8(up)
	addi	up, up, 16
	mulld	r24, r6, r6
	mulhdu	r25, r6, r6
	mulld	r26, r7, r7
	mulhdu	r27, r7, r7
	ld	r10,  0(tp)
	ld	r11,  8(tp)
	addi	tp, tp, 16
	addc	r10, r10, r10
	adde	r11, r11, r11
	addze	climb, r27
	addc	r10, r10, r25
	adde	r11, r11, r26
	std	r24,  0(rp)
	std	r10,  8(rp)
	std	r11, 16(rp)
	addi	rp, rp, 24
	bdnz	L(top)
	b	L(end)

L(b0):	ld	r6,   0(up)
	ld	r7,   8(up)
	ld	r12, 16(up)
	ld	r23, 24(up)
	addi	up, up, 32
	mulld	r24, r6, r6
	mulhdu	r25, r6, r6
	mulld	r26, r7, r7
	mulhdu	r27, r7, r7
	mulld	r28, r12, r12
	mulhdu	r29, r12, r12
	mulld	r30, r23, r23
	mulhdu	r31, r23, r23
	ld	r10,  0(tp)
	ld	r11,  8(tp)
	ld	r6,  16(tp)
	ld	r7,  24(tp)
	ld	r12, 32(tp)
	ld	r23, 40(tp)
	addi	tp, tp, 48
	addc	r10, r10, r10
	adde	r11, r11, r11
	adde	r6, r6, r6
	adde	r7, r7, r7
	adde	r12, r12, r12
	adde	r23, r23, r23
	addze	climb, r31
	std	r24,  0(rp)
	addc	r10, r10, r25
	std	r10,  8(rp)
	adde	r11, r11, r26
	std	r11, 16(rp)
	adde	r6, r6, r27
	std	r6,  24(rp)
	adde	r7, r7, r28
	std	r7,  32(rp)
	adde	r12, r12, r29
	std	r12, 40(rp)
	adde	r23, r23, r30
	std	r23, 48(rp)
	addi	rp, rp, 56
	bdnz	L(top)
	b	L(end)

L(b1):	ld	r6, 0(up)
	addi	up, up, 8
	mulld	r24, r6, r6
	mulhdu	climb, r6, r6
	std	r24, 0(rp)
	addic	rp, rp, 8		C clear carry as side-effect

	ALIGN(32)
L(top):	ld	r6,   0(up)
	ld	r7,   8(up)
	ld	r12, 16(up)
	ld	r23, 24(up)
	addi	up, up, 32
	mulld	r24, r6, r6
	mulhdu	r25, r6, r6
	mulld	r26, r7, r7
	mulhdu	r27, r7, r7
	mulld	r28, r12, r12
	mulhdu	r29, r12, r12
	mulld	r30, r23, r23
	mulhdu	r31, r23, r23
	ld	r8,   0(tp)
	ld	r9,   8(tp)
	adde	r8, r8, r8
	adde	r9, r9, r9
	ld	r10, 16(tp)
	ld	r11, 24(tp)
	adde	r10, r10, r10
	adde	r11, r11, r11
	ld	r6,  32(tp)
	ld	r7,  40(tp)
	adde	r6, r6, r6
	adde	r7, r7, r7
	ld	r12, 48(tp)
	ld	r23, 56(tp)
	adde	r12, r12, r12
	adde	r23, r23, r23
	addi	tp, tp, 64
	addze	r31, r31
	addc	r8, r8, climb
	std	r8,   0(rp)
	adde	r9, r9, r24
	std	r9,   8(rp)
	adde	r10, r10, r25
	std	r10, 16(rp)
	adde	r11, r11, r26
	std	r11, 24(rp)
	adde	r6, r6, r27
	std	r6,  32(rp)
	adde	r7, r7, r28
	std	r7,  40(rp)
	adde	r12, r12, r29
	std	r12, 48(rp)
	adde	r23, r23, r30
	std	r23, 56(rp)
	mr	climb, r31
	addi	rp, rp, 64
	bdnz	L(top)

L(end):	addze	climb, climb
	std	climb,  0(rp)

L(ret):	ld	r31, -8(r1)
	ld	r30, -16(r1)
	ld	r29, -24(r1)
	ld	r28, -32(r1)
	ld	r27, -40(r1)
	ld	r26, -48(r1)
	ld	r25, -56(r1)
	ld	r24, -64(r1)
	ld	r23, -72(r1)
	blr
EPILOGUE()
