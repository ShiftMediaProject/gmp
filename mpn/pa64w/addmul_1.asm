dnl  HP-PA 2.0 64-bit mpn_addmul_1 -- Multiply a limb vector with a limb and
dnl  add the result to a second limb vector.

dnl  Copyright 1998, 1999, 2000, 2002 Free Software Foundation, Inc.

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


dnl  This runs at 6.25 cycles/limb on a PA8500.  This is very close to optimal,
dnl  but with more unrolling one could shave off one cycle.


include(`../config.m4')

dnl INPUT PARAMETERS:
define(`rp',`%r26')	dnl
define(`up',`%r25')	dnl
define(`n',`%r24')	dnl
define(`s2limb',`%r23')	dnl

define(`climb',`%r18')	dnl

	.level	2.0w
PROLOGUE(mpn_addmul_1)
	std		s2limb, -16(%r30)
	std,ma		%r3, 256(%r30)
	std		%r4, -248(%r30)
	std		%r5, -240(%r30)

	ldo		0(%r0), climb		C clear climb
	fldd		-272(%r30), %fr8


define(`p032a1',`%r1')	dnl
define(`p032a2',`%r19')	dnl

define(`m032',`%r20')	dnl
define(`m096',`%r21')	dnl

define(`p000a',`%r22')	dnl
define(`p064a',`%r29')	dnl

define(`s000',`%r31')	dnl

define(`ma000',`%r4')	dnl
define(`ma064',`%r20')	dnl

define(`r000',`%r3')	dnl

	extrd,u		n, 63, 2, %r5
	comb,=		%r5, %r0, L(BIG)
	nop

	fldd		0(up), %fr4
	ldo		8(up), up
	xmpyu		%fr8R, %fr4L, %fr22
	xmpyu		%fr8L, %fr4R, %fr23
	fstd		%fr22, -0x78(%r30)	C mid product to bytes  -0x78..-113
	xmpyu		%fr8R, %fr4R, %fr24
	xmpyu		%fr8L, %fr4L, %fr25
	fstd		%fr23, -0x70(%r30)	C mid product to bytes  -0x70..-105
	fstd		%fr24, -0x80(%r30)	C low product to bytes  -0x80..-121
	addib,<>	-1, %r5, L(two_or_more)
	fstd		%fr25, -0x68(%r30)	C high product to bytes -0x68..-97
L(one)
	ldd		-0x78(%r30), p032a1
	ldd		-0x70(%r30), p032a2
	ldd		-0x80(%r30), p000a
	b		L(0_one_out)
	ldd		-0x68(%r30), p064a

L(two_or_more)
	fldd		0(up), %fr4
	ldo		8(up), up
	xmpyu		%fr8R, %fr4L, %fr22
	xmpyu		%fr8L, %fr4R, %fr23
	ldd		-0x78(%r30), p032a1
	fstd		%fr22, -0x78(%r30)	C mid product to bytes  -0x78..-113
	xmpyu		%fr8R, %fr4R, %fr24
	xmpyu		%fr8L, %fr4L, %fr25
	ldd		-0x70(%r30), p032a2
	fstd		%fr23, -0x70(%r30)	C mid product to bytes  -0x70..-105
	ldd		-0x80(%r30), p000a
	fstd		%fr24, -0x80(%r30)	C low product to bytes  -0x80..-121
	ldd		-0x68(%r30), p064a
	addib,<>	-1, %r5, L(three_or_more)
	fstd		%fr25, -0x68(%r30)	C high product to bytes -0x68..-97
L(two)
	add		p032a1, p032a2, m032
	add,dc		%r0, %r0, m096
	depd,z		m032, 31, 32, ma000
	extrd,u		m032, 31, 32, ma064
	ldd		0(rp), r000
	b		L(0_two_out)
	depd		m096, 31, 32, ma064

L(three_or_more)
	fldd		0(up), %fr4
	add		p032a1, p032a2, m032
	add,dc		%r0, %r0, m096
	depd,z		m032, 31, 32, ma000
	extrd,u		m032, 31, 32, ma064
	ldd		0(rp), r000
dnl	addib,=		-1, %r5, L(0_out)
	depd		m096, 31, 32, ma064
L(oop0)
dnl	xmpyu		%fr8R, %fr4L, %fr22
dnl	xmpyu		%fr8L, %fr4R, %fr23
dnl	ldd		-0x78(%r30), p032a1
dnl	fstd		%fr22, -0x78(%r30)	C mid product to bytes  -0x78..-113
dnl
dnl	xmpyu		%fr8R, %fr4R, %fr24
dnl	xmpyu		%fr8L, %fr4L, %fr25
dnl	ldd		-0x70(%r30), p032a2
dnl	fstd		%fr23, -0x70(%r30)	C mid product to bytes  -0x70..-105
dnl
dnl	ldo		8(rp), rp
dnl	add		climb, p000a, s000
dnl	ldd		-0x80(%r30), p000a
dnl	fstd		%fr24, -0x80(%r30)	C low product to bytes  -0x80..-121
dnl
dnl	add,dc		p064a, %r0, climb
dnl	ldo		8(up), up
dnl	ldd		-0x68(%r30), p064a
dnl	fstd		%fr25, -0x68(%r30)	C high product to bytes -0x68..-97
dnl
dnl	add		ma000, s000, s000
dnl	add,dc		ma064, climb, climb
dnl	fldd		0(up), %fr4
dnl
dnl	add		r000, s000, s000
dnl	add,dc		%r0, climb, climb
dnl	std		s000, -8(rp)
dnl
dnl	add		p032a1, p032a2, m032
dnl	add,dc		%r0, %r0, m096
dnl
dnl	depd,z		m032, 31, 32, ma000
dnl	extrd,u		m032, 31, 32, ma064
dnl	ldd		0(rp), r000
dnl	addib,<>	-1, %r5, L(oop0)
dnl	depd		m096, 31, 32, ma064
L(0_out)
	ldo		8(up), up
	xmpyu		%fr8R, %fr4L, %fr22
	xmpyu		%fr8L, %fr4R, %fr23
	ldd		-0x78(%r30), p032a1
	fstd		%fr22, -0x78(%r30)	C mid product to bytes  -0x78..-113
	xmpyu		%fr8R, %fr4R, %fr24
	xmpyu		%fr8L, %fr4L, %fr25
	ldd		-0x70(%r30), p032a2
	fstd		%fr23, -0x70(%r30)	C mid product to bytes  -0x70..-105
	ldo		8(rp), rp
	add		climb, p000a, s000
	ldd		-0x80(%r30), p000a
	fstd		%fr24, -0x80(%r30)	C low product to bytes  -0x80..-121
	add,dc		p064a, %r0, climb
	ldd		-0x68(%r30), p064a
	fstd		%fr25, -0x68(%r30)	C high product to bytes -0x68..-97
	add		ma000, s000, s000
	add,dc		ma064, climb, climb
	add		r000, s000, s000
	add,dc		%r0, climb, climb
	std		s000, -8(rp)
	add		p032a1, p032a2, m032
	add,dc		%r0, %r0, m096
	depd,z		m032, 31, 32, ma000
	extrd,u		m032, 31, 32, ma064
	ldd		0(rp), r000
	depd		m096, 31, 32, ma064
L(0_two_out)
	ldd		-0x78(%r30), p032a1
	ldd		-0x70(%r30), p032a2
	ldo		8(rp), rp
	add		climb, p000a, s000
	ldd		-0x80(%r30), p000a
	add,dc		p064a, %r0, climb
	ldd		-0x68(%r30), p064a
	add		ma000, s000, s000
	add,dc		ma064, climb, climb
	add		r000, s000, s000
	add,dc		%r0, climb, climb
	std		s000, -8(rp)
L(0_one_out)
	add		p032a1, p032a2, m032
	add,dc		%r0, %r0, m096
	depd,z		m032, 31, 32, ma000
	extrd,u		m032, 31, 32, ma064
	ldd		0(rp), r000
	depd		m096, 31, 32, ma064

	add		climb, p000a, s000
	add,dc		p064a, %r0, climb
	add		ma000, s000, s000
	add,dc		ma064, climb, climb
	add		r000, s000, s000
	add,dc		%r0, climb, climb
	std		s000, 0(rp)

	comib,<		4, n, L(BIG)		C Conditionally proceed to BIG code
	ldo		8(rp), rp

dnl We're done already, restore registers and return
	copy		climb, %r28
	ldd		-240(%r30), %r5
	ldd		-248(%r30), %r4
	bve		(%r2)
	ldd,mb		-256(%r30), %r3


dnl 4-way unrolled code.

L(BIG)

define(`p032a1',`%r1')	dnl
define(`p032a2',`%r19')	dnl
define(`p096b1',`%r20')	dnl
define(`p096b2',`%r21')	dnl
define(`p160c1',`%r22')	dnl
define(`p160c2',`%r29')	dnl
define(`p224d1',`%r31')	dnl
define(`p224d2',`%r3')	dnl
			dnl
define(`m032',`%r4')	dnl
define(`m096',`%r5')	dnl
define(`m160',`%r6')	dnl
define(`m224',`%r7')	dnl
define(`m288',`%r8')	dnl
			dnl
define(`p000a',`%r1')	dnl
define(`p064a',`%r19')	dnl
define(`p064b',`%r20')	dnl
define(`p128b',`%r21')	dnl
define(`p128c',`%r22')	dnl
define(`p192c',`%r29')	dnl
define(`p192d',`%r31')	dnl
define(`p256d',`%r3')	dnl
			dnl
define(`s000',`%r10')	dnl
define(`s064',`%r11')	dnl
define(`s128',`%r12')	dnl
define(`s192',`%r13')	dnl
			dnl
define(`ma000',`%r9')	dnl
define(`ma064',`%r4')	dnl
define(`ma128',`%r5')	dnl
define(`ma192',`%r6')	dnl
define(`ma256',`%r7')	dnl
			dnl
define(`r000',`%r1')	dnl
define(`r064',`%r19')	dnl
define(`r128',`%r20')	dnl
define(`r192',`%r21')	dnl

	std		%r6, -232(%r30)
	std		%r7, -224(%r30)
	std		%r8, -216(%r30)
	std		%r9, -208(%r30)
	std		%r10, -200(%r30)
	std		%r11, -192(%r30)
	std		%r12, -184(%r30)
	std		%r13, -176(%r30)
	std		%r18, -136(%r30)

	extrd,u		n, 61, 62, n		C right shift 2 in HP lingo

L(4_or_more)
	fldd		0(up), %fr4
	fldd		8(up), %fr5
	fldd		16(up), %fr6
	fldd		24(up), %fr7
	xmpyu		%fr8R, %fr4L, %fr22
	xmpyu		%fr8L, %fr4R, %fr23
	xmpyu		%fr8R, %fr5L, %fr24
	xmpyu		%fr8L, %fr5R, %fr25
	xmpyu		%fr8R, %fr6L, %fr26
	xmpyu		%fr8L, %fr6R, %fr27
	fstd		%fr22, -0x78(%r30)	C mid product to bytes  -0x78..-113
	xmpyu		%fr8R, %fr7L, %fr28
	xmpyu		%fr8L, %fr7R, %fr29
	fstd		%fr23, -0x70(%r30)	C mid product to bytes  -0x70..-105
	xmpyu		%fr8R, %fr4R, %fr30
	xmpyu		%fr8L, %fr4L, %fr31
	fstd		%fr24, -0x38(%r30)	C mid product to bytes  -0x38..-49
	xmpyu		%fr8R, %fr5R, %fr22
	xmpyu		%fr8L, %fr5L, %fr23
	fstd		%fr25, -0x30(%r30)	C mid product to bytes  -0x30..-41
	xmpyu		%fr8R, %fr6R, %fr24
	xmpyu		%fr8L, %fr6L, %fr25
	fstd		%fr26, -0x58(%r30)	C mid product to bytes  -0x58..-81
	xmpyu		%fr8R, %fr7R, %fr26
	fstd		%fr27, -0x50(%r30)	C mid product to bytes  -0x50..-73
	addib,<>	-1, n, L(8_or_more)
	xmpyu		%fr8L, %fr7L, %fr27
	fstd		%fr28, -0x18(%r30)	C mid product to bytes  -0x18..-17
	fstd		%fr29, -0x10(%r30)	C mid product to bytes  -0x10..-9
	fstd		%fr30, -0x80(%r30)	C low product to bytes  -0x80..-121
	fstd		%fr31, -0x68(%r30)	C high product to bytes -0x68..-97
	fstd		%fr22, -0x40(%r30)	C low product to bytes  -0x40..-57
	fstd		%fr23, -0x28(%r30)	C high product to bytes -0x28..-33
	fstd		%fr24, -0x60(%r30)	C low product to bytes  -0x60..-89
	fstd		%fr25, -0x48(%r30)	C high product to bytes -0x48..-65
	fstd		%fr26, -0x20(%r30)	C low product to bytes  -0x20..-25
	fstd		%fr27, -0x08(%r30)	C high product to bytes -0x08..-1
	ldd		-0x78(%r30), p032a1
	ldd		-0x70(%r30), p032a2
	ldd		-0x38(%r30), p096b1
	ldd		-0x30(%r30), p096b2
	ldd		-0x58(%r30), p160c1
	ldd		-0x50(%r30), p160c2
	ldd		-0x18(%r30), p224d1
	ldd		-0x10(%r30), p224d2
	b		L(end1)
	nop

L(8_or_more)
	fstd		%fr28, -0x18(%r30)	C mid product to bytes  -0x18..-17
	fstd		%fr29, -0x10(%r30)	C mid product to bytes  -0x10..-9
	ldo		32(up), up
	fstd		%fr30, -0x80(%r30)	C low product to bytes  -0x80..-121
	fstd		%fr31, -0x68(%r30)	C high product to bytes -0x68..-97
	fstd		%fr22, -0x40(%r30)	C low product to bytes  -0x40..-57
	fstd		%fr23, -0x28(%r30)	C high product to bytes -0x28..-33
	fstd		%fr24, -0x60(%r30)	C low product to bytes  -0x60..-89
	fstd		%fr25, -0x48(%r30)	C high product to bytes -0x48..-65
	fstd		%fr26, -0x20(%r30)	C low product to bytes  -0x20..-25
	fstd		%fr27, -0x08(%r30)	C high product to bytes -0x08..-1
	fldd		0(up), %fr4
	fldd		8(up), %fr5
	fldd		16(up), %fr6
	fldd		24(up), %fr7
	xmpyu		%fr8R, %fr4L, %fr22
	ldd		-0x78(%r30), p032a1
	xmpyu		%fr8L, %fr4R, %fr23
	xmpyu		%fr8R, %fr5L, %fr24
	ldd		-0x70(%r30), p032a2
	xmpyu		%fr8L, %fr5R, %fr25
	xmpyu		%fr8R, %fr6L, %fr26
	ldd		-0x38(%r30), p096b1
	xmpyu		%fr8L, %fr6R, %fr27
	fstd		%fr22, -0x78(%r30)	C mid product to bytes  -0x78..-113
	xmpyu		%fr8R, %fr7L, %fr28
	ldd		-0x30(%r30), p096b2
	xmpyu		%fr8L, %fr7R, %fr29
	fstd		%fr23, -0x70(%r30)	C mid product to bytes  -0x70..-105
	xmpyu		%fr8R, %fr4R, %fr30
	ldd		-0x58(%r30), p160c1
	xmpyu		%fr8L, %fr4L, %fr31
	fstd		%fr24, -0x38(%r30)	C mid product to bytes  -0x38..-49
	xmpyu		%fr8R, %fr5R, %fr22
	ldd		-0x50(%r30), p160c2
	xmpyu		%fr8L, %fr5L, %fr23
	fstd		%fr25, -0x30(%r30)	C mid product to bytes  -0x30..-41
	xmpyu		%fr8R, %fr6R, %fr24
	ldd		-0x18(%r30), p224d1
	xmpyu		%fr8L, %fr6L, %fr25
	fstd		%fr26, -0x58(%r30)	C mid product to bytes  -0x58..-81
	xmpyu		%fr8R, %fr7R, %fr26
	ldd		-0x10(%r30), p224d2
	fstd		%fr27, -0x50(%r30)	C mid product to bytes  -0x50..-73
	addib,=		-1, n, L(end2)
	xmpyu		%fr8L, %fr7L, %fr27
L(oop)
	add		p032a1, p032a2, m032
	ldd		-0x80(%r30), p000a
	add,dc		p096b1, p096b2, m096
	fstd		%fr28, -0x18(%r30)	C mid product to bytes  -0x18..-17

	add,dc		p160c1, p160c2, m160
	ldd		-0x68(%r30), p064a
	add,dc		p224d1, p224d2, m224
	fstd		%fr29, -0x10(%r30)	C mid product to bytes  -0x10..-9

	add,dc		%r0, %r0, m288
	ldd		-0x40(%r30), p064b
	ldo		32(up), up
	fstd		%fr30, -0x80(%r30)	C low product to bytes  -0x80..-121

	depd,z		m032, 31, 32, ma000
	ldd		-0x28(%r30), p128b
	extrd,u		m032, 31, 32, ma064
	fstd		%fr31, -0x68(%r30)	C high product to bytes -0x68..-97

	depd		m096, 31, 32, ma064
	ldd		-0x60(%r30), p128c
	extrd,u		m096, 31, 32, ma128
	fstd		%fr22, -0x40(%r30)	C low product to bytes  -0x40..-57

	depd		m160, 31, 32, ma128
	ldd		-0x48(%r30), p192c
	extrd,u		m160, 31, 32, ma192
	fstd		%fr23, -0x28(%r30)	C high product to bytes -0x28..-33

	depd		m224, 31, 32, ma192
	ldd		-0x20(%r30), p192d
	extrd,u		m224, 31, 32, ma256
	fstd		%fr24, -0x60(%r30)	C low product to bytes  -0x60..-89

	depd		m288, 31, 32, ma256
	ldd		-0x08(%r30), p256d
	add		climb, p000a, s000
	fstd		%fr25, -0x48(%r30)	C high product to bytes -0x48..-65

	add,dc		p064a, p064b, s064
	ldd		0(rp), r000
	add,dc		p128b, p128c, s128
	fstd		%fr26, -0x20(%r30)	C low product to bytes  -0x20..-25

	add,dc		p192c, p192d, s192
	ldd		8(rp), r064
	add,dc		p256d, %r0, climb
	fstd		%fr27, -0x08(%r30)	C high product to bytes -0x08..-1

	ldd		16(rp), r128
	add		ma000, s000, s000	C accum mid 0
	ldd		24(rp), r192
	add,dc		ma064, s064, s064	C accum mid 1

	add,dc		ma128, s128, s128	C accum mid 2
	fldd		0(up), %fr4
	add,dc		ma192, s192, s192	C accum mid 3
	fldd		8(up), %fr5

	add,dc		ma256, climb, climb
	fldd		16(up), %fr6
	add		r000, s000, s000	C accum rlimb 0
	fldd		24(up), %fr7

	add,dc		r064, s064, s064	C accum rlimb 1
	add,dc		r128, s128, s128	C accum rlimb 2
	std		s000, 0(rp)

	add,dc		r192, s192, s192	C accum rlimb 3
	add,dc		%r0, climb, climb
	std		s064, 8(rp)

	xmpyu		%fr8R, %fr4L, %fr22
	ldd		-0x78(%r30), p032a1
	xmpyu		%fr8L, %fr4R, %fr23
	std		s128, 16(rp)

	xmpyu		%fr8R, %fr5L, %fr24
	ldd		-0x70(%r30), p032a2
	xmpyu		%fr8L, %fr5R, %fr25
	std		s192, 24(rp)

	xmpyu		%fr8R, %fr6L, %fr26
	ldd		-0x38(%r30), p096b1
	xmpyu		%fr8L, %fr6R, %fr27
	fstd		%fr22, -0x78(%r30)	C mid product to bytes  -0x78..-113

	xmpyu		%fr8R, %fr7L, %fr28
	ldd		-0x30(%r30), p096b2
	xmpyu		%fr8L, %fr7R, %fr29
	fstd		%fr23, -0x70(%r30)	C mid product to bytes  -0x70..-105

	xmpyu		%fr8R, %fr4R, %fr30
	ldd		-0x58(%r30), p160c1
	xmpyu		%fr8L, %fr4L, %fr31
	fstd		%fr24, -0x38(%r30)	C mid product to bytes  -0x38..-49

	xmpyu		%fr8R, %fr5R, %fr22
	ldd		-0x50(%r30), p160c2
	xmpyu		%fr8L, %fr5L, %fr23
	fstd		%fr25, -0x30(%r30)	C mid product to bytes  -0x30..-41

	xmpyu		%fr8R, %fr6R, %fr24
	ldd		-0x18(%r30), p224d1
	xmpyu		%fr8L, %fr6L, %fr25
	fstd		%fr26, -0x58(%r30)	C mid product to bytes  -0x58..-81

	xmpyu		%fr8R, %fr7R, %fr26
	ldd		-0x10(%r30), p224d2
	fstd		%fr27, -0x50(%r30)	C mid product to bytes  -0x50..-73
	xmpyu		%fr8L, %fr7L, %fr27

	addib,<>	-1, n, L(oop)
	ldo		32(rp), rp

L(end2)
	add		p032a1, p032a2, m032
	ldd		-0x80(%r30), p000a
	add,dc		p096b1, p096b2, m096
	fstd		%fr28, -0x18(%r30)	C mid product to bytes  -0x18..-17
	add,dc		p160c1, p160c2, m160
	ldd		-0x68(%r30), p064a
	add,dc		p224d1, p224d2, m224
	fstd		%fr29, -0x10(%r30)	C mid product to bytes  -0x10..-9
	add,dc		%r0, %r0, m288
	ldd		-0x40(%r30), p064b
	fstd		%fr30, -0x80(%r30)	C low product to bytes  -0x80..-121
	depd,z		m032, 31, 32, ma000
	ldd		-0x28(%r30), p128b
	extrd,u		m032, 31, 32, ma064
	fstd		%fr31, -0x68(%r30)	C high product to bytes -0x68..-97
	depd		m096, 31, 32, ma064
	ldd		-0x60(%r30), p128c
	extrd,u		m096, 31, 32, ma128
	fstd		%fr22, -0x40(%r30)	C low product to bytes  -0x40..-57
	depd		m160, 31, 32, ma128
	ldd		-0x48(%r30), p192c
	extrd,u		m160, 31, 32, ma192
	fstd		%fr23, -0x28(%r30)	C high product to bytes -0x28..-33
	depd		m224, 31, 32, ma192
	ldd		-0x20(%r30), p192d
	extrd,u		m224, 31, 32, ma256
	fstd		%fr24, -0x60(%r30)	C low product to bytes  -0x60..-89
	depd		m288, 31, 32, ma256
	ldd		-0x08(%r30), p256d
	add		climb, p000a, s000
	fstd		%fr25, -0x48(%r30)	C high product to bytes -0x48..-65
	add,dc		p064a, p064b, s064
	ldd		0(rp), r000
	add,dc		p128b, p128c, s128
	fstd		%fr26, -0x20(%r30)	C low product to bytes  -0x20..-25
	add,dc		p192c, p192d, s192
	ldd		8(rp), r064
	add,dc		p256d, %r0, climb
	fstd		%fr27, -0x08(%r30)	C high product to bytes -0x08..-1
	ldd		16(rp), r128
	add		ma000, s000, s000	C accum mid 0
	ldd		24(rp), r192
	add,dc		ma064, s064, s064	C accum mid 1
	add,dc		ma128, s128, s128	C accum mid 2
	add,dc		ma192, s192, s192	C accum mid 3
	add,dc		ma256, climb, climb
	add		r000, s000, s000	C accum rlimb 0
	add,dc		r064, s064, s064	C accum rlimb 1
	add,dc		r128, s128, s128	C accum rlimb 2
	std		s000, 0(rp)
	add,dc		r192, s192, s192	C accum rlimb 3
	add,dc		%r0, climb, climb
	std		s064, 8(rp)
	ldd		-0x78(%r30), p032a1
	std		s128, 16(rp)
	ldd		-0x70(%r30), p032a2
	std		s192, 24(rp)
	ldd		-0x38(%r30), p096b1
	ldd		-0x30(%r30), p096b2
	ldd		-0x58(%r30), p160c1
	ldd		-0x50(%r30), p160c2
	ldd		-0x18(%r30), p224d1
	ldd		-0x10(%r30), p224d2
	ldo		32(rp), rp

L(end1)
	add		p032a1, p032a2, m032
	ldd		-0x80(%r30), p000a
	add,dc		p096b1, p096b2, m096
	add,dc		p160c1, p160c2, m160
	ldd		-0x68(%r30), p064a
	add,dc		p224d1, p224d2, m224
	add,dc		%r0, %r0, m288
	ldd		-0x40(%r30), p064b
	depd,z		m032, 31, 32, ma000
	ldd		-0x28(%r30), p128b
	extrd,u		m032, 31, 32, ma064
	depd		m096, 31, 32, ma064
	ldd		-0x60(%r30), p128c
	extrd,u		m096, 31, 32, ma128
	depd		m160, 31, 32, ma128
	ldd		-0x48(%r30), p192c
	extrd,u		m160, 31, 32, ma192
	depd		m224, 31, 32, ma192
	ldd		-0x20(%r30), p192d
	extrd,u		m224, 31, 32, ma256
	depd		m288, 31, 32, ma256
	ldd		-0x08(%r30), p256d
	add		climb, p000a, s000
	add,dc		p064a, p064b, s064
	ldd		0(rp), r000
	add,dc		p128b, p128c, s128
	add,dc		p192c, p192d, s192
	ldd		8(rp), r064
	add,dc		p256d, %r0, climb
	ldd		16(rp), r128
	add		ma000, s000, s000	C accum mid 0
	ldd		24(rp), r192
	add,dc		ma064, s064, s064	C accum mid 1
	add,dc		ma128, s128, s128	C accum mid 2
	add,dc		ma192, s192, s192	C accum mid 3
	add,dc		ma256, climb, climb
	add		r000, s000, s000	C accum rlimb 0
	add,dc		r064, s064, s064	C accum rlimb 1
	add,dc		r128, s128, s128	C accum rlimb 2
	std		s000, 0(rp)
	add,dc		r192, s192, s192	C accum rlimb 3
	add,dc		%r0, climb, climb
	std		s064, 8(rp)
	std		s128, 16(rp)
	std		s192, 24(rp)

	copy		climb, %r28

	ldd		-136(%r30), %r18
	ldd		-176(%r30), %r13
	ldd		-184(%r30), %r12
	ldd		-192(%r30), %r11
	ldd		-200(%r30), %r10
	ldd		-208(%r30), %r9
	ldd		-216(%r30), %r8
	ldd		-224(%r30), %r7
	ldd		-232(%r30), %r6
	ldd		-240(%r30), %r5
	ldd		-248(%r30), %r4

	bve		(%r2)
	ldd,mb		-256(%r30), %r3
EPILOGUE(mpn_addmul_1)
