/* PowerPC-32 gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 1999, 2000, 2001, 2002, 2003, 2004 Free Software
Foundation, Inc.

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


/* This file is supposed to be used for 604, 604e, 744x/745x/747x (G4+), i.e.,
   32-bit PowerPC processors with reasonably fast integer multiply insns.  The
   values below are chosen to be best for the latter processors, since 604 is
   largely irrelevant today.

   In mpn/powerpc32/750/gmp-mparam.h there are values for 75x (G3) and for
   7400/7410 (G4), both which have much slower multiply instructions.  */

#define MUL_KARATSUBA_THRESHOLD          14
#define MUL_TOOM3_THRESHOLD              73

#define SQR_BASECASE_THRESHOLD            5
#define SQR_KARATSUBA_THRESHOLD          28
#define SQR_TOOM3_THRESHOLD              86

#define DIV_SB_PREINV_THRESHOLD           0  /* always */
#define DIV_DC_THRESHOLD                 45
#define POWM_THRESHOLD                   89

#define HGCD_THRESHOLD                  145
#define GCD_ACCEL_THRESHOLD               3
#define GCD_SCHOENHAGE_THRESHOLD        738
#define GCDEXT_THRESHOLD                 16
#define JACOBI_BASE_METHOD                1

#define DIVREM_1_NORM_THRESHOLD           0  /* always */
#define DIVREM_1_UNNORM_THRESHOLD         0  /* always */
#define MOD_1_NORM_THRESHOLD              0  /* always */
#define MOD_1_UNNORM_THRESHOLD            0  /* always */
#define USE_PREINV_DIVREM_1               1
#define USE_PREINV_MOD_1                  1
#define DIVREM_2_THRESHOLD                0  /* always */
#define DIVEXACT_1_THRESHOLD              0  /* always */
#define MODEXACT_1_ODD_THRESHOLD          0  /* always */

#define GET_STR_DC_THRESHOLD             19
#define GET_STR_PRECOMPUTE_THRESHOLD     33
#define SET_STR_THRESHOLD              6589

#define MUL_FFT_TABLE  { 336, 800, 1408, 3584, 10240, 24576, 0 }
#define MUL_FFT_MODF_THRESHOLD          352
#define MUL_FFT_THRESHOLD              2816

#define SQR_FFT_TABLE  { 368, 928, 1664, 3584, 10240, 40960, 0 }
#define SQR_FFT_MODF_THRESHOLD          384
#define SQR_FFT_THRESHOLD              2816

