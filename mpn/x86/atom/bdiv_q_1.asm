dnl  Intel Atom mpn_bdiv_q_1, mpn_pi1_bdiv_q_1 -- schoolbook Hensel
dnl  division by 1-limb divisor, returning quotient only.

dnl  Copyright 2011 Free Software Foundation, Inc.

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

MULFUNC_PROLOGUE(mpn_bdiv_q_1 mpn_pi1_bdiv_q_1)
include_mpn(`x86/pentium/bdiv_q_1.asm')
