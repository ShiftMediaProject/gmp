/* gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 1996, 2000 Free Software Foundation, Inc.

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

#if 0
#define UMUL_TIME	  50	/* re-measure this! */
#define UDIV_TIME	2360
#endif

#define KARATSUBA_MUL_THRESHOLD       90
#define TOOM3_MUL_THRESHOLD          167

#define KARATSUBA_SQR_THRESHOLD      199
#define TOOM3_SQR_THRESHOLD          399

#define DC_THRESHOLD                1000 /* failed to determine */
#define POWM_THRESHOLD               113

#define FIB_THRESHOLD                202

#define GCD_ACCEL_THRESHOLD            3
#define GCDEXT_THRESHOLD              27

#define DIVREM_1_NORM_THRESHOLD    0  /* (preinv always) */
#define DIVREM_1_UNNORM_THRESHOLD  0
#define MOD_1_NORM_THRESHOLD       0
#define MOD_1_UNNORM_THRESHOLD     0
#define USE_PREINV_MOD_1           1   /* (preinv always) */

#define FFT_MUL_TABLE  { 496, 992, 1984, 3840, 9216, 20480, 49152, 0 }
#define FFT_MODF_MUL_THRESHOLD       232
#define FFT_MUL_THRESHOLD           1056

#define FFT_SQR_TABLE  { 1104, 2336, 4928, 9984, 21504, 53248, 0 }
#define FFT_MODF_SQR_THRESHOLD       600
#define FFT_SQR_THRESHOLD           3616
