dnl  ARM64 mpn_add_n and mpn_sub_n

dnl  Contributed to the GNU project by Torbjörn Granlund.

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
dnl  along with the GNU MP Library.  If not, see https://www.gnu.org/licenses/.

include(`../config.m4')

C	     cycles/limb
C Cortex-A53	 ?
C Cortex-A57	 ?

changecom(@&*$)

define(`rp', `x0')
define(`up', `x1')
define(`vp', `x2')
define(`n',  `x3')

ifdef(`OPERATION_add_n', `
  define(`ADDSUBC',	adcs)
  define(`CLRCY',	`cmn	xzr, xzr')
  define(`SETCY',	`cmp	$1, #1')
  define(`RETVAL',	`adc	x0, xzr, xzr')
  define(`func',	mpn_add_n)
  define(`func_nc',	mpn_add_nc)')
ifdef(`OPERATION_sub_n', `
  define(`ADDSUBC',	sbcs)
  define(`CLRCY',	`cmp	xzr, xzr')
  define(`SETCY',	`subs	$1, xzr, $1')
  define(`RETVAL',	`sbc	x0, xzr, xzr
			and	x0, x0, #1')
  define(`func',	mpn_sub_n)
  define(`func_nc',	mpn_sub_nc)')

MULFUNC_PROLOGUE(mpn_add_n mpn_add_nc mpn_sub_n mpn_sub_nc)

ASM_START()
PROLOGUE(func_nc)
	SETCY(	x4)
	b	L(ent)
EPILOGUE()
PROLOGUE(func)
	CLRCY
L(ent):	tbz	n, #0, L(b0)

	ldr	x4, [up],#8
	ldr	x6, [vp],#8
	sub	n, n, #1
	ADDSUBC	x8, x4, x6
	str	x8, [rp],#8
	cbz	n, L(rt)

L(b0):	ldp	x4, x5, [up],#16
	ldp	x6, x7, [vp],#16
	sub	n, n, #2
	ADDSUBC	x8, x4, x6
	ADDSUBC	x9, x5, x7
	cbz	n, L(end)

L(top):	ldp	x4, x5, [up],#16
	ldp	x6, x7, [vp],#16
	sub	n, n, #2
	stp	x8, x9, [rp],#16
	ADDSUBC	x8, x4, x6
	ADDSUBC	x9, x5, x7
	cbnz	n, L(top)

L(end):	stp	x8, x9, [rp]
L(rt):	RETVAL
	ret
EPILOGUE()
