dnl  IA-64 mpn_mul_1 -- Multiply a limb vector with a limb and store the result
dnl  in a second limb vector.

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

C         cycles/limb
C Itanium:    ?
C Itanium 2:  2

C TODO
C  * Optimize feed-in and wind-down code, both for speed and code density.
C  * Could greatly reduce code size by using conditional execution of ldf8
C    instructions and thereby allow code sharing between feed-in code and code
C    for small n.  (This is true for all similar ia64 functions.)

C INPUT PARAMETERS
define(`rp', `r32')
define(`up', `r33')
define(`n', `r34')
define(`vl', `r35')

ASM_START()
PROLOGUE(mpn_mul_1)
	.prologue
	.save	ar.lc, r2
	mov		r2 = ar.lc
	.body
ifdef(`HAVE_ABI_32',
`	addp4	rp = 0, rp
	addp4	up = 0, up
	zxt4	n = n
	;;
')
	adds		r15 = -1, n
	;;
	setf.sig	f6 = vl
	shr.u		r31 = r15, 2
	and		r14 = 3, n
	;;
	mov		ar.lc = r31
	cmp.eq		p6, p0 = 0, r14
	cmp.eq		p7, p0 = 2, r14
	cmp.eq		p8, p0 = 3, r14
	;;
   (p6)	br.dptk		.Lb00
   (p7)	br.dptk		.Lb10
   (p8)	br.dptk		.Lb11


.Lb01:	ldf8		f35 = [up], 8
	cmp.ne		p6, p7 = r0, r0
	mov		r20 = 0
	br.cloop.dptk	.grt1

	xma.l		f39 = f35, f6, f0
	xma.hu		f43 = f35, f6, f0
	;;
	getf.sig	r8 = f43
	stf8		[rp] = f39
	mov		ar.lc = r2
	br.ret.sptk.many b0

.grt1:	ldf8		f32 = [up], 8
	cmp.ne		p8, p9 = r0, r0
	;;
	ldf8		f33 = [up], 8
	;;
	ldf8		f34 = [up], 8
	xma.l		f39 = f35, f6, f0
	xma.hu		f43 = f35, f6, f0
	;;
	ldf8		f35 = [up], 8
	br.cloop.dptk	.grt5

	xma.l		f36 = f32, f6, f0
	xma.hu		f40 = f32, f6, f0
	;;
	stf8		[rp] = f39, 8
	xma.l		f37 = f33, f6, f0
	xma.hu		f41 = f33, f6, f0
	;;
	getf.sig	r21 = f43
	getf.sig	r18 = f36
	xma.l		f38 = f34, f6, f0
	xma.hu		f42 = f34, f6, f0
	;;
	getf.sig	r22 = f40
	getf.sig	r19 = f37
	xma.l		f39 = f35, f6, f0
	xma.hu		f43 = f35, f6, f0
	;;
	getf.sig	r23 = f41
	getf.sig	r16 = f38
	add		r24 = r17, r20
	br		.Lcj5

.grt5:	xma.l		f36 = f32, f6, f0
	xma.hu		f40 = f32, f6, f0
	;;
	getf.sig	r17 = f39
	ldf8		f32 = [up], 8
	xma.l		f37 = f33, f6, f0
	xma.hu		f41 = f33, f6, f0
	;;
	getf.sig	r21 = f43
	ldf8		f33 = [up], 8
	xma.l		f38 = f34, f6, f0
	;;
	getf.sig	r18 = f36
	xma.hu		f42 = f34, f6, f0
	;;
	getf.sig	r22 = f40
	ldf8		f34 = [up], 8
	xma.l		f39 = f35, f6, f0
	;;
	getf.sig	r19 = f37
	xma.hu		f43 = f35, f6, f0
	br		.LL01


.Lb10:	ldf8		f34 = [up], 8
	cmp.ne		p8, p9 = r0, r0
	mov		r23 = 0
	;;
	ldf8		f35 = [up], 8
	cmp.ne		p6, p7 = r0, r0
	br.cloop.dptk	.grt2

	xma.l		f38 = f34, f6, f0
	xma.hu		f42 = f34, f6, f0
	;;
	stf8		[rp] = f38, 8
	xma.l		f39 = f35, f6, f42
	xma.hu		f43 = f35, f6, f42
	;;
	getf.sig	r8 = f43
	stf8		[rp] = f39
	mov		ar.lc = r2
	br.ret.sptk.many b0


.grt2:	ldf8		f32 = [up], 8
	;;
	ldf8		f33 = [up], 8
	xma.l		f38 = f34, f6, f0
	xma.hu		f42 = f34, f6, f0
	;;
	ldf8		f34 = [up], 8
	xma.l		f39 = f35, f6, f0
	xma.hu		f43 = f35, f6, f0
	;;
	ldf8		f35 = [up], 8
	br.cloop.dptk	.grt6

	stf8		[rp] = f38, 8
	xma.l		f36 = f32, f6, f0
	xma.hu		f40 = f32, f6, f0
	;;
	getf.sig	r20 = f42
	getf.sig	r17 = f39
	xma.l		f37 = f33, f6, f0
	xma.hu		f41 = f33, f6, f0
	;;
	getf.sig	r21 = f43
	getf.sig	r18 = f36
	xma.l		f38 = f34, f6, f0
	xma.hu		f42 = f34, f6, f0
	;;
	getf.sig	r22 = f40
	getf.sig	r19 = f37
	xma.l		f39 = f35, f6, f0
	xma.hu		f43 = f35, f6, f0
	br		.Lcj6

.grt6:	getf.sig	r16 = f38
	xma.l		f36 = f32, f6, f0
	xma.hu		f40 = f32, f6, f0
	;;
	getf.sig	r20 = f42
	ldf8		f32 = [up], 8
	xma.l		f37 = f33, f6, f0
	;;
	getf.sig	r17 = f39
	xma.hu		f41 = f33, f6, f0
	;;
	getf.sig	r21 = f43
	ldf8		f33 = [up], 8
	xma.l		f38 = f34, f6, f0
	;;
	getf.sig	r18 = f36
	xma.hu		f42 = f34, f6, f0
	br		.LL10


.Lb11:	ldf8		f33 = [up], 8
	cmp.ne		p6, p7 = r0, r0
	mov		r22 = 0
	;;
	ldf8		f34 = [up], 8
	cmp.ne		p8, p9 = r0, r0
	;;
	ldf8		f35 = [up], 8
	br.cloop.dptk	.grt3

	xma.l		f37 = f33, f6, f0
	xma.hu		f41 = f33, f6, f0
	;;
	xma.l		f38 = f34, f6, f0
	xma.hu		f42 = f34, f6, f0
	;;
	getf.sig	r23 = f41
	stf8		[rp] = f37, 8
	xma.l		f39 = f35, f6, f0
	getf.sig	r16 = f38
	xma.hu		f43 = f35, f6, f0
	;;
	getf.sig	r20 = f42
	getf.sig	r17 = f39
	getf.sig	r21 = f43
	add		r24 = r16, r23
	br		.Lcj3

.grt3:	ldf8		f32 = [up], 8
	xma.l		f37 = f33, f6, f0
	xma.hu		f41 = f33, f6, f0
	;;
	ldf8		f33 = [up], 8
	xma.l		f38 = f34, f6, f0
	xma.hu		f42 = f34, f6, f0
	;;
	getf.sig	r19 = f37
	ldf8		f34 = [up], 8
	xma.l		f39 = f35, f6, f0
	xma.hu		f43 = f35, f6, f0
	;;
	getf.sig	r23 = f41
	ldf8		f35 = [up], 8
	br.cloop.dptk	.grt7

	getf.sig	r16 = f38
	xma.l		f36 = f32, f6, f0
	getf.sig	r20 = f42
	xma.hu		f40 = f32, f6, f0
	;;
	getf.sig	r17 = f39
	xma.l		f37 = f33, f6, f0
	getf.sig	r21 = f43
	xma.hu		f41 = f33, f6, f0
	;;
	getf.sig	r18 = f36
	st8		[rp] = r19, 8
	xma.l		f38 = f34, f6, f0
	xma.hu		f42 = f34, f6, f0
	br		.Lcj7

.grt7:	getf.sig	r16 = f38
	xma.l		f36 = f32, f6, f0
	xma.hu		f40 = f32, f6, f0
	;;
	getf.sig	r20 = f42
	ldf8		f32 = [up], 8
	xma.l		f37 = f33, f6, f0
	;;
	getf.sig	r17 = f39
	xma.hu		f41 = f33, f6, f0
	br		.LL11


.Lb00:	ldf8		f32 = [up], 8
	cmp.ne		p8, p9 = r0, r0
	mov		r21 = 0
	;;
	ldf8		f33 = [up], 8
	cmp.ne		p6, p7 = r0, r0
	;;
	ldf8		f34 = [up], 8
	;;
	ldf8		f35 = [up], 8
	br.cloop.dptk	.grt4

	xma.l		f36 = f32, f6, f0
	xma.hu		f40 = f32, f6, f0
	;;
	xma.l		f37 = f33, f6, f0
	xma.hu		f41 = f33, f6, f0
	;;
	xma.l		f38 = f34, f6, f0
	xma.hu		f42 = f34, f6, f0
	;;
	getf.sig	r22 = f40
	stf8		[rp] = f36, 8
	xma.l		f39 = f35, f6, f0
	getf.sig	r19 = f37
	xma.hu		f43 = f35, f6, f0
	;;
	getf.sig	r23 = f41
	getf.sig	r16 = f38
	getf.sig	r20 = f42
	getf.sig	r17 = f39
	br		.Lcj4

.grt4:	xma.l		f36 = f32, f6, f0
	xma.hu		f40 = f32, f6, f0
	;;
	ldf8		f32 = [up], 8
	xma.l		f37 = f33, f6, f0
	xma.hu		f41 = f33, f6, f0
	;;
	getf.sig	r18 = f36
	ldf8		f33 = [up], 8
	xma.l		f38 = f34, f6, f0
	xma.hu		f42 = f34, f6, f0
	;;
	getf.sig	r22 = f40
	ldf8		f34 = [up], 8
	xma.l		f39 = f35, f6, f0
	;;
	getf.sig	r19 = f37
	getf.sig	r23 = f41
	xma.hu		f43 = f35, f6, f0
	ldf8		f35 = [up], 8
	br.cloop.dptk	.grt8

	getf.sig	r16 = f38
	xma.l		f36 = f32, f6, f0
	getf.sig	r20 = f42
	xma.hu		f40 = f32, f6, f0
	;;
	getf.sig	r17 = f39
	st8		[rp] = r18, 8
	xma.l		f37 = f33, f6, f0
	xma.hu		f41 = f33, f6, f0
	br		.Lcj8

.grt8:	getf.sig	r16 = f38
	xma.l		f36 = f32, f6, f0
	xma.hu		f40 = f32, f6, f0
	br		.LL00


C *** MAIN LOOP START ***
	ALIGN(32)
.Loop:	.pred.rel "mutex",p6,p7
	getf.sig	r16 = f38
	xma.l		f36 = f32, f6, f0
   (p6)	cmp.leu		p8, p9 = r24, r17
	st8		[rp] = r24, 8
	xma.hu		f40 = f32, f6, f0
   (p7)	cmp.ltu		p8, p9 = r24, r17
	;;
.LL00:	.pred.rel "mutex",p8,p9
	getf.sig	r20 = f42
   (p8)	add		r24 = r18, r21, 1
	nop.b		0
	ldf8		f32 = [up], 8
   (p9)	add		r24 = r18, r21
	nop.b		0
	;;
	.pred.rel "mutex",p8,p9
	getf.sig	r17 = f39
	xma.l		f37 = f33, f6, f0
   (p8)	cmp.leu		p6, p7 = r24, r18
	st8		[rp] = r24, 8
	xma.hu		f41 = f33, f6, f0
   (p9)	cmp.ltu		p6, p7 = r24, r18
	;;
.LL11:	.pred.rel "mutex",p6,p7
	getf.sig	r21 = f43
   (p6)	add		r24 = r19, r22, 1
	nop.b		0
	ldf8		f33 = [up], 8
   (p7)	add		r24 = r19, r22
	nop.b		0
	;;
	.pred.rel "mutex",p6,p7
	getf.sig	r18 = f36
	xma.l		f38 = f34, f6, f0
   (p6)	cmp.leu		p8, p9 = r24, r19
	st8		[rp] = r24, 8
	xma.hu		f42 = f34, f6, f0
   (p7)	cmp.ltu		p8, p9 = r24, r19
	;;
.LL10:	.pred.rel "mutex",p8,p9
	getf.sig	r22 = f40
   (p8)	add		r24 = r16, r23, 1
	nop.b		0
	ldf8		f34 = [up], 8
   (p9)	add		r24 = r16, r23
	nop.b		0
	;;
	.pred.rel "mutex",p8,p9
	getf.sig	r19 = f37
	xma.l		f39 = f35, f6, f0
   (p8)	cmp.leu		p6, p7 = r24, r16
	st8		[rp] = r24, 8
	xma.hu		f43 = f35, f6, f0
   (p9)	cmp.ltu		p6, p7 = r24, r16
	;;
.LL01:	.pred.rel "mutex",p6,p7
	getf.sig	r23 = f41
   (p6)	add		r24 = r17, r20, 1
	nop.b		0
	ldf8		f35 = [up], 8
   (p7)	add		r24 = r17, r20
	br.cloop.dptk	.Loop
C *** MAIN LOOP END ***

.Lcj9:	.pred.rel "mutex",p6,p7
	getf.sig	r16 = f38
	xma.l		f36 = f32, f6, f0
   (p6)	cmp.leu		p8, p9 = r24, r17
	st8		[rp] = r24, 8
	xma.hu		f40 = f32, f6, f0
   (p7)	cmp.ltu		p8, p9 = r24, r17
	;;
	.pred.rel "mutex",p8,p9
	getf.sig	r20 = f42
   (p8)	add		r24 = r18, r21, 1
   (p9)	add		r24 = r18, r21
	;;
	.pred.rel "mutex",p8,p9
	getf.sig	r17 = f39
	xma.l		f37 = f33, f6, f0
   (p8)	cmp.leu		p6, p7 = r24, r18
	st8		[rp] = r24, 8
	xma.hu		f41 = f33, f6, f0
   (p9)	cmp.ltu		p6, p7 = r24, r18
	;;
.Lcj8:	.pred.rel "mutex",p6,p7
	getf.sig	r21 = f43
   (p6)	add		r24 = r19, r22, 1
   (p7)	add		r24 = r19, r22
	;;
	.pred.rel "mutex",p6,p7
	getf.sig	r18 = f36
	xma.l		f38 = f34, f6, f0
   (p6)	cmp.leu		p8, p9 = r24, r19
	st8		[rp] = r24, 8
	xma.hu		f42 = f34, f6, f0
   (p7)	cmp.ltu		p8, p9 = r24, r19
	;;
.Lcj7:	.pred.rel "mutex",p8,p9
	getf.sig	r22 = f40
   (p8)	add		r24 = r16, r23, 1
   (p9)	add		r24 = r16, r23
	;;
	.pred.rel "mutex",p8,p9
	getf.sig	r19 = f37
	xma.l		f39 = f35, f6, f0
   (p8)	cmp.leu		p6, p7 = r24, r16
	st8		[rp] = r24, 8
	xma.hu		f43 = f35, f6, f0
   (p9)	cmp.ltu		p6, p7 = r24, r16
	;;
.Lcj6:	.pred.rel "mutex",p6,p7
	getf.sig	r23 = f41
   (p6)	add		r24 = r17, r20, 1
   (p7)	add		r24 = r17, r20
	;;
	.pred.rel "mutex",p6,p7
	getf.sig	r16 = f38
   (p6)	cmp.leu		p8, p9 = r24, r17
	st8		[rp] = r24, 8
   (p7)	cmp.ltu		p8, p9 = r24, r17
	;;
.Lcj5:	.pred.rel "mutex",p8,p9
	getf.sig	r20 = f42
   (p8)	add		r24 = r18, r21, 1
   (p9)	add		r24 = r18, r21
	;;
	.pred.rel "mutex",p8,p9
	getf.sig	r17 = f39
   (p8)	cmp.leu		p6, p7 = r24, r18
	st8		[rp] = r24, 8
   (p9)	cmp.ltu		p6, p7 = r24, r18
	;;
.Lcj4:	.pred.rel "mutex",p6,p7
	getf.sig	r21 = f43
   (p6)	add		r24 = r19, r22, 1
   (p7)	add		r24 = r19, r22
	;;
	.pred.rel "mutex",p6,p7
   (p6)	cmp.leu		p8, p9 = r24, r19
	st8		[rp] = r24, 8
   (p7)	cmp.ltu		p8, p9 = r24, r19
	;;
	.pred.rel "mutex",p8,p9
   (p8)	add		r24 = r16, r23, 1
   (p9)	add		r24 = r16, r23
	;;
.Lcj3:	.pred.rel "mutex",p8,p9
   (p8)	cmp.leu		p6, p7 = r24, r16
	st8		[rp] = r24, 8
   (p9)	cmp.ltu		p6, p7 = r24, r16
	;;
	.pred.rel "mutex",p6,p7
   (p6)	add		r24 = r17, r20, 1
   (p7)	add		r24 = r17, r20
	;;
.Lcj2:	.pred.rel "mutex",p6,p7
   (p6)	cmp.leu		p8, p9 = r24, r17
	st8		[rp] = r24, 8
   (p7)	cmp.ltu		p8, p9 = r24, r17
	;;
	.pred.rel "mutex",p8,p9
   (p8)	add		r8 = r0, r21, 1
   (p9)	add		r8 = r0, r21
	mov		ar.lc = r2
	br.ret.sptk.many b0
EPILOGUE()
ASM_END()
