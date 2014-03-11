/* POWER7 gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 1999-2003, 2009-2011, 2013, 2014 Free Software
Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of either:

  * the GNU Lesser General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your
    option) any later version.

or

  * the GNU General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any
    later version.

or both in parallel, as here.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received copies of the GNU General Public License and the
GNU Lesser General Public License along with the GNU MP Library.  If not,
see https://www.gnu.org/licenses/.  */

#define GMP_LIMB_BITS 64
#define GMP_LIMB_BYTES 8

/* 3700 MHz POWER7/SMT4 (gcc111.fsffrance.org) */

#define MOD_1_NORM_THRESHOLD                 0  /* always */
#define MOD_1_UNNORM_THRESHOLD               0  /* always */
#define MOD_1N_TO_MOD_1_1_THRESHOLD          6
#define MOD_1U_TO_MOD_1_1_THRESHOLD          5
#define MOD_1_1_TO_MOD_1_2_THRESHOLD         8
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        24
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD     13
#define USE_PREINV_DIVREM_1                  0
#define DIV_QR_1N_PI1_METHOD                 1
#define DIV_QR_1_NORM_THRESHOLD              1
#define DIV_QR_1_UNNORM_THRESHOLD            1
#define DIV_QR_2_PI2_THRESHOLD           MP_SIZE_T_MAX  /* never */
#define DIVEXACT_1_THRESHOLD                 0  /* always (native) */
#define BMOD_1_TO_MOD_1_THRESHOLD           28

#define MUL_TOOM22_THRESHOLD                22
#define MUL_TOOM33_THRESHOLD                72
#define MUL_TOOM44_THRESHOLD               200
#define MUL_TOOM6H_THRESHOLD               298
#define MUL_TOOM8H_THRESHOLD               406

#define MUL_TOOM32_TO_TOOM43_THRESHOLD      69
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     140
#define MUL_TOOM42_TO_TOOM53_THRESHOLD     132
#define MUL_TOOM42_TO_TOOM63_THRESHOLD     138
#define MUL_TOOM43_TO_TOOM54_THRESHOLD     124

#define SQR_BASECASE_THRESHOLD               0  /* always (native) */
#define SQR_TOOM2_THRESHOLD                 36
#define SQR_TOOM3_THRESHOLD                109
#define SQR_TOOM4_THRESHOLD                196
#define SQR_TOOM6_THRESHOLD                414
#define SQR_TOOM8_THRESHOLD                547

#define MULMID_TOOM42_THRESHOLD             58

#define MULMOD_BNM1_THRESHOLD               15
#define SQRMOD_BNM1_THRESHOLD               20

#define MUL_FFT_MODF_THRESHOLD             412  /* k = 5 */
#define MUL_FFT_TABLE3                                      \
  { {    412, 5}, {     19, 6}, {     10, 5}, {     21, 6}, \
    {     21, 7}, {     12, 6}, {     25, 7}, {     21, 8}, \
    {     11, 7}, {     25, 8}, {     13, 7}, {     28, 8}, \
    {     15, 7}, {     32, 8}, {     17, 7}, {     35, 8}, \
    {     19, 7}, {     39, 8}, {     29, 9}, {     15, 8}, \
    {     35, 9}, {     19, 8}, {     41, 9}, {     23, 8}, \
    {     47, 9}, {     27,10}, {     15, 9}, {     31, 8}, \
    {     63, 9}, {     43,10}, {     23, 9}, {     51,11}, \
    {     15,10}, {     31, 9}, {     67,10}, {     39, 9}, \
    {     79,10}, {     47, 9}, {     95,10}, {     55,11}, \
    {     31,10}, {     79,11}, {     47,10}, {     95,12}, \
    {     31,11}, {     63,10}, {    135,11}, {     79,10}, \
    {    159,11}, {     95,10}, {    191, 9}, {    383,11}, \
    {    111,12}, {     63,11}, {    127,10}, {    255, 9}, \
    {    511,11}, {    143,10}, {    287, 9}, {    575,10}, \
    {    303,11}, {    159,10}, {    319,12}, {     95,11}, \
    {    191,10}, {    383, 9}, {    767,11}, {    207,10}, \
    {    415,13}, {     63,12}, {    127,11}, {    255,10}, \
    {    511,11}, {    271,10}, {    543,11}, {    287,10}, \
    {    575,11}, {    303,12}, {    159,11}, {    319,10}, \
    {    639,11}, {    335,10}, {    671,11}, {    351,10}, \
    {    703,11}, {    383,10}, {    767,11}, {    415,10}, \
    {    831,12}, {    223,11}, {    447,10}, {    895,13}, \
    {    127,12}, {    255,11}, {    511,10}, {   1023,11}, \
    {    543,10}, {   1087,12}, {    287,11}, {    575,10}, \
    {   1151,11}, {    607,12}, {    319,11}, {    639,10}, \
    {   1279,11}, {    671,12}, {    351,11}, {    703,12}, \
    {    383,11}, {    767,12}, {    415,11}, {    831,10}, \
    {   1663,12}, {    447,11}, {    895,12}, {    479,14}, \
    {    127,13}, {    255,12}, {    511,11}, {   1023,12}, \
    {    543,11}, {   1087,12}, {    575,11}, {   1151,12}, \
    {    607,11}, {   1215,13}, {    319,12}, {    639,11}, \
    {   1279,12}, {    671,11}, {   1343,12}, {    703,11}, \
    {   1407,13}, {    383,12}, {    767,11}, {   1535,12}, \
    {    799,11}, {   1599,12}, {    831,11}, {   1663,13}, \
    {    447,12}, {    895,11}, {   1791,12}, {    959,11}, \
    {   1919,14}, {    255,13}, {    511,12}, {   1087,13}, \
    {    575,12}, {   1215,11}, {   2431,13}, {   8192,14}, \
    {  16384,15}, {  32768,16}, {  65536,17}, { 131072,18}, \
    { 262144,19}, { 524288,20}, {1048576,21}, {2097152,22}, \
    {4194304,23}, {8388608,24} }
#define MUL_FFT_TABLE3_SIZE 162
#define MUL_FFT_THRESHOLD                 4736

#define SQR_FFT_MODF_THRESHOLD             368  /* k = 5 */
#define SQR_FFT_TABLE3                                      \
  { {    368, 5}, {     17, 6}, {      9, 5}, {     19, 6}, \
    {     10, 5}, {     21, 6}, {     21, 7}, {     11, 6}, \
    {     23, 7}, {     12, 6}, {     25, 7}, {     15, 6}, \
    {     31, 7}, {     21, 8}, {     11, 7}, {     25, 8}, \
    {     13, 7}, {     28, 8}, {     15, 7}, {     32, 8}, \
    {     17, 7}, {     35, 8}, {     21, 9}, {     11, 8}, \
    {     29, 9}, {     15, 8}, {     35, 9}, {     19, 8}, \
    {     41, 9}, {     23, 8}, {     47, 9}, {     27,10}, \
    {     15, 9}, {     39,10}, {     23, 9}, {     51,11}, \
    {     15,10}, {     31, 9}, {     67,10}, {     39, 9}, \
    {     79,10}, {     47, 9}, {     95,10}, {     55,11}, \
    {     31,10}, {     79,11}, {     47,10}, {     95,12}, \
    {     31,11}, {     63,10}, {    135,11}, {     79,10}, \
    {    159, 9}, {    319,11}, {     95,10}, {    191, 9}, \
    {    383,12}, {     63,11}, {    127,10}, {    255, 9}, \
    {    511,10}, {    271,11}, {    143,10}, {    287, 9}, \
    {    575,10}, {    303,11}, {    159,10}, {    319,12}, \
    {     95,11}, {    191,10}, {    383,11}, {    207,13}, \
    {     63,12}, {    127,11}, {    255,10}, {    511,11}, \
    {    271,10}, {    543,11}, {    287,10}, {    575,11}, \
    {    303,10}, {    607,12}, {    159,11}, {    319,10}, \
    {    639,11}, {    335,10}, {    671,11}, {    351,10}, \
    {    703,12}, {    191,11}, {    383,10}, {    767,11}, \
    {    415,10}, {    831,12}, {    223,11}, {    447,10}, \
    {    895,11}, {    479,13}, {    127,12}, {    255,11}, \
    {    543,12}, {    287,11}, {    575,10}, {   1151,11}, \
    {    607,12}, {    319,11}, {    639,10}, {   1279,11}, \
    {    671,12}, {    351,11}, {    703,13}, {    191,12}, \
    {    383,11}, {    767,12}, {    415,11}, {    831,12}, \
    {    447,11}, {    895,12}, {    479,14}, {    127,13}, \
    {    255,12}, {    543,11}, {   1087,12}, {    575,11}, \
    {   1151,12}, {    607,13}, {    319,12}, {    639,11}, \
    {   1279,12}, {    671,11}, {   1343,12}, {    703,11}, \
    {   1407,13}, {    383,12}, {    767,11}, {   1535,12}, \
    {    831,13}, {    447,12}, {    895,11}, {   1791,12}, \
    {    959,14}, {    255,13}, {    511,12}, {   1087,13}, \
    {    575,12}, {   1215,13}, {   8192,14}, {  16384,15}, \
    {  32768,16}, {  65536,17}, { 131072,18}, { 262144,19}, \
    { 524288,20}, {1048576,21}, {2097152,22}, {4194304,23}, \
    {8388608,24} }
#define SQR_FFT_TABLE3_SIZE 157
#define SQR_FFT_THRESHOLD                 3264

#define MULLO_BASECASE_THRESHOLD             3
#define MULLO_DC_THRESHOLD                  34
#define MULLO_MUL_N_THRESHOLD             9174

#define DC_DIV_QR_THRESHOLD                 33
#define DC_DIVAPPR_Q_THRESHOLD             126
#define DC_BDIV_QR_THRESHOLD                63
#define DC_BDIV_Q_THRESHOLD                152

#define INV_MULMOD_BNM1_THRESHOLD           54
#define INV_NEWTON_THRESHOLD               155
#define INV_APPR_THRESHOLD                 125

#define BINV_NEWTON_THRESHOLD              294
#define REDC_1_TO_REDC_2_THRESHOLD          17
#define REDC_2_TO_REDC_N_THRESHOLD         115

#define MU_DIV_QR_THRESHOLD               1334
#define MU_DIVAPPR_Q_THRESHOLD            1334
#define MUPI_DIV_QR_THRESHOLD               54
#define MU_BDIV_QR_THRESHOLD              1142
#define MU_BDIV_Q_THRESHOLD               1470

#define POWM_SEC_TABLE  1,14,62,642,960

#define MATRIX22_STRASSEN_THRESHOLD         14
#define HGCD_THRESHOLD                     126
#define HGCD_APPR_THRESHOLD                184
#define HGCD_REDUCE_THRESHOLD             3014
#define GCD_DC_THRESHOLD                   440
#define GCDEXT_DC_THRESHOLD                386
#define JACOBI_BASE_METHOD                   4

#define GET_STR_DC_THRESHOLD                11
#define GET_STR_PRECOMPUTE_THRESHOLD        17
#define SET_STR_DC_THRESHOLD              1655
#define SET_STR_PRECOMPUTE_THRESHOLD      3417

#define FAC_DSC_THRESHOLD                 1138
#define FAC_ODD_THRESHOLD                   27
