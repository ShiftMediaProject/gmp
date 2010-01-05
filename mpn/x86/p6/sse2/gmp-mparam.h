/* Intel P6/sse2 gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 1999, 2000, 2001, 2002, 2003, 2008, 2009, 2010 Free
Software Foundation, Inc.

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


/* 1867 MHz P6 model 13 */

#define MOD_1_NORM_THRESHOLD                 3
#define MOD_1_UNNORM_THRESHOLD               5
#define MOD_1N_TO_MOD_1_1_THRESHOLD         10
#define MOD_1U_TO_MOD_1_1_THRESHOLD          5
#define MOD_1_1_TO_MOD_1_2_THRESHOLD         6
#define MOD_1_2_TO_MOD_1_4_THRESHOLD         0
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD     23
#define USE_PREINV_DIVREM_1                  1  /* native */
#define DIVEXACT_1_THRESHOLD                 0  /* always (native) */
#define BMOD_1_TO_MOD_1_THRESHOLD           22

#define MUL_TOOM22_THRESHOLD                20
#define MUL_TOOM33_THRESHOLD                77
#define MUL_TOOM44_THRESHOLD               182
#define MUL_TOOM6H_THRESHOLD               252
#define MUL_TOOM8H_THRESHOLD               381

#define MUL_TOOM32_TO_TOOM43_THRESHOLD      75
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     122
#define MUL_TOOM42_TO_TOOM53_THRESHOLD     115
#define MUL_TOOM42_TO_TOOM63_THRESHOLD      79

#define SQR_BASECASE_THRESHOLD               0  /* always (native) */
#define SQR_TOOM2_THRESHOLD                 30
#define SQR_TOOM3_THRESHOLD                101
#define SQR_TOOM4_THRESHOLD                154
#define SQR_TOOM6_THRESHOLD                222
#define SQR_TOOM8_THRESHOLD                547

#define MULMOD_BNM1_THRESHOLD               13
#define SQRMOD_BNM1_THRESHOLD               18

#define MUL_FFT_TABLE  { 400, 928, 1664, 3584, 10240, 24576, 163840, 0 }
#define MUL_FFT_MODF_THRESHOLD             720
#define MUL_FFT_THRESHOLD                 7552

#define SQR_FFT_TABLE  { 400, 928, 1664, 3584, 10240, 40960, 163840, 0 }
#define SQR_FFT_MODF_THRESHOLD             592
#define SQR_FFT_THRESHOLD                 5760

#define MULLO_BASECASE_THRESHOLD             0  /* always */
#define MULLO_DC_THRESHOLD                  34
#define MULLO_MUL_N_THRESHOLD            13463

#define DC_DIV_QR_THRESHOLD                 19
#define DC_DIVAPPR_Q_THRESHOLD              56
#define DC_BDIV_QR_THRESHOLD                60
#define DC_BDIV_Q_THRESHOLD                132

#define INV_MULMOD_BNM1_THRESHOLD          124
#define INV_NEWTON_THRESHOLD                81
#define INV_APPR_THRESHOLD                  61

#define BINV_NEWTON_THRESHOLD              276
#define REDC_1_TO_REDC_N_THRESHOLD          63

#define MU_DIV_QR_THRESHOLD               1308
#define MU_DIVAPPR_Q_THRESHOLD             998
#define MUPI_DIV_QR_THRESHOLD              124
#define MU_BDIV_QR_THRESHOLD              1442
#define MU_BDIV_Q_THRESHOLD               1470

#define MATRIX22_STRASSEN_THRESHOLD         17
#define HGCD_THRESHOLD                      60
#define GCD_DC_THRESHOLD                   393
#define GCDEXT_DC_THRESHOLD                303
#define JACOBI_BASE_METHOD                   1

#define GET_STR_DC_THRESHOLD                13
#define GET_STR_PRECOMPUTE_THRESHOLD        22
#define SET_STR_DC_THRESHOLD               587
#define SET_STR_PRECOMPUTE_THRESHOLD       983
