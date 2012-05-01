/* gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 1999, 2000, 2001, 2002, 2003, 2009, 2010, 2012 Free
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

/* 700MHz ARM11 (raspberry pi) */

#define DIVREM_1_NORM_THRESHOLD              0  /* preinv always */
#define DIVREM_1_UNNORM_THRESHOLD            0  /* always */
#define MOD_1_1P_METHOD                      2
#define MOD_1_NORM_THRESHOLD                 0  /* always */
#define MOD_1_UNNORM_THRESHOLD               0  /* always */
#define MOD_1N_TO_MOD_1_1_THRESHOLD          3
#define MOD_1U_TO_MOD_1_1_THRESHOLD          9
#define MOD_1_1_TO_MOD_1_2_THRESHOLD        14
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        39
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD     35
#define USE_PREINV_DIVREM_1                  1  /* preinv always */
#define DIV_QR_2_PI2_THRESHOLD           MP_SIZE_T_MAX  /* never */
#define DIVEXACT_1_THRESHOLD                 0  /* always */
#define BMOD_1_TO_MOD_1_THRESHOLD        MP_SIZE_T_MAX  /* never */

#define MUL_TOOM22_THRESHOLD                38
#define MUL_TOOM33_THRESHOLD               129
#define MUL_TOOM44_THRESHOLD               312
#define MUL_TOOM6H_THRESHOLD               446
#define MUL_TOOM8H_THRESHOLD               597

#define MUL_TOOM32_TO_TOOM43_THRESHOLD     129
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     233
#define MUL_TOOM42_TO_TOOM53_THRESHOLD     204
#define MUL_TOOM42_TO_TOOM63_THRESHOLD     209
#define MUL_TOOM43_TO_TOOM54_THRESHOLD     292

#define SQR_BASECASE_THRESHOLD               0  /* always (native) */
#define SQR_TOOM2_THRESHOLD                 55
#define SQR_TOOM3_THRESHOLD                185
#define SQR_TOOM4_THRESHOLD                474
#define SQR_TOOM6_THRESHOLD                674
#define SQR_TOOM8_THRESHOLD                866

#define MULMID_TOOM42_THRESHOLD             78

#define MULMOD_BNM1_THRESHOLD               25
#define SQRMOD_BNM1_THRESHOLD               25

#define POWM_SEC_TABLE  6,47,447,1580

#define MUL_FFT_MODF_THRESHOLD             440  /* k = 5 */
#define MUL_FFT_TABLE3                                      \
  { {    440, 5}, {     21, 6}, {     11, 5}, {     25, 6}, \
    {     13, 5}, {     27, 6}, {     28, 7}, {     15, 6}, \
    {     33, 7}, {     17, 6}, {     35, 7}, {     19, 6}, \
    {     39, 7}, {     23, 6}, {     47, 7}, {     29, 8}, \
    {     15, 7}, {     35, 8}, {     19, 7}, {     41, 8}, \
    {     23, 7}, {     49, 8}, {     27, 7}, {     55, 9}, \
    {     15, 8}, {     31, 7}, {     63, 8}, {     43, 9}, \
    {     23, 8}, {     55, 9}, {     31, 8}, {     67, 9}, \
    {     39, 8}, {     83, 9}, {     47, 8}, {     95, 9}, \
    {     55,10}, {     31, 9}, {     79,10}, {     47, 9}, \
    {    512,10}, {   1024,11}, {   2048,12}, {   4096,13}, \
    {   8192,14}, {  16384,15}, {  32768,16} }
#define MUL_FFT_TABLE3_SIZE 47
#define MUL_FFT_THRESHOLD                 4736

#define SQR_FFT_MODF_THRESHOLD             440  /* k = 5 */
#define SQR_FFT_TABLE3                                      \
  { {    440, 5}, {     25, 6}, {     13, 5}, {     27, 6}, \
    {     28, 7}, {     15, 6}, {     33, 7}, {     17, 6}, \
    {     36, 7}, {     19, 6}, {     39, 7}, {     23, 6}, \
    {     47, 7}, {     29, 8}, {     15, 7}, {     35, 8}, \
    {     19, 7}, {     43, 8}, {     23, 7}, {     49, 8}, \
    {     27, 7}, {     55, 8}, {     31, 7}, {     63, 8}, \
    {     39, 7}, {     79, 8}, {     43, 9}, {     23, 8}, \
    {     55, 9}, {     31, 8}, {     71, 9}, {     39, 8}, \
    {     83, 9}, {     47, 8}, {     95, 9}, {     55,10}, \
    {     31, 9}, {     79,10}, {     47, 9}, {    512,10}, \
    {   1024,11}, {   2048,12}, {   4096,13}, {   8192,14}, \
    {  16384,15}, {  32768,16} }
#define SQR_FFT_TABLE3_SIZE 46
#define SQR_FFT_THRESHOLD                 3648

#define MULLO_BASECASE_THRESHOLD             0  /* always */
#define MULLO_DC_THRESHOLD                  66
#define MULLO_MUL_N_THRESHOLD             8907

#define DC_DIV_QR_THRESHOLD                 43
#define DC_DIVAPPR_Q_THRESHOLD             159
#define DC_BDIV_QR_THRESHOLD                84
#define DC_BDIV_Q_THRESHOLD                212

#define INV_MULMOD_BNM1_THRESHOLD           74
#define INV_NEWTON_THRESHOLD               150
#define INV_APPR_THRESHOLD                 156

#define BINV_NEWTON_THRESHOLD              372
#define REDC_1_TO_REDC_2_THRESHOLD           4
#define REDC_2_TO_REDC_N_THRESHOLD         132

#define MU_DIV_QR_THRESHOLD               2130
#define MU_DIVAPPR_Q_THRESHOLD            2089
#define MUPI_DIV_QR_THRESHOLD               73
#define MU_BDIV_QR_THRESHOLD              1718
#define MU_BDIV_Q_THRESHOLD               2130

#define MATRIX22_STRASSEN_THRESHOLD         28
#define HGCD_THRESHOLD                      90
#define HGCD_APPR_THRESHOLD                113
#define HGCD_REDUCE_THRESHOLD             3389
#define GCD_DC_THRESHOLD                   321
#define GCDEXT_DC_THRESHOLD                309
#define JACOBI_BASE_METHOD                   1

#define GET_STR_DC_THRESHOLD                23
#define GET_STR_PRECOMPUTE_THRESHOLD        46
#define SET_STR_DC_THRESHOLD               266
#define SET_STR_PRECOMPUTE_THRESHOLD      1397

#define FAC_DSC_THRESHOLD                  396
#define FAC_ODD_THRESHOLD                  145
