/* PowerPC-32 gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 2002, 2004 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.  */

#define BITS_PER_MP_LIMB 32
#define BYTES_PER_MP_LIMB 4


/* This file is used for 75x (G3) and for 7400/7410 (G4), both which have
   much slow multiply instructions.  */

#define MUL_KARATSUBA_THRESHOLD          10
#define MUL_TOOM3_THRESHOLD              66

#define SQR_BASECASE_THRESHOLD            4
#define SQR_KARATSUBA_THRESHOLD          20
#define SQR_TOOM3_THRESHOLD              62

#define DIV_SB_PREINV_THRESHOLD           0  /* always */
#define DIV_DC_THRESHOLD                 35
#define POWM_THRESHOLD                   48

#define HGCD_THRESHOLD                   93
#define GCD_ACCEL_THRESHOLD               3
#define GCD_DC_THRESHOLD                676
#define JACOBI_BASE_METHOD                1

#define DIVREM_1_NORM_THRESHOLD           0  /* always */
#define DIVREM_1_UNNORM_THRESHOLD         3  /* always */
#define MOD_1_NORM_THRESHOLD              3  /* always */
#define MOD_1_UNNORM_THRESHOLD            3  /* always */
#define USE_PREINV_DIVREM_1               1
#define USE_PREINV_MOD_1                  1
#define DIVREM_2_THRESHOLD                0  /* always */
#define DIVEXACT_1_THRESHOLD              0  /* always */
#define MODEXACT_1_ODD_THRESHOLD          0  /* always */

#define GET_STR_DC_THRESHOLD             11
#define GET_STR_PRECOMPUTE_THRESHOLD     23
#define SET_STR_THRESHOLD              3134

#define MUL_FFT_TABLE  { 304, 672, 1152, 2560, 10240, 24576, 0 }
#define MUL_FFT_MODF_THRESHOLD          264
#define MUL_FFT_THRESHOLD              1792

#define SQR_FFT_TABLE  { 336, 736, 1408, 3584, 10240, 24576, 0 }
#define SQR_FFT_MODF_THRESHOLD          280
#define SQR_FFT_THRESHOLD              2304
