dnl  IA-64 mpn_add_n/mpn_sub_n -- mpn addition and subtraction.

dnl  Copyright (C) 2003 Free Software Foundation, Inc.

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
C Itanium:    2?
C Itanium 2:  1.25

C This code has been optimized for Itanium 2 at the expense of Itanium 1, since
C the latter chip is very rare out there.

C TODO
C  * Time the 7 parallel br, compare to indirect jump.
C  * Consider using special code for small n, using something like
C    "case (16 * (n >= 16) + (n mod 6))" to enter it and feed-in code.  Not
C    all 32 outcomes need special code

C INPUT PARAMETERS
define(`rp',`r32')
define(`up',`r33')
define(`vp',`r34')
define(`n',`r35')

ifdef(`OPERATION_add_n',`
  define(ADDSUB,       add)
  define(PRED,	       ltu)
  define(INCR,	       1)
  define(LIM,	       -1)
  define(func, mpn_add_n)
')
ifdef(`OPERATION_sub_n',`
  define(ADDSUB,       sub)
  define(PRED,	       gtu)
  define(INCR,	       -1)
  define(LIM,	       0)
  define(func, mpn_sub_n)
')

C Some useful aliases for registers we use
define(`u0',`r14') define(`u1',`r15') define(`u2',`r16') define(`u3',`r17')
define(`u4',`r18') define(`u5',`r19') define(`u6',`r20') define(`u7',`r21')
define(`v0',`r24') define(`v1',`r25') define(`v2',`r26') define(`v3',`r27')
define(`v4',`r28') define(`v5',`r29') define(`v6',`r30') define(`v7',`r31')
define(`w0',`r22') define(`w1',`r9') define(`w2',`r8') define(`w3',`r23')
define(`w4',`r22') define(`w5',`r9') define(`w6',`r8') define(`w7',`r23')
define(`rpx',`r3')

ASM_START()
PROLOGUE(func)
	.prologue
	.save	ar.lc, r2
	.body
ifdef(`HAVE_ABI_32',`
	addp4		rp = 0, rp
	addp4		up = 0, up
	addp4		vp = 0, vp
')
	and		r14 = 7, n
	adds		n = -1, n
	mov.i		r2 = ar.lc
	;;
	cmp.eq		p6, p0 = 0, r14
	shr		r15 = n, 3
	cmp.eq		p7, p0 = 2, r14
	cmp.eq		p8, p0 = 3, r14
	cmp.eq		p9, p0 = 4, r14
	cmp.eq		p10, p0 = 5, r14
	;;
	cmp.eq		p11, p0 = 6, r14
	cmp.eq		p12, p0 = 7, r14
	mov.i		ar.lc = r15
   (p6)	br.dptk		.Lb000
   (p7)	br.dptk		.Lb010
   (p8)	br.dptk		.Lb011
	;;
   (p9)	br.dptk		.Lb100
  (p10)	br.dptk		.Lb101
  (p11)	br.dptk		.Lb110
  (p12)	br.dptk		.Lb111

.Lb001:	adds		rpx = 16, rp
	ld8		v0 = [vp], 8
	ld8		u0 = [up], 8
	br.cloop.dptk	.Lb001_grt1
	;;
	ADDSUB		w0 = u0, v0
	;;
	cmp.PRED	p6, p0 = w0, u0
	mov		r8 = 0
	br		.Lw1

.Lb001_grt1:
	ld8		v1 = [vp], 8
	ld8		u1 = [up], 8
	;;
	ld8		v2 = [vp], 8
	ld8		u2 = [up], 8
	;;
	ld8		v3 = [vp], 8
	ld8		u3 = [up], 8
	;;
	ld8		v4 = [vp], 8
	ld8		u4 = [up], 8
	;;
	ld8		v5 = [vp], 8
	ld8		u5 = [up], 8
	;;
	ld8		v6 = [vp], 8
	ld8		u6 = [up], 8
	;;
	ld8		v7 = [vp], 8
	ld8		u7 = [up], 8
	ADDSUB		w0 = u0, v0
	;;
	ld8		v0 = [vp], 8
	cmp.PRED	p6, p0 = w0, u0
	ADDSUB		w1 = u1, v1
	ld8		u0 = [up], 8
	cmp.ne		p9, p0 = r0, r0
	br.cloop.dptk	.Loop
	br		.Lskip

.Lb010:	adds		rpx = 24, rp
	ld8		v7 = [vp], 8
	ld8		u7 = [up], 8
	;;
	ld8		v0 = [vp], 8
	ld8		u0 = [up], 8
	br.cloop.dptk	.Lb010_grt2
	ADDSUB		w7 = u7, v7
	;;
	cmp.PRED	p9, p0 = w7, u7
	ADDSUB		w0 = u0, v0
	br		.Lw23

.Lb010_grt2:
	ld8		v1 = [vp], 8
	ld8		u1 = [up], 8
	;;
	ld8		v2 = [vp], 8
	ld8		u2 = [up], 8
	;;
	ld8		v3 = [vp], 8
	ld8		u3 = [up], 8
	;;
	ld8		v4 = [vp], 8
	ld8		u4 = [up], 8
	;;
	ld8		v5 = [vp], 8
	ld8		u5 = [up], 8
	;;
	ld8		v6 = [vp], 8
	ld8		u6 = [up], 8
	ADDSUB		w7 = u7, v7
	;;
	ld8		v7 = [vp], 8
	cmp.PRED	p9, p0 = w7, u7
	ld8		u7 = [up], 8
	;;
	ADDSUB		w0 = u0, v0
	br		.LL01x

.Lb011:	adds		rpx = 32, rp
	ld8		v6 = [vp], 8
	ld8		u6 = [up], 8
	;;
	ld8		v7 = [vp], 8
	ld8		u7 = [up], 8
	;;
	ld8		v0 = [vp], 8
	ld8		u0 = [up], 8
	br.cloop.dptk	.Lb011_grt3
	ADDSUB		w6 = u6, v6
	;;
	cmp.PRED	p8, p0 = w6, u6
	ADDSUB		w7 = u7, v7
	;;
	cmp.PRED	p9, p0 = w7, u7
	;;
	st8		[rp] = w6, 8
   (p8)	cmp.eq.or	p9, p0 = LIM, w7
   (p8)	add		w7 = INCR, w7
	ADDSUB		w0 = u0, v0
	br	.Lw23

.Lb011_grt3:
	ld8		v1 = [vp], 8
	ld8		u1 = [up], 8
	;;
	ld8		v2 = [vp], 8
	ld8		u2 = [up], 8
	;;
	ld8		v3 = [vp], 8
	ld8		u3 = [up], 8
	;;
	ld8		v4 = [vp], 8
	ld8		u4 = [up], 8
	;;
	ld8		v5 = [vp], 8
	ld8		u5 = [up], 8
	ADDSUB		w6 = u6, v6
	;;
	ld8		v6 = [vp], 8
	cmp.PRED	p8, p0 = w6, u6
	ld8		u6 = [up], 8
	ADDSUB		w7 = u7, v7
	;;
	ld8		v7 = [vp], 8
	cmp.PRED	p9, p0 = w7, u7
	ld8		u7 = [up], 8
	;;
	st8		[rp] = w6, 8
   (p8)	cmp.eq.or	p9, p0 = LIM, w7
   (p8)	add		w7 = INCR, w7
	ADDSUB		w0 = u0, v0
	br		.LL01x

.Lb100:	adds		rpx = 8, rp
	ld8		v5 = [vp], 8
	ld8		u5 = [up], 8
	;;
	ld8		v6 = [vp], 8
	ld8		u6 = [up], 8
	;;
	ld8		v7 = [vp], 8
	ld8		u7 = [up], 8
	;;
	ld8		v0 = [vp], 8
	ld8		u0 = [up], 8
	br.cloop.dptk	.Lb100_grt4
	ADDSUB		w5 = u5, v5
	;;
	cmp.PRED	p7, p0 = w5, u5
	ADDSUB		w6 = u6, v6
	;;
	cmp.PRED	p8, p0 = w6, u6
   (p6)	add		w5 = INCR, w5
	ADDSUB		w7 = u7, v7
	;;
	cmp.PRED	p9, p0 = w7, u7
   (p7)	cmp.eq.or	p8, p0 = LIM, w6
   (p7)	add		w6 = INCR, w6
	br		.Lw4

.Lb100_grt4:
	ld8		v1 = [vp], 8
	ld8		u1 = [up], 8
	;;
	ld8		v2 = [vp], 8
	ld8		u2 = [up], 8
	;;
	ld8		v3 = [vp], 8
	ld8		u3 = [up], 8
	;;
	ld8		v4 = [vp], 8
	ADDSUB		w5 = u5, v5
	ld8		u4 = [up], 8
	;;
	ld8		v5 = [vp], 8
	cmp.PRED	p7, p0 = w5, u5
	ld8		u5 = [up], 8
	ADDSUB		w6 = u6, v6
	;;
	ld8		v6 = [vp], 8
	cmp.PRED	p8, p0 = w6, u6
	ld8		u6 = [up], 8
   (p6)	add		w5 = INCR, w5
	ADDSUB		w7 = u7, v7
	;;
	ld8		v7 = [vp], 8
	cmp.PRED	p9, p0 = w7, u7
   (p7)	cmp.eq.or	p8, p0 = LIM, w6
	ld8		u7 = [up], 8
   (p7)	add		w6 = INCR, w6
	br		.LL100

.Lb101:	adds		rpx = 16, rp
	ld8		v4 = [vp], 8
	ld8		u4 = [up], 8
	;;
	ld8		v5 = [vp], 8
	ld8		u5 = [up], 8
	;;
	ld8		v6 = [vp], 8
	ld8		u6 = [up], 8
	;;
	ld8		v7 = [vp], 8
	ld8		u7 = [up], 8
	;;
	ld8		v0 = [vp], 8
	ld8		u0 = [up], 8
	br.cloop.dptk	.Lb101_grt5
	ADDSUB		w4 = u4, v4
	;;
	cmp.PRED	p6, p0 = w4, u4
	ADDSUB		w5 = u5, v5
	;;
	cmp.PRED	p7, p0 = w5, u5
	ADDSUB		w6 = u6, v6
	br		.Lw5

.Lb101_grt5:
	ld8		v1 = [vp], 8
	ld8		u1 = [up], 8
	;;
	ld8		v2 = [vp], 8
	ld8		u2 = [up], 8
	;;
	ld8		v3 = [vp], 8
	ld8		u3 = [up], 8
	ADDSUB		w4 = u4, v4
	;;
	ld8		v4 = [vp], 8
	cmp.PRED	p6, p0 = w4, u4
	ADDSUB		w5 = u5, v5
	ld8		u4 = [up], 8
	;;
	ld8		v5 = [vp], 8
	cmp.PRED	p7, p0 = w5, u5
	ld8		u5 = [up], 8
	ADDSUB		w6 = u6, v6
	br	.LL101

.Lb110:	adds		rpx = 24, rp
	ld8		v3 = [vp], 8
	ld8		u3 = [up], 8
	;;
	ld8		v4 = [vp], 8
	ld8		u4 = [up], 8
	;;
	ld8		v5 = [vp], 8
	ld8		u5 = [up], 8
	;;
	ld8		v6 = [vp], 8
	ld8		u6 = [up], 8
	;;
	ld8		v7 = [vp], 8
	ld8		u7 = [up], 8
	;;
	ld8		v0 = [vp], 8
	ld8		u0 = [up], 8
	br.cloop.dptk	.Lb110_grt6
	ADDSUB		w3 = u3, v3
	;;
	ld8		v3 = [vp], 8
	cmp.PRED	p9, p0 = w3, u3
	ld8		u3 = [up], 8
	;;
	ADDSUB		w4 = u4, v4
	br		.Lw67

.Lb110_grt6:
	ld8		v1 = [vp], 8
	ld8		u1 = [up], 8
	;;
	ld8		v2 = [vp], 8
	ld8		u2 = [up], 8
	ADDSUB		w3 = u3, v3
	;;
	ld8		v3 = [vp], 8
	cmp.PRED	p9, p0 = w3, u3
	ld8		u3 = [up], 8
	ADDSUB		w4 = u4, v4
	br		.LL11x

.Lb111:	adds		rpx = 32, rp
	ld8		v2 = [vp], 8
	ld8		u2 = [up], 8
	;;
	ld8		v3 = [vp], 8
	ld8		u3 = [up], 8
	;;
	ld8		v4 = [vp], 8
	ld8		u4 = [up], 8
	;;
	ld8		v5 = [vp], 8
	ld8		u5 = [up], 8
	;;
	ld8		v6 = [vp], 8
	ld8		u6 = [up], 8
	;;
	ld8		v7 = [vp], 8
	ld8		u7 = [up], 8
	;;
	ld8		v0 = [vp], 8
	ld8		u0 = [up], 8
	br.cloop.dptk	.Lb111_grt7
	ADDSUB		w2 = u2, v2
	;;
	cmp.PRED	p8, p0 = w2, u2
	ADDSUB		w3 = u3, v3
	;;
	cmp.PRED	p9, p0 = w3, u3
	;;
	st8		[rp] = w2, 8
   (p8)	cmp.eq.or	p9, p0 = LIM, w3
   (p8)	add		w3 = INCR, w3
	ADDSUB		w4 = u4, v4
	br		.Lw67

.Lb111_grt7:
	ld8		v1 = [vp], 8
	ld8		u1 = [up], 8
	ADDSUB		w2 = u2, v2
	;;
	ld8		v2 = [vp], 8
	cmp.PRED	p8, p0 = w2, u2
	ld8		u2 = [up], 8
	ADDSUB		w3 = u3, v3
	;;
	ld8		v3 = [vp], 8
	cmp.PRED	p9, p0 = w3, u3
	ld8		u3 = [up], 8
	;;
	st8		[rp] = w2, 8
   (p8)	cmp.eq.or	p9, p0 = LIM, w3
   (p8)	add		w3 = INCR, w3
	ADDSUB		w4 = u4, v4
	br		.LL11x

.Lb000:	adds		rpx = 8, rp
	ld8		v1 = [vp], 8
	ld8		u1 = [up], 8
	;;
	ld8		v2 = [vp], 8
	ld8		u2 = [up], 8
	;;
	ld8		v3 = [vp], 8
	ld8		u3 = [up], 8
	;;
	ld8		v4 = [vp], 8
	ld8		u4 = [up], 8
	;;
	ld8		v5 = [vp], 8
	ld8		u5 = [up], 8
	;;
	ld8		v6 = [vp], 8
	ld8		u6 = [up], 8
	;;
	ld8		v7 = [vp], 8
	ld8		u7 = [up], 8
	;;
	ld8		v0 = [vp], 8
	ADDSUB		w1 = u1, v1
	ld8		u0 = [up], 8
	br.cloop.dptk	.Lb000_grt8
	cmp.PRED	p7, p0 = w1, u1
	ADDSUB		w2 = u2, v2
	;;
	cmp.PRED	p8, p0 = w2, u2
	ADDSUB		w3 = u3, v3
	;;
	cmp.PRED	p9, p0 = w3, u3
   (p7)	cmp.eq.or	p8, p0 = LIM, w2
   (p7)	add		w2 = INCR, w2
	br		.Lw8

.Lb000_grt8:
	ld8		v1 = [vp], 8
	cmp.PRED	p7, p0 = w1, u1
	ld8		u1 = [up], 8
	ADDSUB		w2 = u2, v2
	;;
	ld8		v2 = [vp], 8
	cmp.PRED	p8, p0 = w2, u2
	ld8		u2 = [up], 8
	ADDSUB		w3 = u3, v3
	;;
	ld8		v3 = [vp], 8
	cmp.PRED	p9, p0 = w3, u3
   (p7)	cmp.eq.or	p8, p0 = LIM, w2
	ld8		u3 = [up], 8
   (p7)	add		w2 = INCR, w2
	br		.LL000

C *** MAIN LOOP START ***
	ALIGN(32)
.Loop:	ld8		v1 = [vp], 8
	cmp.PRED	p7, p0 = w1, u1
   (p9)	cmp.eq.or	p6, p0 = LIM, w0
	ld8		u1 = [up], 8
   (p9)	add		w0 = INCR, w0
	ADDSUB		w2 = u2, v2
	;;
	ld8		v2 = [vp], 8
	cmp.PRED	p8, p0 = w2, u2
   (p6)	cmp.eq.or	p7, p0 = LIM, w1
	ld8		u2 = [up], 8
   (p6)	add		w1 = INCR, w1
	ADDSUB		w3 = u3, v3
	;;
	st8		[rp] = w0, 8
	ld8		v3 = [vp], 8
	cmp.PRED	p9, p0 = w3, u3
   (p7)	cmp.eq.or	p8, p0 = LIM, w2
	ld8		u3 = [up], 8
   (p7)	add		w2 = INCR, w2
	;;
.LL000:	st8		[rp] = w1, 16
	st8		[rpx] = w2, 32
   (p8)	cmp.eq.or	p9, p0 = LIM, w3
   (p8)	add		w3 = INCR, w3
	ADDSUB		w4 = u4, v4
	;;
.LL11x:	st8		[rp] = w3, 8
	ld8		v4 = [vp], 8
	cmp.PRED	p6, p0 = w4, u4
	ADDSUB		w5 = u5, v5
	ld8		u4 = [up], 8
	;;
	ld8		v5 = [vp], 8
	cmp.PRED	p7, p0 = w5, u5
   (p9)	cmp.eq.or	p6, p0 = LIM, w4
	ld8		u5 = [up], 8
   (p9)	add		w4 = INCR, w4
	ADDSUB		w6 = u6, v6
	;;
.LL101:	ld8		v6 = [vp], 8
	cmp.PRED	p8, p0 = w6, u6
   (p6)	cmp.eq.or	p7, p0 = LIM, w5
	ld8		u6 = [up], 8
   (p6)	add		w5 = INCR, w5
	ADDSUB		w7 = u7, v7
	;;
	st8		[rp] = w4, 8
	ld8		v7 = [vp], 8
	cmp.PRED	p9, p0 = w7, u7
   (p7)	cmp.eq.or	p8, p0 = LIM, w6
	ld8		u7 = [up], 8
   (p7)	add		w6 = INCR, w6
	;;
.LL100:	st8		[rp] = w5, 16
	st8		[rpx] = w6, 32
   (p8)	cmp.eq.or	p9, p0 = LIM, w7
   (p8)	add		w7 = INCR, w7
	ADDSUB		w0 = u0, v0
	;;
.LL01x:	st8		[rp] = w7, 8
	ld8		v0 = [vp], 8
	cmp.PRED	p6, p0 = w0, u0
	ADDSUB		w1 = u1, v1
	ld8		u0 = [up], 8
	br.cloop.dptk	.Loop
	;;
C *** MAIN LOOP END ***

.Lskip:	cmp.PRED	p7, p0 = w1, u1
   (p9)	cmp.eq.or	p6, p0 = LIM, w0
   (p9)	add		w0 = INCR, w0
	ADDSUB		w2 = u2, v2
	;;
	cmp.PRED	p8, p0 = w2, u2
   (p6)	cmp.eq.or	p7, p0 = LIM, w1
   (p6)	add		w1 = INCR, w1
	ADDSUB		w3 = u3, v3
	;;
	st8		[rp] = w0, 8
	cmp.PRED	p9, p0 = w3, u3
   (p7)	cmp.eq.or	p8, p0 = LIM, w2
   (p7)	add		w2 = INCR, w2
	;;
.Lw8:	st8		[rp] = w1, 16
	st8		[rpx] = w2, 32
   (p8)	cmp.eq.or	p9, p0 = LIM, w3
   (p8)	add		w3 = INCR, w3
	ADDSUB		w4 = u4, v4
	;;
.Lw67:	st8		[rp] = w3, 8
	cmp.PRED	p6, p0 = w4, u4
	ADDSUB		w5 = u5, v5
	;;
	cmp.PRED	p7, p0 = w5, u5
   (p9)	cmp.eq.or	p6, p0 = LIM, w4
   (p9)	add		w4 = INCR, w4
	ADDSUB		w6 = u6, v6
	;;
.Lw5:	cmp.PRED	p8, p0 = w6, u6
   (p6)	cmp.eq.or	p7, p0 = LIM, w5
   (p6)	add		w5 = INCR, w5
	ADDSUB		w7 = u7, v7
	;;
	st8		[rp] = w4, 8
	cmp.PRED	p9, p0 = w7, u7
   (p7)	cmp.eq.or	p8, p0 = LIM, w6
   (p7)	add		w6 = INCR, w6
	;;
.Lw4:	st8		[rp] = w5, 16
	st8		[rpx] = w6, 32
   (p8)	cmp.eq.or	p9, p0 = LIM, w7
   (p8)	add		w7 = INCR, w7
	ADDSUB		w0 = u0, v0
	;;
.Lw23:	st8		[rp] = w7, 8
	cmp.PRED	p6, p0 = w0, u0
	;;
   (p9)	cmp.eq.or	p6, p0 = LIM, w0
   (p9)	add		w0 = INCR, w0
	mov		r8 = 0
	;;
.Lw1:	st8		[rp] = w0, 8
   (p6)	mov		r8 = 1
	mov.i		ar.lc = r2
	br.ret.sptk.many `rp'
EPILOGUE()
ASM_END()
