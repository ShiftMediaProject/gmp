/* Alpha EV5 gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 1999, 2000, 2001, 2002, 2004, 2005, 2008, 2009
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


/* 600 MHz 21164A */

#define DIVREM_1_NORM_THRESHOLD              0  /* preinv always */
#define DIVREM_1_UNNORM_THRESHOLD            0  /* always */
#define MOD_1_NORM_THRESHOLD                 0  /* always */
#define MOD_1_UNNORM_THRESHOLD               0  /* always */
#define MOD_1N_TO_MOD_1_1_THRESHOLD         36
#define MOD_1U_TO_MOD_1_1_THRESHOLD          3
#define MOD_1_1_TO_MOD_1_2_THRESHOLD         0
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        14
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD     76
#define USE_PREINV_DIVREM_1                  1  /* preinv always */
#define DIVEXACT_1_THRESHOLD                 0  /* always */
#define BMOD_1_TO_MOD_1_THRESHOLD           72

#define MUL_TOOM22_THRESHOLD                14
#define MUL_TOOM33_THRESHOLD                74
#define MUL_TOOM44_THRESHOLD               130
#define MUL_TOOM6H_THRESHOLD               155
#define MUL_TOOM8H_THRESHOLD               236

#define MUL_TOOM32_TO_TOOM43_THRESHOLD      73
#define MUL_TOOM32_TO_TOOM53_THRESHOLD      84
#define MUL_TOOM42_TO_TOOM53_THRESHOLD      73
#define MUL_TOOM42_TO_TOOM63_THRESHOLD      56

#define SQR_BASECASE_THRESHOLD               4
#define SQR_TOOM2_THRESHOLD                 26
#define SQR_TOOM3_THRESHOLD                 53
#define SQR_TOOM4_THRESHOLD                136
#define SQR_TOOM6_THRESHOLD                173
#define SQR_TOOM8_THRESHOLD                254

#define MULMOD_BNM1_THRESHOLD                9
#define SQRMOD_BNM1_THRESHOLD               14

#define MUL_FFT_TABLE  { 240, 480, 1344, 1792, 5120, 20480, 81920, 196608, 0 }
#define MUL_FFT_MODF_THRESHOLD             240
#define MUL_FFT_THRESHOLD                 1920

#define SQR_FFT_TABLE  { 240, 480, 1216, 1792, 5120, 12288, 81920, 196608, 0 }
#define SQR_FFT_MODF_THRESHOLD             208
#define SQR_FFT_THRESHOLD                 1920

#define MULLO_BASECASE_THRESHOLD             0  /* always */
#define MULLO_DC_THRESHOLD                  44
#define MULLO_MUL_N_THRESHOLD              246

#define DC_DIV_QR_THRESHOLD                 47
#define DC_DIVAPPR_Q_THRESHOLD             182
#define DC_BDIV_QR_THRESHOLD                47
#define DC_BDIV_Q_THRESHOLD                168

#define INV_MULMOD_BNM1_THRESHOLD           55
#define INV_NEWTON_THRESHOLD               187
#define INV_APPR_THRESHOLD                 179

#define BINV_NEWTON_THRESHOLD              220
#define REDC_1_TO_REDC_N_THRESHOLD          77

#define MATRIX22_STRASSEN_THRESHOLD         11
#define HGCD_THRESHOLD                      96
#define GCD_DC_THRESHOLD                   309
#define GCDEXT_DC_THRESHOLD                233
#define JACOBI_BASE_METHOD                   2

#define DIVREM_1_NORM_THRESHOLD              0  /* preinv always */
#define DIVREM_1_UNNORM_THRESHOLD            0  /* always */
#define MOD_1_NORM_THRESHOLD                 0  /* always */
#define MOD_1_UNNORM_THRESHOLD               0  /* always */
#define MOD_1N_TO_MOD_1_1_THRESHOLD         33
#define MOD_1U_TO_MOD_1_1_THRESHOLD          5
#define MOD_1_1_TO_MOD_1_2_THRESHOLD         0
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        16
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD     76
#define USE_PREINV_DIVREM_1                  1  /* preinv always */
#define DIVEXACT_1_THRESHOLD                 0  /* always */
#define BMOD_1_TO_MOD_1_THRESHOLD           75

#define GET_STR_DC_THRESHOLD                15
#define GET_STR_PRECOMPUTE_THRESHOLD        25
#define SET_STR_DC_THRESHOLD               470
#define SET_STR_PRECOMPUTE_THRESHOLD      1452

/* These tables need to be updated.  */

#define MUL_FFT_TABLE2 {{1, 4}, {177, 5}, {193, 4}, {209, 5}, {353, 6}, {385, 5}, {417, 6}, {833, 7}, {897, 6}, {961, 7}, {1025, 6}, {1089, 7}, {1665, 8}, {1793, 7}, {2177, 8}, {2305, 7}, {2433, 8}, {2817, 7}, {2945, 8}, {3329, 9}, {3457, 8}, {4865, 9}, {5633, 8}, {6401, 10}, {7169, 9}, {11777, 10}, {12801, 9}, {13825, 10}, {15361, 9}, {19969, 10}, {23553, 9}, {24065, 11}, {30721, 10}, {48129, 11}, {63489, 10}, {72705, 11}, {96257, 12}, {126977, 11}, {194561, 12}, {258049, 11}, {325633, 12}, {389121, 13}, {516097, 12}, {MP_SIZE_T_MAX,0}}

#define SQR_FFT_TABLE2 {{1, 4}, {177, 5}, {193, 4}, {209, 5}, {353, 6}, {385, 5}, {417, 6}, {961, 7}, {1025, 6}, {1089, 7}, {1153, 6}, {1217, 7}, {1665, 8}, {1793, 7}, {2177, 8}, {2305, 7}, {2561, 8}, {2817, 7}, {2945, 8}, {3329, 9}, {3585, 8}, {5377, 9}, {5633, 8}, {6401, 9}, {6657, 10}, {6913, 9}, {11777, 10}, {13313, 9}, {13825, 10}, {15361, 9}, {18945, 10}, {19457, 9}, {19969, 10}, {23553, 9}, {24065, 11}, {30721, 10}, {48129, 11}, {53249, 10}, {56321, 11}, {63489, 10}, {72705, 11}, {73729, 10}, {79873, 11}, {96257, 12}, {126977, 11}, {194561, 12}, {258049, 11}, {325633, 12}, {389121, 13}, {516097, 12}, {1699841, 13}, {1708033, 12}, {1732609, 13}, {1748993, 12}, {1757185, 13}, {1773569, 12}, {1777665, 13}, {1781761, 12}, {1789953, 13}, {1806337, 12}, {1818625, 13}, {1822721, 12}, {1826817, 13}, {1830913, 12}, {1961985, 13}, {MP_SIZE_T_MAX,0}}
