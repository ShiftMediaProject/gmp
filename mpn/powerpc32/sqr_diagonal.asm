dnl PowerPC-32 mpn_sqr_diagonal.

dnl Copyright 2001, 2002 Free Software Foundation, Inc.

dnl This file is part of the GNU MP Library.

dnl The GNU MP Library is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU Lesser General Public License as published by
dnl the Free Software Foundation; either version 2.1 of the License, or (at your
dnl option) any later version.

dnl The GNU MP Library is distributed in the hope that it will be useful, but
dnl WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
dnl License for more details.

dnl You should have received a copy of the GNU Lesser General Public License
dnl along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
dnl the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
dnl MA 02111-1307, USA.


dnl INPUT PARAMETERS
dnl res_ptr	r3
dnl s1_ptr	r4
dnl size	r5

dnl This runs at 4 cycles/limb on the PPC604.  That's saturating the multiply
dnl unit.

include(`../config.m4')

ASM_START()
PROLOGUE(mpn_sqr_diagonal)
	lwz	r6,0(r4)
	mtctr	r5

	addi	r3,r3,-4
	bdz	L(end1)

	lwzu	r7,4(r4)
	mullw	r9,r6,r6
	mulhwu	r11,r6,r6
	bdz	L(end2)

	lwzu	r6,4(r4)
	mullw	r8,r7,r7
	mulhwu	r10,r7,r7
	bdz	L(ende)

L(oop):	lwzu	r7,4(r4)
	stw	r9,4(r3)
	mullw	r9,r6,r6
	stwu	r11,8(r3)
	mulhwu	r11,r6,r6
	bdz	L(endo)
	lwzu	r6,4(r4)
	stw	r8,4(r3)
	mullw	r8,r7,r7
	stwu	r10,8(r3)
	mulhwu	r10,r7,r7
	bdnz	L(oop)

L(ende):
	stw	r9,4(r3)
	mullw	r9,r6,r6
	stw	r11,8(r3)
	mulhwu	r11,r6,r6
	stw	r8,12(r3)
	stw	r10,16(r3)
	stw	r9,20(r3)
	stw	r11,24(r3)
	blr
L(endo):
	stw	r8,4(r3)
	mullw	r8,r7,r7
	stw	r10,8(r3)
	mulhwu	r10,r7,r7
	stw	r9,12(r3)
	stw	r11,16(r3)
	stw	r8,20(r3)
	stw	r10,24(r3)
	blr

L(end2):
	mullw	r8,r7,r7
	stw	r9,4(r3)
	mulhwu	r10,r7,r7
	stw	r11,8(r3)
	stw	r8,12(r3)
	stw	r10,16(r3)
	blr
L(end1):
	mullw	r9,r6,r6
	mulhwu	r11,r6,r6
	stw	r9,4(r3)
	stw	r11,8(r3)
	blr
EPILOGUE(mpn_sqr_diagonal)
