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
dnl  along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.

include(`../config.m4')

C	     cycles/limb
C Cortex-A53	 ?
C Cortex-A57	 ?

define(`rp', `r0')
define(`up', `r1')
define(`vp', `r2')
define(`n',  `r3')

ifdef(`OPERATION_add_n', `
  define(`ADDSUB',	adds)
  define(`ADDSUBC',	adcs)
  define(`CLRCY',	`cmn	xzr, xzr')
  define(`SETCY',	`cmp	$1, #1')
  define(`RETVAL',	`adc	x0, xzr, xzr')
  define(`func',	mpn_add_n)
  define(`func_nc',	mpn_add_nc)')
ifdef(`OPERATION_sub_n', `
  define(`ADDSUB',	subs)
  define(`ADDSUBC',	sbcs)
  define(`CLRCY',	`cmp	xzr, xzr')
  define(`SETCY',	`rsbs	$1, $1, xzr')
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
L(ent):	lsl	x3, x3, #3
	add	x0, x0, x3
	add	x1, x1, x3
	add	x2, x2, x3
	neg	x3, x3

L(top):	ldr	x6, [x1,x3]
	ldr	x5, [x2,x3]
	adcs	x7, x6, x5
	str	x7, [x0,x3]
	add	x3, x3, #8
	cbnz	x3, L(top)

	adc	x0, xzr, xzr
	ret
EPILOGUE()
