dnl  IA-64 mpn_rsh1add_n/mpn_rsh1sub_n -- rp[] = (up[] +- vp[]) >> 1.

dnl  Copyright 2003 Free Software Foundation, Inc.

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

C         cycles/limb	 1   2   3   4   5   6   7   8   9  10  11  12  13  14
C Itanium:    2.5?
C Itanium 2:  1.5	7.0 7.0 6.0 5.4 4.5 4.1 3.9 3.7 3.4 3.3 3.2 3.0 3.0 2.9

C This code has been optimized for Itanium 2 at the expense of Itanium 1, since
C the latter chip is very rare out there.

C TODO
C  * Micro-optimize feed-in and wind-down code.

C INPUT PARAMETERS
define(`rp',`r32')
define(`up',`r33')
define(`vp',`r34')
define(`n',`r35')

ifdef(`OPERATION_rsh1add_n',`
  define(ADDSUB,       add)
  define(PRED,	       ltu)
  define(INCR,	       1)
  define(LIM,	       -1)
  define(func, mpn_rsh1add_n)
')
ifdef(`OPERATION_rsh1sub_n',`
  define(ADDSUB,       sub)
  define(PRED,	       gtu)
  define(INCR,	       -1)
  define(LIM,	       0)
  define(func, mpn_rsh1sub_n)
')

C Some useful aliases for registers we use
define(`u0',`r14') define(`u1',`r15') define(`u2',`r16') define(`u3',`r17')
define(`v0',`r18') define(`v1',`r19') define(`v2',`r20') define(`v3',`r21')
define(`w0',`r22') define(`w1',`r23') define(`w2',`r24') define(`w3',`r25')
define(`x0',`r26') define(`x1',`r9') define(`x2',`r30') define(`x3',`r31')

MULFUNC_PROLOGUE(mpn_rsh1add_n mpn_rsh1sub_n)

ASM_START()
PROLOGUE(func)
	.prologue
	.save	ar.lc, r2
	.body
ifdef(`HAVE_ABI_32',`
	addp4		rp = 0, rp
	addp4		up = 0, up
	addp4		vp = 0, vp
	zxt4		n = n
	;;
')
	and		r14 = 3, n
	adds		n = -1, n
	mov.i		r2 = ar.lc
	;;
	shr.u		r15 = n, 2
	cmp.eq		p6, p0 = 1, r14
	cmp.eq		p7, p0 = 2, r14
	cmp.eq		p8, p0 = 3, r14
	;;
	mov.i		ar.lc = r15
   (p6)	br.dptk		.Lb01
   (p7)	br.dptk		.Lb10
   (p8)	br.dptk		.Lb11
	br.dptk		.Lb00		C FIXME, one should fall through
	;;


.Lb11:	ld8		v0 = [vp], 8
	ld8		u0 = [up], 8
	;;
	ld8		v1 = [vp], 8
	ld8		u1 = [up], 8
	;;
	ld8		v2 = [vp], 8
	ld8		u2 = [up], 8
	br.cloop.dptk	.Lb11_grt3

	ADDSUB		w0 = u0, v0
	;;
	cmp.PRED	p8, p0 = w0, u0
	ADDSUB		w1 = u1, v1
	and		r8 = 1, w0
	;;
	cmp.PRED	p9, p0 = w1, u1
	;;
	ADDSUB		w2 = u2, v2
   (p8)	cmp.eq.or	p9, p0 = LIM, w1
   (p8)	add		w1 = INCR, w1
	;;
	cmp.PRED	p6, p0 = w2, u2
	shrp		x0 = w1, w0, 1
	;;
   (p9)	cmp.eq.or	p6, p0 = LIM, w2
   (p9)	add		w2 = INCR, w2
	br		.Lcj3

.Lb11_grt3:
	ld8		v3 = [vp], 8
	ld8		u3 = [up], 8
	;;
	ADDSUB		w0 = u0, v0
	ld8		v0 = [vp], 8
	;;
	cmp.PRED	p8, p0 = w0, u0
	ld8		u0 = [up], 8
	ADDSUB		w1 = u1, v1
	and		r8 = 1, w0
	;;
	ld8		v1 = [vp], 8
	cmp.PRED	p9, p0 = w1, u1
	ld8		u1 = [up], 8
	;;
	ADDSUB		w2 = u2, v2
	ld8		v2 = [vp], 8
   (p8)	cmp.eq.or	p9, p0 = LIM, w1
   (p8)	add		w1 = INCR, w1
	;;
	cmp.PRED	p6, p0 = w2, u2
	shrp		x0 = w1, w0, 1
	ld8		u2 = [up], 8
	ADDSUB		w3 = u3, v3
	br.cloop.dptk	.Lb11_grt7

	cmp.PRED	p7, p0 = w3, u3
   (p9)	cmp.eq.or	p6, p0 = LIM, w2
   (p9)	add		w2 = INCR, w2
	br		.Lcj7

.Lb11_grt7:
	ld8		v3 = [vp], 8
	cmp.PRED	p7, p0 = w3, u3
	ld8		u3 = [up], 8
   (p9)	cmp.eq.or	p6, p0 = LIM, w2
   (p9)	add		w2 = INCR, w2
	br		.LL11


.Lb10:	ld8		v1 = [vp], 8
	ld8		u1 = [up], 8
	;;
	ld8		v2 = [vp], 8
	ld8		u2 = [up], 8
	br.cloop.dptk	.Lb10_grt2

	ADDSUB		w1 = u1, v1
	;;
	cmp.PRED	p9, p0 = w1, u1
	and		r8 = 1, w1
	ADDSUB		w2 = u2, v2
	;;
	cmp.PRED	p6, p0 = w2, u2
	;;
   (p9)	cmp.eq.or	p6, p0 = LIM, w2
   (p9)	add		w2 = INCR, w2
	;;
	shrp		x1 = w2, w1, 1
	shr.u		x2 = w2, 1
	br		.Lcj2

.Lb10_grt2:
	ld8		v3 = [vp], 8
	ld8		u3 = [up], 8
	;;
	ld8		v0 = [vp], 8
	ld8		u0 = [up], 8
	ADDSUB		w1 = u1, v1
	;;
	ld8		v1 = [vp], 8
	cmp.PRED	p9, p0 = w1, u1
	ld8		u1 = [up], 8
	and		r8 = 1, w1
	;;
	ADDSUB		w2 = u2, v2
	ld8		v2 = [vp], 8
	;;
	cmp.PRED	p6, p0 = w2, u2
	ld8		u2 = [up], 8
	ADDSUB		w3 = u3, v3
	br.cloop.dptk	.Lb10_grt6

	cmp.PRED	p7, p0 = w3, u3
   (p9)	cmp.eq.or	p6, p0 = LIM, w2
   (p9)	add		w2 = INCR, w2
	;;
	shrp		x1 = w2, w1, 1
	ADDSUB		w0 = u0, v0
   (p6)	cmp.eq.or	p7, p0 = LIM, w3
   (p6)	add		w3 = INCR, w3
	br		.Lcj6

.Lb10_grt6:
	ld8		v3 = [vp], 8
	cmp.PRED	p7, p0 = w3, u3
	ld8		u3 = [up], 8
   (p9)	cmp.eq.or	p6, p0 = LIM, w2
   (p9)	add		w2 = INCR, w2
	;;
	shrp		x1 = w2, w1, 1
	ADDSUB		w0 = u0, v0
	ld8		v0 = [vp], 8
   (p6)	cmp.eq.or	p7, p0 = LIM, w3
   (p6)	add		w3 = INCR, w3
	br		.LL10


.Lb01:	ld8		v2 = [vp], 8
	ld8		u2 = [up], 8
	br.cloop.dptk	.Lb01_grt1

	ADDSUB		w2 = u2, v2
	;;
	cmp.PRED	p6, p7 = w2, u2
	and		r8 = 1, w2
	shr.u		x2 = w2, 1
	;;
   (p6)	dep		x2 = -1, x2, 63, 1
	br		.Lcj1

.Lb01_grt1:
	ld8		v3 = [vp], 8
	ld8		u3 = [up], 8
	;;
	ld8		v0 = [vp], 8
	ld8		u0 = [up], 8
	;;
	ld8		v1 = [vp], 8
	ld8		u1 = [up], 8
	;;
	ADDSUB		w2 = u2, v2
	ld8		v2 = [vp], 8
	;;
	cmp.PRED	p6, p0 = w2, u2
	ld8		u2 = [up], 8
	and		r8 = 1, w2
	ADDSUB		w3 = u3, v3
	br.cloop.dptk	.Lb01_grt5

	cmp.PRED	p7, p0 = w3, u3
	;;
	ADDSUB		w0 = u0, v0
   (p6)	cmp.eq.or	p7, p0 = LIM, w3
   (p6)	add		w3 = INCR, w3
	;;
	cmp.PRED	p8, p0 = w0, u0
	shrp		x2 = w3, w2, 1
	ADDSUB		w1 = u1, v1
	;;
	cmp.PRED	p9, p0 = w1, u1
   (p7)	cmp.eq.or	p8, p0 = LIM, w0
   (p7)	add		w0 = INCR, w0
	br		.Lcj5

.Lb01_grt5:
	ld8		v3 = [vp], 8
	cmp.PRED	p7, p0 = w3, u3
	ld8		u3 = [up], 8
	;;
	ADDSUB		w0 = u0, v0
	ld8		v0 = [vp], 8
   (p6)	cmp.eq.or	p7, p0 = LIM, w3
   (p6)	add		w3 = INCR, w3
	;;
	cmp.PRED	p8, p0 = w0, u0
	shrp		x2 = w3, w2, 1
	ld8		u0 = [up], 8
	ADDSUB		w1 = u1, v1
	;;
	ld8		v1 = [vp], 8
	cmp.PRED	p9, p0 = w1, u1
	ld8		u1 = [up], 8
   (p7)	cmp.eq.or	p8, p0 = LIM, w0
   (p7)	add		w0 = INCR, w0
	br		.LL01


.Lb00:	ld8		v3 = [vp], 8
	ld8		u3 = [up], 8
	;;
	ld8		v0 = [vp], 8
	ld8		u0 = [up], 8
	;;
	ld8		v1 = [vp], 8
	ld8		u1 = [up], 8
	;;
	ld8		v2 = [vp], 8
	ld8		u2 = [up], 8
	ADDSUB		w3 = u3, v3
	br.cloop.dptk	.Lb00_grt4

	cmp.PRED	p7, p0 = w3, u3
	and		r8 = 1, w3
	;;
	ADDSUB		w0 = u0, v0
	;;
	cmp.PRED	p8, p0 = w0, u0
	ADDSUB		w1 = u1, v1
	;;
	cmp.PRED	p9, p0 = w1, u1
   (p7)	cmp.eq.or	p8, p0 = LIM, w0
   (p7)	add		w0 = INCR, w0
	;;
	shrp		x3 = w0, w3, 1
	ADDSUB		w2 = u2, v2
   (p8)	cmp.eq.or	p9, p0 = LIM, w1
   (p8)	add		w1 = INCR, w1
	br		.Lcj4

.Lb00_grt4:
	ld8		v3 = [vp], 8
	cmp.PRED	p7, p0 = w3, u3
	ld8		u3 = [up], 8
	and		r8 = 1, w3
	;;
	ADDSUB		w0 = u0, v0
	ld8		v0 = [vp], 8
	;;
	cmp.PRED	p8, p0 = w0, u0
	ld8		u0 = [up], 8
	ADDSUB		w1 = u1, v1
	;;
	ld8		v1 = [vp], 8
	cmp.PRED	p9, p0 = w1, u1
	ld8		u1 = [up], 8
   (p7)	cmp.eq.or	p8, p0 = LIM, w0
   (p7)	add		w0 = INCR, w0
	;;
	shrp		x3 = w0, w3, 1
	ADDSUB		w2 = u2, v2
	ld8		v2 = [vp], 8
   (p8)	cmp.eq.or	p9, p0 = LIM, w1
   (p8)	add		w1 = INCR, w1
	br		.LL00


C *** MAIN LOOP START ***
	ALIGN(32)
.Loop:	st8		[rp] = x3, 8
	ld8		v3 = [vp], 8
	cmp.PRED	p7, p0 = w3, u3
	ld8		u3 = [up], 8
   (p9)	cmp.eq.or	p6, p0 = LIM, w2
   (p9)	add		w2 = INCR, w2
	;;
.LL11:	st8		[rp] = x0, 8
	shrp		x1 = w2, w1, 1
	ADDSUB		w0 = u0, v0
	ld8		v0 = [vp], 8
   (p6)	cmp.eq.or	p7, p0 = LIM, w3
   (p6)	add		w3 = INCR, w3
	;;
.LL10:	cmp.PRED	p8, p0 = w0, u0
	shrp		x2 = w3, w2, 1
	nop.b		0
	ld8		u0 = [up], 8
	ADDSUB		w1 = u1, v1
	nop.b		0
	;;
	st8		[rp] = x1, 8
	ld8		v1 = [vp], 8
	cmp.PRED	p9, p0 = w1, u1
	ld8		u1 = [up], 8
   (p7)	cmp.eq.or	p8, p0 = LIM, w0
   (p7)	add		w0 = INCR, w0
	;;
.LL01:	st8		[rp] = x2, 8
	shrp		x3 = w0, w3, 1
	ADDSUB		w2 = u2, v2
	ld8		v2 = [vp], 8
   (p8)	cmp.eq.or	p9, p0 = LIM, w1
   (p8)	add		w1 = INCR, w1
	;;
.LL00:	cmp.PRED	p6, p0 = w2, u2
	shrp		x0 = w1, w0, 1
	nop.b		0
	ld8		u2 = [up], 8
	ADDSUB		w3 = u3, v3
	br.cloop.dptk	.Loop
C *** MAIN LOOP END ***

.Lskip:	st8		[rp] = x3, 8
	cmp.PRED	p7, p0 = w3, u3
   (p9)	cmp.eq.or	p6, p0 = LIM, w2
   (p9)	add		w2 = INCR, w2
	;;
.Lcj7:	st8		[rp] = x0, 8
	shrp		x1 = w2, w1, 1
	ADDSUB		w0 = u0, v0
   (p6)	cmp.eq.or	p7, p0 = LIM, w3
   (p6)	add		w3 = INCR, w3
	;;
.Lcj6:	cmp.PRED	p8, p0 = w0, u0
	shrp		x2 = w3, w2, 1
	ADDSUB		w1 = u1, v1
	;;
	st8		[rp] = x1, 8
	cmp.PRED	p9, p0 = w1, u1
   (p7)	cmp.eq.or	p8, p0 = LIM, w0
   (p7)	add		w0 = INCR, w0
	;;
.Lcj5:	st8		[rp] = x2, 8
	shrp		x3 = w0, w3, 1
	ADDSUB		w2 = u2, v2
   (p8)	cmp.eq.or	p9, p0 = LIM, w1
   (p8)	add		w1 = INCR, w1
	;;
.Lcj4:	cmp.PRED	p6, p0 = w2, u2
	shrp		x0 = w1, w0, 1
	;;
	st8		[rp] = x3, 8
   (p9)	cmp.eq.or	p6, p0 = LIM, w2
   (p9)	add		w2 = INCR, w2
	;;
.Lcj3:	st8		[rp] = x0, 8
	shrp		x1 = w2, w1, 1
	shr.u		x2 = w2, 1
	;;
.Lcj2:	st8		[rp] = x1, 8
   (p6)	dep		x2 = -1, x2, 63, 1
	;;
.Lcj1:	st8		[rp] = x2
	mov.i		ar.lc = r2
	br.ret.sptk.many `rp'
EPILOGUE()
