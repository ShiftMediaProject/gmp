/* Core 2 gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007,
2008, 2009 Free Software Foundation, Inc.

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

/* 2133 MHz Core 2 (65nm) */

#define MOD_1_NORM_THRESHOLD                 0  /* always */
#define MOD_1_UNNORM_THRESHOLD               0  /* always */
#define MOD_1N_TO_MOD_1_1_THRESHOLD          5
#define MOD_1U_TO_MOD_1_1_THRESHOLD          4
#define MOD_1_1_TO_MOD_1_2_THRESHOLD         0
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        14
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD     10
#define USE_PREINV_DIVREM_1                  1  /* native */
#define DIVEXACT_1_THRESHOLD                 0  /* always (native) */
#define BMOD_1_TO_MOD_1_THRESHOLD           26

#define MUL_TOOM22_THRESHOLD                23
#define MUL_TOOM33_THRESHOLD                65
#define MUL_TOOM44_THRESHOLD               183
#define MUL_TOOM6H_THRESHOLD               254
#define MUL_TOOM8H_THRESHOLD               381

#define MUL_TOOM32_TO_TOOM43_THRESHOLD      69
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     122
#define MUL_TOOM42_TO_TOOM53_THRESHOLD      73
#define MUL_TOOM42_TO_TOOM63_THRESHOLD      74

#define SQR_BASECASE_THRESHOLD               0  /* always (native) */
#define SQR_TOOM2_THRESHOLD                 26
#define SQR_TOOM3_THRESHOLD                 97
#define SQR_TOOM4_THRESHOLD                148
#define SQR_TOOM6_THRESHOLD                270
#define SQR_TOOM8_THRESHOLD                296

#define MULMOD_BNM1_THRESHOLD               12
#define SQRMOD_BNM1_THRESHOLD               14

#define MUL_FFT_TABLE  { 336, 800, 1600, 3328, 7168, 20480, 81920, 196608, 0 }
#define MUL_FFT_MODF_THRESHOLD             400
#define MUL_FFT_THRESHOLD                 4736

#define SQR_FFT_TABLE  { 336, 672, 1728, 2816, 7168, 20480, 81920, 327680, 786432, 0 }
#define SQR_FFT_MODF_THRESHOLD             352
#define SQR_FFT_THRESHOLD                 3712

#define MULLO_BASECASE_THRESHOLD             3
#define MULLO_DC_THRESHOLD                  20
#define MULLO_MUL_N_THRESHOLD             8648

#define DC_DIV_QR_THRESHOLD                 52
#define DC_DIVAPPR_Q_THRESHOLD             167
#define DC_BDIV_QR_THRESHOLD                57
#define DC_BDIV_Q_THRESHOLD                156

#define INV_MULMOD_BNM1_THRESHOLD          126
#define INV_NEWTON_THRESHOLD               198
#define INV_APPR_THRESHOLD                 178

#define BINV_NEWTON_THRESHOLD              260
#define REDC_1_TO_REDC_2_THRESHOLD          10
#define REDC_2_TO_REDC_N_THRESHOLD          63

#define MU_DIV_QR_THRESHOLD               1334
#define MU_DIVAPPR_Q_THRESHOLD            1334
#define MUPI_DIV_QR_THRESHOLD              126
#define MU_BDIV_QR_THRESHOLD              1037
#define MU_BDIV_Q_THRESHOLD               1334

#define MATRIX22_STRASSEN_THRESHOLD         18
#define HGCD_THRESHOLD                     138
#define GCD_DC_THRESHOLD                   465
#define GCDEXT_DC_THRESHOLD                365
#define JACOBI_BASE_METHOD                   1

#define GET_STR_DC_THRESHOLD                 9
#define GET_STR_PRECOMPUTE_THRESHOLD        20
#define SET_STR_DC_THRESHOLD               552
#define SET_STR_PRECOMPUTE_THRESHOLD      1790
