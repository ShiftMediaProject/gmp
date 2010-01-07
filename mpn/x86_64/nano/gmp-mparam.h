/* VIA Nano gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007,
2008, 2009, 2010 Free Software Foundation, Inc.

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

/* 1600 MHz Nano 2xxx */

#define MOD_1_NORM_THRESHOLD                 0  /* always */
#define MOD_1_UNNORM_THRESHOLD               0  /* always */
#define MOD_1N_TO_MOD_1_1_THRESHOLD          8
#define MOD_1U_TO_MOD_1_1_THRESHOLD          6
#define MOD_1_1_TO_MOD_1_2_THRESHOLD        10
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        14
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD     14
#define USE_PREINV_DIVREM_1                  1  /* native */
#define DIVEXACT_1_THRESHOLD                 0  /* always (native) */
#define BMOD_1_TO_MOD_1_THRESHOLD           24

#define MUL_TOOM22_THRESHOLD                28
#define MUL_TOOM33_THRESHOLD                37
#define MUL_TOOM44_THRESHOLD               315
#define MUL_TOOM6H_THRESHOLD               746
#define MUL_TOOM8H_THRESHOLD               866

#define MUL_TOOM32_TO_TOOM43_THRESHOLD      73
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     201
#define MUL_TOOM42_TO_TOOM53_THRESHOLD     211
#define MUL_TOOM42_TO_TOOM63_THRESHOLD     219

#define SQR_BASECASE_THRESHOLD               0  /* always (native) */
#define SQR_TOOM2_THRESHOLD                 38
#define SQR_TOOM3_THRESHOLD                 77
#define SQR_TOOM4_THRESHOLD                620
#define SQR_TOOM6_THRESHOLD                996
#define SQR_TOOM8_THRESHOLD               1138

#define MULMOD_BNM1_THRESHOLD               15
#define SQRMOD_BNM1_THRESHOLD               17

#define MUL_FFT_TABLE  { 368, 928, 1856, 2816, 7168, 28672, 81920, 327680, 0 }
#define MUL_FFT_MODF_THRESHOLD             464
#define MUL_FFT_THRESHOLD                 3712

#define SQR_FFT_TABLE  { 400, 864, 1856, 3328, 7168, 20480, 81920, 327680, 0 }
#define SQR_FFT_MODF_THRESHOLD             416
#define SQR_FFT_THRESHOLD                 3712

#define MULLO_BASECASE_THRESHOLD            19
#define MULLO_DC_THRESHOLD                   0  /* never mpn_mullo_basecase */
#define MULLO_MUL_N_THRESHOLD             7246

#define DC_DIV_QR_THRESHOLD                 55
#define DC_DIVAPPR_Q_THRESHOLD             160
#define DC_BDIV_QR_THRESHOLD                51
#define DC_BDIV_Q_THRESHOLD                 80

#define INV_MULMOD_BNM1_THRESHOLD          140
#define INV_NEWTON_THRESHOLD               163
#define INV_APPR_THRESHOLD                 157

#define BINV_NEWTON_THRESHOLD              228
#define REDC_1_TO_REDC_2_THRESHOLD          16
#define REDC_2_TO_REDC_N_THRESHOLD          71

#define MU_DIV_QR_THRESHOLD               1858
#define MU_DIVAPPR_Q_THRESHOLD            2009
#define MUPI_DIV_QR_THRESHOLD               79
#define MU_BDIV_QR_THRESHOLD              1387
#define MU_BDIV_Q_THRESHOLD               1787

#define MATRIX22_STRASSEN_THRESHOLD         21
#define HGCD_THRESHOLD                     118
#define GCD_DC_THRESHOLD                   416
#define GCDEXT_DC_THRESHOLD                511
#define JACOBI_BASE_METHOD                   1

#define GET_STR_DC_THRESHOLD                13
#define GET_STR_PRECOMPUTE_THRESHOLD        21
#define SET_STR_DC_THRESHOLD               592
#define SET_STR_PRECOMPUTE_THRESHOLD      2044
