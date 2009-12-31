/* PowerPC970 gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 2008, 2009 Free Software Foundation, Inc.

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

#define GMP_LIMB_BITS 64
#define BYTES_PER_MP_LIMB 8


#define MOD_1_NORM_THRESHOLD                 0  /* always */
#define MOD_1_UNNORM_THRESHOLD               0  /* always */
#define MOD_1N_TO_MOD_1_1_THRESHOLD          7
#define MOD_1U_TO_MOD_1_1_THRESHOLD          6
#define MOD_1_1_TO_MOD_1_2_THRESHOLD         9
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        26
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD     14
#define USE_PREINV_DIVREM_1                  0
#define DIVEXACT_1_THRESHOLD                 0  /* always (native) */
#define BMOD_1_TO_MOD_1_THRESHOLD           43

#define MUL_TOOM22_THRESHOLD                14
#define MUL_TOOM33_THRESHOLD                54
#define MUL_TOOM44_THRESHOLD               154
#define MUL_TOOM6H_THRESHOLD               206
#define MUL_TOOM8H_THRESHOLD               309

#define MUL_TOOM32_TO_TOOM43_THRESHOLD      89
#define MUL_TOOM32_TO_TOOM53_THRESHOLD      99
#define MUL_TOOM42_TO_TOOM53_THRESHOLD      97
#define MUL_TOOM42_TO_TOOM63_THRESHOLD      97

#define SQR_BASECASE_THRESHOLD               5
#define SQR_TOOM2_THRESHOLD                 36
#define SQR_TOOM3_THRESHOLD                 61
#define SQR_TOOM4_THRESHOLD                154
#define SQR_TOOM6_THRESHOLD                206
#define SQR_TOOM8_THRESHOLD                309

#define MULMOD_BNM1_THRESHOLD               12
#define SQRMOD_BNM1_THRESHOLD               14

#define MUL_FFT_TABLE  { 368, 544, 1856, 2816, 7168, 20480, 81920, 327680, 0 }
#define MUL_FFT_MODF_THRESHOLD             384
#define MUL_FFT_THRESHOLD                18432

#define SQR_FFT_TABLE  { 304, 672, 1600, 2816, 7168, 20480, 81920, 196608, 0 }
#define SQR_FFT_MODF_THRESHOLD             320
#define SQR_FFT_THRESHOLD                 7680

#define MULLO_BASECASE_THRESHOLD             5
#define MULLO_DC_THRESHOLD                  34
#define MULLO_MUL_N_THRESHOLD            26552

#define DC_DIV_QR_THRESHOLD                 30
#define DC_DIVAPPR_Q_THRESHOLD             103
#define DC_BDIV_QR_THRESHOLD                48
#define DC_BDIV_Q_THRESHOLD                120

#define INV_MULMOD_BNM1_THRESHOLD           92
#define INV_NEWTON_THRESHOLD               147
#define INV_APPR_THRESHOLD                 122

#define BINV_NEWTON_THRESHOLD              206
#define REDC_1_TO_REDC_N_THRESHOLD          56

#define MU_DIV_QR_THRESHOLD               2541
#define MU_DIVAPPR_Q_THRESHOLD            2089
#define MUPI_DIV_QR_THRESHOLD              165
#define MU_BDIV_QR_THRESHOLD              2259
#define MU_BDIV_Q_THRESHOLD               2089

#define MATRIX22_STRASSEN_THRESHOLD         17
#define HGCD_THRESHOLD                      86
#define GCD_DC_THRESHOLD                   233
#define GCDEXT_DC_THRESHOLD                221
#define JACOBI_BASE_METHOD                   1

#define GET_STR_DC_THRESHOLD                11
#define GET_STR_PRECOMPUTE_THRESHOLD        24
#define SET_STR_DC_THRESHOLD               532
#define SET_STR_PRECOMPUTE_THRESHOLD      1790
