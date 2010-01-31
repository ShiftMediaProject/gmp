/* Intel P6/mmx gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2009,
2010 Free Software Foundation, Inc.

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


/* NOTE: In a fat binary build SQR_TOOM2_THRESHOLD here cannot be more than the
   value in mpn/x86/p6/gmp-mparam.h.  The latter is used as a hard limit in
   mpn/x86/p6/sqr_basecase.asm.  */


/* 800 MHz P6 model 8 */

#define MOD_1_NORM_THRESHOLD                 4
#define MOD_1_UNNORM_THRESHOLD               5
#define MOD_1N_TO_MOD_1_1_THRESHOLD          8
#define MOD_1U_TO_MOD_1_1_THRESHOLD          7
#define MOD_1_1_TO_MOD_1_2_THRESHOLD         0
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        12
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD     15
#define USE_PREINV_DIVREM_1                  1  /* native */
#define DIVEXACT_1_THRESHOLD                 0  /* always (native) */
#define BMOD_1_TO_MOD_1_THRESHOLD           49

#define MUL_TOOM22_THRESHOLD                22
#define MUL_TOOM33_THRESHOLD                73
#define MUL_TOOM44_THRESHOLD               193
#define MUL_TOOM6H_THRESHOLD               254
#define MUL_TOOM8H_THRESHOLD               381

#define MUL_TOOM32_TO_TOOM43_THRESHOLD      73
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     122
#define MUL_TOOM42_TO_TOOM53_THRESHOLD      73
#define MUL_TOOM42_TO_TOOM63_THRESHOLD      80

#define SQR_BASECASE_THRESHOLD               0  /* always (native) */
#define SQR_TOOM2_THRESHOLD                 48
#define SQR_TOOM3_THRESHOLD                 81
#define SQR_TOOM4_THRESHOLD                142
#define SQR_TOOM6_THRESHOLD                258
#define SQR_TOOM8_THRESHOLD                399

#define MULMOD_BNM1_THRESHOLD               15
#define SQRMOD_BNM1_THRESHOLD               18

#define MUL_FFT_TABLE  { 368, 800, 1408, 3584, 10240, 40960, 163840, 0 }
#define MUL_FFT_MODF_THRESHOLD             528
#define MUL_FFT_THRESHOLD                 7424

#define SQR_FFT_TABLE  { 336, 800, 1408, 3584, 10240, 40960, 163840, 0 }
#define SQR_FFT_MODF_THRESHOLD             432
#define SQR_FFT_THRESHOLD                 4736

#define MULLO_BASECASE_THRESHOLD             8
#define MULLO_DC_THRESHOLD                  60
#define MULLO_MUL_N_THRESHOLD            13765

#define DC_DIV_QR_THRESHOLD                 83
#define DC_DIVAPPR_Q_THRESHOLD             246
#define DC_BDIV_QR_THRESHOLD                76
#define DC_BDIV_Q_THRESHOLD                175

#define INV_MULMOD_BNM1_THRESHOLD           82
#define INV_NEWTON_THRESHOLD               268
#define INV_APPR_THRESHOLD                 250

#define BINV_NEWTON_THRESHOLD              276
#define REDC_1_TO_REDC_N_THRESHOLD          74

#define MU_DIV_QR_THRESHOLD               1442
#define MU_DIVAPPR_Q_THRESHOLD            1442
#define MUPI_DIV_QR_THRESHOLD              132
#define MU_BDIV_QR_THRESHOLD              1142
#define MU_BDIV_Q_THRESHOLD               1442

#define MATRIX22_STRASSEN_THRESHOLD         18
#define HGCD_THRESHOLD                     121
#define GCD_DC_THRESHOLD                   478
#define GCDEXT_DC_THRESHOLD                361
#define JACOBI_BASE_METHOD                   1

#define GET_STR_DC_THRESHOLD                13
#define GET_STR_PRECOMPUTE_THRESHOLD        26
#define SET_STR_DC_THRESHOLD               272
#define SET_STR_PRECOMPUTE_THRESHOLD      1074
