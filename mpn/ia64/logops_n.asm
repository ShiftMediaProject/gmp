dnl  IA-64 mpn_and_n, mpn_andn_n, mpn_nand_n, mpn_ior_n, mpn_iorn_n,
dnl  mpn_nior_n, mpn_xor_n, mpn_xnor_n -- mpn bitwise logical operations.

dnl  Copyright 2003 Free Software Foundation, Inc.
dnl
dnl  This file is part of the GNU MP Library.
dnl
dnl  The GNU MP Library is free software; you can redistribute it and/or modify
dnl  it under the terms of the GNU Lesser General Public License as published
dnl  by the Free Software Foundation; either version 2.1 of the License, or (at
dnl  your option) any later version.
dnl
dnl  The GNU MP Library is distributed in the hope that it will be useful, but
dnl  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
dnl  License for more details.
dnl
dnl  You should have received a copy of the GNU Lesser General Public License
dnl  along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
dnl  the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
dnl  MA 02111-1307, USA.

include(`../config.m4')

C         cycles/limb
C Itanium 2:  1

ifdef(`OPERATION_and_n',
`	define(`func',`mpn_and_n')
	define(`logop',		`and	$1 = $2, $3')
	define(`notormov',	`mov	$1 = $2')')
ifdef(`OPERATION_andn_n',
`	define(`func',`mpn_andn_n')
	define(`logop',		`andcm	$1 = $2, $3')
	define(`notormov',	`mov	$1 = $2')')
ifdef(`OPERATION_nand_n',
`	define(`func',`mpn_nand_n')
	define(`logop',		`and	$1 = $2, $3')
	define(`notormov',	`sub	$1 = -1, $2')')
ifdef(`OPERATION_ior_n',
`	define(`func',`mpn_ior_n')
	define(`logop',		`or	$1 = $2, $3')
	define(`notormov',	`mov	$1 = $2')')
ifdef(`OPERATION_iorn_n',
`	define(`func',`mpn_iorn_n')
	define(`logop',		`andcm	$1 = $3, $2')
	define(`notormov',	`sub	$1 = -1, $2')')
ifdef(`OPERATION_nior_n',
`	define(`func',`mpn_nior_n')
	define(`logop',		`or	$1 = $2, $3')
	define(`notormov',	`sub	$1 = -1, $2')')
ifdef(`OPERATION_xor_n',
`	define(`func',`mpn_xor_n')
	define(`logop',		`xor	$1 = $2, $3')
	define(`notormov',	`mov	$1 = $2')')
ifdef(`OPERATION_xnor_n',
`	define(`func',`mpn_xnor_n')
	define(`logop',		`xor	$1 = $2, $3')
	define(`notormov',	`sub	$1 = -1, $2')')

C INPUT PARAMETERS
define(`rp', `r32')
define(`up', `r33')
define(`vp', `r34')
define(`n', `r35')

MULFUNC_PROLOGUE(mpn_and_n mpn_andn_n mpn_nand_n mpn_ior_n mpn_iorn_n mpn_nior_n mpn_xor_n mpn_xnor_n)

ASM_START()
PROLOGUE(func)
	.prologue
	.save	ar.lc, r2
	.body
ifdef(`HAVE_ABI_32',
`	addp4	rp = 0, rp
	addp4	up = 0, up
	addp4	vp = 0, vp
	zxt4	n = n
	;;
')
	and		r14 = 3, n
	adds		n = -1, n
	mov		r2 = ar.lc
	;;
	shr.u		r15 = n, 2
	cmp.eq		p6, p0 = 0, r14
	cmp.eq		p7, p0 = 1, r14
	cmp.eq		p8, p0 = 3, r14
	;;
	mov.i		ar.lc = r15
   (p6)	br.dptk		.Lb00
   (p7)	br.dptk		.Lb01
   (p8)	br.dptk		.Lb11

.Lb10:	ld8		r18 = [up], 8
	ld8		r22 = [vp], 8
	;;
	ld8		r19 = [up], 8
	ld8		r23 = [vp], 8
	br.cloop.dptk	.grt2

	logop(		r14, r18, r22)
	;;
	logop(		r15, r19, r23)
	notormov(	r8, r14)
	br		.Lcj2

.grt2:	ld8		r16 = [up], 8
	ld8		r20 = [vp], 8
	;;
	ld8		r17 = [up], 8
	ld8		r21 = [vp], 8
	;;
	logop(		r14, r18, r22)
	ld8		r18 = [up], 8
	ld8		r22 = [vp], 8
	;;
	logop(		r15, r19, r23)
	ld8		r19 = [up], 8
	notormov(	r8, r14)
	ld8		r23 = [vp], 8
	br.cloop.dptk	.Loop
	br		.Lcj6

.Lb11:	ld8		r17 = [up], 8
	ld8		r21 = [vp], 8
	;;
	ld8		r18 = [up], 8
	ld8		r22 = [vp], 8
	;;
	ld8		r19 = [up], 8
	ld8		r23 = [vp], 8
	br.cloop.dptk	.grt3

	logop(		r15, r17, r21)
	;;
	logop(		r14, r18, r22)
	notormov(	r9, r15)
	br		.Lcj3

.grt3:	ld8		r16 = [up], 8
	ld8		r20 = [vp], 8
	;;
	logop(		r15, r17, r21)
	ld8		r17 = [up], 8
	ld8		r21 = [vp], 8
	;;
	logop(		r14, r18, r22)
	ld8		r18 = [up], 8
	notormov(	r9, r15)
	ld8		r22 = [vp], 8
	br		.LL11

.Lb00:	ld8		r16 = [up], 8
	ld8		r20 = [vp], 8
	;;
	ld8		r17 = [up], 8
	ld8		r21 = [vp], 8
	;;
	ld8		r18 = [up], 8
	ld8		r22 = [vp], 8
	;;
	ld8		r19 = [up], 8
	ld8		r23 = [vp], 8
	br.cloop.dptk	.grt4

	logop(		r14, r16, r20)
	;;
	logop(		r15, r17, r21)
	notormov(	r8, r14)
	br		.Lcj4

.grt4:	logop(		r14, r16, r20)
	ld8		r16 = [up], 8
	ld8		r20 = [vp], 8
	;;
	logop(		r15, r17, r21)
	ld8		r17 = [up], 8
	notormov(	r8, r14)
	ld8		r21 = [vp], 8
	br		.LL00

.Lb01:	ld8		r19 = [up], 8
	ld8		r23 = [vp], 8
	br.cloop.dptk	.grt1

	logop(		r15, r19, r23)
	;;
	notormov(	r9, r15)
	br		.Lcj1

.grt1:	ld8		r16 = [up], 8
	ld8		r20 = [vp], 8
	;;
	ld8		r17 = [up], 8
	ld8		r21 = [vp], 8
	;;
	ld8		r18 = [up], 8
	ld8		r22 = [vp], 8
	;;
	logop(		r15, r19, r23)
	ld8		r19 = [up], 8
	ld8		r23 = [vp], 8
	br.cloop.dptk	.grt5

	logop(		r14, r16, r20)
	notormov(	r9, r15)
	br		.Lcj5

.grt5:	logop(		r14, r16, r20)
	ld8		r16 = [up], 8
	notormov(	r9, r15)
	ld8		r20 = [vp], 8
	br		.LL01

C *** MAIN LOOP START ***
	ALIGN(32)
.Loop:	st8		[rp] = r8, 8
	logop(		r14, r16, r20)
	notormov(	r9, r15)
	ld8		r16 = [up], 8
	ld8		r20 = [vp], 8
	nop.b		0
	;;
.LL01:	st8		[rp] = r9, 8
	logop(		r15, r17, r21)
	notormov(	r8, r14)
	ld8		r17 = [up], 8
	ld8		r21 = [vp], 8
	nop.b		0
	;;
.LL00:	st8		[rp] = r8, 8
	logop(		r14, r18, r22)
	notormov(	r9, r15)
	ld8		r18 = [up], 8
	ld8		r22 = [vp], 8
	nop.b		0
	;;
.LL11:	st8		[rp] = r9, 8
	logop(		r15, r19, r23)
	notormov(	r8, r14)
	ld8		r19 = [up], 8
	ld8		r23 = [vp], 8
	br.cloop.dptk	.Loop	;;
C *** MAIN LOOP END ***

.Lcj6:	st8		[rp] = r8, 8
	logop(		r14, r16, r20)
	notormov(	r9, r15)
	;;
.Lcj5:	st8		[rp] = r9, 8
	logop(		r15, r17, r21)
	notormov(	r8, r14)
	;;
.Lcj4:	st8		[rp] = r8, 8
	logop(		r14, r18, r22)
	notormov(	r9, r15)
	;;
.Lcj3:	st8		[rp] = r9, 8
	logop(		r15, r19, r23)
	notormov(	r8, r14)
	;;
.Lcj2:	st8		[rp] = r8, 8
	notormov(	r9, r15)
	;;
.Lcj1:	st8		[rp] = r9, 8
	mov	ar.lc = r2
	br.ret.sptk.many b0
EPILOGUE()
ASM_END()
