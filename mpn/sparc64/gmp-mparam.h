/* Sparc64 gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 1999, 2000, 2001, 2002, 2004, 2006, 2008, 2009
Free Software Foundation, Inc.

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

/* 500 MHz ultrasparc2 running GNU/Linux */

#define MUL_TOOM22_THRESHOLD               38
#define MUL_TOOM33_THRESHOLD              133
#define MUL_TOOM44_THRESHOLD              187

#define MUL_TOOM32_TO_TOOM43_THRESHOLD    233
#define MUL_TOOM32_TO_TOOM53_THRESHOLD    222
#define MUL_TOOM42_TO_TOOM53_THRESHOLD    111
#define MUL_TOOM42_TO_TOOM63_THRESHOLD    115

#define SQR_BASECASE_THRESHOLD              8
#define SQR_TOOM2_THRESHOLD                64
#define SQR_TOOM3_THRESHOLD               228
#define SQR_TOOM4_THRESHOLD               324

#define MULMOD_BNM1_THRESHOLD              15
#define SQRMOD_BNM1_THRESHOLD              17

#define MUL_FFT_TABLE  { 272, 608, 1344, 2304, 7168, 20480, 81920, 196608, 786432, 0 }
#define MUL_FFT_MODF_THRESHOLD            184
#define MUL_FFT_THRESHOLD                1408

#define SQR_FFT_TABLE  { 304, 672, 1600, 2304, 7168, 20480, 81920, 196608, 786432, 0 }
#define SQR_FFT_MODF_THRESHOLD            216
#define SQR_FFT_THRESHOLD                1408

#define MULLO_BASECASE_THRESHOLD            0  /* always */
#define MULLO_DC_THRESHOLD                 37
#define MULLO_MUL_N_THRESHOLD            2801

#define DC_DIV_QR_THRESHOLD                25
#define DC_DIVAPPR_Q_THRESHOLD             88
#define DC_BDIV_QR_THRESHOLD               45
#define DC_BDIV_Q_THRESHOLD               133

#define INV_MULMOD_BNM1_THRESHOLD          74
#define INV_NEWTON_THRESHOLD               11
#define INV_APPR_THRESHOLD                  7

#define BINV_NEWTON_THRESHOLD             191
#define REDC_1_TO_REDC_2_THRESHOLD         10
#define REDC_2_TO_REDC_N_THRESHOLD        124

#define MATRIX22_STRASSEN_THRESHOLD         8
#define HGCD_THRESHOLD                     55
#define GCD_DC_THRESHOLD                  283
#define GCDEXT_DC_THRESHOLD               183
#define JACOBI_BASE_METHOD                  2

#define DIVREM_1_NORM_THRESHOLD             3
#define DIVREM_1_UNNORM_THRESHOLD           3
#define MOD_1_NORM_THRESHOLD                3
#define MOD_1_UNNORM_THRESHOLD              4
#define MOD_1_1_THRESHOLD                   3
#define MOD_1_2_THRESHOLD               MP_SIZE_T_MAX  /* never */
#define MOD_1_4_THRESHOLD               MP_SIZE_T_MAX  /* never */
#define USE_PREINV_DIVREM_1                 1
#define USE_PREINV_MOD_1                    1
#define DIVREM_2_THRESHOLD                  6
#define DIVEXACT_1_THRESHOLD                0  /* always */
#define MODEXACT_1_ODD_THRESHOLD            0  /* always */

#define GET_STR_DC_THRESHOLD               13
#define GET_STR_PRECOMPUTE_THRESHOLD       16
#define SET_STR_DC_THRESHOLD              547
#define SET_STR_PRECOMPUTE_THRESHOLD     1639
