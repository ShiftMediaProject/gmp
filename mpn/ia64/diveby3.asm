dnl  IA-64 mpn_divexact_by3c -- mpn by 3 exact division.

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
C Itanium:    6.0?
C Itanium 2:  4.0

C TODO
C  * Handle odd n better
C  * Consider scheduling ldf8 closer to xma for simpler feed-in/wind-down code
C  * Optimize function entry code

C INPUT PARAMETERS
define(`rp',`r32')
define(`up',`r33')
define(`n',`r34')
define(`cy',`r35')

define(`upf',`r31')
define(`l0',`r15')
define(`s0',`r21')
define(`s1',`r23')
define(`x',`r22')
define(`r0x5555555555555555',r18)
define(`r0xAAAAAAAAAAAAAAAA',r19)

ASM_START()
PROLOGUE(mpn_divexact_by3c)
	.prologue
	.save	ar.lc, r2
	mov		r2 = ar.lc
	.body
ifdef(`HAVE_ABI_32',`
	addp4		rp = 0, rp
	addp4		up = 0, up
	zxt4		n = n
	;;')
	add		r15 = -2, n		C FIXME
	;;
	shr		r15 = r15, 1
	;;
	mov		ar.lc = r15		C loop count
	mov		upf = up		C for loads into fpreg
	movl		r14 = 0xAAAAAAAAAAAAAAAB
	;;
	setf.sig	f6 = r14
	sub		r0x5555555555555555 = 0, r14
	add		r0xAAAAAAAAAAAAAAAA = -1, r14
	mov		l0 = 0
	cmp.eq		p6, p0 = 1, cy
	;;
   (p6)	mov		l0 = r0x5555555555555555
	cmp.eq		p7, p0 = 2, cy
	;;
   (p7)	add		l0 = r0x5555555555555555, r0x5555555555555555



	tbit.z		p6, p0 = n, 0
	cmp.ltu		p0, p12 = 2, n
	;;
   (p6)	br.dptk		.Lkeepgoing		C jump if n is even
	ldf8		f11 = [upf], 8
	;;
	xma.lu		f9 = f6, f11, f0
	;;
	getf.sig	r9 = f9
	ld8		s1 = [up], 8
	;;
	add		x = l0, r9
	cmp.ltu		p10, p11 = s1, cy
	mov		cy = 0
	;;
	st8		[rp] = x, 8
  (p10)	mov		cy = 1
	cmp.gtu		p6, p0 = x, r0x5555555555555555
	cmp.gtu		p7, p0 = x, r0xAAAAAAAAAAAAAAAA
  (p11)	mov		l0 = 0
  (p10)	mov		l0 = r0x5555555555555555
	;;
   (p6)	add		cy = 1, cy
   (p6)	add		l0 = l0, r0x5555555555555555
	;;
   (p7)	add		cy = 1, cy
   (p7)	add		l0 = l0, r0x5555555555555555
	;;

  (p12)	br.dpnt		.Lret

.Lkeepgoing:
	ldf8		f10 = [upf], 8
	;;
	ldf8		f11 = [upf], 8
	br.cloop.dptk	.Lfi_grt_2

C n = 2
	xma.lu		f8 = f6, f10, f0
	;;
	xma.lu		f9 = f6, f11, f0
	getf.sig	r8 = f8
	;;
	getf.sig	r9 = f9
	ld8		s0 = [up], 8
	br		.L2

.Lfi_grt_2:
	xma.lu		f8 = f6, f10, f0
	ldf8		f10 = [upf], 8
	;;
	xma.lu		f9 = f6, f11, f0
	ldf8		f11 = [upf], 8
	getf.sig	r8 = f8
	;;
	br.cloop.dptk	.Lfi_grt_4
C n = 4
	ld8		s0 = [up], 8
	xma.lu		f8 = f6, f10, f0
	getf.sig	r9 = f9
	br		.L4

.Lfi_grt_4:
	ld8		s0 = [up], 8
	xma.lu		f8 = f6, f10, f0
	ldf8		f10 = [upf], 8
	getf.sig	r9 = f9
	;;
	ld8		s1 = [up], 8
	xma.lu		f9 = f6, f11, f0
	add		x = l0, r8
	ldf8		f11 = [upf], 8
	cmp.ltu		p10, p11 = s0, cy
	mov		cy = 0
	;;
	st8		[rp] = x, 8
  (p10)	mov		cy = 1
	cmp.gtu		p6, p0 = x, r0x5555555555555555
	cmp.gtu		p7, p0 = x, r0xAAAAAAAAAAAAAAAA
  (p11)	mov		l0 = 0
  (p10)	mov		l0 = r0x5555555555555555
	;;
	getf.sig	r8 = f8
   (p6)	add		cy = 1, cy
   (p6)	add		l0 = l0, r0x5555555555555555
	;;
   (p7)	add		cy = 1, cy
   (p7)	add		l0 = l0, r0x5555555555555555
	br.cloop.dptk	.Ltop
	br		.Lbot


C *** MAIN LOOP START ***
	ALIGN(32)
.Ltop:
	ld8		s0 = [up], 8
	xma.lu		f8 = f6, f10, f0
	add		x = l0, r9
	ldf8		f10 = [upf], 8
	cmp.ltu		p10, p11 = s1, cy
	mov		cy = 0
	;;
	st8		[rp] = x, 8
  (p10)	mov		cy = 1
	cmp.gtu		p6, p0 = x, r0x5555555555555555
	cmp.gtu		p7, p0 = x, r0xAAAAAAAAAAAAAAAA
  (p11)	mov		l0 = 0
  (p10)	mov		l0 = r0x5555555555555555
	;;
	getf.sig	r9 = f9
   (p6)	add		cy = 1, cy
   (p6)	add		l0 = l0, r0x5555555555555555
	;;
   (p7)	add		cy = 1, cy
   (p7)	add		l0 = l0, r0x5555555555555555
	nop.b		0
	;;
.Lmid:
	ld8		s1 = [up], 8
	xma.lu		f9 = f6, f11, f0
	add		x = l0, r8
	ldf8		f11 = [upf], 8
	cmp.ltu		p10, p11 = s0, cy
	mov		cy = 0
	;;
	st8		[rp] = x, 8
  (p10)	mov		cy = 1
	cmp.gtu		p6, p0 = x, r0x5555555555555555
	cmp.gtu		p7, p0 = x, r0xAAAAAAAAAAAAAAAA
  (p11)	mov		l0 = 0
  (p10)	mov		l0 = r0x5555555555555555
	;;
	getf.sig	r8 = f8
   (p6)	add		cy = 1, cy
   (p6)	add		l0 = l0, r0x5555555555555555
	;;
   (p7)	add		cy = 1, cy
   (p7)	add		l0 = l0, r0x5555555555555555
	br.cloop.dptk	.Ltop
	;;
C *** MAIN LOOP END ***

.Lbot:	ld8		s0 = [up], 8
	xma.lu		f8 = f6, f10, f0
	add		x = l0, r9
	cmp.ltu		p10, p11 = s1, cy
	mov		cy = 0
	;;
	st8		[rp] = x, 8
  (p10)	mov		cy = 1
	cmp.gtu		p6, p0 = x, r0x5555555555555555
	cmp.gtu		p7, p0 = x, r0xAAAAAAAAAAAAAAAA
  (p11)	mov		l0 = 0
  (p10)	mov		l0 = r0x5555555555555555
	;;
	getf.sig	r9 = f9
   (p6)	add		cy = 1, cy
   (p6)	add		l0 = l0, r0x5555555555555555
	;;
   (p7)	add		cy = 1, cy
   (p7)	add		l0 = l0, r0x5555555555555555
	;;
.L4:	ld8		s1 = [up], 8
	xma.lu		f9 = f6, f11, f0
	add		x = l0, r8
	cmp.ltu		p10, p11 = s0, cy
	mov		cy = 0
	;;
	st8		[rp] = x, 8
  (p10)	mov		cy = 1
	cmp.gtu		p6, p0 = x, r0x5555555555555555
	cmp.gtu		p7, p0 = x, r0xAAAAAAAAAAAAAAAA
  (p11)	mov		l0 = 0
  (p10)	mov		l0 = r0x5555555555555555
	;;
	getf.sig	r8 = f8
   (p6)	add		cy = 1, cy
   (p6)	add		l0 = l0, r0x5555555555555555
	;;
   (p7)	add		cy = 1, cy
   (p7)	add		l0 = l0, r0x5555555555555555
	;;
	ld8		s0 = [up], 8
	add		x = l0, r9
	cmp.ltu		p10, p11 = s1, cy
	mov		cy = 0
	;;
	st8		[rp] = x, 8
  (p10)	mov		cy = 1
	cmp.gtu		p6, p0 = x, r0x5555555555555555
	cmp.gtu		p7, p0 = x, r0xAAAAAAAAAAAAAAAA
  (p11)	mov		l0 = 0
  (p10)	mov		l0 = r0x5555555555555555
	;;
	getf.sig	r9 = f9
   (p6)	add		cy = 1, cy
   (p6)	add		l0 = l0, r0x5555555555555555
	;;
   (p7)	add		cy = 1, cy
   (p7)	add		l0 = l0, r0x5555555555555555
	;;
.L2:	ld8		s1 = [up], 8
	add		x = l0, r8
	cmp.ltu		p10, p11 = s0, cy
	mov		cy = 0
	;;
	st8		[rp] = x, 8
  (p10)	mov		cy = 1
	cmp.gtu		p6, p0 = x, r0x5555555555555555
	cmp.gtu		p7, p0 = x, r0xAAAAAAAAAAAAAAAA
  (p11)	mov		l0 = 0
  (p10)	mov		l0 = r0x5555555555555555
	;;
   (p6)	add		cy = 1, cy
   (p6)	add		l0 = l0, r0x5555555555555555
	;;
   (p7)	add		cy = 1, cy
   (p7)	add		l0 = l0, r0x5555555555555555
	;;
	add		x = l0, r9
	cmp.ltu		p10, p11 = s1, cy
	mov		cy = 0
	;;
	st8		[rp] = x, 8
  (p10)	mov		cy = 1
	cmp.gtu		p6, p0 = x, r0x5555555555555555
	cmp.gtu		p7, p0 = x, r0xAAAAAAAAAAAAAAAA
	;;
   (p6)	add		cy = 1, cy
	;;
   (p7)	add		cy = 1, cy
	;;
.Lret:	mov	r8 = cy
	mov.i	ar.lc = r2
	br.ret.sptk.many `rp'
EPILOGUE()
