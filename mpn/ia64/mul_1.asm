dnl  IA-64 mpn_mul_1 -- Multiply a limb vector with a limb and store the result
dnl  in a second limb vector.

dnl  Copyright (C) 2000 Free Software Foundation, Inc.

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

dnl  This code runs at 10 cycles/limb on the Itanium.  That's pretty poor, and
dnl  very far from the peak execution speed of the Itanium pipeline.  With
dnl  n-way unrolling we should be able to get close to 2 cycles/limb.  See
dnl  README.

include(`../config.m4')

C INPUT PARAMETERS
C rp = r32
C s1p = r33
C n = r34
C s2limb = r35

ASM_START()
PROLOGUE(mpn_mul_1)
	.prologue
	.save	ar.lc, r2
		mov	r2 = ar.lc
	.body
		cmp.eq	p8, p9 = 1, r34
		cmp.eq	p10, p11 = 2, r34
		add	r34 = -3, r34
		setf.sig f97 = r35
		add	r8 = 0, r0
		cmp.ne	p6, p7 = r0, r0
		mov	r20 = r32
		ldf8	f96 = [r33], 8;;
		xma.l	f98 = f96, f97, f0
		xma.hu	f99 = f96, f97, f0
	(p8)	br	.Lend1
		ldf8	f96 = [r33], 8
	(p10)	br	.Lend2
		mov	ar.lc = r34
	.align 16
.Loop:		getf.sig r14 = f98
		getf.sig r15 = f99;;
		xma.l	f98 = f96, f97, f0
		xma.hu	f99 = f96, f97, f0;;
		ldf8	f96 = [r33], 8;;
	(p6)	add	r14 = r14, r8, 1
	(p7)	add	r14 = r14, r8;;
	(p6)	cmp.leu	p6, p7 = r14, r8
	(p7)	cmp.ltu	p6, p7 = r14, r8
		mov	r8 = r15
		st8	[r20] = r14, 8
		br.cloop.dptk	.Loop

		getf.sig r14 = f98
		getf.sig r15 = f99;;
		xma.l	f98 = f96, f97, f0
		xma.hu	f99 = f96, f97, f0;;
	(p6)	add	r14 = r14, r8, 1
	(p7)	add	r14 = r14, r8;;
	(p6)	cmp.leu	p6, p7 = r14, r8
	(p7)	cmp.ltu	p6, p7 = r14, r8
		mov	r8 = r15
		st8	[r20] = r14, 8
		getf.sig r14 = f98
		getf.sig r15 = f99;;
	(p6)	add	r14 = r14, r8, 1
	(p7)	add	r14 = r14, r8;;
	(p6)	cmp.leu	p6, p7 = r14, r8
	(p7)	cmp.ltu	p6, p7 = r14, r8
		mov	r8 = r15
		st8	[r20] = r14;;
	(p6)	add	r8 = 1, r8
		mov	ar.lc = r2
		br.ret.sptk.many b0
.Lend2:
		stf8	[r20] = f98, 8
		xma.l	f100 = f96, f97, f0
		getf.sig r14 = f99
		xma.hu	f101 = f96, f97, f0;;
		getf.sig r16 = f100
		getf.sig r8 = f101;;
		add	r14 = r14, r16;;
		cmp.ltu	p6, p7 = r14, r16
		st8	[r20] = r14;;
	(p6)	add	r8 = 1, r8
		mov	ar.lc = r2
		br.ret.sptk.many b0
.Lend1:
		stf8	[r20] = f98
		getf.sig r8 = f99
		mov	ar.lc = r2
		br.ret.sptk.many b0
EPILOGUE(mpn_mul_1)
ASM_END()
