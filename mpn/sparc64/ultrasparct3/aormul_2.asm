dnl  SPARC v9 mpn_mul_2 and mpn_addmul_2 for T3/T4.

dnl  Copyright 2013 Free Software Foundation, Inc.

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


C		    cycles/limb     cycles/limb
C		       mul_2          addmul_2
C UltraSPARC T3:	23		24
C UltraSPARC T4:	~3.5		~4


C This code is based on the summation algorithm used for x86-64 where the
C multiply instruction clobbers the carry flag.  It would be possible to
C keep carry alive and thereby save 3 instructions per iteration.
C
C The code is reasonably scheduled for long-latency instructions, but no micro-
C scheduling has been done.  There is hope that this could run at around 3.5
C c/l on T4 if an optimal schedule was found.


C INPUT PARAMETERS
define(`rp', `%i0')
define(`up', `%i1')
define(`n',  `%i2')
define(`vp', `%i3')

define(`v0', `%o0')
define(`v1', `%o1')
define(`w0', `%o2')
define(`w1', `%o3')
define(`w2', `%o4')
define(`w3', `%o5')

C Free registers: i5 o7.  We use g2,g3 for the missing.m4 emulation mechanism.

ifdef(`OPERATION_mul_2',`
      define(`ADDSUB',   `add')
      define(`AM2',      `')
      define(`ADDX',	 `addcc`'$1')
      define(`func',     `mpn_mul_2')
')
ifdef(`OPERATION_addmul_2',`
      define(`ADDSUB',   `sub')
      define(`AM2',      `$1')
      define(`ADDX',	 `addxccc($1,$2,$3)')
      define(`func',     `mpn_addmul_2')
')


MULFUNC_PROLOGUE(mpn_mul_2 mpn_addmul_2)

ASM_START()
ifdef(`FAKE_T3',`
	REGISTER(%g2,#scratch)
	REGISTER(%g3,#scratch)
')
PROLOGUE(func)
	save	%sp, -176, %sp

	ldx	[vp+0], v0		C load v0
	ldx	[vp+8], v1		C load v1
	ldx	[up+0], %i4		C load u0
AM2(`	ldx	[rp+0], %g1	')	C load r0 if addmul_2

	mulx	%i4, v0, %l0
	umulxhi(%i4, v0, %l4)

	and	n, 3, %g5
	brz	%g5, L(b0)
	 cmp	%g5, 2
	bcs	L(b1)
	 nop
	be	L(b2)
	 nop

L(b3):	mov	%l0, w1
	mov	%l4, w2
	mov	0, w3
	add	rp, -16, rp
	b	L(lo3)
	 add	up, -16, up

L(b2):	mov	%l0, w2
	mov	%l4, w3
	mov	0, w0
	add	rp, -24, rp
	b	L(lo2)
	 add	up, -24, up

L(b1):	mov	%l0, w3
	mov	%l4, w0
	mov	0, w1
	b	L(top)
	 add	n, -1, n

L(b0):	mov	%l0, w0
	mov	%l4, w1
	mov	0, w2
	add	rp, -8, rp
	b	L(lo0)
	 add	up, -8, up

	ALIGN(32)
L(top):	mulx	%i4, v1, %l1
	umulxhi(%i4, v1, %l5)
	ldx	[up+8], %i4
AM2(`	addcc	w3, %g1, w3')
	stx	w3, [rp+0]
	ADDX(`	%l1, w0, w0')
	addxccc(%l5, w1, w1)
	mulx	%i4, v0, %l2
	umulxhi(%i4, v0, %l6)
AM2(`	ldx	[rp+8], %g1')
	addcc	%l2, w0, w0
	addxccc(%l6, w1, w1)
	addxc(	%g0, %g0, w2)
L(lo0):	mulx	%i4, v1, %l3
	umulxhi(%i4, v1, %l7)
	ldx	[up+16], %i4
AM2(`	addcc	w0, %g1, w0')
	stx	w0, [rp+8]
	ADDX(`	%l3, w1, w1')
	addxccc(%l7, w2, w2)
	mulx	%i4, v0, %l0
	umulxhi(%i4, v0, %l4)
AM2(`	ldx	[rp+16], %g1')
	addcc	%l0, w1, w1
	addxccc(%l4, w2, w2)
	addxc(	%g0, %g0, w3)
L(lo3):	mulx	%i4, v1, %l1
	umulxhi(%i4, v1, %l5)
	ldx	[up+24], %i4
AM2(`	addcc	w1, %g1, w1')
	stx	w1, [rp+16]
	ADDX(`	%l1, w2, w2')
	addxccc(%l5, w3, w3)
	mulx	%i4, v0, %l2
	umulxhi(%i4, v0, %l6)
AM2(`	ldx	[rp+24], %g1')
	addcc	%l2, w2, w2
	addxccc(%l6, w3, w3)
	addxc(	%g0, %g0, w0)
L(lo2):	mulx	%i4, v1, %l3
	umulxhi(%i4, v1, %l7)
	ldx	[up+32], %i4
AM2(`	addcc	w2, %g1, w2')
	stx	w2, [rp+24]
	ADDX(`	%l3, w3, w3')
	addxccc(%l7, w0, w0)
	mulx	%i4, v0, %l0
	umulxhi(%i4, v0, %l4)
AM2(`	ldx	[rp+32], %g1')
	addcc	%l0, w3, w3
	addxccc(%l4, w0, w0)
	addxc(	%g0, %g0, w1)
	add	n, -4, n
	add	rp, 32, rp
	brgz	n, L(top)
	 add	up, 32, up

L(end):	mulx	%i4, v1, %l1
	umulxhi(%i4, v1, %l5)
AM2(`	addcc	w3, %g1, w3')
	stx	w3, [rp+0]
	ADDX(`	%l1, w0, w0')
	stx	w0, [rp+8]
	addxc(	%l5, w1, %i0)

	ret
	 restore
EPILOGUE()
