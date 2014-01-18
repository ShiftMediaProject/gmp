dnl  Intel Pentium-II mpn_popcount, mpn_hamdist -- population count and
dnl  hamming distance.

dnl  Copyright 2000, 2002 Free Software Foundation, Inc.

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


C P6MMX: popcount 11 cycles/limb (approx), hamdist 11.5 cycles/limb (approx)


MULFUNC_PROLOGUE(mpn_popcount mpn_hamdist)
include_mpn(`x86/k6/mmx/popham.asm')
