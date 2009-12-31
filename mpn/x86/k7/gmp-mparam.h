/* AMD K7 gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 2000, 2001, 2002, 2003, 2004, 2005, 2008, 2009
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

#define GMP_LIMB_BITS 32
#define BYTES_PER_MP_LIMB 4


#define MOD_1_NORM_THRESHOLD                 0  /* always */
#define MOD_1_UNNORM_THRESHOLD               5
#define MOD_1N_TO_MOD_1_1_THRESHOLD         10
#define MOD_1U_TO_MOD_1_1_THRESHOLD          4
#define MOD_1_1_TO_MOD_1_2_THRESHOLD         7
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        16
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD     14
#define USE_PREINV_DIVREM_1                  1  /* native */
#define DIVEXACT_1_THRESHOLD                 0  /* always (native) */
#define BMOD_1_TO_MOD_1_THRESHOLD           30

#define MUL_TOOM22_THRESHOLD                28
#define MUL_TOOM33_THRESHOLD                86
#define MUL_TOOM44_THRESHOLD               142
#define MUL_TOOM6H_THRESHOLD               208
#define MUL_TOOM8H_THRESHOLD               454

#define MUL_TOOM32_TO_TOOM43_THRESHOLD      81
#define MUL_TOOM32_TO_TOOM53_THRESHOLD      98
#define MUL_TOOM42_TO_TOOM53_THRESHOLD      85
#define MUL_TOOM42_TO_TOOM63_THRESHOLD      95

#define SQR_BASECASE_THRESHOLD               0  /* always (native) */
#define SQR_TOOM2_THRESHOLD                 50
#define SQR_TOOM3_THRESHOLD                 87
#define SQR_TOOM4_THRESHOLD                208
#define SQR_TOOM6_THRESHOLD                306
#define SQR_TOOM8_THRESHOLD                454

#define MULMOD_BNM1_THRESHOLD               18
#define SQRMOD_BNM1_THRESHOLD               18

#define MUL_FFT_TABLE  { 432, 928, 1664, 3584, 10240, 40960, 98304, 0 }
#define MUL_FFT_MODF_THRESHOLD             624
#define MUL_FFT_THRESHOLD                69632

#define SQR_FFT_TABLE  { 432, 928, 1664, 3584, 10240, 40960, 163840, 0 }
#define SQR_FFT_MODF_THRESHOLD             496
#define SQR_FFT_THRESHOLD                53248

#define MULLO_BASECASE_THRESHOLD             8
#define MULLO_DC_THRESHOLD                  37
#define MULLO_MUL_N_THRESHOLD            69883

#define DC_DIV_QR_THRESHOLD                 44
#define DC_DIVAPPR_Q_THRESHOLD             204
#define DC_BDIV_QR_THRESHOLD                41
#define DC_BDIV_Q_THRESHOLD                136

#define INV_MULMOD_BNM1_THRESHOLD           54
#define INV_NEWTON_THRESHOLD               226
#define INV_APPR_THRESHOLD                 218

#define BINV_NEWTON_THRESHOLD              268
#define REDC_1_TO_REDC_N_THRESHOLD          63

#define MU_DIV_QR_THRESHOLD               2350
#define MU_DIVAPPR_Q_THRESHOLD            1895
#define MUPI_DIV_QR_THRESHOLD              245
#define MU_BDIV_QR_THRESHOLD              1858
#define MU_BDIV_Q_THRESHOLD               1822

#define MATRIX22_STRASSEN_THRESHOLD         15
#define HGCD_THRESHOLD                     133
#define GCD_DC_THRESHOLD                   443
#define GCDEXT_DC_THRESHOLD                336
#define JACOBI_BASE_METHOD                   1

#define GET_STR_DC_THRESHOLD                15
#define GET_STR_PRECOMPUTE_THRESHOLD        30
#define SET_STR_DC_THRESHOLD               264
#define SET_STR_PRECOMPUTE_THRESHOLD      1079

/* These tables need to be updated.  */

#define MUL_FFT_TABLE2 {{1, 4}, {401, 5}, {801, 6}, {817, 5}, {865, 6}, {1025, 5}, {1057, 6}, {1601, 7}, {1633, 6}, {1729, 7}, {1921, 6}, {2113, 7}, {2177, 6}, {2241, 7}, {2433, 6}, {2497, 7}, {2945, 6}, {3009, 7}, {3457, 8}, {3521, 7}, {4481, 8}, {4865, 7}, {5249, 8}, {5889, 7}, {6017, 8}, {7553, 9}, {7681, 8}, {9985, 9}, {11777, 8}, {13057, 9}, {13825, 8}, {14081, 9}, {15873, 8}, {16641, 9}, {16897, 8}, {17153, 9}, {19969, 8}, {20225, 9}, {20737, 8}, {20993, 9}, {24065, 8}, {24577, 9}, {25089, 8}, {25345, 9}, {27393, 10}, {27649, 9}, {28161, 10}, {31745, 9}, {38913, 10}, {39425, 9}, {40449, 10}, {48129, 9}, {48641, 11}, {63489, 10}, {98305, 11}, {99329, 10}, {100353, 11}, {101377, 10}, {103425, 11}, {104449, 10}, {110593, 11}, {112641, 10}, {113665, 11}, {129025, 10}, {162817, 11}, {194561, 10}, {195585, 12}, {258049, 11}, {391169, 12}, {520193, 11}, {718849, 12}, {782337, 11}, {849921, 13}, {1040385, 12}, {2879489, 13}, {3137537, 12}, {3928065, 13}, {4186113, 12}, {4976641, 13}, {5234689, 12}, {6025217, 13}, {6283265, 12}, {MP_SIZE_T_MAX,0}}

#define SQR_FFT_TABLE2 {{1, 4}, {401, 5}, {417, 4}, {433, 5}, {881, 6}, {961, 5}, {993, 6}, {1857, 7}, {1921, 6}, {2049, 7}, {2177, 6}, {2241, 7}, {2433, 6}, {2497, 7}, {3457, 8}, {3841, 7}, {4481, 8}, {4609, 7}, {4737, 8}, {4865, 7}, {5249, 8}, {5889, 7}, {6273, 8}, {7041, 9}, {7681, 8}, {9985, 9}, {10241, 8}, {10497, 9}, {11777, 8}, {13057, 9}, {15873, 8}, {16385, 9}, {16897, 8}, {17153, 9}, {19969, 8}, {20225, 9}, {20737, 8}, {20993, 9}, {24065, 8}, {24321, 9}, {24577, 10}, {24833, 9}, {25601, 10}, {27137, 9}, {27649, 10}, {31745, 9}, {38401, 10}, {38913, 9}, {40449, 10}, {48129, 9}, {48641, 11}, {63489, 10}, {99329, 11}, {101377, 10}, {103425, 11}, {104449, 10}, {107521, 11}, {110593, 10}, {113665, 11}, {129025, 10}, {154625, 11}, {155649, 10}, {162817, 11}, {194561, 12}, {258049, 11}, {391169, 12}, {520193, 11}, {718849, 12}, {727041, 11}, {729089, 12}, {782337, 11}, {849921, 13}, {1040385, 12}, {2879489, 13}, {3137537, 12}, {3928065, 13}, {4186113, 12}, {4714497, 13}, {5234689, 12}, {6025217, 13}, {6283265, 12}, {7073793, 13}, {7331841, 12}, {MP_SIZE_T_MAX,0}}
