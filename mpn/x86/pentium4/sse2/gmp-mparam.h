/* Intel Pentium-4 gmp-mparam.h -- Compiler/machine parameter header file.

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


/* 1700 MHz Pentium 4 (socket 423), gcc 2.95.3 */

#define KARATSUBA_MUL_THRESHOLD       31
#define TOOM3_MUL_THRESHOLD          264

#define BASECASE_SQR_THRESHOLD         0
#define KARATSUBA_SQR_THRESHOLD       68
#define TOOM3_SQR_THRESHOLD          422

#define SB_PREINV_THRESHOLD            0
#define DC_THRESHOLD                  42
#define POWM_THRESHOLD               110

#define GCD_ACCEL_THRESHOLD            6
#define GCDEXT_THRESHOLD              66

#define USE_PREINV_MOD_1               0
#define DIVREM_2_THRESHOLD         MP_SIZE_T_MAX
#define DIVEXACT_1_THRESHOLD           0
#define MODEXACT_1_ODD_THRESHOLD       0

#define FFT_MUL_TABLE  { 784, 1888, 3712, 9728, 22528, 57344, 0 }
#define FFT_MODF_MUL_THRESHOLD       800
#define FFT_MUL_THRESHOLD           7936

#define FFT_SQR_TABLE  { 848, 2272, 3712, 9728, 22528, 57344, 0 }
#define FFT_MODF_SQR_THRESHOLD       792
#define FFT_SQR_THRESHOLD           7424
