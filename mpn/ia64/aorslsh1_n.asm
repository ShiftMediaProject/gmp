dnl  IA-64 mpn_addlsh1_n/mpn_sublsh1_n -- rp[] = up[] +- (vp[] << 1).

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

C         cycles/limb
C Itanium:    2.5?
C Itanium 2:  1.5

C This code has been optimized for Itanium 2 at the expense of Itanium 1, since
C the latter chip is very rare out there.

C TODO
C  * Micro-optimize feed-in and wind-down code.

C INPUT PARAMETERS
define(`rp',`r32')
define(`up',`r33')
define(`vp',`r34')
define(`n',`r35')

ifdef(`OPERATION_addlsh1_n',`
  define(ADDSUB,       add)
  define(PRED,	       ltu)
  define(INCR,	       1)
  define(LIM,	       -1)
  define(func, mpn_addlsh1_n)
')
ifdef(`OPERATION_sublsh1_n',`
  define(ADDSUB,       sub)
  define(PRED,	       gtu)
  define(INCR,	       -1)
  define(LIM,	       0)
  define(func, mpn_sublsh1_n)
')

C Some useful aliases for registers we use
define(`u0',`r14') define(`u1',`r15') define(`u2',`r16') define(`u3',`r17')
define(`v0',`r18') define(`v1',`r19') define(`v2',`r20') define(`v3',`r21')
define(`w0',`r22') define(`w1',`r23') define(`w2',`r24') define(`w3',`r25')
define(`s0',`r26') define(`s1',`r27') define(`s2',`r28') define(`s3',`r29')
define(`x0',`r8') define(`x1',`r9') define(`x2',`r30') define(`x3',`r31')

MULFUNC_PROLOGUE(mpn_addlsh1_n mpn_addlsh1_n)

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
	and		r14 = 3, n
	adds		n = -1, n
	mov.i		r2 = ar.lc
	;;
	shr		r15 = n, 2
	cmp.eq		p6, p0 = 0, r14
	cmp.eq		p7, p0 = 1, r14
	cmp.eq		p8, p0 = 3, r14
	;;
	mov.i		ar.lc = r15
   (p6)	br.dptk		.Lb00
   (p7)	br.dptk		.Lb01
   (p8)	br.dptk		.Lb11

.Lb10:  C  n = 2, 6, 10, ...
	ld8		v1 = [vp], 8
	ld8		u1 = [up], 8
	;;
	ld8		v2 = [vp], 8
	ld8		u2 = [up], 8
	br.cloop.dptk	.Lb10_grt2
					C n = 2
	shrp		x1 = v1, r0, 63
	;;
	ADDSUB		w1 = u1, x1
	shrp		x2 = v2, v1, 63
	;;
	cmp.PRED	p8, p9 = w1, u1
	shrp		x3 = r0, v2, 63
	ADDSUB		w2 = u2, x2
	;;
	cmp.PRED	p6, p7 = w2, u2
	br		.Lcj2

.Lb10_grt2:				C n = 6, 10, 14, ...
	ld8		v3 = [vp], 8
	ld8		u3 = [up], 8
	shrp		x1 = v1, r0, 63
	;;
	ld8		v0 = [vp], 8
	ld8		u0 = [up], 8
	ADDSUB		w1 = u1, x1
	;;
	shrp		x2 = v2, v1, 63
	ld8		v1 = [vp], 8
	cmp.PRED	p8, p9 = w1, u1
	;;
	ld8		u1 = [up], 8
	shrp		x3 = v3, v2, 63
	ld8		v2 = [vp], 8
	ADDSUB		w2 = u2, x2
	;;
	cmp.PRED	p6, p7 = w2, u2
	ld8		u2 = [up], 8
	ADDSUB		w3 = u3, x3
	br.cloop.dptk	.Loop		C n = 10, 14, 18, ...
	br		.Lskip		C n = 6

.Lb11:					C  n = 3, 7, 11, 15, ...
	ld8		v0 = [vp], 8
	ld8		u0 = [up], 8
	;;
	ld8		v1 = [vp], 8
	ld8		u1 = [up], 8
	;;
	ld8		v2 = [vp], 8
	ld8		u2 = [up], 8
	br.cloop.dptk	.Lb11_grt3
					C n = 3
	shrp		x0 = v0, r0, 63
	;;
	shrp		x1 = v1, v0, 63
	ADDSUB		w0 = u0, x0
	;;
	cmp.PRED	p6, p7 = w0, u0
	ADDSUB		w1 = u1, x1
	;;
	shrp		x2 = v2, v1, 63
	cmp.PRED	p8, p9 = w1, u1
	br		.Lcj3

.Lb11_grt3:				C n = 7, 11, 15, ...
	shrp		x0 = v0, r0, 63
	ld8		v3 = [vp], 8
	;;
	ld8		u3 = [up], 8
	shrp		x1 = v1, v0, 63
	ld8		v0 = [vp], 8
	ADDSUB		w0 = u0, x0
	;;
	cmp.PRED	p6, p7 = w0, u0
	ld8		u0 = [up], 8
	ADDSUB		w1 = u1, x1
	;;
	shrp		x2 = v2, v1, 63
	ld8		v1 = [vp], 8
	cmp.PRED	p8, p9 = w1, u1
	br		.LL11

.Lb00:					C  n = 4, 8, 12, ...
	ld8		v3 = [vp], 8
	ld8		u3 = [up], 8
	;;
	ld8		v0 = [vp], 8
	ld8		u0 = [up], 8
	;;
	ld8		v1 = [vp], 8
	ld8		u1 = [up], 8
	shrp		x3 = v3, r0, 63
	;;
	ld8		v2 = [vp], 8
	ld8		u2 = [up], 8
	ADDSUB		w3 = u3, x3
	br.cloop.dptk	.Lb00_grt4
					C n = 4
	shrp		x0 = v0, v3, 63
	cmp.PRED	p8, p9 = w3, u3
	;;
	shrp		x1 = v1, v0, 63
	ADDSUB		w0 = u0, x0
	;;
	cmp.PRED	p6, p7 = w0, u0
	ADDSUB		w1 = u1, x1
	br		.Lcj4

.Lb00_grt4:				C n = 8, 12, 16, ...
	shrp		x0 = v0, v3, 63
	ld8		v3 = [vp], 8
	cmp.PRED	p8, p9 = w3, u3
	;;
	ld8		u3 = [up], 8
	shrp		x1 = v1, v0, 63
	ld8		v0 = [vp], 8
	ADDSUB		w0 = u0, x0
	;;
	cmp.PRED	p6, p7 = w0, u0
	ld8		u0 = [up], 8
	ADDSUB		w1 = u1, x1
	br		.LL00

.Lb01:					C  n = 1, 5, 9, ...
	ld8		v2 = [vp], 8
	ld8		u2 = [up], 8
	br.cloop.dptk	.Lb01_grt1
					C n = 1
	shrp		x2 = v2, r0, 63
	;;
	ADDSUB		w2 = u2, x2
	shrp		x3 = r0, v2, 63
	;;
	cmp.PRED	p6, p7 = w2, u2
	br		.Lcj1

.Lb01_grt1:				C n = 5, 9, 13, ...
	ld8		v3 = [vp], 8
	ld8		u3 = [up], 8
	;;
	ld8		v0 = [vp], 8
	ld8		u0 = [up], 8
	shrp		x2 = v2, r0, 63
	;;
	ld8		v1 = [vp], 8
	ld8		u1 = [up], 8
	shrp		x3 = v3, v2, 63
	;;
	ld8		v2 = [vp], 8
	ADDSUB		w2 = u2, x2
	;;
	cmp.PRED	p6, p7 = w2, u2
	ld8		u2 = [up], 8
	ADDSUB		w3 = u3, x3
	br.cloop.dptk	.Lb01_grt5
					C n = 5
	shrp		x0 = v0, v3, 63
	cmp.PRED	p8, p9 = w3, u3
	br		.Lcj5

.Lb01_grt5:				C n = 9, 13, 17, ...
	shrp		x0 = v0, v3, 63
	ld8		v3 = [vp], 8
	cmp.PRED	p8, p9 = w3, u3
	br		.LL01

C *** MAIN LOOP START ***
	ALIGN(32)
.Loop:	st8		[rp] = w1, 8
	shrp		x0 = v0, v3, 63
   (p8)	cmp.eq.or	p6, p7 = LIM, w2
   (p8)	add		w2 = INCR, w2
	ld8		v3 = [vp], 8
	cmp.PRED	p8, p9 = w3, u3
	;;
.LL01:	ld8		u3 = [up], 8
	shrp		x1 = v1, v0, 63
   (p6)	cmp.eq.or	p8, p9 = LIM, w3
   (p6)	add		w3 = INCR, w3
	ld8		v0 = [vp], 8
	ADDSUB		w0 = u0, x0
	;;
	st8		[rp] = w2, 8
	cmp.PRED	p6, p7 = w0, u0
	ld8		u0 = [up], 8
	ADDSUB		w1 = u1, x1
	;;
.LL00:	st8		[rp] = w3, 8
	shrp		x2 = v2, v1, 63
   (p8)	cmp.eq.or	p6, p7 = LIM, w0
   (p8)	add		w0 = INCR, w0
	ld8		v1 = [vp], 8
	cmp.PRED	p8, p9 = w1, u1
	;;
.LL11:	ld8		u1 = [up], 8
	shrp		x3 = v3, v2, 63
   (p6)	cmp.eq.or	p8, p9 = LIM, w1
   (p6)	add		w1 = INCR, w1
	ld8		v2 = [vp], 8
	ADDSUB		w2 = u2, x2
	;;
	st8		[rp] = w0, 8
	cmp.PRED	p6, p7 = w2, u2
	ld8		u2 = [up], 8
	ADDSUB		w3 = u3, x3
	br.cloop.dptk	.Loop
C *** MAIN LOOP END ***

.Lskip:	st8		[rp] = w1, 8
	shrp		x0 = v0, v3, 63
   (p8)	cmp.eq.or	p6, p7 = LIM, w2
   (p8)	add		w2 = INCR, w2
	cmp.PRED	p8, p9 = w3, u3
	;;
.Lcj5:	shrp		x1 = v1, v0, 63
   (p6)	cmp.eq.or	p8, p9 = LIM, w3
   (p6)	add		w3 = INCR, w3
	ADDSUB		w0 = u0, x0
	;;
	st8		[rp] = w2, 8
	cmp.PRED	p6, p7 = w0, u0
	ADDSUB		w1 = u1, x1
	;;
.Lcj4:	st8		[rp] = w3, 8
	shrp		x2 = v2, v1, 63
   (p8)	cmp.eq.or	p6, p7 = LIM, w0
   (p8)	add		w0 = INCR, w0
	cmp.PRED	p8, p9 = w1, u1
	;;
.Lcj3:	shrp		x3 = r0, v2, 63
   (p6)	cmp.eq.or	p8, p9 = LIM, w1
   (p6)	add		w1 = INCR, w1
	ADDSUB		w2 = u2, x2
	;;
	st8		[rp] = w0, 8
	cmp.PRED	p6, p7 = w2, u2
	;;
.Lcj2:	st8		[rp] = w1, 8
   (p8)	cmp.eq.or	p6, p7 = LIM, w2
   (p8)	add		w2 = INCR, w2
	;;
.Lcj1:	st8		[rp] = w2, 8
   (p6)	add		x3 = 1, x3
	;;
	mov		r8 = x3
	mov.i		ar.lc = r2
	br.ret.sptk.many `rp'
EPILOGUE()
ASM_END()
