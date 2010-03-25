dnl  IA-64 mpn_add_n_sub_n -- mpn parallel addition and subtraction.

dnl  Copyright 2010 Free Software Foundation, Inc.

dnl  This file is part of the GNU MP Library.

dnl  The GNU MP Library is free software; you can redistribute it and/or modify
dnl  it under the terms of the GNU Lesser General Public License as published
dnl  by the Free Software Foundation; either version 3 of the License, or (at
dnl  your option) any later version.

dnl  The GNU MP Library is distributed in the hope that it will be useful, but
dnl  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
dnl  License for more details.

dnl  You should have received a copy of the GNU Lesser General Public License
dnl  along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.

include(`../config.m4')

C           cycles/limb
C Itanium:      ?
C Itanium 2:    2.25

C INPUT PARAMETERS
define(`sp', `r32')
define(`dp', `r33')
define(`up', `r34')
define(`vp', `r35')
define(`n',  `r36')

C Some useful aliases for registers we use
define(`u0',`r16') define(`u1',`r17') define(`u2',`r18') define(`u3',`r19')
define(`v0',`r20') define(`v1',`r21') define(`v2',`r22') define(`v3',`r23')
define(`s0',`r24') define(`s1',`r25') define(`s2',`r26') define(`s3',`r27')
define(`d0',`r28') define(`d1',`r29') define(`d2',`r30') define(`d3',`r31')
define(`up0',`up')
define(`up1',`r14')
define(`vp0',`vp')
define(`vp1',`r15')

define(`cmpltu',  `cmp.ltu')
define(`cmpeqor', `cmp.eq.or')

ASM_START()
PROLOGUE(mpn_add_n_sub_n)
	.prologue
	.save	ar.lc, r2
	.body
ifdef(`HAVE_ABI_32',`
	addp4	rp = 0, rp		C			M I
	addp4	up = 0, up		C			M I
	addp4	vp = 0, vp		C			M I
	zxt4	n = n			C			I
	;;
')

	mov.i	r2 = ar.lc		C			I0
	and	r9 = 3, n
	add	up1 = 8, up0
	add	vp1 = 8, vp0
	add	r8 = -2, n
	add	r10 = 256, up
	;;

	shr.u	r8 = r8, 2		C			I0
	cmp.eq	p10, p0 = 0, r9		C M I
	cmp.eq	p11, p0 = 2, r9		C M I
	cmp.eq	p12, p0 = 3, r9		C M I
	add	r11 = 256, vp
	;;
	mov.i	ar.lc = r8		C			I0
  (p10)	br	L(b0)
  (p11)	br	L(b2)
  (p12)	br	L(b3)

L(b1):	ld8	u3 = [up0], 8
	add	up1 = 8, up1
	cmpltu	p14, p15 = 4, n
	ld8	v3 = [vp0], 8
	add	vp1 = 8, vp1
	;;
	add	s3 = u3, v3
	sub	d3 = u3, v3
	mov	r8 = 0
	;;
	cmpltu	p9, p0 = s3, v3		C  carry from add3
	cmpltu	p13, p0 = u3, v3	C borrow from sub3
  (p15)	br	L(cj1)
	st8	[sp] = s3, 8
	st8	[dp] = d3, 8
	br	L(c0)

L(b0):	cmp.ne	p9, p0 = r0, r0
	cmp.ne	p13, p0 = r0, r0
L(c0):	ld8	u0 = [up0], 16
	ld8	u1 = [up1], 16
	;;
	ld8	v0 = [vp0], 16
	ld8	v1 = [vp1], 16
	;;
	ld8	u2 = [up0], 16
	ld8	u3 = [up1], 16
	;;
	ld8	v2 = [vp0], 16
	ld8	v3 = [vp1], 16
	;;
	add	s0 = u0, v0
	add	s1 = u1, v1
	sub	d0 = u0, v0
	sub	d1 = u1, v1
	;;
	cmpltu	p6, p0 = s0, v0		C  carry from add0
	cmpltu	p7, p0 = s1, v1		C  carry from add1
	cmpltu	p10, p0 = u0, v0	C borrow from sub0
	cmpltu	p11, p0 = u1, v1	C borrow from sub1
	;;
	nop	0
	br.cloop.dptk	L(top)
	br	L(end)

L(b3):	ld8	u1 = [up0], 8
	add	up1 = 8, up1
	ld8	v1 = [vp0], 8
	;;
	add	vp1 = 8, vp1
	add	s1 = u1, v1
	sub	d1 = u1, v1
	;;
	cmpltu	p7, p0 = s1, v1		C  carry from add1
	cmpltu	p11, p0 = u1, v1	C borrow from sub1
	;;
	st8	[sp] = s1, 8
	st8	[dp] = d1, 8
	br	L(c2)

	ALIGN(32)
L(b2):	cmp.ne	p7, p0 = r0, r0
	cmp.ne	p11, p0 = r0, r0
	nop	0
L(c2):	ld8	u2 = [up0], 16
	ld8	u3 = [up1], 16
	cmpltu	p14, p0 = 4, n
	;;
	ld8	v2 = [vp0], 16
	ld8	v3 = [vp1], 16
  (p14)	br	L(gt4)
	;;
	add	s2 = u2, v2
	add	s3 = u3, v3
	sub	d2 = u2, v2
	sub	d3 = u3, v3
	;;
	cmpltu	p8, p0 = s2, v2		C  carry from add0
	cmpltu	p9, p0 = s3, v3		C  carry from add3
	cmpltu	p12, p0 = u2, v2	C borrow from sub2
	cmpltu	p13, p0 = u3, v3	C borrow from sub3
	br	L(cj2)
	;;
L(gt4):	ld8	u0 = [up0], 16
	ld8	u1 = [up1], 16
	;;
	ld8	v0 = [vp0], 16
	ld8	v1 = [vp1], 16
	;;
	add	s2 = u2, v2
	add	s3 = u3, v3
	sub	d2 = u2, v2
	sub	d3 = u3, v3
	;;
	cmpltu	p8, p0 = s2, v2		C  carry from add0
	cmpltu	p9, p0 = s3, v3		C  carry from add1
	cmpltu	p12, p0 = u2, v2	C borrow from sub0
	cmpltu	p13, p0 = u3, v3	C borrow from sub1
	br.cloop.dptk	L(mid)

	ALIGN(32)
L(top):
	ld8	u0 = [up0], 16
	ld8	u1 = [up1], 16
   (p9)	cmpeqor	p6, p0 = -1, s0
   (p9)	add	s0 = 1, s0
  (p13)	cmpeqor	p10, p0 = 0, d0
  (p13)	add	d0 = -1, d0
	;;
	ld8	v0 = [vp0], 16
	ld8	v1 = [vp1], 16
   (p6)	cmpeqor	p7, p0 = -1, s1
   (p6)	add	s1 = 1, s1
  (p10)	cmpeqor	p11, p0 = 0, d1
  (p10)	add	d1 = -1, d1
	;;
	st8	[sp] = s0, 8
	st8	[dp] = d0, 8
	add	s2 = u2, v2
	add	s3 = u3, v3
	sub	d2 = u2, v2
	sub	d3 = u3, v3
	;;
	st8	[sp] = s1, 8
	st8	[dp] = d1, 8
	cmpltu	p8, p0 = s2, v2		C  carry from add2
	cmpltu	p9, p0 = s3, v3		C  carry from add3
	cmpltu	p12, p0 = u2, v2	C borrow from sub2
	cmpltu	p13, p0 = u3, v3	C borrow from sub3
	;;
L(mid):
	ld8	u2 = [up0], 16
	ld8	u3 = [up1], 16
   (p7)	cmpeqor	p8, p0 = -1, s2
   (p7)	add	s2 = 1, s2
  (p11)	cmpeqor	p12, p0 = 0, d2
  (p11)	add	d2 = -1, d2
	;;
	ld8	v2 = [vp0], 16
	ld8	v3 = [vp1], 16
   (p8)	cmpeqor	p9, p0 = -1, s3
   (p8)	add	s3 = 1, s3
  (p12)	cmpeqor	p13, p0 = 0, d3
  (p12)	add	d3 = -1, d3
	;;
	st8	[sp] = s2, 8
	st8	[dp] = d2, 8
	add	s0 = u0, v0
	add	s1 = u1, v1
	sub	d0 = u0, v0
	sub	d1 = u1, v1
	;;
	st8	[sp] = s3, 8
	st8	[dp] = d3, 8
	cmpltu	p6, p0 = s0, v0		C  carry from add0
	cmpltu	p7, p0 = s1, v1		C  carry from add1
	cmpltu	p10, p0 = u0, v0	C borrow from sub0
	cmpltu	p11, p0 = u1, v1	C borrow from sub1
	;;
	lfetch	[r10], 32
	lfetch	[r11], 32
	br.cloop.dptk	L(top)
	;;

L(end):
	nop	0
	nop	0
   (p9)	cmpeqor	p6, p0 = -1, s0
   (p9)	add	s0 = 1, s0
  (p13)	cmpeqor	p10, p0 = 0, d0
  (p13)	add	d0 = -1, d0
	;;
	nop	0
	nop	0
   (p6)	cmpeqor	p7, p0 = -1, s1
   (p6)	add	s1 = 1, s1
  (p10)	cmpeqor	p11, p0 = 0, d1
  (p10)	add	d1 = -1, d1
	;;
	st8	[sp] = s0, 8
	st8	[dp] = d0, 8
	add	s2 = u2, v2
	add	s3 = u3, v3
	sub	d2 = u2, v2
	sub	d3 = u3, v3
	;;
	st8	[sp] = s1, 8
	st8	[dp] = d1, 8
	cmpltu	p8, p0 = s2, v2		C  carry from add2
	cmpltu	p9, p0 = s3, v3		C  carry from add3
	cmpltu	p12, p0 = u2, v2	C borrow from sub2
	cmpltu	p13, p0 = u3, v3	C borrow from sub3
	;;
L(cj2):
   (p7)	cmpeqor	p8, p0 = -1, s2
   (p7)	add	s2 = 1, s2
  (p11)	cmpeqor	p12, p0 = 0, d2
  (p11)	add	d2 = -1, d2
	mov	r8 = 0
	nop	0
	;;
	st8	[sp] = s2, 8
	st8	[dp] = d2, 8
   (p8)	cmpeqor	p9, p0 = -1, s3
   (p8)	add	s3 = 1, s3
  (p12)	cmpeqor	p13, p0 = 0, d3
  (p12)	add	d3 = -1, d3
	;;
L(cj1):
   (p9)	mov	r8 = 2
	;;
  (p13)	add	r8 = 1, r8
	nop	0
	st8	[sp] = s3, 8
	st8	[dp] = d3, 8
	br.ret.sptk.many b0
EPILOGUE()
ASM_END()
