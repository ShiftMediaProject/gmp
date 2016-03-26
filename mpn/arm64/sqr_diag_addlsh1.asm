dnl  ARM64 mpn_sqr_dial_addlsh1.

dnl  Contributed to the GNU project by Torbjörn Granlund.

dnl  Copyright 2016 Free Software Foundation, Inc.

dnl  This file is part of the GNU MP Library.
dnl
dnl  The GNU MP Library is free software; you can redistribute it and/or modify
dnl  it under the terms of either:
dnl
dnl    * the GNU Lesser General Public License as published by the Free
dnl      Software Foundation; either version 3 of the License, or (at your
dnl      option) any later version.
dnl
dnl  or
dnl
dnl    * the GNU General Public License as published by the Free Software
dnl      Foundation; either version 2 of the License, or (at your option) any
dnl      later version.
dnl
dnl  or both in parallel, as here.
dnl
dnl  The GNU MP Library is distributed in the hope that it will be useful, but
dnl  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
dnl  for more details.
dnl
dnl  You should have received copies of the GNU General Public License and the
dnl  GNU Lesser General Public License along with the GNU MP Library.  If not,
dnl  see https://www.gnu.org/licenses/.

include(`../config.m4')

C	     cycles/limb
C Cortex-A53	 6.85
C Cortex-A57	 ?
C X-Gene	 4.1

define(`rp', `x0')
define(`tp', `x1')
define(`up', `x2')
define(`n',  `x3')

ASM_START()
PROLOGUE(mpn_sqr_diag_addlsh1)
	ldp	x4, x5, [up], #16
	mul	x10, x4, x4
	umulh	x13, x4, x4
	str	x10, [rp], #8

	ldp	x6, x7, [tp], #16
	mul	x4, x5, x5
	adds	x10, x13, xzr
	sub	n, n, #1
	b	L(dm)

L(top):	ldr	x5, [up], #8
	adds	x10, x6, x10
	adcs	x11, x7, x4
	ldp	x6, x7, [tp], #16
	mul	x4, x5, x5
	stp	x10, x11, [rp], #16
	add	x10, x13, x12
L(dm):	adcs	x6, x6, x6
	sub	n, n, #1
	adcs	x7, x7, x7
	umulh	x13, x5, x5
	adc	x12, xzr, xzr
	cbnz	n, L(top)

	adds	x10, x6, x10
	adcs	x11, x7, x4
	stp	x10, x11, [rp], #16
	adc	x13, x12, x13
	str	x13, [rp]

	ret
EPILOGUE()
