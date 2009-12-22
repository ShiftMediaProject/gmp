/* gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 1999, 2000, 2001, 2002, 2003, 2009 Free Software
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

#define GMP_LIMB_BITS 64
#define BYTES_PER_MP_LIMB 8

/* POWER5 (friggms.hpc.ntnu.no) */

#define MUL_TOOM22_THRESHOLD               14
#define MUL_TOOM33_THRESHOLD               53
#define MUL_TOOM44_THRESHOLD              145

#define MUL_TOOM32_TO_TOOM43_THRESHOLD     81
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     91
#define MUL_TOOM42_TO_TOOM53_THRESHOLD     97
#define MUL_TOOM42_TO_TOOM63_THRESHOLD     88

#define SQR_BASECASE_THRESHOLD             11
#define SQR_TOOM2_THRESHOLD                38
#define SQR_TOOM3_THRESHOLD                63
#define SQR_TOOM4_THRESHOLD               112

#define MULMOD_BNM1_THRESHOLD              14
#define SQRMOD_BNM1_THRESHOLD              12

#define MUL_FFT_TABLE  { 336, 736, 1856, 2816, 7168, 20480, 49152, 196608, 786432, 0 }
#define MUL_FFT_MODF_THRESHOLD            328
#define MUL_FFT_THRESHOLD                2432

#define SQR_FFT_TABLE  { 304, 608, 1344, 2816, 7168, 20480, 49152, 196608, 786432, 0 }
#define SQR_FFT_MODF_THRESHOLD            320
#define SQR_FFT_THRESHOLD                1920

#define MULLO_BASECASE_THRESHOLD            6
#define MULLO_DC_THRESHOLD                 30
#define MULLO_MUL_N_THRESHOLD            3478

#define DC_DIV_QR_THRESHOLD                31
#define DC_DIVAPPR_Q_THRESHOLD            117
#define DC_BDIV_QR_THRESHOLD               49
#define DC_BDIV_Q_THRESHOLD               120

#define INV_MULMOD_BNM1_THRESHOLD         126
#define INV_NEWTON_THRESHOLD              121
#define INV_APPR_THRESHOLD                 11

#define BINV_NEWTON_THRESHOLD             214
#define REDC_1_TO_REDC_N_THRESHOLD         51

#define MATRIX22_STRASSEN_THRESHOLD        17
#define HGCD_THRESHOLD                     80
#define GCD_DC_THRESHOLD                  315
#define GCDEXT_DC_THRESHOLD               221
#define JACOBI_BASE_METHOD                  1

#define MOD_1_NORM_THRESHOLD                0  /* always */
#define MOD_1_UNNORM_THRESHOLD              0  /* always */
#define MOD_1_1_THRESHOLD                  31
#define MOD_1_2_THRESHOLD                  32
#define MOD_1_4_THRESHOLD                  33
#define USE_PREINV_DIVREM_1                 0
#define USE_PREINV_MOD_1                    0
#define DIVEXACT_1_THRESHOLD                0  /* always (native) */
#define MODEXACT_1_ODD_THRESHOLD            0  /* always */

#define GET_STR_DC_THRESHOLD               13
#define GET_STR_PRECOMPUTE_THRESHOLD       23
#define SET_STR_DC_THRESHOLD              552
#define SET_STR_PRECOMPUTE_THRESHOLD     1655
