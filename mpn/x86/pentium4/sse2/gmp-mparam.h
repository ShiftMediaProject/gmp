/* Intel Pentium-4 gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 2000, 2001, 2002, 2003, 2004, 2005, 2007, 2008,
2009, 2010 Free Software Foundation, Inc.

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


#define MOD_1_NORM_THRESHOLD                 4
#define MOD_1_UNNORM_THRESHOLD               6
#define MOD_1N_TO_MOD_1_1_THRESHOLD          5
#define MOD_1U_TO_MOD_1_1_THRESHOLD          5
#define MOD_1_1_TO_MOD_1_2_THRESHOLD        10
#define MOD_1_2_TO_MOD_1_4_THRESHOLD         0  /* never mpn_mod_1s_2p */
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD      7
#define USE_PREINV_DIVREM_1                  1  /* native */
#define DIVEXACT_1_THRESHOLD                 0  /* always (native) */
#define BMOD_1_TO_MOD_1_THRESHOLD           20

#define MUL_TOOM22_THRESHOLD                36
#define MUL_TOOM33_THRESHOLD               126
#define MUL_TOOM44_THRESHOLD               312
#define MUL_TOOM6H_THRESHOLD               430
#define MUL_TOOM8H_THRESHOLD               626

#define MUL_TOOM32_TO_TOOM43_THRESHOLD     211
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     229
#define MUL_TOOM42_TO_TOOM53_THRESHOLD     204
#define MUL_TOOM42_TO_TOOM63_THRESHOLD     213

#define SQR_BASECASE_THRESHOLD               0  /* always (native) */
#define SQR_TOOM2_THRESHOLD                 49
#define SQR_TOOM3_THRESHOLD                180
#define SQR_TOOM4_THRESHOLD                482
#define SQR_TOOM6_THRESHOLD                614
#define SQR_TOOM8_THRESHOLD                933

#define MULMOD_BNM1_THRESHOLD               19
#define SQRMOD_BNM1_THRESHOLD               22

#define MUL_FFT_MODF_THRESHOLD             636  /* k = 5 */
#define MUL_FFT_TABLE3                                      \
  { {    636, 5}, {     29, 6}, {     15, 5}, {     33, 6}, \
    {     17, 5}, {     35, 6}, {     19, 5}, {     39, 6}, \
    {     29, 7}, {     15, 6}, {     33, 7}, {     17, 6}, \
    {     35, 7}, {     19, 6}, {     40, 7}, {     21, 6}, \
    {     43, 7}, {     23, 6}, {     47, 7}, {     35, 8}, \
    {     19, 7}, {     43, 8}, {     23, 7}, {     49, 8}, \
    {     27, 7}, {     55, 8}, {     31, 7}, {     63, 8}, \
    {     43, 9}, {     23, 8}, {     55, 9}, {     31, 8}, \
    {     67, 9}, {     39, 8}, {     79, 9}, {     47, 8}, \
    {     95, 9}, {     55,10}, {     31, 9}, {     79,10}, \
    {     47, 9}, {     95,11}, {     31,10}, {     63, 9}, \
    {    135,10}, {     79, 9}, {    159,10}, {     95, 9}, \
    {    191,10}, {    111,11}, {     63,10}, {    143, 9}, \
    {    287,10}, {    159,11}, {     95,10}, {    191,12}, \
    {     63,11}, {    127,10}, {    287,11}, {    159,10}, \
    {    319, 9}, {    639,10}, {    335, 9}, {    671,10}, \
    {    351,11}, {    191,10}, {    383, 9}, {    767,10}, \
    {    415,12}, {   4096,13}, {   8192,14}, {  16384,15}, \
    {  32768,16} }
#define MUL_FFT_TABLE3_SIZE 73
#define MUL_FFT_THRESHOLD                 6784

#define SQR_FFT_MODF_THRESHOLD             560  /* k = 5 */
#define SQR_FFT_TABLE3                                      \
  { {    560, 5}, {     29, 6}, {     15, 5}, {     35, 6}, \
    {     29, 7}, {     15, 6}, {     33, 7}, {     17, 6}, \
    {     36, 7}, {     19, 6}, {     41, 7}, {     21, 6}, \
    {     43, 7}, {     23, 6}, {     47, 7}, {     37, 8}, \
    {     19, 7}, {     43, 8}, {     23, 7}, {     49, 8}, \
    {     27, 7}, {     55, 8}, {     31, 7}, {     63, 8}, \
    {     43, 9}, {     23, 8}, {     55, 9}, {     31, 8}, \
    {     67, 9}, {     39, 8}, {     83, 9}, {     47, 8}, \
    {     95, 9}, {     55,10}, {     31, 9}, {     79,10}, \
    {     47, 9}, {     95,11}, {     31,10}, {     63, 9}, \
    {    127,10}, {     79, 9}, {    159,10}, {     95,11}, \
    {     63,10}, {    159,11}, {     95,10}, {    191,12}, \
    {     63,11}, {    127,10}, {    255, 9}, {    511, 8}, \
    {   1023,10}, {    287,11}, {    159,10}, {    319, 9}, \
    {    639,10}, {    335, 9}, {    671,11}, {    191,10}, \
    {    383, 9}, {    767,10}, {    399, 9}, {    799,10}, \
    {    415,12}, {   4096,13}, {   8192,14}, {  16384,15}, \
    {  32768,16} }
#define SQR_FFT_TABLE3_SIZE 69
#define SQR_FFT_THRESHOLD                 4736

#define MULLO_BASECASE_THRESHOLD             9
#define MULLO_DC_THRESHOLD                  55
#define MULLO_MUL_N_THRESHOLD            13463

#define DC_DIV_QR_THRESHOLD                 67
#define DC_DIVAPPR_Q_THRESHOLD              77
#define DC_BDIV_QR_THRESHOLD                72
#define DC_BDIV_Q_THRESHOLD                 98

#define INV_MULMOD_BNM1_THRESHOLD           70
#define INV_NEWTON_THRESHOLD               126
#define INV_APPR_THRESHOLD                  80

#define BINV_NEWTON_THRESHOLD              454
#define REDC_1_TO_REDC_N_THRESHOLD          83

#define MU_DIV_QR_THRESHOLD               2089
#define MU_DIVAPPR_Q_THRESHOLD            2089
#define MUPI_DIV_QR_THRESHOLD               66
#define MU_BDIV_QR_THRESHOLD              1787
#define MU_BDIV_Q_THRESHOLD               2304

#define MATRIX22_STRASSEN_THRESHOLD         29
#define HGCD_THRESHOLD                      99
#define GCD_DC_THRESHOLD                   263
#define GCDEXT_DC_THRESHOLD                198
#define JACOBI_BASE_METHOD                   4

#define GET_STR_DC_THRESHOLD                13
#define GET_STR_PRECOMPUTE_THRESHOLD        26
#define SET_STR_DC_THRESHOLD               102
#define SET_STR_PRECOMPUTE_THRESHOLD      1099
