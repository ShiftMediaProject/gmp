dnl  PowerPC-64 mpn_add_n/mpn_sub_n -- mpn addition and subtraction.

dnl  Copyright 1999, 2000, 2001, 2003, 2004 Free Software Foundation, Inc.

dnl  This file is part of the GNU MP Library.

dnl  The GNU MP Library is free software; you can redistribute it and/or modify
dnl  it under the terms of the GNU Lesser General Public License as published
dnl  by the Free Software Foundation; either version 2.1 of the License, or (at
dnl  your option) any later version.

dnl  The GNU MP Library is distributed in the hope that it will be useful, but
dnl  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
dnl  License for more details.

dnl  You should have received a copy of the GNU Lesser General Public License
dnl  along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
dnl  the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
dnl  MA 02111-1307, USA.

include(`../config.m4')

C		cycles/limb
C POWER3/PPC630:     1.5
C POWER4/PPC970:     2

C	   POWER3/PPC630   POWER4/PPC970
C     1		9.35	       10.58
C     2		5.92		7.71
C     3		4.43		5.48
C     4		3.73		3.98
C     5		3.52		4.01
C     6		3.15		3.66
C     7		2.63		3.03
C     8		2.68		3.11
C     9		2.70		3.13
C    10		2.45		3.03
C    11		2.25		2.94
C    12		2.34		2.77
C    13		2.44		2.80
C    14		2.27		2.70
C    15		2.08		2.71
C    50		1.73		2.42
C   100		1.62		2.20
C   200		1.71		2.11
C   400		1.53		2.06
C  1000		1.52		2.03#
C  2000		1.51#		2.04
C  4000		2.65		2.46
C  8000		3.02		2.45
C 16000		3.02		4.76
C 32000		3.06	       19.44
C 64000		3.22	       21.20

C This code is a little bit slower for POWER3/PPC630 than the simple code used
C previously, but it is much faster for POWER4/PPC970.  The reason for the
C POWER3/PPC630 slowdown can be attributed to the saving and restoring of 4
C registers.

C INPUT PARAMETERS
C rp	r3
C up	r4
C vp	r5
C n	r6

ifdef(`OPERATION_add_n',`
  define(ADDSUBC,	adde)
  define(ADDSUB,	addc)
  define(func, mpn_add_n)
  define(GENRVAL,	`addi	r3, r3, 1')
  define(INITCY,	`addic	r0, r0, 0')
')
ifdef(`OPERATION_sub_n',`
  define(ADDSUBC,	subfe)
  define(ADDSUB,	subfc)
  define(func, mpn_sub_n)
  define(GENRVAL,	`neg	r3, r3')
  define(INITCY,	`addic	r0, r1, -1')
')


ASM_START()
PROLOGUE(func)
	std	r31, -8(r1)
	std	r30, -16(r1)
	std	r29, -24(r1)
	std	r28, -32(r1)

	rldicl.	r0, r6, 0,62	C r0 = n & 3, set cr0
	cmpdi	cr6, r0, 2
	addi	r6, r6, 3	C compute count...
	srdi	r6, r6, 2	C ...for ctr
	mtctr	r6		C copy count into ctr
	beq	cr0, .Lb00
	blt	cr6, .Lb01
	beq	cr6, .Lb10

.Lb11:	ld	r8, 0(r4)	C load s1 limb
	ld	r9, 0(r5)	C load s2 limb
	ld	r10, 8(r4)	C load s1 limb
	ld	r11, 8(r5)	C load s2 limb
	ld	r12, 16(r4)	C load s1 limb
	addi	r4, r4, 24
	ld	r0, 16(r5)	C load s2 limb
	addi	r5, r5, 24
	ADDSUB	r29, r9, r8
	ADDSUBC	r30, r11, r10
	ADDSUBC	r31, r0, r12
	std	r29, 0(r3)
	std	r30, 8(r3)
	std	r31, 16(r3)
	addi	r3, r3, 24
	bdnz	.Lgo
	b	.Lret

.Lb01:	ld	r12, 0(r4)	C load s1 limb
	addi	r4, r4, 8
	ld	r0, 0(r5)	C load s2 limb
	addi	r5, r5, 8
	ADDSUB	r31, r0, r12	C add
	std	r31, 0(r3)
	addi	r3, r3, 8
	bdnz	.Lgo
	b	.Lret

.Lb10:	ld	r10, 0(r4)	C load s1 limb
	ld	r11, 0(r5)	C load s2 limb
	ld	r12, 8(r4)	C load s1 limb
	addi	r4, r4, 16
	ld	r0, 8(r5)	C load s2 limb
	addi	r5, r5, 16
	ADDSUB	r30, r11, r10	C add
	ADDSUBC	r31, r0, r12	C add
	std	r30, 0(r3)
	std	r31, 8(r3)
	addi	r3, r3, 16
	bdnz	.Lgo
	b	.Lret

.Lb00:	INITCY			C clear/set cy
.Lgo:	ld	r6, 0(r4)	C load s1 limb
	ld	r7, 0(r5)	C load s2 limb
	ld	r8, 8(r4)	C load s1 limb
	ld	r9, 8(r5)	C load s2 limb
	ld	r10, 16(r4)	C load s1 limb
	ld	r11, 16(r5)	C load s2 limb
	ld	r12, 24(r4)	C load s1 limb
	ld	r0, 24(r5)	C load s2 limb
	bdz	.Lend

	addi	r4, r4, 32
	addi	r5, r5, 32

.Loop:	ADDSUBC	r28, r7, r6
	ld	r6, 0(r4)	C load s1 limb
	ld	r7, 0(r5)	C load s2 limb
	ADDSUBC	r29, r9, r8
	ld	r8, 8(r4)	C load s1 limb
	ld	r9, 8(r5)	C load s2 limb
	ADDSUBC	r30, r11, r10
	ld	r10, 16(r4)	C load s1 limb
	ld	r11, 16(r5)	C load s2 limb
	ADDSUBC	r31, r0, r12
	ld	r12, 24(r4)	C load s1 limb
	ld	r0, 24(r5)	C load s2 limb
	std	r28, 0(r3)
	addi	r4, r4, 32
	std	r29, 8(r3)
	addi	r5, r5, 32
	std	r30, 16(r3)
	std	r31, 24(r3)
	addi	r3, r3, 32
	bdnz	.Loop		C decrement ctr and loop back

.Lend:	ADDSUBC	r28, r7, r6
	ADDSUBC	r29, r9, r8
	ADDSUBC	r30, r11, r10
	ADDSUBC	r31, r0, r12
	std	r28, 0(r3)
	std	r29, 8(r3)
	std	r30, 16(r3)
	std	r31, 24(r3)

.Lret:	ld	r31, -8(r1)
	ld	r30, -16(r1)
	ld	r29, -24(r1)
	ld	r28, -32(r1)

	subfe	r3, r0, r0	C -cy
	GENRVAL
	blr
EPILOGUE()
