/* SPARC v9 32-bit gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 1999, 2000, 2001, 2002, 2004, 2009 Free Software
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

#define GMP_LIMB_BITS 32
#define BYTES_PER_MP_LIMB 4

#define MUL_TOOM22_THRESHOLD                29
#define MUL_TOOM33_THRESHOLD                90
#define MUL_TOOM44_THRESHOLD               149
#define MUL_TOOM6H_THRESHOLD               216
#define MUL_TOOM8H_THRESHOLD               462

#define MUL_TOOM32_TO_TOOM43_THRESHOLD     102
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     162
#define MUL_TOOM42_TO_TOOM53_THRESHOLD      98
#define MUL_TOOM42_TO_TOOM63_THRESHOLD     102

#define SQR_BASECASE_THRESHOLD               8
#define SQR_TOOM2_THRESHOLD                 61
#define SQR_TOOM3_THRESHOLD                102
#define SQR_TOOM4_THRESHOLD                224
#define SQR_TOOM6_THRESHOLD                236
#define SQR_TOOM8_THRESHOLD                582

#define MULMOD_BNM1_THRESHOLD               14
#define SQRMOD_BNM1_THRESHOLD               18

#define MUL_FFT_TABLE  { 272, 736, 1152, 3584, 10240, 24576, 98304, 393216, 0 }
#define MUL_FFT_MODF_THRESHOLD             288
#define MUL_FFT_THRESHOLD                 7168

#define SQR_FFT_TABLE  { 304, 736, 1408, 3584, 6144, 24576, 98304, 655360, 0 }
#define SQR_FFT_MODF_THRESHOLD             232
#define SQR_FFT_THRESHOLD                 2304

#define MULLO_BASECASE_THRESHOLD             0  /* always */
#define MULLO_DC_THRESHOLD                 103
#define MULLO_MUL_N_THRESHOLD             7248

#define DC_DIV_QR_THRESHOLD                120
#define DC_DIVAPPR_Q_THRESHOLD             410
#define DC_BDIV_QR_THRESHOLD               120
#define DC_BDIV_Q_THRESHOLD                270

#define INV_MULMOD_BNM1_THRESHOLD           58
#define INV_NEWTON_THRESHOLD               363
#define INV_APPR_THRESHOLD                 363

#define BINV_NEWTON_THRESHOLD              396
#define REDC_1_TO_REDC_N_THRESHOLD          82

#define MATRIX22_STRASSEN_THRESHOLD         12
#define HGCD_THRESHOLD                     145
#define GCD_DC_THRESHOLD                   634
#define GCDEXT_DC_THRESHOLD                419
#define JACOBI_BASE_METHOD                   2

#define DIVREM_1_NORM_THRESHOLD              5
#define DIVREM_1_UNNORM_THRESHOLD            7
#define MOD_1_NORM_THRESHOLD                 4
#define MOD_1_UNNORM_THRESHOLD               5
#define MOD_1N_TO_MOD_1_1_THRESHOLD      MP_SIZE_T_MAX  /* never */
#define MOD_1U_TO_MOD_1_1_THRESHOLD      MP_SIZE_T_MAX  /* never */
#define MOD_1_1_TO_MOD_1_2_THRESHOLD     MP_SIZE_T_MAX  /* never */
#define MOD_1_2_TO_MOD_1_4_THRESHOLD     MP_SIZE_T_MAX  /* never */
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD  MP_SIZE_T_MAX  /* never */
#define USE_PREINV_DIVREM_1                  1
#define DIVREM_2_THRESHOLD                   0  /* always */
#define DIVEXACT_1_THRESHOLD                 0  /* always */
#define BMOD_1_TO_MOD_1_THRESHOLD        MP_SIZE_T_MAX  /* never */

#define GET_STR_DC_THRESHOLD                10
#define GET_STR_PRECOMPUTE_THRESHOLD        17
#define SET_STR_DC_THRESHOLD               592
#define SET_STR_PRECOMPUTE_THRESHOLD      1617
