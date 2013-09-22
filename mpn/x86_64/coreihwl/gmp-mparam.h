/* Haswell gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007,
2008, 2009, 2010, 2011, 2012, 2013 Free Software Foundation, Inc.

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

/* 2900 MHz Core i5 Haswell */

#define MOD_1_NORM_THRESHOLD                 0  /* always */
#define MOD_1_UNNORM_THRESHOLD               0  /* always */
#define MOD_1N_TO_MOD_1_1_THRESHOLD          4
#define MOD_1U_TO_MOD_1_1_THRESHOLD          3
#define MOD_1_1_TO_MOD_1_2_THRESHOLD        10
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        28
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD     11
#define USE_PREINV_DIVREM_1                  1  /* native */
#define DIV_QR_2_PI2_THRESHOLD              35
#define DIVEXACT_1_THRESHOLD                 0  /* always (native) */
#define BMOD_1_TO_MOD_1_THRESHOLD           25

#define MUL_TOOM22_THRESHOLD                22
#define MUL_TOOM33_THRESHOLD                67
#define MUL_TOOM44_THRESHOLD               196
#define MUL_TOOM6H_THRESHOLD               274
#define MUL_TOOM8H_THRESHOLD               393

#define MUL_TOOM32_TO_TOOM43_THRESHOLD     129
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     138
#define MUL_TOOM42_TO_TOOM53_THRESHOLD     127
#define MUL_TOOM42_TO_TOOM63_THRESHOLD     131
#define MUL_TOOM43_TO_TOOM54_THRESHOLD     183

#define SQR_BASECASE_THRESHOLD               0  /* always (native) */
#define SQR_TOOM2_THRESHOLD                 34
#define SQR_TOOM3_THRESHOLD                117
#define SQR_TOOM4_THRESHOLD                336
#define SQR_TOOM6_THRESHOLD                430
#define SQR_TOOM8_THRESHOLD                  0  /* always */

#define MULMID_TOOM42_THRESHOLD             42

#define MULMOD_BNM1_THRESHOLD               14
#define SQRMOD_BNM1_THRESHOLD               17

#define MUL_FFT_MODF_THRESHOLD             376  /* k = 5 */
#define MUL_FFT_TABLE3                                      \
  { {    376, 5}, {     19, 6}, {     10, 5}, {     21, 6}, \
    {     11, 5}, {     23, 6}, {     21, 7}, {     11, 6}, \
    {     25, 7}, {     13, 6}, {     27, 7}, {     21, 8}, \
    {     11, 7}, {     25, 8}, {     13, 7}, {     28, 8}, \
    {     15, 7}, {     31, 8}, {     21, 9}, {     11, 8}, \
    {     27, 9}, {     15, 8}, {     33, 9}, {     19, 8}, \
    {     41, 9}, {     23, 8}, {     47, 9}, {     27,10}, \
    {     15, 9}, {     39,10}, {     23, 9}, {     51,11}, \
    {     15,10}, {     31, 9}, {     67,10}, {     39, 9}, \
    {     83,10}, {     47,11}, {     31,10}, {     63, 9}, \
    {    127,10}, {     79,11}, {     47,10}, {     95,12}, \
    {     31,11}, {     63,10}, {    127, 9}, {    255,10}, \
    {    135,11}, {     79,10}, {    159, 9}, {    319,11}, \
    {     95,10}, {    191, 9}, {    383,11}, {    111,12}, \
    {     63,11}, {    127, 9}, {    511,10}, {    271,11}, \
    {    143,10}, {    287, 9}, {    575,10}, {    303, 9}, \
    {    607,11}, {    159,10}, {    319,12}, {     95,10}, \
    {    383, 9}, {    767,11}, {    207,13}, {     63,12}, \
    {    127,11}, {    255,10}, {    511,11}, {    271,10}, \
    {    543,11}, {    287,10}, {    575,11}, {    303,10}, \
    {    607,12}, {    159,11}, {    319,10}, {    639,11}, \
    {    335,10}, {    671,11}, {    351,10}, {    703,11}, \
    {    367,12}, {    191,11}, {    383,10}, {    767,11}, \
    {    415,10}, {    831,12}, {    223,11}, {    447,10}, \
    {    895,13}, {    127,12}, {    255,11}, {    543,12}, \
    {    287,11}, {    607,10}, {   1215,12}, {    319,11}, \
    {    671,12}, {    351,11}, {    703,10}, {   1407,13}, \
    {    191,12}, {    383,11}, {    767,12}, {    415,11}, \
    {    831,12}, {    447,11}, {    895,12}, {    479,14}, \
    {    127,13}, {    255,12}, {    511,11}, {   1023,12}, \
    {    543,11}, {   1087,12}, {    607,11}, {   1215,13}, \
    {    319,12}, {    703,11}, {   1407,12}, {    735,13}, \
    {    383,12}, {    767,11}, {   1535,12}, {    831,13}, \
    {    447,12}, {    959,14}, {    255,13}, {    511,12}, \
    {   1087,13}, {    575,12}, {   1151,11}, {   2303,12}, \
    {   1215,13}, {    639,12}, {   1279,13}, {    703,12}, \
    {   1407,14}, {    383,13}, {    767,12}, {   1535,13}, \
    {    831,12}, {   1663,13}, {    959,14}, {    511,13}, \
    {   1087,12}, {   2175,13}, {   1215,14}, {  16384,15}, \
    {  32768,16}, {  65536,17}, { 131072,18}, { 262144,19}, \
    { 524288,20}, {1048576,21}, {2097152,22}, {4194304,23}, \
    {8388608,24} }
#define MUL_FFT_TABLE3_SIZE 165
#define MUL_FFT_THRESHOLD                 4224

#define SQR_FFT_MODF_THRESHOLD             344  /* k = 5 */
#define SQR_FFT_TABLE3                                      \
  { {    344, 5}, {     11, 4}, {     23, 5}, {     21, 6}, \
    {     11, 5}, {     23, 6}, {     25, 7}, {     13, 6}, \
    {     27, 7}, {     25, 8}, {     13, 7}, {     28, 8}, \
    {     15, 7}, {     31, 8}, {     21, 9}, {     11, 8}, \
    {     27, 9}, {     15, 8}, {     35, 9}, {     19, 8}, \
    {     41, 9}, {     23, 8}, {     47, 9}, {     27,10}, \
    {     15, 9}, {     39,10}, {     23, 9}, {     51,11}, \
    {     15,10}, {     31, 9}, {     63,10}, {     39, 9}, \
    {     79,10}, {     55,11}, {     31,10}, {     79,11}, \
    {     47,10}, {     95,12}, {     31,11}, {     63,10}, \
    {    127, 9}, {    255,10}, {    135,11}, {     79, 9}, \
    {    319,11}, {     95, 9}, {    383,11}, {    111,12}, \
    {     63,11}, {    127,10}, {    255, 9}, {    511,10}, \
    {    271, 9}, {    543,11}, {    143,10}, {    287, 9}, \
    {    575,10}, {    303, 9}, {    607,10}, {    319, 9}, \
    {    639,12}, {     95,11}, {    191,10}, {    383, 9}, \
    {    767,11}, {    207,13}, {     63,12}, {    127,11}, \
    {    255,10}, {    511,11}, {    271,10}, {    543,11}, \
    {    287,10}, {    575,11}, {    303,10}, {    607,11}, \
    {    319,10}, {    639,11}, {    335,10}, {    671,11}, \
    {    351,10}, {    703,11}, {    367,12}, {    191,11}, \
    {    383,10}, {    767,11}, {    415,10}, {    831,12}, \
    {    223,11}, {    447,10}, {    895,11}, {    479,13}, \
    {    127,12}, {    255,11}, {    511,10}, {   1023,11}, \
    {    543,10}, {   1087,12}, {    287,11}, {    607,12}, \
    {    319,11}, {    735,12}, {    383,11}, {    767,12}, \
    {    415,11}, {    831,12}, {    447,11}, {    895,12}, \
    {    479,14}, {    127,13}, {    255,12}, {    511,11}, \
    {   1023,12}, {    543,11}, {   1087,12}, {    575,11}, \
    {   1151,12}, {    607,11}, {   1215,13}, {    319,12}, \
    {    671,11}, {   1343,12}, {    703,11}, {   1407,12}, \
    {    735,13}, {    383,12}, {    767,11}, {   1535,12}, \
    {    831,13}, {    447,12}, {    959,14}, {    255,13}, \
    {    511,12}, {   1087,13}, {    575,12}, {   1215,13}, \
    {    639,12}, {   1343,13}, {    703,12}, {   1407,14}, \
    {    383,13}, {    767,12}, {   1535,13}, {    831,12}, \
    {   1663,13}, {    959,14}, {    511,13}, {   1087,12}, \
    {   2175,13}, {   1215,14}, {  16384,15}, {  32768,16}, \
    {  65536,17}, { 131072,18}, { 262144,19}, { 524288,20}, \
    {1048576,21}, {2097152,22}, {4194304,23}, {8388608,24} }
#define SQR_FFT_TABLE3_SIZE 160
#define SQR_FFT_THRESHOLD                 3264

#define MULLO_BASECASE_THRESHOLD             0  /* always */
#define MULLO_DC_THRESHOLD                  67
#define MULLO_MUL_N_THRESHOLD             8907

#define DC_DIV_QR_THRESHOLD                 63
#define DC_DIVAPPR_Q_THRESHOLD             198
#define DC_BDIV_QR_THRESHOLD                56
#define DC_BDIV_Q_THRESHOLD                143

#define INV_MULMOD_BNM1_THRESHOLD           46
#define INV_NEWTON_THRESHOLD               197
#define INV_APPR_THRESHOLD                 195

#define BINV_NEWTON_THRESHOLD              240
#define REDC_1_TO_REDC_2_THRESHOLD          46
#define REDC_2_TO_REDC_N_THRESHOLD          63

#define MU_DIV_QR_THRESHOLD               1470
#define MU_DIVAPPR_Q_THRESHOLD            1589
#define MUPI_DIV_QR_THRESHOLD               78
#define MU_BDIV_QR_THRESHOLD              1442
#define MU_BDIV_Q_THRESHOLD               1528

#define POWM_SEC_TABLE  2,24,195,322,1168,1604

#define MATRIX22_STRASSEN_THRESHOLD         16
#define HGCD_THRESHOLD                     119
#define HGCD_APPR_THRESHOLD                 50
#define HGCD_REDUCE_THRESHOLD             2479
#define GCD_DC_THRESHOLD                   767
#define GCDEXT_DC_THRESHOLD                411
#define JACOBI_BASE_METHOD                   4

#define GET_STR_DC_THRESHOLD                12
#define GET_STR_PRECOMPUTE_THRESHOLD        19
#define SET_STR_DC_THRESHOLD              1502
#define SET_STR_PRECOMPUTE_THRESHOLD      2923

#define FAC_DSC_THRESHOLD                  692
#define FAC_ODD_THRESHOLD                    0  /* always */
