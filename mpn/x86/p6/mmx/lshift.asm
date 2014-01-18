dnl  Intel Pentium-II mpn_lshift -- mpn left shift.

dnl  Copyright 2001 Free Software Foundation, Inc.

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


dnl  The P55 code runs well on P-II/III, but could stand some minor tweaks
dnl  at some stage probably.

include(`../config.m4')

MULFUNC_PROLOGUE(mpn_lshift)
include_mpn(`x86/pentium/mmx/lshift.asm')
