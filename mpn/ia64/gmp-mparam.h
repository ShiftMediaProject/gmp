/* gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 2000, 2001, 2002, 2003, 2004, 2005, 2009, 2010 Free Software
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

/* 1300MHz Itanium2 (babe.fsffrance.org) */


#define MOD_1_NORM_THRESHOLD                 0  /* always */
#define MOD_1_UNNORM_THRESHOLD               0  /* always */
#define MOD_1N_TO_MOD_1_1_THRESHOLD         14
#define MOD_1U_TO_MOD_1_1_THRESHOLD          8
#define MOD_1_1_TO_MOD_1_2_THRESHOLD         0
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        14
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD     22
#define USE_PREINV_DIVREM_1                  1  /* native */
#define DIVEXACT_1_THRESHOLD                 0  /* always (native) */
#define BMOD_1_TO_MOD_1_THRESHOLD        MP_SIZE_T_MAX  /* never */

#define MUL_TOOM22_THRESHOLD                44
#define MUL_TOOM33_THRESHOLD                89
#define MUL_TOOM44_THRESHOLD               232
#define MUL_TOOM6H_THRESHOLD               351
#define MUL_TOOM8H_THRESHOLD               454

#define MUL_TOOM32_TO_TOOM43_THRESHOLD     101
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     160
#define MUL_TOOM42_TO_TOOM53_THRESHOLD     138
#define MUL_TOOM42_TO_TOOM63_THRESHOLD     159

#define SQR_BASECASE_THRESHOLD              26
#define SQR_TOOM2_THRESHOLD                119
#define SQR_TOOM3_THRESHOLD                141
#define SQR_TOOM4_THRESHOLD                282
#define SQR_TOOM6_THRESHOLD                375
#define SQR_TOOM8_THRESHOLD                527

#define MULMOD_BNM1_THRESHOLD               24
#define SQRMOD_BNM1_THRESHOLD               19

#define MUL_FFT_TABLE  { 560, 1184, 1856, 3840, 11264, 28672, 114688, 327680, 0 }
#define MUL_FFT_MODF_THRESHOLD             944
#define MUL_FFT_THRESHOLD                 9856

#define SQR_FFT_TABLE  { 592, 1248, 1856, 3840, 11264, 28672, 81920, 327680, 0 }
#define SQR_FFT_MODF_THRESHOLD             752
#define SQR_FFT_THRESHOLD                 7552

#define MULLO_BASECASE_THRESHOLD            17
#define MULLO_DC_THRESHOLD                  84
#define MULLO_MUL_N_THRESHOLD            19187

#define DC_DIV_QR_THRESHOLD                 68
#define DC_DIVAPPR_Q_THRESHOLD             252
#define DC_BDIV_QR_THRESHOLD               107
#define DC_BDIV_Q_THRESHOLD                292

#define INV_MULMOD_BNM1_THRESHOLD          123
#define INV_NEWTON_THRESHOLD               187
#define INV_APPR_THRESHOLD                 179

#define BINV_NEWTON_THRESHOLD              296
#define REDC_1_TO_REDC_2_THRESHOLD          10
#define REDC_2_TO_REDC_N_THRESHOLD         164

#define MU_DIV_QR_THRESHOLD               1752
#define MU_DIVAPPR_Q_THRESHOLD            1442
#define MUPI_DIV_QR_THRESHOLD                0  /* always */
#define MU_BDIV_QR_THRESHOLD              1895
#define MU_BDIV_Q_THRESHOLD               2089

#define MATRIX22_STRASSEN_THRESHOLD         23
#define HGCD_THRESHOLD                     120
#define GCD_DC_THRESHOLD                   630
#define GCDEXT_DC_THRESHOLD                440
#define JACOBI_BASE_METHOD                   2

#define GET_STR_DC_THRESHOLD                14
#define GET_STR_PRECOMPUTE_THRESHOLD        21
#define SET_STR_DC_THRESHOLD              1240
#define SET_STR_PRECOMPUTE_THRESHOLD      3566
