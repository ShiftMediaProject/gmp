dnl  Alpha ev6 nails mpn_add_n.

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
define(`vp',`r18')
define(`n',`r19')

define(`rl0',`r0')
define(`rl1',`r1')
define(`rl2',`r2')
define(`rl3',`r3')

define(`ul0',`r4')
define(`ul1',`r5')
define(`ul2',`r6')
define(`ul3',`r7')

define(`vl0',`r22')
define(`vl1',`r23')
define(`vl2',`r24')
define(`vl3',`r25')

define(`numb_mask',`r21')

define(`NAIL_BITS',`GMP_NAIL_BITS')
define(`CYSH',`GMP_NUMB_BITS')

dnl  This declaration is munged by configure
NAILS_SUPPORT(1-63)

dnl  Runs at 2.5 cycles/limb.  It would be possible to reach 2.0 cycles/limb
dnl  with 8-way unrolling.

ASM_START()
PROLOGUE(mpn_add_n)
	lda	numb_mask, -1(r31)
	srl	numb_mask, NAIL_BITS, numb_mask
	bis	r31,	r31,	r20

	and	n,	3,	r25
	lda	n,	-4(n)
	beq	r25,	L_4_or_more

Loop0:	ldq	ul0,	0(up)
	lda	up,	8(up)
	ldq	vl0,	0(vp)
	lda	vp,	8(vp)
	lda	rp,	8(rp)
	lda	r25,	-1(r25)
	addq	ul0,	vl0,	rl0
	addq	rl0,	r20,	rl0
	and	rl0, numb_mask,	r28
	stq	r28,	-8(rp)
	srl	rl0,	CYSH,	r20
	bne	r25,	Loop0

	blt	n,	Lret

L_4_or_more:
	ldq	ul0,	0(up)
	ldq	vl0,	0(vp)
	ldq	ul1,	8(up)
	ldq	vl1,	8(vp)
	ldq	ul2,	16(up)
	ldq	vl2,	16(vp)
	ldq	ul3,	24(up)
	ldq	vl3,	24(vp)
	lda	up,	32(up)
	lda	vp,	32(vp)
	lda	n,	-4(n)
	bge	n,	L_8_or_more
L_0_to_7:
	addq	ul0,	vl0,	rl0	C		main-add 0
	addq	rl0,	r20,	rl0	C		cy-add 0
	addq	ul1,	vl1,	rl1	C		main-add 1
	srl	rl0,	CYSH,	r20	C		gen cy 0
	addq	rl1,	r20,	rl1	C		cy-add 1
	and	rl0,numb_mask,	r27
	br	r31,	Lcj0

L_8_or_more:
	addq	ul0,	vl0,	rl0	C		main-add 0
	ldq	ul0,	0(up)
	ldq	vl0,	0(vp)
	addq	rl0,	r20,	rl0	C		cy-add 0
	addq	ul1,	vl1,	rl1	C		main-add 1
	srl	rl0,	CYSH,	r20	C		gen cy 0
	ldq	ul1,	8(up)
	ldq	vl1,	8(vp)
	addq	rl1,	r20,	rl1	C		cy-add 1
	and	rl0,numb_mask,	r27
	addq	ul2,	vl2,	rl2	C		main-add 2
	srl	rl1,	CYSH,	r20	C		gen cy 1
	ldq	ul2,	16(up)
	ldq	vl2,	16(vp)
	addq	rl2,	r20,	rl2	C		cy-add 2
	and	rl1,numb_mask,	r28
	stq	r27,	0(rp)
	addq	ul3,	vl3,	rl3	C		main-add 3
	srl	rl2,	CYSH,	r20	C		gen cy 2
	ldq	ul3,	24(up)
	ldq	vl3,	24(vp)
	addq	rl3,	r20,	rl3	C		cy-add 3
	and	rl2,numb_mask,	r27
	stq	r28,	8(rp)
	lda	rp,	32(rp)
	lda	up,	32(up)
	lda	vp,	32(vp)
	lda	n,	-4(n)
	blt	n,	L_end

	ALIGN(32)
Loop:
	addq	ul0,	vl0,	rl0	C		main-add 0
	srl	rl3,	CYSH,	r20	C		gen cy 3
	ldq	ul0,	0(up)
	ldq	vl0,	0(vp)

	addq	rl0,	r20,	rl0	C		cy-add 0
	and	rl3,numb_mask,	r28
	stq	r27,	-16(rp)
	bis	r31,	r31,	r31

	addq	ul1,	vl1,	rl1	C		main-add 1
	srl	rl0,	CYSH,	r20	C		gen cy 0
	ldq	ul1,	8(up)
	ldq	vl1,	8(vp)

	addq	rl1,	r20,	rl1	C		cy-add 1
	and	rl0,numb_mask,	r27
	stq	r28,	-8(rp)
	bis	r31,	r31,	r31

	addq	ul2,	vl2,	rl2	C		main-add 2
	srl	rl1,	CYSH,	r20	C		gen cy 1
	ldq	ul2,	16(up)
	ldq	vl2,	16(vp)

	addq	rl2,	r20,	rl2	C		cy-add 2
	and	rl1,numb_mask,	r28
	stq	r27,	0(rp)
	bis	r31,	r31,	r31

	addq	ul3,	vl3,	rl3	C		main-add 3
	srl	rl2,	CYSH,	r20	C		gen cy 2
	ldq	ul3,	24(up)
	ldq	vl3,	24(vp)

	addq	rl3,	r20,	rl3	C		cy-add 3
	and	rl2,numb_mask,	r27
	stq	r28,	8(rp)
	bis	r31,	r31,	r31

	bis	r31,	r31,	r31
	lda	n,	-4(n)
	lda	up,	32(up)
	lda	vp,	32(vp)

	bis	r31,	r31,	r31
	bis	r31,	r31,	r31
	lda	rp,	32(rp)
	bge	n,	Loop
L_end:
	addq	ul0,	vl0,	rl0	C		main-add 0
	srl	rl3,	CYSH,	r20	C		gen cy 3
	addq	rl0,	r20,	rl0	C		cy-add 0
	and	rl3,numb_mask,	r28
	stq	r27,	-16(rp)
	addq	ul1,	vl1,	rl1	C		main-add 1
	srl	rl0,	CYSH,	r20	C		gen cy 0
	addq	rl1,	r20,	rl1	C		cy-add 1
	and	rl0,numb_mask,	r27
	stq	r28,	-8(rp)
Lcj0:	addq	ul2,	vl2,	rl2	C		main-add 2
	srl	rl1,	CYSH,	r20	C		gen cy 1
	addq	rl2,	r20,	rl2	C		cy-add 2
	and	rl1,numb_mask,	r28
	stq	r27,	0(rp)
	addq	ul3,	vl3,	rl3	C		main-add 3
	srl	rl2,	CYSH,	r20	C		gen cy 2
	addq	rl3,	r20,	rl3	C		cy-add 3
	and	rl2,numb_mask,	r27
	stq	r28,	8(rp)

	srl	rl3,	CYSH,	r20	C		gen cy 3
	and	rl3,numb_mask,	r28
	stq	r27,	16(rp)
	stq	r28,	24(rp)
Lret:
	and	r20,	1,	r0
	ret	r31,	(r26),	1
EPILOGUE(mpn_add_n)
ASM_END()
