dnl  x86 mpn_udiv_qrnnd -- 2 by 1 limb division

dnl  Copyright 1999, 2000, 2002 Free Software Foundation, Inc.

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


C mp_limb_t mpn_udiv_qrnnd (mp_limb_t *remptr, mp_limb_t high, mp_limb_t low,
C                           mp_limb_t divisor);

defframe(PARAM_DIVISOR, 16)
defframe(PARAM_LOW,     12)
defframe(PARAM_HIGH,    8)
defframe(PARAM_REMPTR,  4)

	TEXT
	ALIGN(8)
PROLOGUE(mpn_udiv_qrnnd)
deflit(`FRAME',0)
	movl	PARAM_LOW, %eax
	movl	PARAM_HIGH, %edx
	divl	PARAM_DIVISOR
	movl	PARAM_REMPTR, %ecx
	movl	%edx, (%ecx)
	ret
EPILOGUE()
