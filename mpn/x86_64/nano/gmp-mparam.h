/* VIA Nano gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 2000-2010, 2012 Free Software Foundation, Inc.

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

#define SHLD_SLOW 1
#define SHRD_SLOW 1

/* 1600 MHz Nano 2xxx */

#define MOD_1_NORM_THRESHOLD                 0  /* always */
#define MOD_1_UNNORM_THRESHOLD               0  /* always */
#define MOD_1N_TO_MOD_1_1_THRESHOLD          4
#define MOD_1U_TO_MOD_1_1_THRESHOLD          2
#define MOD_1_1_TO_MOD_1_2_THRESHOLD        18
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        20
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD      8
#define USE_PREINV_DIVREM_1                  1  /* native */
#define DIV_QR_1_NORM_THRESHOLD              1
#define DIV_QR_1_UNNORM_THRESHOLD        MP_SIZE_T_MAX  /* never */
#define DIV_QR_2_PI2_THRESHOLD           MP_SIZE_T_MAX  /* never */
#define DIVEXACT_1_THRESHOLD                 0  /* always (native) */
#define BMOD_1_TO_MOD_1_THRESHOLD           22

#define MUL_TOOM22_THRESHOLD                27
#define MUL_TOOM33_THRESHOLD                38
#define MUL_TOOM44_THRESHOLD               324
#define MUL_TOOM6H_THRESHOLD               450
#define MUL_TOOM8H_THRESHOLD               638

#define MUL_TOOM32_TO_TOOM43_THRESHOLD      73
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     207
#define MUL_TOOM42_TO_TOOM53_THRESHOLD     211
#define MUL_TOOM42_TO_TOOM63_THRESHOLD     217
#define MUL_TOOM43_TO_TOOM54_THRESHOLD     315

#define SQR_BASECASE_THRESHOLD              10
#define SQR_TOOM2_THRESHOLD                 52
#define SQR_TOOM3_THRESHOLD                 75
#define SQR_TOOM4_THRESHOLD                414
#define SQR_TOOM6_THRESHOLD                650
#define SQR_TOOM8_THRESHOLD                753

#define MULMID_TOOM42_THRESHOLD             32

#define MULMOD_BNM1_THRESHOLD               13
#define SQRMOD_BNM1_THRESHOLD               15

#define MUL_FFT_MODF_THRESHOLD             376  /* k = 5 */
#define MUL_FFT_TABLE3                                      \
  { {    376, 5}, {     19, 6}, {     10, 5}, {     21, 6}, \
    {     23, 7}, {     12, 6}, {     25, 7}, {     21, 8}, \
    {     11, 7}, {     24, 8}, {     13, 7}, {     27, 8}, \
    {     15, 7}, {     31, 8}, {     17, 7}, {     35, 8}, \
    {     19, 7}, {     39, 8}, {     21, 9}, {     11, 8}, \
    {     27, 9}, {     15, 8}, {     35, 9}, {     19, 8}, \
    {     41, 9}, {     23, 8}, {     49, 9}, {     27,10}, \
    {     15, 9}, {     43,10}, {     23, 9}, {     55,11}, \
    {     15,10}, {     31, 9}, {     67,10}, {     39, 9}, \
    {     83,10}, {     47, 9}, {     95,10}, {     55,11}, \
    {     31,10}, {     63, 9}, {    127,10}, {     79,11}, \
    {     47,10}, {    103,12}, {     31,11}, {     63,10}, \
    {    135, 9}, {    271, 8}, {    543,10}, {    143,11}, \
    {     79,10}, {    167,11}, {     95,10}, {    191, 9}, \
    {    383, 8}, {    767,10}, {    207,11}, {    111,12}, \
    {     63,11}, {    127,10}, {    271, 9}, {    543,11}, \
    {    143, 9}, {    575, 8}, {   1151,11}, {    159,10}, \
    {    319, 9}, {    639, 8}, {   1279,11}, {    175,12}, \
    {     95,11}, {    191,10}, {    383, 9}, {    767,11}, \
    {    207,10}, {    415, 9}, {    831,13}, {   8192,14}, \
    {  16384,15}, {  32768,16}, {  65536,17}, { 131072,18}, \
    { 262144,19}, { 524288,20}, {1048576,21}, {2097152,22}, \
    {4194304,23}, {8388608,24} }
#define MUL_FFT_TABLE3_SIZE 90
#define MUL_FFT_THRESHOLD                 3520

#define SQR_FFT_MODF_THRESHOLD             340  /* k = 5 */
#define SQR_FFT_TABLE3                                      \
  { {    340, 5}, {     19, 6}, {     10, 5}, {     21, 6}, \
    {     21, 7}, {     11, 6}, {     23, 7}, {     21, 8}, \
    {     11, 7}, {     25, 8}, {     13, 7}, {     27, 8}, \
    {     15, 7}, {     31, 8}, {     21, 9}, {     11, 8}, \
    {     27, 9}, {     15, 8}, {     35, 9}, {     19, 8}, \
    {     41, 9}, {     23, 8}, {     47, 9}, {     27,10}, \
    {     15, 9}, {     39,10}, {     23, 9}, {     51,11}, \
    {     15,10}, {     31, 9}, {     67,10}, {     39, 9}, \
    {     79,10}, {     47, 9}, {     95,10}, {     55,11}, \
    {     31,10}, {     63, 9}, {    127,10}, {     79,11}, \
    {     47,10}, {     95, 8}, {    383,10}, {    103,12}, \
    {     31,11}, {     63,10}, {    127, 9}, {    255, 8}, \
    {    511,10}, {    143, 9}, {    287,11}, {     79, 9}, \
    {    319, 8}, {    639, 9}, {    335,10}, {    175,11}, \
    {     95, 9}, {    383, 8}, {    767,11}, {    111, 9}, \
    {    447,12}, {     63,11}, {    127,10}, {    255, 9}, \
    {    511, 8}, {   1023,10}, {    271, 9}, {    543, 8}, \
    {   1087,11}, {    143, 9}, {    575, 8}, {   1151, 9}, \
    {    607, 8}, {   1215,11}, {    159,10}, {    319, 9}, \
    {    639, 8}, {   1279,11}, {    175,12}, {     95,11}, \
    {    191,10}, {    383, 9}, {    767,11}, {    207,10}, \
    {    415, 9}, {    831,11}, {    223,10}, {    447,13}, \
    {   8192,14}, {  16384,15}, {  32768,16}, {  65536,17}, \
    { 131072,18}, { 262144,19}, { 524288,20}, {1048576,21}, \
    {2097152,22}, {4194304,23}, {8388608,24} }
#define SQR_FFT_TABLE3_SIZE 99
#define SQR_FFT_THRESHOLD                 2496

#define MULLO_BASECASE_THRESHOLD            14
#define MULLO_DC_THRESHOLD                  38
#define MULLO_MUL_N_THRESHOLD             6633

#define DC_DIV_QR_THRESHOLD                 56
#define DC_DIVAPPR_Q_THRESHOLD             171
#define DC_BDIV_QR_THRESHOLD                54
#define DC_BDIV_Q_THRESHOLD                107

#define INV_MULMOD_BNM1_THRESHOLD           53
#define INV_NEWTON_THRESHOLD               204
#define INV_APPR_THRESHOLD                 171

#define BINV_NEWTON_THRESHOLD              246
#define REDC_1_TO_REDC_2_THRESHOLD           9
#define REDC_2_TO_REDC_N_THRESHOLD          85

#define MU_DIV_QR_THRESHOLD               1499
#define MU_DIVAPPR_Q_THRESHOLD            1620
#define MUPI_DIV_QR_THRESHOLD               83
#define MU_BDIV_QR_THRESHOLD              1210
#define MU_BDIV_Q_THRESHOLD               1499

#define POWM_SEC_TABLE  1,28,131,642,2280

#define MATRIX22_STRASSEN_THRESHOLD         15
#define HGCD_THRESHOLD                     137
#define HGCD_APPR_THRESHOLD                196
#define HGCD_REDUCE_THRESHOLD             2479
#define GCD_DC_THRESHOLD                   535
#define GCDEXT_DC_THRESHOLD                496
#define JACOBI_BASE_METHOD                   4

#define GET_STR_DC_THRESHOLD                12
#define GET_STR_PRECOMPUTE_THRESHOLD        24
#define SET_STR_DC_THRESHOLD               767
#define SET_STR_PRECOMPUTE_THRESHOLD      1941

#define FAC_DSC_THRESHOLD                 1737
#define FAC_ODD_THRESHOLD                   44
