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


#define MOD_1_NORM_THRESHOLD                24
#define MOD_1_UNNORM_THRESHOLD           MP_SIZE_T_MAX  /* never */
#define MOD_1N_TO_MOD_1_1_THRESHOLD         26
#define MOD_1U_TO_MOD_1_1_THRESHOLD          9
#define MOD_1_1_TO_MOD_1_2_THRESHOLD         0
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        10
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD     34
#define USE_PREINV_DIVREM_1                  1  /* native */
#define DIVEXACT_1_THRESHOLD                 0  /* always (native) */
#define BMOD_1_TO_MOD_1_THRESHOLD           22

#define MUL_TOOM22_THRESHOLD                30
#define MUL_TOOM33_THRESHOLD               120
#define MUL_TOOM44_THRESHOLD               296
#define MUL_TOOM6H_THRESHOLD               414
#define MUL_TOOM8H_THRESHOLD               620

#define MUL_TOOM32_TO_TOOM43_THRESHOLD     198
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     216
#define MUL_TOOM42_TO_TOOM53_THRESHOLD     194
#define MUL_TOOM42_TO_TOOM63_THRESHOLD     209

#define SQR_BASECASE_THRESHOLD               0  /* always (native) */
#define SQR_TOOM2_THRESHOLD                 48
#define SQR_TOOM3_THRESHOLD                170
#define SQR_TOOM4_THRESHOLD                454
#define SQR_TOOM6_THRESHOLD                454
#define SQR_TOOM8_THRESHOLD                915

#define MULMOD_BNM1_THRESHOLD               19
#define SQRMOD_BNM1_THRESHOLD               24

#define MUL_FFT_TABLE  { 592, 928, 1920, 3584, 14336, 40960, 163840, 393216, 0 }
#define MUL_FFT_MODF_THRESHOLD             960
#define MUL_FFT_THRESHOLD                 7808

#define SQR_FFT_TABLE  { 592, 928, 1920, 3584, 14336, 40960, 98304, 393216, 0 }
#define SQR_FFT_MODF_THRESHOLD             848
#define SQR_FFT_THRESHOLD                 6784

#define MULLO_BASECASE_THRESHOLD            12
#define MULLO_DC_THRESHOLD                  51
#define MULLO_MUL_N_THRESHOLD            13463

#define DC_DIV_QR_THRESHOLD                 28
#define DC_DIVAPPR_Q_THRESHOLD              61
#define DC_BDIV_QR_THRESHOLD                55
#define DC_BDIV_Q_THRESHOLD                 82

#define INV_MULMOD_BNM1_THRESHOLD           58
#define INV_NEWTON_THRESHOLD               157
#define INV_APPR_THRESHOLD                  16

#define BINV_NEWTON_THRESHOLD              327
#define REDC_1_TO_REDC_N_THRESHOLD          63

#define MU_DIV_QR_THRESHOLD               2350
#define MU_DIVAPPR_Q_THRESHOLD            2089
#define MUPI_DIV_QR_THRESHOLD                7
#define MU_BDIV_QR_THRESHOLD              2089
#define MU_BDIV_Q_THRESHOLD               2089

#define MATRIX22_STRASSEN_THRESHOLD         34
#define HGCD_THRESHOLD                      74
#define GCD_DC_THRESHOLD                   321
#define GCDEXT_DC_THRESHOLD                209
#define JACOBI_BASE_METHOD                   1

#define GET_STR_DC_THRESHOLD                12
#define GET_STR_PRECOMPUTE_THRESHOLD        28
#define SET_STR_DC_THRESHOLD               123
#define SET_STR_PRECOMPUTE_THRESHOLD      1265
