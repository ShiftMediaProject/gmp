dnl  IA-64 mpn_lshift/mpn_rshift.

dnl  Copyright 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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

C         cycles/limb	1   2   3   4   5   6   7   8   9  10  11  12  13  14
C Itanium:    2.0?
C Itanium 2:  1.0     12.3 9.5 6.7 5.3 4.6 4.2 3.9 3.6 3.1 3.0 2.8 3.0 2.8 2.4

C This code is scheduled deeply since the plain shift instructions shr and shl
C have a latency of 4 (on Itanium) or 3 (on Itanium 2).  Poor scheduling of
C these instructions cause a 10 cycle replay trap on Itanium.

C TODO
C  * Rearrange the loop trivially for better (1.5 c/l) performance on Itanium.
C  * Optimize function entry code
C  * The feed-in code might cause replay traps on Itanium.  Explicit bundling
C    might help, without hurting Itanium 2.

C INPUT PARAMETERS
define(`rp',`r32')
define(`up',`r33')
define(`n',`r34')
define(`cnt',`r35')

define(`tnc',`r9')

ifdef(`OPERATION_lshift',`
	define(`FSH',`shl')
	define(`BSH',`shr.u')
	define(`UPD',`-8')
	define(`func',`mpn_lshift')
')
ifdef(`OPERATION_rshift',`
	define(`FSH',`shr.u')
	define(`BSH',`shl')
	define(`UPD',`8')
	define(`func',`mpn_rshift')
')

MULFUNC_PROLOGUE(mpn_lshift mpn_rshift)

ASM_START()
PROLOGUE(func)
	.prologue
	.save		ar.lc, r2
	.body
ifdef(`HAVE_ABI_32',
`	addp4	rp = 0, rp
	addp4	up = 0, up
	sxt4	n = n
	zxt4	cnt = cnt
	;;
')
	and		r14 = 3, n
	add		r15 = -1, n
	add		n = -1, n
	sub		tnc = 64, cnt
	mov		r2 = ar.lc
	;;
	shr.u		r15 = r15, 2
	cmp.eq		p6, p0 = 1, r14
	cmp.eq		p7, p0 = 2, r14
	cmp.eq		p8, p0 = 3, r14
ifdef(`OPERATION_lshift',`
	shladd		up = n, 3, up
	shladd		rp = n, 3, rp
')
	;;
	mov.i		ar.lc = r15
   (p6)	br.dptk		.Lb01
   (p7)	br.dptk		.Lb10
   (p8)	br.dptk		.Lb11


.Lb00:	ld8		r18 = [up], UPD
	;;
	ld8		r19 = [up], UPD
	;;
	ld8		r16 = [up], UPD
	;;
	BSH		r8 = r18, tnc		C function return value
	ld8		r17 = [up], UPD
	br.cloop.sptk	.L_grt_4

	FSH		r24 = r18, cnt
	BSH		r25 = r19, tnc
	;;
	FSH		r26 = r19, cnt
	BSH		r27 = r16, tnc
	;;
	FSH		r20 = r16, cnt
	BSH		r21 = r17, tnc
	;;
	or		r14 = r25, r24
	FSH		r22 = r17, cnt
	BSH		r23 = r18, tnc
	br		.Lr4

.L_grt_4:
	FSH		r24 = r18, cnt
	BSH		r25 = r19, tnc
	ld8		r18 = [up], UPD
	;;
	FSH		r26 = r19, cnt
	BSH		r27 = r16, tnc
	ld8		r19 = [up], UPD
	;;
	FSH		r20 = r16, cnt
	BSH		r21 = r17, tnc
	ld8		r16 = [up], UPD
	;;
	or		r14 = r25, r24
	FSH		r22 = r17, cnt
	BSH		r23 = r18, tnc
	ld8		r17 = [up], UPD
	br.cloop.sptk	.Ltop
	br		.Lbot


.Lb01:	ld8		r17 = [up], UPD
	br.cloop.sptk	.L_grt_1

	BSH		r8 = r17, tnc		C function return value
	FSH		r22 = r17, cnt
	br		.Lr1			C return

.L_grt_1:
	ld8		r18 = [up], UPD
	;;
	ld8		r19 = [up], UPD
	;;
	BSH		r8 = r17, tnc		C function return value
	ld8		r16 = [up], UPD
	;;
	FSH		r22 = r17, cnt
	BSH		r23 = r18, tnc
	ld8		r17 = [up], UPD
	br.cloop.sptk	.L_grt_5

	FSH		r24 = r18, cnt
	BSH		r25 = r19, tnc
	ld8		r18 = [up], UPD
	;;
	FSH		r26 = r19, cnt
	BSH		r27 = r16, tnc
	ld8		r19 = [up], UPD
	;;
	or		r15 = r23, r22
	FSH		r20 = r16, cnt
	BSH		r21 = r17, tnc
	br		.Lr5

.L_grt_5:
	FSH		r24 = r18, cnt
	BSH		r25 = r19, tnc
	ld8		r18 = [up], UPD
	;;
	FSH		r26 = r19, cnt
	BSH		r27 = r16, tnc
	ld8		r19 = [up], UPD
	;;
	or		r15 = r23, r22
	FSH		r20 = r16, cnt
	BSH		r21 = r17, tnc
	ld8		r16 = [up], UPD
	br		.LL01


.Lb10:	ld8		r16 = [up], UPD
	;;
	ld8		r17 = [up], UPD
	br.cloop.sptk	.L_grt_2

	BSH		r8 = r16, tnc		C function return value
	;;
	FSH		r20 = r16, cnt
	BSH		r21 = r17, tnc
	;;
	or		r14 = r21, r20
	FSH		r22 = r17, cnt
	br		.Lr2			C return

.L_grt_2:
	ld8		r18 = [up], UPD
	;;
	BSH		r8 = r16, tnc		C function return value
	ld8		r19 = [up], UPD
	;;
	FSH		r20 = r16, cnt
	BSH		r21 = r17, tnc
	ld8		r16 = [up], UPD
	;;
	FSH		r22 = r17, cnt
	BSH		r23 = r18, tnc
	ld8		r17 = [up], UPD
	br.cloop.sptk	.L_grt_6

	FSH		r24 = r18, cnt
	BSH		r25 = r19, tnc
	ld8		r18 = [up], UPD
	;;
	or		r14 = r21, r20
	FSH		r26 = r19, cnt
	BSH		r27 = r16, tnc
	br		.Lr6

.L_grt_6:
	FSH		r24 = r18, cnt
	BSH		r25 = r19, tnc
	ld8		r18 = [up], UPD
	;;
	or		r14 = r21, r20
	FSH		r26 = r19, cnt
	BSH		r27 = r16, tnc
	ld8		r19 = [up], UPD
	br		.LL10


.Lb11:	ld8		r19 = [up], UPD
	;;
	ld8		r16 = [up], UPD
	;;
	ld8		r17 = [up], UPD
	br.cloop.sptk	.L_grt_3

	BSH		r8 = r19, tnc		C function return value
	;;
	FSH		r26 = r19, cnt
	BSH		r27 = r16, tnc
	;;
	FSH		r20 = r16, cnt
	BSH		r21 = r17, tnc
	;;
	FSH		r22 = r17, cnt
	or		r15 = r27, r26
	br		.Lr3			C return

.L_grt_3:
	BSH		r8 = r19, tnc		C function return value
	ld8		r18 = [up], UPD
	;;
	FSH		r26 = r19, cnt
	BSH		r27 = r16, tnc
	ld8		r19 = [up], UPD
	;;
	FSH		r20 = r16, cnt
	BSH		r21 = r17, tnc
	ld8		r16 = [up], UPD
	;;
	FSH		r22 = r17, cnt
	BSH		r23 = r18, tnc
	ld8		r17 = [up], UPD
	br.cloop.sptk	.L_grt_7

	or		r15 = r27, r26
	FSH		r24 = r18, cnt
	BSH		r25 = r19, tnc
	br		.Lr7

.L_grt_7:
	or		r15 = r27, r26
	FSH		r24 = r18, cnt
	BSH		r25 = r19, tnc
	ld8		r18 = [up], UPD
	br		.LL11

C *** MAIN LOOP START ***
	ALIGN(32)
.Ltop:	or		r15 = r27, r26
	FSH		r24 = r18, cnt
	BSH		r25 = r19, tnc
	st8		[rp] = r14, UPD
	ld8		r18 = [up], UPD
	nop.b		0
	;;
.LL11:	or		r14 = r21, r20
	FSH		r26 = r19, cnt
	BSH		r27 = r16, tnc
	st8		[rp] = r15, UPD
	ld8		r19 = [up], UPD
	nop.b		0
	;;
.LL10:	or		r15 = r23, r22
	FSH		r20 = r16, cnt
	BSH		r21 = r17, tnc
	st8		[rp] = r14, UPD
	ld8		r16 = [up], UPD
	nop.b		0
	;;
.LL01:	or		r14 = r25, r24
	FSH		r22 = r17, cnt
	BSH		r23 = r18, tnc
	st8		[rp] = r15, UPD
	ld8		r17 = [up], UPD
	br.cloop.sptk	.Ltop
C *** MAIN LOOP END ***

.Lbot:	or		r15 = r27, r26
	FSH		r24 = r18, cnt
	BSH		r25 = r19, tnc
	st8		[rp] = r14, UPD
	;;
.Lr7:	or		r14 = r21, r20
	FSH		r26 = r19, cnt
	BSH		r27 = r16, tnc
	st8		[rp] = r15, UPD
	;;
.Lr6:	or		r15 = r23, r22
	FSH		r20 = r16, cnt
	BSH		r21 = r17, tnc
	st8		[rp] = r14, UPD
	;;
.Lr5:	or		r14 = r25, r24
	FSH		r22 = r17, cnt
	st8		[rp] = r15, UPD
	;;

.Lr4:	or		r15 = r27, r26
	st8		[rp] = r14, UPD
	;;
.Lr3:	or		r14 = r21, r20
	st8		[rp] = r15, UPD
	;;
.Lr2:	st8		[rp] = r14, UPD
	;;
.Lr1:	st8		[rp] = r22, UPD
	mov	ar.lc = r2
	br.ret.sptk.many b0
EPILOGUE(func)
ASM_END()
