/* AMD K7 gmp-mparam.h -- Compiler/machine parameter header file.

Copyright (C) 1991, 1993, 1994 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#define BITS_PER_MP_LIMB 32
#define BYTES_PER_MP_LIMB 4
#define BITS_PER_LONGINT 32
#define BITS_PER_INT 32
#define BITS_PER_SHORTINT 16
#define BITS_PER_CHAR 8

#ifndef KARATSUBA_MUL_THRESHOLD
#define KARATSUBA_MUL_THRESHOLD 28
#endif

/* mpn/generic/sqr_basecase.c, gcc -O2, and no K7 specific lshift yet */
#ifndef KARATSUBA_SQR_THRESHOLD
#define KARATSUBA_SQR_THRESHOLD 88
#endif

#define UMUL_TIME   4   /* cycles back-to-back */
#define UDIV_TIME   39  /* cycles back-to-back */
