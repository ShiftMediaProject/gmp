dnl  PowerPC-64 mpn_and_n, mpn_andn_n, mpn_nand_n, mpn_ior_n, mpn_iorn_n,
dnl  mpn_nior_n, mpn_xor_n, mpn_xnor_n -- mpn bitwise logical operations.

dnl  Copyright 2003, 2004 Free Software Foundation, Inc.

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
C POWER3/PPC630:     1.75
C POWER4/PPC970:     2.125

C	   POWER3/PPC630   POWER4/PPC970
C     1		7.38		7.85
C     2		4.93		5.73
C     3		3.51		5.51
C     4		3.40		4.81
C     5		3.35		3.76
C     6		3.01		3.59
C     7		2.63		3.29
C     8		2.79		3.13
C     9		2.70		3.18
C    10		2.45		2.93
C    11		2.34		2.91
C    12		2.40		2.79
C    13		2.43		2.88
C    14		2.34		2.68
C    15		2.19		2.68
C    50		1.91		2.68
C   100		1.83		2.32
C   200		1.89		2.21
C   400		1.78		2.14
C  1000		1.76		2.10
C  2000		1.76		2.14
C  4000		2.78		2.58
C  8000		3.02		2.60
C 16000		3.07		7.67
C 32000		3.07	       17.92
C 64000		3.12	       21.57

ifdef(`OPERATION_and_n',
`	define(`func',`mpn_and_n')
	define(`logop',		`and')')
ifdef(`OPERATION_andn_n',
`	define(`func',`mpn_andn_n')
	define(`logop',		`andc')')
ifdef(`OPERATION_nand_n',
`	define(`func',`mpn_nand_n')
	define(`logop',		`nand')')
ifdef(`OPERATION_ior_n',
`	define(`func',`mpn_ior_n')
	define(`logop',		`or')')
ifdef(`OPERATION_iorn_n',
`	define(`func',`mpn_iorn_n')
	define(`logop',		`orc')')
ifdef(`OPERATION_nior_n',
`	define(`func',`mpn_nior_n')
	define(`logop',		`nor')')
ifdef(`OPERATION_xor_n',
`	define(`func',`mpn_xor_n')
	define(`logop',		`xor')')
ifdef(`OPERATION_xnor_n',
`	define(`func',`mpn_xnor_n')
	define(`logop',		`eqv')')

C INPUT PARAMETERS
C rp	r3
C up	r4
C vp	r5
C n	r6

ASM_START()
PROLOGUE(func)
	ld	r8, 0(r4)	C read lowest u limb
	ld	r9, 0(r5)	C read lowest v limb
	addi	r6, r6, 3	C compute branch count (1)
	rldicl.	r0, r6, 3, 59	C r0 = (n-1 & 3) << 3; cr0 = (n == 4(t+1))?
	cmpldi	cr6, r0, 16	C cr6 = (n cmp 4t + 3)

	srdi	r6, r6, 2	C compute branch count (2)
	mtctr	r6		C compute branch count (3)

	ld	r6, 0(r4)	C read lowest u limb (again)
	ld	r7, 0(r5)	C read lowest v limb (again)

	add	r5, r5, r0	C offset vp
	add	r4, r4, r0	C offset up
	add	r3, r3, r0	C offset rp

	beq	cr0, .LL01
	blt	cr6, .LL10
	beq	cr6, .LL11
	b	.LL00

.Loop:	ld	r8, -24(r4)
	ld	r9, -24(r5)
	logop	r10, r6, r7
	std	r10, -32(r3)
.LL00:	ld	r6, -16(r4)
	ld	r7, -16(r5)
	logop	r10, r8, r9
	std	r10, -24(r3)
.LL11:	ld	r8, -8(r4)
	ld	r9, -8(r5)
	logop	r10, r6, r7
	std	r10, -16(r3)
.LL10:	ld	r6, 0(r4)
	ld	r7, 0(r5)
	logop	r10, r8, r9
	std	r10, -8(r3)
.LL01:	addi	r5, r5, 32
	addi	r4, r4, 32
	addi	r3, r3, 32
	bdnz	.Loop

	logop	r10, r6, r7
	std	r10, -32(r3)
	blr
EPILOGUE()
