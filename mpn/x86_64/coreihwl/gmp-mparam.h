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
#define MUL_TOOM33_THRESHOLD                66
#define MUL_TOOM44_THRESHOLD               193
#define MUL_TOOM6H_THRESHOLD               274
#define MUL_TOOM8H_THRESHOLD               381

#define MUL_TOOM32_TO_TOOM43_THRESHOLD     121
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     131
#define MUL_TOOM42_TO_TOOM53_THRESHOLD     127
#define MUL_TOOM42_TO_TOOM63_THRESHOLD     131
#define MUL_TOOM43_TO_TOOM54_THRESHOLD     178

#define SQR_BASECASE_THRESHOLD               0  /* always (native) */
#define SQR_TOOM2_THRESHOLD                 34
#define SQR_TOOM3_THRESHOLD                117
#define SQR_TOOM4_THRESHOLD                336
#define SQR_TOOM6_THRESHOLD                422
#define SQR_TOOM8_THRESHOLD                562

#define MULMID_TOOM42_THRESHOLD             44

#define MULMOD_BNM1_THRESHOLD               14
#define SQRMOD_BNM1_THRESHOLD               17

#define MUL_FFT_MODF_THRESHOLD             376  /* k = 5 */
#define MUL_FFT_TABLE3                                      \
  { {    376, 5}, {     19, 6}, {     10, 5}, {     21, 6}, \
    {     11, 5}, {     23, 6}, {     21, 7}, {     11, 6}, \
    {     25, 7}, {     13, 6}, {     27, 7}, {     21, 8}, \
    {     11, 7}, {     25, 8}, {     13, 7}, {     27, 8}, \
    {     15, 7}, {     31, 8}, {     21, 9}, {     11, 8}, \
    {     27, 9}, {     15, 8}, {     33, 9}, {     19, 8}, \
    {     39, 9}, {     23, 8}, {     49, 9}, {     27,10}, \
    {     15, 9}, {     39,10}, {     23, 9}, {     51,11}, \
    {     15,10}, {     31, 9}, {     71,10}, {     39, 9}, \
    {     83,10}, {     47, 9}, {     95,11}, {     31,10}, \
    {     79,11}, {     47,10}, {     95,12}, {     31,11}, \
    {     63,10}, {    127, 9}, {    255,10}, {    135,11}, \
    {     79,10}, {    159, 9}, {    319,10}, {    167,11}, \
    {     95,10}, {    191, 9}, {    383,11}, {    111,12}, \
    {     63,11}, {    127, 9}, {    511,10}, {    271,11}, \
    {    143,10}, {    287, 9}, {    575,10}, {    303, 9}, \
    {    607,11}, {    159,10}, {    319,12}, {     95,11}, \
    {    191,10}, {    383,11}, {    207,13}, {   8192,14}, \
    {  16384,15}, {  32768,16}, {  65536,17}, { 131072,18}, \
    { 262144,19}, { 524288,20}, {1048576,21}, {2097152,22}, \
    {4194304,23}, {8388608,24} }
#define MUL_FFT_TABLE3_SIZE 82
#define MUL_FFT_THRESHOLD                 4224

#define SQR_FFT_MODF_THRESHOLD             340  /* k = 5 */
#define SQR_FFT_TABLE3                                      \
  { {    340, 5}, {     21, 6}, {     11, 5}, {     23, 6}, \
    {     25, 7}, {     13, 6}, {     27, 7}, {     25, 8}, \
    {     13, 7}, {     28, 8}, {     15, 7}, {     31, 8}, \
    {     21, 9}, {     11, 8}, {     27, 9}, {     15, 8}, \
    {     35, 9}, {     19, 8}, {     41, 9}, {     23, 8}, \
    {     47, 9}, {     27,10}, {     15, 9}, {     39,10}, \
    {     23, 9}, {     51,11}, {     15,10}, {     31, 9}, \
    {     67,10}, {     39, 9}, {     79,10}, {     55,11}, \
    {     31,10}, {     79,11}, {     47,10}, {     95,12}, \
    {     31,11}, {     63,10}, {    127, 9}, {    255,10}, \
    {    135,11}, {     79,10}, {    159, 9}, {    319,11}, \
    {     95,10}, {    191, 9}, {    383,12}, {     63,11}, \
    {    127,10}, {    255, 9}, {    511,10}, {    271, 9}, \
    {    543,11}, {    143,10}, {    287, 9}, {    575,10}, \
    {    303, 9}, {    607,10}, {    319, 9}, {    639,12}, \
    {     95,11}, {    191,10}, {    383, 9}, {    767,11}, \
    {    207,13}, {   8192,14}, {  16384,15}, {  32768,16}, \
    {  65536,17}, { 131072,18}, { 262144,19}, { 524288,20}, \
    {1048576,21}, {2097152,22}, {4194304,23}, {8388608,24} }
#define SQR_FFT_TABLE3_SIZE 76
#define SQR_FFT_THRESHOLD                 3264

#define MULLO_BASECASE_THRESHOLD             0  /* always */
#define MULLO_DC_THRESHOLD                  61
#define MULLO_MUL_N_THRESHOLD             8397

#define DC_DIV_QR_THRESHOLD                 60
#define DC_DIVAPPR_Q_THRESHOLD             195
#define DC_BDIV_QR_THRESHOLD                60
#define DC_BDIV_Q_THRESHOLD                152

#define INV_MULMOD_BNM1_THRESHOLD           42
#define INV_NEWTON_THRESHOLD               202
#define INV_APPR_THRESHOLD                 197

#define BINV_NEWTON_THRESHOLD              236
#define REDC_1_TO_REDC_2_THRESHOLD          48
#define REDC_2_TO_REDC_N_THRESHOLD          72

#define MU_DIV_QR_THRESHOLD               1470
#define MU_DIVAPPR_Q_THRESHOLD            1589
#define MUPI_DIV_QR_THRESHOLD               82
#define MU_BDIV_QR_THRESHOLD              1442
#define MU_BDIV_Q_THRESHOLD               1470

#define POWM_SEC_TABLE  4,20,219,572,905

#define MATRIX22_STRASSEN_THRESHOLD         17
#define HGCD_THRESHOLD                     105
#define HGCD_APPR_THRESHOLD                 50
#define HGCD_REDUCE_THRESHOLD             2479
#define GCD_DC_THRESHOLD                   812
#define GCDEXT_DC_THRESHOLD                435
#define JACOBI_BASE_METHOD                   4

#define GET_STR_DC_THRESHOLD                12
#define GET_STR_PRECOMPUTE_THRESHOLD        19
#define SET_STR_DC_THRESHOLD              1562
#define SET_STR_PRECOMPUTE_THRESHOLD      2649

#define FAC_DSC_THRESHOLD                  692
#define FAC_ODD_THRESHOLD                    0  /* always */
