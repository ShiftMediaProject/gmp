/* gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 1996 Free Software Foundation, Inc.

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

#define BITS_PER_MP_LIMB 64
#define BYTES_PER_MP_LIMB 8
#define BITS_PER_LONGINT 64
#define BITS_PER_INT 64
#define BITS_PER_SHORTINT 64
#define BITS_PER_CHAR 8

#define UMUL_TIME	 519
#define UDIV_TIME	2360

#define KARATSUBA_SQR_THRESHOLD   1000 /* failed to determine */
#define TOOM3_MUL_THRESHOLD       2000 /* failed to determine */
#define KARATSUBA_SQR_THRESHOLD    114 /* small due to sqr_basecase slowness */
#define TOOM3_SQR_THRESHOLD        228 /* small due to sqr_basecase slowness */
#define DC_THRESHOLD              5000 /* failed to determine */
#define POWM_THRESHOLD              55
#define FIB_THRESHOLD              225
#define GCD_ACCEL_THRESHOLD          3
#define GCDEXT_THRESHOLD            86
