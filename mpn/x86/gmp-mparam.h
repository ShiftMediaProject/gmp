/* Generic x86 gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 2000, 2001 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#define BITS_PER_MP_LIMB 32
#define BYTES_PER_MP_LIMB 4


/* close-ish for p5 and up, though 386/486 are much slower */
#ifndef UMUL_TIME
#define UMUL_TIME   10   /* cycles */
#endif

/* about right for most x86s */
#ifndef UDIV_TIME
#define UDIV_TIME   40  /* cycles */
#endif

/* udiv_qrnnd_preinv is a bit complicated and doesn't really suit the x86s
   without careful assembler coding, so mark it as slower then plain
   division.  */
#ifndef UDIV_NORM_PREINV_TIME
#define UDIV_NORM_PREINV_TIME   50 /* cycles */
#endif

/* Generic x86 mpn_divexact_1 is faster than generic x86 mpn_divrem_1 on all
   of p5, p6, k6 and k7, so use it always.  It's probably slower on 386 and
   486, but that's too bad.  */
#define DIVEXACT_1_THRESHOLD  0
