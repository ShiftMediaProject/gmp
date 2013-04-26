dnl  ARM mpn_and_n, et al.

dnl  Contributed to the GNU project by Richard Henderson.

dnl  Copyright 2013 Richard Henderson.

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

C            cycles/limb             cycles/limb
C          and andn ior iorn xor   nand nior xnor
C StrongARM	 ?			 ?
C XScale	 ?			 ?
C Cortex-A8	 ?			 ?
C Cortex-A9	 2.1			 2.6
C Cortex-A15	 0.78			 0.94

define(`rp', `r0')
define(`up', `r1')
define(`vp', `r2')
define(`n',  `r3')

define(`POSTOP')

ifdef(`OPERATION_and_n',`
  define(`func',    `mpn_and_n')
  define(`LOGOP',   `vand	$1, $2, $3')')
ifdef(`OPERATION_andn_n',`
  define(`func',    `mpn_andn_n')
  define(`LOGOP',   `vbic	$1, $2, $3')')
ifdef(`OPERATION_nand_n',`
  define(`func',    `mpn_nand_n')
  define(`POSTOP',  `vmvn	$1, $1')
  define(`LOGOP',   `vand	$1, $2, $3')')
ifdef(`OPERATION_ior_n',`
  define(`func',    `mpn_ior_n')
  define(`LOGOP',   `vorr	$1, $2, $3')')
ifdef(`OPERATION_iorn_n',`
  define(`func',    `mpn_iorn_n')
  define(`LOGOP',   `vorn	$1, $2, $3')')
ifdef(`OPERATION_nior_n',`
  define(`func',    `mpn_nior_n')
  define(`POSTOP',  `vmvn	$1, $1')
  define(`LOGOP',   `vorr	$1, $2, $3')')
ifdef(`OPERATION_xor_n',`
  define(`func',    `mpn_xor_n')
  define(`LOGOP',   `veor	$1, $2, $3')')
ifdef(`OPERATION_xnor_n',`
  define(`func',    `mpn_xnor_n')
  define(`POSTOP',  `vmvn	$1, $1')
  define(`LOGOP',   `veor	$1, $2, $3')')

MULFUNC_PROLOGUE(mpn_and_n mpn_andn_n mpn_nand_n mpn_ior_n mpn_iorn_n mpn_nior_n mpn_xor_n mpn_xnor_n)

ASM_START()
	.fpu	neon
PROLOGUE(func)
	cmp		n, #7
	ble		L(bc)

C Perform a few initial operation until rp is 128-bit aligned
	tst		rp, #4
	beq		L(al1)
	vld1.32		{d0[0]}, [up]!
	vld1.32		{d1[0]}, [vp]!
	sub		n, n, #1
	LOGOP(		d0, d0, d1)
	POSTOP(		d0, d0)
	vst1.32		{d0[0]}, [rp]!
L(al1):	tst		rp, #8
	beq		L(al2)
	vld1.32		{d0}, [up]!
	vld1.32		{d1}, [vp]!
	sub		n, n, #2
	LOGOP(		d0, d0, d1)
	POSTOP(		d0, d0)
	vst1.32		{d0}, [rp:64]!
L(al2):	vld1.32		{q2}, [up]!
	vld1.32		{q3}, [vp]!
	subs		n, n, #12
	blt		L(end)

	ALIGN(16)
L(top):	vld1.32		{q0}, [up]!
	LOGOP(		q2, q2, q3)
	vld1.32		{q1}, [vp]!
	POSTOP(		q2, q2)
	subs		n, n, #8
	vst1.32		{q2}, [rp:128]!
	vld1.32		{q2}, [up]!
	LOGOP(		q0, q0, q1)
	vld1.32		{q3}, [vp]!
	POSTOP(		q0, q0)
	vst1.32		{q0}, [rp:128]!
	bge	L(top)

L(end):	LOGOP(		q2, q2, q3)
	POSTOP(		q2, q2)
	vst1.32		{q2}, [rp:128]!

C Handle last 0-7 limbs.  Note that rp is aligned after loop, but not when we
C arrive here via L(bc)
L(bc):	tst		n, #4
	beq		L(tl1)
	vld1.32		{q0}, [up]!
	vld1.32		{q1}, [vp]!
	LOGOP(		q0, q0, q1)
	POSTOP(		q0, q0)
	vst1.32		{q0}, [rp]!
L(tl1):	tst		n, #2
	beq		L(tl2)
	vld1.32		{d0}, [up]!
	vld1.32		{d1}, [vp]!
	LOGOP(		d0, d0, d1)
	POSTOP(		d0, d0)
	vst1.32		{d0}, [rp]!
L(tl2):	tst		n, #1
	beq		L(tl3)
	vld1.32		{d0[0]}, [up]!
	vld1.32		{d1[0]}, [vp]!
	LOGOP(		d0, d0, d1)
	POSTOP(		d0, d0)
	vst1.32		{d0[0]}, [rp]!
L(tl3):	bx		lr
EPILOGUE()
