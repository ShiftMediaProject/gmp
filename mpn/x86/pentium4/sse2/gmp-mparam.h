/* Intel Pentium-4 gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 2000, 2001, 2002, 2003, 2004, 2005, 2007, 2008,
2009 Free Software Foundation, Inc.

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


#define MUL_TOOM22_THRESHOLD                30
#define MUL_TOOM33_THRESHOLD               130
#define MUL_TOOM44_THRESHOLD               292
#define MUL_TOOM6H_THRESHOLD               378
#define MUL_TOOM8H_THRESHOLD               632

#define MUL_TOOM32_TO_TOOM43_THRESHOLD     198
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     214
#define MUL_TOOM42_TO_TOOM53_THRESHOLD     194
#define MUL_TOOM42_TO_TOOM63_THRESHOLD     138

#define SQR_BASECASE_THRESHOLD               0  /* always (native) */
#define SQR_TOOM2_THRESHOLD                 50
#define SQR_TOOM3_THRESHOLD                173
#define SQR_TOOM4_THRESHOLD                466
#define SQR_TOOM6_THRESHOLD                466
#define SQR_TOOM8_THRESHOLD                915

#define MULMOD_BNM1_THRESHOLD               19
#define SQRMOD_BNM1_THRESHOLD               18

#define MUL_FFT_TABLE  { 592, 928, 1920, 3584, 14336, 40960, 229376, 655360, 0 }
#define MUL_FFT_MODF_THRESHOLD             960
#define MUL_FFT_THRESHOLD                36864

#define SQR_FFT_TABLE  { 592, 928, 1920, 3584, 14336, 40960, 98304, 655360, 0 }
#define SQR_FFT_MODF_THRESHOLD             816
#define SQR_FFT_THRESHOLD                15360

#define MULLO_BASECASE_THRESHOLD            12
#define MULLO_DC_THRESHOLD                  51
#define MULLO_MUL_N_THRESHOLD            37217

#define DC_DIV_QR_THRESHOLD                 35
#define DC_DIVAPPR_Q_THRESHOLD              71
#define DC_BDIV_QR_THRESHOLD                51
#define DC_BDIV_Q_THRESHOLD                 85

#define INV_MULMOD_BNM1_THRESHOLD           58
#define INV_NEWTON_THRESHOLD                15
#define INV_APPR_THRESHOLD                  87

#define BINV_NEWTON_THRESHOLD              348
#define REDC_1_TO_REDC_N_THRESHOLD          67

#define MATRIX22_STRASSEN_THRESHOLD         26
#define HGCD_THRESHOLD                      77
#define GCD_DC_THRESHOLD                   298
#define GCDEXT_DC_THRESHOLD                217
#define JACOBI_BASE_METHOD                   1

#define MOD_1_NORM_THRESHOLD                24
#define MOD_1_UNNORM_THRESHOLD           MP_SIZE_T_MAX  /* never */
#define MOD_1N_TO_MOD_1_1_THRESHOLD         20
#define MOD_1U_TO_MOD_1_1_THRESHOLD          9
#define MOD_1_1_TO_MOD_1_2_THRESHOLD         0
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        10
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD     32
#define USE_PREINV_DIVREM_1                  1  /* native */
#define DIVEXACT_1_THRESHOLD                 0  /* always (native) */
#define BMOD_1_TO_MOD_1_THRESHOLD           22

#define GET_STR_DC_THRESHOLD                11
#define GET_STR_PRECOMPUTE_THRESHOLD        27
#define SET_STR_DC_THRESHOLD               142
#define SET_STR_PRECOMPUTE_THRESHOLD       998
