/* gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 1999-2003, 2009, 2010, 2012, 2013 Free Software
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

#define GMP_LIMB_BITS 32
#define GMP_LIMB_BYTES 4

/* 1000MHz Cortex-A9 */

#define MOD_1_NORM_THRESHOLD                 0  /* always */
#define MOD_1_UNNORM_THRESHOLD               0  /* always */
#define MOD_1N_TO_MOD_1_1_THRESHOLD          2
#define MOD_1U_TO_MOD_1_1_THRESHOLD          5
#define MOD_1_1_TO_MOD_1_2_THRESHOLD        13
#define MOD_1_2_TO_MOD_1_4_THRESHOLD     MP_SIZE_T_MAX
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD     12
#define USE_PREINV_DIVREM_1                  1  /* native */
#define DIV_QR_2_PI2_THRESHOLD           MP_SIZE_T_MAX  /* never */
#define DIVEXACT_1_THRESHOLD             MP_SIZE_T_MAX  /* never */
#define BMOD_1_TO_MOD_1_THRESHOLD           21

#define MUL_TOOM22_THRESHOLD                37
#define MUL_TOOM33_THRESHOLD                66
#define MUL_TOOM44_THRESHOLD               195
#define MUL_TOOM6H_THRESHOLD               333
#define MUL_TOOM8H_THRESHOLD              1105

#define MUL_TOOM32_TO_TOOM43_THRESHOLD     120
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     115
#define MUL_TOOM42_TO_TOOM53_THRESHOLD      79
#define MUL_TOOM42_TO_TOOM63_THRESHOLD     120
#define MUL_TOOM43_TO_TOOM54_THRESHOLD     131

#define SQR_BASECASE_THRESHOLD               0  /* always (native) */
#define SQR_TOOM2_THRESHOLD                 46
#define SQR_TOOM3_THRESHOLD                 99
#define SQR_TOOM4_THRESHOLD                158
#define SQR_TOOM6_THRESHOLD                296
#define SQR_TOOM8_THRESHOLD                360

#define MULMID_TOOM42_THRESHOLD             60

#define MULMOD_BNM1_THRESHOLD                9
#define SQRMOD_BNM1_THRESHOLD               13

#define MUL_FFT_MODF_THRESHOLD             432  /* k = 5 */
#define MUL_FFT_TABLE3                                      \
  { {    432, 5}, {     14, 4}, {     29, 5}, {     15, 4}, \
    {     32, 5}, {     18, 4}, {     37, 5}, {     29, 6}, \
    {     15, 5}, {     32, 6}, {     29, 7}, {     15, 6}, \
    {     32, 7}, {     17, 6}, {     36, 7}, {     19, 6}, \
    {     39, 7}, {     23, 6}, {     47, 7}, {     29, 8}, \
    {     15, 7}, {     31, 6}, {     63, 7}, {     35, 8}, \
    {     19, 7}, {     41, 8}, {     23, 7}, {     49, 8}, \
    {     27, 9}, {     15, 8}, {     31, 7}, {     65, 8}, \
    {     35, 7}, {     71, 8}, {     39, 7}, {     79, 8}, \
    {     43, 9}, {     23, 8}, {     55,10}, {     15, 9}, \
    {     31, 8}, {     71, 9}, {     39, 8}, {     83, 9}, \
    {     47, 8}, {     99, 9}, {     55,10}, {     31, 9}, \
    {     79,10}, {     47, 9}, {    103,11}, {     31,10}, \
    {     63, 9}, {    135,10}, {     79, 9}, {    159,10}, \
    {     95, 9}, {    191,10}, {    111,11}, {     63,10}, \
    {    127, 9}, {    255,10}, {    143, 9}, {    287,10}, \
    {    159,11}, {     95,10}, {    191, 9}, {    383,12}, \
    {     63,11}, {    127,10}, {    255, 9}, {    511,10}, \
    {    271, 9}, {    543,10}, {    287,11}, {    159,10}, \
    {    319, 9}, {    639,10}, {    335, 9}, {    671,10}, \
    {    351,11}, {    191,10}, {    383, 9}, {    767,10}, \
    {    415,11}, {    223,12}, {   4096,13}, {   8192,14}, \
    {  16384,15}, {  32768,16} }
#define MUL_FFT_TABLE3_SIZE 90
#define MUL_FFT_THRESHOLD                 5312

#define SQR_FFT_MODF_THRESHOLD             400  /* k = 5 */
#define SQR_FFT_TABLE3                                      \
  { {    400, 5}, {     13, 4}, {     27, 5}, {     14, 4}, \
    {     34, 5}, {     19, 4}, {     39, 5}, {     29, 6}, \
    {     15, 5}, {     33, 6}, {     29, 7}, {     15, 6}, \
    {     37, 7}, {     19, 6}, {     39, 7}, {     21, 6}, \
    {     43, 7}, {     23, 6}, {     47, 7}, {     29, 8}, \
    {     15, 7}, {     37, 8}, {     19, 7}, {     43, 8}, \
    {     23, 7}, {     49, 8}, {     27, 7}, {     55, 9}, \
    {     15, 8}, {     31, 7}, {     63, 8}, {     43, 9}, \
    {     23, 8}, {     55,10}, {     15, 9}, {     31, 8}, \
    {     67, 9}, {     39, 8}, {     79, 9}, {     55,10}, \
    {     31, 9}, {     79,10}, {     47, 9}, {     95,11}, \
    {     31,10}, {     63, 9}, {    143,10}, {     79, 9}, \
    {    159, 8}, {    319,10}, {    111,11}, {     63,10}, \
    {    127, 9}, {    255,10}, {    143, 9}, {    287, 8}, \
    {    575, 9}, {    303, 8}, {    607,10}, {    159, 9}, \
    {    319, 8}, {    639,11}, {     95, 9}, {    383,12}, \
    {     63,11}, {    127,10}, {    255, 9}, {    511, 8}, \
    {   1023, 9}, {    527,10}, {    271, 9}, {    543,10}, \
    {    287, 9}, {    575,10}, {    303, 9}, {    607,11}, \
    {    159,10}, {    319, 9}, {    639,10}, {    335, 9}, \
    {    671,10}, {    351, 9}, {    703,10}, {    367, 9}, \
    {    735,10}, {    383, 9}, {    767,10}, {    415, 9}, \
    {    831,11}, {    223,12}, {   4096,13}, {   8192,14}, \
    {  16384,15}, {  32768,16} }
#define SQR_FFT_TABLE3_SIZE 94
#define SQR_FFT_THRESHOLD                 2880

#define MULLO_BASECASE_THRESHOLD             7
#define MULLO_DC_THRESHOLD                  29
#define MULLO_MUL_N_THRESHOLD            10323

#define DC_DIV_QR_THRESHOLD                 25
#define DC_DIVAPPR_Q_THRESHOLD              54
#define DC_BDIV_QR_THRESHOLD                40
#define DC_BDIV_Q_THRESHOLD                 96

#define INV_MULMOD_BNM1_THRESHOLD           74
#define INV_NEWTON_THRESHOLD                25
#define INV_APPR_THRESHOLD                  25

#define BINV_NEWTON_THRESHOLD              198
#define REDC_1_TO_REDC_2_THRESHOLD           3
#define REDC_2_TO_REDC_N_THRESHOLD         142

#define MU_DIV_QR_THRESHOLD                 40
#define MU_DIVAPPR_Q_THRESHOLD            2089
#define MUPI_DIV_QR_THRESHOLD               13
#define MU_BDIV_QR_THRESHOLD              1652
#define MU_BDIV_Q_THRESHOLD               2130

#define POWM_SEC_TABLE  37,98,246,1298

#define MATRIX22_STRASSEN_THRESHOLD         18
#define HGCD_THRESHOLD                      42
#define HGCD_APPR_THRESHOLD                 56
#define HGCD_REDUCE_THRESHOLD             3134
#define GCD_DC_THRESHOLD                   315
#define GCDEXT_DC_THRESHOLD                263
#define JACOBI_BASE_METHOD                   4

#define GET_STR_DC_THRESHOLD                18
#define GET_STR_PRECOMPUTE_THRESHOLD        27
#define SET_STR_DC_THRESHOLD               136
#define SET_STR_PRECOMPUTE_THRESHOLD       960

#define FAC_DSC_THRESHOLD                  327
#define FAC_ODD_THRESHOLD                   30
