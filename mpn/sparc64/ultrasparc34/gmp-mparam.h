/* ultrasparc3/4 gmp-mparam.h -- Compiler/machine parameter header file.

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

/* 1593 MHz ultrasparc3 running Solaris 10 (swift.nada.kth.se) */

#define DIVREM_1_NORM_THRESHOLD              0  /* always */
#define DIVREM_1_UNNORM_THRESHOLD            0  /* always */
#define MOD_1_NORM_THRESHOLD                 0  /* always */
#define MOD_1_UNNORM_THRESHOLD               0  /* always */
#define MOD_1N_TO_MOD_1_1_THRESHOLD      MP_SIZE_T_MAX  /* never */
#define MOD_1U_TO_MOD_1_1_THRESHOLD      MP_SIZE_T_MAX
#define MOD_1_1_TO_MOD_1_2_THRESHOLD     MP_SIZE_T_MAX
#define MOD_1_2_TO_MOD_1_4_THRESHOLD     MP_SIZE_T_MAX
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD  MP_SIZE_T_MAX  /* never */
#define USE_PREINV_DIVREM_1                  1
#define DIVREM_2_THRESHOLD                   0  /* always */
#define DIVEXACT_1_THRESHOLD                 0  /* always */
#define BMOD_1_TO_MOD_1_THRESHOLD        MP_SIZE_T_MAX  /* never */

#define MUL_TOOM22_THRESHOLD                30
#define MUL_TOOM33_THRESHOLD                93
#define MUL_TOOM44_THRESHOLD               143
#define MUL_TOOM6H_THRESHOLD               165
#define MUL_TOOM8H_THRESHOLD               303

#define MUL_TOOM32_TO_TOOM43_THRESHOLD      93
#define MUL_TOOM32_TO_TOOM53_THRESHOLD      95
#define MUL_TOOM42_TO_TOOM53_THRESHOLD      85
#define MUL_TOOM42_TO_TOOM63_THRESHOLD      50

#define SQR_BASECASE_THRESHOLD              10
#define SQR_TOOM2_THRESHOLD                 72
#define SQR_TOOM3_THRESHOLD                 97
#define SQR_TOOM4_THRESHOLD                179
#define SQR_TOOM6_THRESHOLD                191
#define SQR_TOOM8_THRESHOLD                339

#define MULMOD_BNM1_THRESHOLD               14
#define SQRMOD_BNM1_THRESHOLD                9

#define MUL_FFT_TABLE  { 240, 480, 1344, 2304, 5120, 12288, 81920, 327680, 0 }
#define MUL_FFT_MODF_THRESHOLD             256
#define MUL_FFT_THRESHOLD                 4608

#define SQR_FFT_TABLE  { 304, 608, 1344, 2304, 7168, 12288, 81920, 327680, 0 }
#define SQR_FFT_MODF_THRESHOLD             248
#define SQR_FFT_THRESHOLD                 2688

#define MULLO_BASECASE_THRESHOLD            26
#define MULLO_DC_THRESHOLD                   0  /* never mpn_mullo_basecase */
#define MULLO_MUL_N_THRESHOLD             8648

#define DC_DIV_QR_THRESHOLD                 16
#define DC_DIVAPPR_Q_THRESHOLD              66
#define DC_BDIV_QR_THRESHOLD                26
#define DC_BDIV_Q_THRESHOLD                 92

#define INV_MULMOD_BNM1_THRESHOLD           76
#define INV_NEWTON_THRESHOLD                17
#define INV_APPR_THRESHOLD                  17

#define BINV_NEWTON_THRESHOLD              134
#define REDC_1_TO_REDC_2_THRESHOLD          10
#define REDC_2_TO_REDC_N_THRESHOLD         117

#define MU_DIV_QR_THRESHOLD               1120
#define MU_DIVAPPR_Q_THRESHOLD             924
#define MUPI_DIV_QR_THRESHOLD                0  /* always */
#define MU_BDIV_QR_THRESHOLD              1120
#define MU_BDIV_Q_THRESHOLD               1120

#define MATRIX22_STRASSEN_THRESHOLD         12
#define HGCD_THRESHOLD                      39
#define GCD_DC_THRESHOLD                   130
#define GCDEXT_DC_THRESHOLD                134
#define JACOBI_BASE_METHOD                   2

#define GET_STR_DC_THRESHOLD                18
#define GET_STR_PRECOMPUTE_THRESHOLD        27
#define SET_STR_DC_THRESHOLD               315
#define SET_STR_PRECOMPUTE_THRESHOLD      1037
