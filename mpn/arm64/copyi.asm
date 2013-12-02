dnl  ARM64 mpn_copyi.

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
define(`n',  `x2')

ASM_START()
PROLOGUE(mpn_copyi)
	cmp	n, #3
	b.le	L(bc)

C Copy until rp is 128-bit aligned
	tbz	rp, #3, L(al2)
	ld1	{v22.1d}, [up], #8
	sub	n, n, #1
	st1	{v22.1d}, [rp], #8

L(al2):	ld1	{v26.2d}, [up], #16
	subs	n, n, #6
	b.lt	L(end)

	ALIGN(16)
L(top):	ld1	{v22.2d}, [up], #16
	st1	{v26.2d}, [rp], #16
	ld1	{v26.2d}, [up], #16
	st1	{v22.2d}, [rp], #16
	subs	n, n, #4
	b.ge	L(top)

L(end):	st1	{v26.2d}, [rp], #16

C Copy last 0-3 limbs.  Note that rp is aligned after loop, but not when we
C arrive here via L(bc)
L(bc):	tbz	n, #1, L(tl1)
	ld1	{v22.2d}, [up], #16
	st1	{v22.2d}, [rp], #16
L(tl1):	tbz	n, #0, L(tl2)
	ld1	{v22.1d}, [up]
	st1	{v22.1d}, [rp]
L(tl2):	ret
EPILOGUE()
