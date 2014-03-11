/* Intel Pentium-4 gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 2000-2005, 2007-2010 Free Software Foundation, Inc.

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


#define MOD_1_NORM_THRESHOLD                17
#define MOD_1_UNNORM_THRESHOLD           MP_SIZE_T_MAX  /* never */
#define MOD_1N_TO_MOD_1_1_THRESHOLD          6
#define MOD_1U_TO_MOD_1_1_THRESHOLD          5
#define MOD_1_1_TO_MOD_1_2_THRESHOLD        12
#define MOD_1_2_TO_MOD_1_4_THRESHOLD         0  /* never mpn_mod_1s_2p */
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD      2
#define USE_PREINV_DIVREM_1                  1  /* native */
#define DIV_QR_1N_PI1_METHOD                 2
#define DIV_QR_1_NORM_THRESHOLD             19
#define DIV_QR_1_UNNORM_THRESHOLD        MP_SIZE_T_MAX  /* never */
#define DIV_QR_2_PI2_THRESHOLD           MP_SIZE_T_MAX  /* never */
#define DIVEXACT_1_THRESHOLD                 0  /* always (native) */
#define BMOD_1_TO_MOD_1_THRESHOLD           20

#define MUL_TOOM22_THRESHOLD                35
#define MUL_TOOM33_THRESHOLD               130
#define MUL_TOOM44_THRESHOLD               288
#define MUL_TOOM6H_THRESHOLD               402
#define MUL_TOOM8H_THRESHOLD               592

#define MUL_TOOM32_TO_TOOM43_THRESHOLD     198
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     214
#define MUL_TOOM42_TO_TOOM53_THRESHOLD     193
#define MUL_TOOM42_TO_TOOM63_THRESHOLD     211
#define MUL_TOOM43_TO_TOOM54_THRESHOLD     287

#define SQR_BASECASE_THRESHOLD               0  /* always (native) */
#define SQR_TOOM2_THRESHOLD                 48
#define SQR_TOOM3_THRESHOLD                169
#define SQR_TOOM4_THRESHOLD                375
#define SQR_TOOM6_THRESHOLD                  0  /* always */
#define SQR_TOOM8_THRESHOLD                874

#define MULMID_TOOM42_THRESHOLD             65

#define MULMOD_BNM1_THRESHOLD               19
#define SQRMOD_BNM1_THRESHOLD               22

#define MUL_FFT_MODF_THRESHOLD            1125  /* k = 6 */
#define MUL_FFT_TABLE3                                      \
  { {   1125, 6}, {     19, 5}, {     39, 6}, {     27, 7}, \
    {     15, 6}, {     33, 7}, {     17, 6}, {     35, 7}, \
    {     19, 6}, {     39, 7}, {     21, 6}, {     43, 7}, \
    {     23, 6}, {     49, 7}, {     27, 6}, {     55, 7}, \
    {     31, 6}, {     63, 7}, {     43, 8}, {     23, 7}, \
    {     51, 8}, {     27, 7}, {     55, 8}, {     31, 7}, \
    {     63, 8}, {     39, 7}, {     79, 8}, {     43, 9}, \
    {     23, 8}, {     55, 9}, {     31, 8}, {     71, 9}, \
    {     39, 8}, {     79, 9}, {     47, 8}, {     95, 9}, \
    {     55,10}, {     31, 9}, {     63, 8}, {    127, 9}, \
    {     79,10}, {     47, 9}, {    111,11}, {     31,10}, \
    {     63, 9}, {    143,10}, {     79, 9}, {    159,10}, \
    {     95, 9}, {    191,10}, {    111,11}, {     63,10}, \
    {    127, 9}, {    255,10}, {    159, 9}, {    319,10}, \
    {    175,11}, {     95,10}, {    223,12}, {     63,11}, \
    {    127,10}, {    287,11}, {    159,10}, {    319, 9}, \
    {    639,11}, {    191,10}, {    383,11}, {    223,12}, \
    {   4096,13}, {   8192,14}, {  16384,15}, {  32768,16} }
#define MUL_FFT_TABLE3_SIZE 72
#define MUL_FFT_THRESHOLD                 7808

#define SQR_FFT_MODF_THRESHOLD             864  /* k = 5 */
#define SQR_FFT_TABLE3                                      \
  { {    864, 5}, {     28, 6}, {     15, 5}, {     32, 6}, \
    {     17, 5}, {     35, 6}, {     27, 7}, {     15, 6}, \
    {     33, 7}, {     17, 6}, {     36, 7}, {     19, 6}, \
    {     41, 7}, {     23, 6}, {     47, 7}, {     27, 6}, \
    {     55, 8}, {     15, 7}, {     31, 6}, {     63, 7}, \
    {     37, 8}, {     19, 7}, {     41, 8}, {     23, 7}, \
    {     51, 8}, {     31, 7}, {     63, 8}, {     35, 7}, \
    {     71, 8}, {     43, 9}, {     23, 8}, {     55, 9}, \
    {     31, 8}, {     71, 9}, {     39, 8}, {     79, 9}, \
    {     47, 8}, {     95, 9}, {     55,10}, {     31, 9}, \
    {     79,10}, {     47, 9}, {    103,11}, {     31,10}, \
    {     63, 9}, {    127,10}, {     79, 9}, {    159,10}, \
    {     95, 9}, {    191,11}, {     63,10}, {    127, 9}, \
    {    255,10}, {    143,11}, {     95,10}, {    191,12}, \
    {     63,11}, {    127,10}, {    287,11}, {    159,10}, \
    {    319, 9}, {    639,10}, {    335,11}, {    191,12}, \
    {   4096,13}, {   8192,14}, {  16384,15}, {  32768,16} }
#define SQR_FFT_TABLE3_SIZE 68
#define SQR_FFT_THRESHOLD                 7296

#define MULLO_BASECASE_THRESHOLD            15
#define MULLO_DC_THRESHOLD                  50
#define MULLO_MUL_N_THRESHOLD            14281

#define DC_DIV_QR_THRESHOLD                 37
#define DC_DIVAPPR_Q_THRESHOLD              77
#define DC_BDIV_QR_THRESHOLD                50
#define DC_BDIV_Q_THRESHOLD                 85

#define INV_MULMOD_BNM1_THRESHOLD           69
#define INV_NEWTON_THRESHOLD               165
#define INV_APPR_THRESHOLD                 100

#define BINV_NEWTON_THRESHOLD              348
#define REDC_1_TO_REDC_N_THRESHOLD          64

#define MU_DIV_QR_THRESHOLD               2350
#define MU_DIVAPPR_Q_THRESHOLD            2172
#define MUPI_DIV_QR_THRESHOLD               72
#define MU_BDIV_QR_THRESHOLD              2172
#define MU_BDIV_Q_THRESHOLD               2350

#define POWM_SEC_TABLE  1,22,102,615,2111

#define MATRIX22_STRASSEN_THRESHOLD         25
#define HGCD_THRESHOLD                      84
#define HGCD_APPR_THRESHOLD                 93
#define HGCD_REDUCE_THRESHOLD             5859
#define GCD_DC_THRESHOLD                   321
#define GCDEXT_DC_THRESHOLD                225
#define JACOBI_BASE_METHOD                   4

#define GET_STR_DC_THRESHOLD                11
#define GET_STR_PRECOMPUTE_THRESHOLD        24
#define SET_STR_DC_THRESHOLD               126
#define SET_STR_PRECOMPUTE_THRESHOLD      1057

#define FAC_DSC_THRESHOLD                  795
#define FAC_ODD_THRESHOLD                   24
