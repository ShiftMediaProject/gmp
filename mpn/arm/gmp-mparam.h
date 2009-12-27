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

#define GMP_LIMB_BITS 32
#define BYTES_PER_MP_LIMB 4

/* 593MHz ARM (gcc50.fsffrance.org) */

#define MUL_TOOM22_THRESHOLD                34
#define MUL_TOOM33_THRESHOLD               121
#define MUL_TOOM44_THRESHOLD               191
#define MUL_TOOM6H_THRESHOLD               366
#define MUL_TOOM8H_THRESHOLD               547

#define MUL_TOOM32_TO_TOOM43_THRESHOLD     129
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     199
#define MUL_TOOM42_TO_TOOM53_THRESHOLD     117
#define MUL_TOOM42_TO_TOOM63_THRESHOLD     134

#define SQR_BASECASE_THRESHOLD              13
#define SQR_TOOM2_THRESHOLD                 78
#define SQR_TOOM3_THRESHOLD                141
#define SQR_TOOM4_THRESHOLD                212
#define SQR_TOOM6_THRESHOLD                330
#define SQR_TOOM8_THRESHOLD                422

#define MULMOD_BNM1_THRESHOLD               19
#define SQRMOD_BNM1_THRESHOLD               25

#define MUL_FFT_TABLE  { 400, 928, 1664, 4608, 14336, 40960, 163840, 655360, 0 }
#define MUL_FFT_MODF_THRESHOLD             416
#define MUL_FFT_THRESHOLD                13312

#define SQR_FFT_TABLE  { 432, 928, 1664, 4608, 10240, 40960, 163840, 655360, 0 }
#define SQR_FFT_MODF_THRESHOLD             392
#define SQR_FFT_THRESHOLD                11264

#define MULLO_BASECASE_THRESHOLD             0  /* always */
#define MULLO_DC_THRESHOLD                 120
#define MULLO_MUL_N_THRESHOLD            19371

#define DC_DIV_QR_THRESHOLD                132
#define DC_DIVAPPR_Q_THRESHOLD             446
#define DC_BDIV_QR_THRESHOLD               127
#define DC_BDIV_Q_THRESHOLD                298

#define INV_MULMOD_BNM1_THRESHOLD           66
#define INV_NEWTON_THRESHOLD               462
#define INV_APPR_THRESHOLD                 454

#define BINV_NEWTON_THRESHOLD             1055
#define REDC_1_TO_REDC_N_THRESHOLD         116

#define MATRIX22_STRASSEN_THRESHOLD         17
#define HGCD_THRESHOLD                     105
#define GCD_DC_THRESHOLD                   947
#define GCDEXT_DC_THRESHOLD                403
#define JACOBI_BASE_METHOD                   2

#define DIVREM_1_NORM_THRESHOLD              0  /* preinv always */
#define DIVREM_1_UNNORM_THRESHOLD            0  /* always */
#define MOD_1_NORM_THRESHOLD                 0  /* always */
#define MOD_1_UNNORM_THRESHOLD               0  /* always */
#define MOD_1N_TO_MOD_1_1_THRESHOLD         17
#define MOD_1U_TO_MOD_1_1_THRESHOLD         10
#define MOD_1_1_TO_MOD_1_2_THRESHOLD     MP_SIZE_T_MAX  /* never */
#define MOD_1_2_TO_MOD_1_4_THRESHOLD     MP_SIZE_T_MAX  /* never */
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD     27
#define USE_PREINV_DIVREM_1                  1  /* preinv always */
#define DIVREM_2_THRESHOLD                   0  /* preinv always */
#define DIVEXACT_1_THRESHOLD                 0  /* always */
#define BMOD_1_TO_MOD_1_THRESHOLD           44

#define GET_STR_DC_THRESHOLD                14
#define GET_STR_PRECOMPUTE_THRESHOLD        29
#define SET_STR_DC_THRESHOLD               318
#define SET_STR_PRECOMPUTE_THRESHOLD      1043
