/* AMD K7 gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 2000-2005, 2008-2010 Free Software Foundation, Inc.

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


#define MOD_1_NORM_THRESHOLD                 0  /* always */
#define MOD_1_UNNORM_THRESHOLD               3
#define MOD_1N_TO_MOD_1_1_THRESHOLD          7
#define MOD_1U_TO_MOD_1_1_THRESHOLD          3
#define MOD_1_1_TO_MOD_1_2_THRESHOLD        24
#define MOD_1_2_TO_MOD_1_4_THRESHOLD         0  /* never mpn_mod_1s_2p */
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD     10
#define USE_PREINV_DIVREM_1                  1  /* native */
#define DIV_QR_1N_PI1_METHOD                 1
#define DIV_QR_1_NORM_THRESHOLD              3
#define DIV_QR_1_UNNORM_THRESHOLD        MP_SIZE_T_MAX  /* never */
#define DIV_QR_2_PI2_THRESHOLD           MP_SIZE_T_MAX  /* never */
#define DIVEXACT_1_THRESHOLD                 0  /* always (native) */
#define BMOD_1_TO_MOD_1_THRESHOLD           24

#define MUL_TOOM22_THRESHOLD                28
#define MUL_TOOM33_THRESHOLD                85
#define MUL_TOOM44_THRESHOLD               148
#define MUL_TOOM6H_THRESHOLD               214
#define MUL_TOOM8H_THRESHOLD               309

#define MUL_TOOM32_TO_TOOM43_THRESHOLD      85
#define MUL_TOOM32_TO_TOOM53_THRESHOLD      95
#define MUL_TOOM42_TO_TOOM53_THRESHOLD      97
#define MUL_TOOM42_TO_TOOM63_THRESHOLD     102
#define MUL_TOOM43_TO_TOOM54_THRESHOLD     142

#define SQR_BASECASE_THRESHOLD               0  /* always (native) */
#define SQR_TOOM2_THRESHOLD                 50
#define SQR_TOOM3_THRESHOLD                 83
#define SQR_TOOM4_THRESHOLD                218
#define SQR_TOOM6_THRESHOLD                306
#define SQR_TOOM8_THRESHOLD                454

#define MULMID_TOOM42_THRESHOLD             56

#define MULMOD_BNM1_THRESHOLD               17
#define SQRMOD_BNM1_THRESHOLD               19

#define MUL_FFT_MODF_THRESHOLD             904  /* k = 6 */
#define MUL_FFT_TABLE3                                      \
  { {    904, 6}, {     25, 7}, {     13, 6}, {     27, 7}, \
    {     15, 6}, {     31, 7}, {     17, 6}, {     35, 7}, \
    {     19, 6}, {     39, 7}, {     23, 6}, {     47, 7}, \
    {     27, 8}, {     15, 7}, {     31, 6}, {     63, 7}, \
    {     35, 8}, {     19, 7}, {     39, 8}, {     23, 7}, \
    {     47, 8}, {     31, 7}, {     63, 8}, {     39, 9}, \
    {     23, 8}, {     47, 7}, {     95, 8}, {     51, 9}, \
    {     31, 8}, {     67, 9}, {     39, 8}, {     79, 9}, \
    {     47, 8}, {     95, 9}, {     55,10}, {     31, 9}, \
    {     63, 8}, {    127, 9}, {     71, 8}, {    143, 9}, \
    {     79,10}, {     47, 9}, {     95, 8}, {    191, 9}, \
    {    103,11}, {     31,10}, {     63, 9}, {    127, 8}, \
    {    255, 9}, {    143,10}, {     79, 9}, {    167,10}, \
    {     95, 9}, {    199,10}, {    111,11}, {     63,10}, \
    {    127, 9}, {    255,10}, {    143, 9}, {    287,10}, \
    {    159, 9}, {    319,11}, {     95,10}, {    191, 9}, \
    {    383,12}, {     63,11}, {    127,10}, {    271, 9}, \
    {    543, 8}, {   1087,10}, {    287,11}, {    159,10}, \
    {    319, 9}, {    639, 8}, {   1279,11}, {    191,10}, \
    {    383, 9}, {    799, 8}, {   1599, 9}, {    831,11}, \
    {    223,12}, {   4096,13}, {   8192,14}, {  16384,15}, \
    {  32768,16} }
#define MUL_FFT_TABLE3_SIZE 85
#define MUL_FFT_THRESHOLD                 7808

#define SQR_FFT_MODF_THRESHOLD             888  /* k = 6 */
#define SQR_FFT_TABLE3                                      \
  { {    888, 6}, {     21, 7}, {     11, 6}, {     25, 7}, \
    {     13, 6}, {     27, 7}, {     15, 6}, {     31, 7}, \
    {     17, 6}, {     35, 7}, {     19, 6}, {     39, 7}, \
    {     23, 6}, {     47, 7}, {     27, 8}, {     15, 7}, \
    {     31, 6}, {     63, 7}, {     35, 8}, {     19, 7}, \
    {     39, 8}, {     23, 7}, {     47, 8}, {     27, 9}, \
    {     15, 8}, {     31, 7}, {     63, 8}, {     39, 9}, \
    {     23, 8}, {     47, 7}, {     95, 8}, {     51, 9}, \
    {     31, 8}, {     67, 9}, {     39, 8}, {     79, 9}, \
    {     47, 8}, {     95,10}, {     31, 9}, {     63, 8}, \
    {    127, 9}, {     79,10}, {     47, 9}, {    103,11}, \
    {     31,10}, {     63, 9}, {    143,10}, {     79, 9}, \
    {    159,10}, {     95, 9}, {    191,10}, {    111,11}, \
    {     63,10}, {    127, 9}, {    255, 8}, {    511,10}, \
    {    143, 9}, {    287, 8}, {    575,10}, {    159,11}, \
    {     95,10}, {    191, 8}, {    767,12}, {     63,11}, \
    {    127,10}, {    255, 9}, {    511, 8}, {   1023, 9}, \
    {    543, 8}, {   1087,10}, {    287, 9}, {    575,11}, \
    {    159, 9}, {    639, 8}, {   1279, 9}, {    671, 8}, \
    {   1407,11}, {    191, 9}, {    767, 8}, {   1599, 9}, \
    {    831, 8}, {   1663,11}, {    223,12}, {   4096,13}, \
    {   8192,14}, {  16384,15}, {  32768,16} }
#define SQR_FFT_TABLE3_SIZE 87
#define SQR_FFT_THRESHOLD                 7296

#define MULLO_BASECASE_THRESHOLD            11
#define MULLO_DC_THRESHOLD                  35
#define MULLO_MUL_N_THRESHOLD            13463

#define DC_DIV_QR_THRESHOLD                 41
#define DC_DIVAPPR_Q_THRESHOLD             204
#define DC_BDIV_QR_THRESHOLD                39
#define DC_BDIV_Q_THRESHOLD                144

#define INV_MULMOD_BNM1_THRESHOLD           62
#define INV_NEWTON_THRESHOLD               204
#define INV_APPR_THRESHOLD                 204

#define BINV_NEWTON_THRESHOLD              228
#define REDC_1_TO_REDC_N_THRESHOLD          59

#define MU_DIV_QR_THRESHOLD               1752
#define MU_DIVAPPR_Q_THRESHOLD            1470
#define MUPI_DIV_QR_THRESHOLD               82
#define MU_BDIV_QR_THRESHOLD              1360
#define MU_BDIV_Q_THRESHOLD               1470

#define POWM_SEC_TABLE  1,16,114,289,1185

#define MATRIX22_STRASSEN_THRESHOLD         16
#define HGCD_THRESHOLD                     120
#define HGCD_APPR_THRESHOLD                142
#define HGCD_REDUCE_THRESHOLD             4818
#define GCD_DC_THRESHOLD                   448
#define GCDEXT_DC_THRESHOLD                333
#define JACOBI_BASE_METHOD                   4

#define GET_STR_DC_THRESHOLD                14
#define GET_STR_PRECOMPUTE_THRESHOLD        32
#define SET_STR_DC_THRESHOLD               324
#define SET_STR_PRECOMPUTE_THRESHOLD      1105

#define FAC_DSC_THRESHOLD                  602
#define FAC_ODD_THRESHOLD                   28
