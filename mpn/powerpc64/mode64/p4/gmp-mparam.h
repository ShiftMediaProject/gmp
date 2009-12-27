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


#define MUL_TOOM22_THRESHOLD                14
#define MUL_TOOM33_THRESHOLD                54
#define MUL_TOOM44_THRESHOLD               136
#define MUL_TOOM6H_THRESHOLD               204
#define MUL_TOOM8H_THRESHOLD               309

#define MUL_TOOM32_TO_TOOM43_THRESHOLD      89
#define MUL_TOOM32_TO_TOOM53_THRESHOLD      99
#define MUL_TOOM42_TO_TOOM53_THRESHOLD      94
#define MUL_TOOM42_TO_TOOM63_THRESHOLD      98

#define SQR_BASECASE_THRESHOLD               5
#define SQR_TOOM2_THRESHOLD                 32
#define SQR_TOOM3_THRESHOLD                 62
#define SQR_TOOM4_THRESHOLD                136
#define SQR_TOOM6_THRESHOLD                214
#define SQR_TOOM8_THRESHOLD                309

#define MULMOD_BNM1_THRESHOLD               12
#define SQRMOD_BNM1_THRESHOLD                9

#define MUL_FFT_TABLE  { 336, 800, 1856, 2816, 7168, 20480, 81920, 327680, 0 }
#define MUL_FFT_MODF_THRESHOLD             328
#define MUL_FFT_THRESHOLD                18432

#define SQR_FFT_TABLE  { 272, 544, 1600, 2816, 7168, 20480, 81920, 327680, 786432, 0 }
#define SQR_FFT_MODF_THRESHOLD             288
#define SQR_FFT_THRESHOLD                 6656

#define MULLO_BASECASE_THRESHOLD             5
#define MULLO_DC_THRESHOLD                  34
#define MULLO_MUL_N_THRESHOLD            18620

#define DC_DIV_QR_THRESHOLD                 32
#define DC_DIVAPPR_Q_THRESHOLD             111
#define DC_BDIV_QR_THRESHOLD                47
#define DC_BDIV_Q_THRESHOLD                122

#define INV_MULMOD_BNM1_THRESHOLD          108
#define INV_NEWTON_THRESHOLD               147
#define INV_APPR_THRESHOLD                 117

#define BINV_NEWTON_THRESHOLD              214
#define REDC_1_TO_REDC_N_THRESHOLD          56

#define MATRIX22_STRASSEN_THRESHOLD         11
#define HGCD_THRESHOLD                      94
#define GCD_DC_THRESHOLD                   253
#define GCDEXT_DC_THRESHOLD                221
#define JACOBI_BASE_METHOD                   1

#define MOD_1_NORM_THRESHOLD                 0  /* always */
#define MOD_1_UNNORM_THRESHOLD               0  /* always */
#define MOD_1N_TO_MOD_1_1_THRESHOLD          9
#define MOD_1U_TO_MOD_1_1_THRESHOLD          6
#define MOD_1_1_TO_MOD_1_2_THRESHOLD         9
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        23
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD     16
#define USE_PREINV_DIVREM_1                  0
#define DIVEXACT_1_THRESHOLD                 0  /* always (native) */
#define BMOD_1_TO_MOD_1_THRESHOLD           43

#define GET_STR_DC_THRESHOLD                11
#define GET_STR_PRECOMPUTE_THRESHOLD        22
#define SET_STR_DC_THRESHOLD               802
#define SET_STR_PRECOMPUTE_THRESHOLD      1815
