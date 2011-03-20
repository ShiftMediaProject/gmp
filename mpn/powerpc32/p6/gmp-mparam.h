/* PowerPC-32 gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 1999, 2000, 2001, 2002, 2003, 2004, 2008, 2009,
2010, 2011 Free Software Foundation, Inc.

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

/* 3500 MHz POWER6 */

#define DIVREM_1_NORM_THRESHOLD              0  /* always */
#define DIVREM_1_UNNORM_THRESHOLD            0  /* always */
#define MOD_1_1P_METHOD                      2
#define MOD_1_NORM_THRESHOLD                 3
#define MOD_1_UNNORM_THRESHOLD               0  /* always */
#define MOD_1N_TO_MOD_1_1_THRESHOLD          3
#define MOD_1U_TO_MOD_1_1_THRESHOLD          8
#define MOD_1_1_TO_MOD_1_2_THRESHOLD         0  /* never mpn_mod_1_1p */
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        15
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD      9
#define USE_PREINV_DIVREM_1                  1
#define DIVEXACT_1_THRESHOLD                 0  /* always */
#define BMOD_1_TO_MOD_1_THRESHOLD        MP_SIZE_T_MAX  /* never */

#define MUL_TOOM22_THRESHOLD                34
#define MUL_TOOM33_THRESHOLD                70
#define MUL_TOOM44_THRESHOLD               187
#define MUL_TOOM6H_THRESHOLD               286
#define MUL_TOOM8H_THRESHOLD               321

#define MUL_TOOM32_TO_TOOM43_THRESHOLD     110
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     118
#define MUL_TOOM42_TO_TOOM53_THRESHOLD     107
#define MUL_TOOM42_TO_TOOM63_THRESHOLD     145

#define SQR_BASECASE_THRESHOLD               0  /* always */
#define SQR_TOOM2_THRESHOLD                 68
#define SQR_TOOM3_THRESHOLD                113
#define SQR_TOOM4_THRESHOLD                312
#define SQR_TOOM6_THRESHOLD                330
#define SQR_TOOM8_THRESHOLD                357

#define MULMOD_BNM1_THRESHOLD               19
#define SQRMOD_BNM1_THRESHOLD               20

#define MUL_FFT_MODF_THRESHOLD             304  /* k = 5 */
#define MUL_FFT_TABLE3                                      \
  { {    304, 5}, {     17, 6}, {      9, 5}, {     19, 6}, \
    {     10, 5}, {     21, 6}, {     17, 7}, {      9, 6}, \
    {     20, 7}, {     11, 6}, {     24, 7}, {     13, 8}, \
    {      7, 7}, {     21, 8}, {     11, 7}, {     27, 9}, \
    {      7, 8}, {     15, 7}, {     33, 8}, {     19, 7}, \
    {     41, 8}, {     23, 7}, {     47, 8}, {     27, 9}, \
    {     15, 8}, {     39, 9}, {     23, 8}, {     47,10}, \
    {     15, 9}, {     31, 8}, {     67, 9}, {     39, 8}, \
    {     79, 9}, {     47, 8}, {     95,10}, {     31, 9}, \
    {     79,10}, {     47, 9}, {     95,11}, {     31,10}, \
    {     63, 9}, {    127, 8}, {    255, 9}, {    135,10}, \
    {     79, 9}, {    159, 8}, {    319,10}, {     95, 9}, \
    {    191, 8}, {    383,11}, {     63,10}, {    127, 9}, \
    {    255, 8}, {    511, 9}, {    271,10}, {    143, 9}, \
    {    287,10}, {    159, 9}, {    319,11}, {     95,10}, \
    {    191, 9}, {    383,12}, {     63,11}, {    127,10}, \
    {    255, 9}, {    511,10}, {    271, 9}, {    543,10}, \
    {    287,11}, {    159,10}, {    319, 9}, {    639,10}, \
    {    351,11}, {    191,10}, {    383, 9}, {    767,10}, \
    {    415,11}, {    223,10}, {    447,12}, {   4096,13}, \
    {   8192,14}, {  16384,15}, {  32768,16} }
#define MUL_FFT_TABLE3_SIZE 83
#define MUL_FFT_THRESHOLD                 4736

#define SQR_FFT_MODF_THRESHOLD             312  /* k = 5 */
#define SQR_FFT_TABLE3                                      \
  { {    312, 5}, {     21, 6}, {     11, 5}, {     23, 6}, \
    {     21, 7}, {     11, 6}, {     24, 7}, {     13, 6}, \
    {     27, 7}, {     17, 6}, {     35, 7}, {     21, 8}, \
    {     11, 7}, {     27, 8}, {     15, 7}, {     33, 8}, \
    {     19, 7}, {     39, 8}, {     23, 7}, {     47, 8}, \
    {     27, 9}, {     15, 8}, {     39, 9}, {     23, 8}, \
    {     47,10}, {     15, 9}, {     31, 8}, {     67, 9}, \
    {     39, 8}, {     79, 9}, {     47,10}, {     31, 9}, \
    {     79,10}, {     47,11}, {     31,10}, {     63, 9}, \
    {    127, 8}, {    255,10}, {     79, 9}, {    159, 8}, \
    {    319,10}, {     95, 9}, {    191,11}, {     63,10}, \
    {    127, 9}, {    255, 8}, {    511, 9}, {    271,10}, \
    {    143, 9}, {    287, 8}, {    575,10}, {    159, 9}, \
    {    319,11}, {     95,10}, {    191, 9}, {    383,12}, \
    {     63,11}, {    127,10}, {    255, 9}, {    511,10}, \
    {    271, 9}, {    543,10}, {    287, 9}, {    575,11}, \
    {    159,10}, {    319, 9}, {    639,10}, {    351,11}, \
    {    191,10}, {    383, 9}, {    767,10}, {    415,11}, \
    {    223,10}, {    447,12}, {   4096,13}, {   8192,14}, \
    {  16384,15}, {  32768,16} }
#define SQR_FFT_TABLE3_SIZE 78
#define SQR_FFT_THRESHOLD                 2752

#define MULLO_BASECASE_THRESHOLD             0  /* always */
#define MULLO_DC_THRESHOLD                 151
#define MULLO_MUL_N_THRESHOLD             1175

#define DC_DIV_QR_THRESHOLD                133
#define DC_DIVAPPR_Q_THRESHOLD             442
#define DC_BDIV_QR_THRESHOLD               130
#define DC_BDIV_Q_THRESHOLD                324

#define INV_MULMOD_BNM1_THRESHOLD          116
#define INV_NEWTON_THRESHOLD               507
#define INV_APPR_THRESHOLD                 454

#define BINV_NEWTON_THRESHOLD              507
#define REDC_1_TO_REDC_N_THRESHOLD         118

#define MU_DIV_QR_THRESHOLD               1652
#define MU_DIVAPPR_Q_THRESHOLD            1752
#define MUPI_DIV_QR_THRESHOLD              225
#define MU_BDIV_QR_THRESHOLD               762
#define MU_BDIV_Q_THRESHOLD               1017

#define MATRIX22_STRASSEN_THRESHOLD         28
#define HGCD_THRESHOLD                      76
#define GCD_DC_THRESHOLD                   333
#define GCDEXT_DC_THRESHOLD                245
#define JACOBI_BASE_METHOD                   4

#define GET_STR_DC_THRESHOLD                10
#define GET_STR_PRECOMPUTE_THRESHOLD        20
#define SET_STR_DC_THRESHOLD               199
#define SET_STR_PRECOMPUTE_THRESHOLD       478
