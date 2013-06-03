/* Sandy Bridge gmp-mparam.h -- Compiler/machine parameter header file.

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

/* 3300 MHz Core i5 Sandy Bridge */

#define MOD_1_NORM_THRESHOLD                 0  /* always */
#define MOD_1_UNNORM_THRESHOLD               0  /* always */
#define MOD_1N_TO_MOD_1_1_THRESHOLD          3
#define MOD_1U_TO_MOD_1_1_THRESHOLD          2
#define MOD_1_1_TO_MOD_1_2_THRESHOLD         9
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        22
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD      6
#define USE_PREINV_DIVREM_1                  1  /* native */
#define DIV_QR_2_PI2_THRESHOLD           MP_SIZE_T_MAX  /* never */
#define DIVEXACT_1_THRESHOLD                 0  /* always (native) */
#define BMOD_1_TO_MOD_1_THRESHOLD           30

#define MUL_TOOM22_THRESHOLD                16
#define MUL_TOOM33_THRESHOLD                57
#define MUL_TOOM44_THRESHOLD               154
#define MUL_TOOM6H_THRESHOLD               226
#define MUL_TOOM8H_THRESHOLD               309

#define MUL_TOOM32_TO_TOOM43_THRESHOLD      97
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     102
#define MUL_TOOM42_TO_TOOM53_THRESHOLD     103
#define MUL_TOOM42_TO_TOOM63_THRESHOLD     105
#define MUL_TOOM43_TO_TOOM54_THRESHOLD     118

#define SQR_BASECASE_THRESHOLD               0  /* always (native) */
#define SQR_TOOM2_THRESHOLD                 26
#define SQR_TOOM3_THRESHOLD                 93
#define SQR_TOOM4_THRESHOLD                226
#define SQR_TOOM6_THRESHOLD                318
#define SQR_TOOM8_THRESHOLD                430

#define MULMID_TOOM42_THRESHOLD             24

#define MULMOD_BNM1_THRESHOLD               11
#define SQRMOD_BNM1_THRESHOLD               14

#define MUL_FFT_MODF_THRESHOLD             595  /* k = 5 */
#define MUL_FFT_TABLE3                                      \
  { {    595, 5}, {     19, 6}, {     10, 5}, {     21, 6}, \
    {     11, 5}, {     23, 6}, {     25, 7}, {     25, 8}, \
    {     13, 7}, {     28, 8}, {     15, 7}, {     32, 8}, \
    {     17, 7}, {     35, 8}, {     21, 9}, {     11, 8}, \
    {     27, 9}, {     15, 8}, {     35, 9}, {     19, 8}, \
    {     39, 9}, {     23, 8}, {     47, 9}, {     27,10}, \
    {     15, 9}, {     39,10}, {     23, 9}, {     51,11}, \
    {     15,10}, {     31, 9}, {     67,10}, {     39, 9}, \
    {     79,10}, {     47, 9}, {     95,10}, {     55,11}, \
    {     31,10}, {     79,11}, {     47,10}, {     95,12}, \
    {     31,11}, {     63,10}, {    135,11}, {     79,10}, \
    {    167,11}, {     95,10}, {    191, 9}, {    383,11}, \
    {    111,12}, {     63,11}, {    143, 9}, {    575,12}, \
    {     95,10}, {    383,13}, {     63,12}, {    127,10}, \
    {    575,12}, {    159,10}, {    639,11}, {    335,10}, \
    {    671,11}, {    351,10}, {    703,12}, {    191,11}, \
    {    383,10}, {    767,11}, {    415,10}, {    831,11}, \
    {    447,13}, {    127,11}, {    543,10}, {   1087,11}, \
    {    607,10}, {   1215, 9}, {   2431,11}, {    671,12}, \
    {    351,11}, {    703,12}, {    383,11}, {    767,12}, \
    {    415,11}, {    831,12}, {    447,11}, {    895,12}, \
    {    479,14}, {    127,12}, {    511,11}, {   1023,12}, \
    {    543,11}, {   1087,12}, {    607,11}, {   1215,13}, \
    {    319,12}, {    735,13}, {    383,12}, {    831,13}, \
    {    447,12}, {    959,13}, {    511,12}, {   1087,13}, \
    {    575,12}, {   1215,13}, {   8192,14}, {  16384,15}, \
    {  32768,16}, {  65536,17}, { 131072,18}, { 262144,19}, \
    { 524288,20}, {1048576,21}, {2097152,22}, {4194304,23}, \
    {8388608,24} }
#define MUL_FFT_TABLE3_SIZE 113
#define MUL_FFT_THRESHOLD                 5760

#define SQR_FFT_MODF_THRESHOLD             565  /* k = 5 */
#define SQR_FFT_TABLE3                                      \
  { {    565, 5}, {     19, 6}, {     10, 5}, {     21, 6}, \
    {     25, 7}, {     25, 8}, {     13, 7}, {     28, 8}, \
    {     15, 7}, {     31, 8}, {     17, 7}, {     35, 8}, \
    {     21, 9}, {     11, 8}, {     27, 9}, {     15, 8}, \
    {     35, 9}, {     19, 8}, {     41, 9}, {     23, 8}, \
    {     47, 9}, {     27,10}, {     15, 9}, {     39,10}, \
    {     23, 9}, {     51,11}, {     15,10}, {     31, 9}, \
    {     67,10}, {     39, 9}, {     79,10}, {     47,11}, \
    {     31,10}, {     79,11}, {     47,10}, {     95,12}, \
    {     31,11}, {     63,10}, {    135,11}, {     79, 9}, \
    {    319,11}, {     95, 9}, {    383,12}, {     63,11}, \
    {    127, 9}, {    543,11}, {    143, 9}, {    575,10}, \
    {    303, 9}, {    607,10}, {    319,12}, {     95,11}, \
    {    191,13}, {     63,12}, {    127,10}, {    511, 9}, \
    {   1023,10}, {    543,11}, {    287,10}, {    575,11}, \
    {    303,10}, {    639,11}, {    335,10}, {    703,11}, \
    {    383,10}, {    767,11}, {    415,10}, {    831,11}, \
    {    479,13}, {    127,11}, {    511,10}, {   1023,11}, \
    {    575,10}, {   1151,11}, {    607,12}, {    319,11}, \
    {    671,12}, {    351,11}, {    703,12}, {    383,11}, \
    {    767,12}, {    415,11}, {    831,12}, {    447,11}, \
    {    895,12}, {    479,14}, {    127,12}, {    511,11}, \
    {   1023,12}, {    607,13}, {    319,12}, {    703,13}, \
    {    383,12}, {    831,13}, {    447,12}, {    895,13}, \
    {    511,12}, {   1023,13}, {    575,12}, {   1151,13}, \
    {   8192,14}, {  16384,15}, {  32768,16}, {  65536,17}, \
    { 131072,18}, { 262144,19}, { 524288,20}, {1048576,21}, \
    {2097152,22}, {4194304,23}, {8388608,24} }
#define SQR_FFT_TABLE3_SIZE 111
#define SQR_FFT_THRESHOLD                 4736

#define MULLO_BASECASE_THRESHOLD             0  /* always */
#define MULLO_DC_THRESHOLD                  51
#define MULLO_MUL_N_THRESHOLD            10950

#define DC_DIV_QR_THRESHOLD                 59
#define DC_DIVAPPR_Q_THRESHOLD             186
#define DC_BDIV_QR_THRESHOLD                59
#define DC_BDIV_Q_THRESHOLD                126

#define INV_MULMOD_BNM1_THRESHOLD           30
#define INV_NEWTON_THRESHOLD               202
#define INV_APPR_THRESHOLD                 189

#define BINV_NEWTON_THRESHOLD              220
#define REDC_1_TO_REDC_2_THRESHOLD          14
#define REDC_2_TO_REDC_N_THRESHOLD          55

#define MU_DIV_QR_THRESHOLD               1718
#define MU_DIVAPPR_Q_THRESHOLD            1652
#define MUPI_DIV_QR_THRESHOLD               83
#define MU_BDIV_QR_THRESHOLD              1442
#define MU_BDIV_Q_THRESHOLD               1620

#define POWM_SEC_TABLE  1,23,195,453,1100,1556

#define MATRIX22_STRASSEN_THRESHOLD         14
#define HGCD_THRESHOLD                     117
#define HGCD_APPR_THRESHOLD                108
#define HGCD_REDUCE_THRESHOLD             3389
#define GCD_DC_THRESHOLD                   555
#define GCDEXT_DC_THRESHOLD                443
#define JACOBI_BASE_METHOD                   4

#define GET_STR_DC_THRESHOLD                11
#define GET_STR_PRECOMPUTE_THRESHOLD        26
#define SET_STR_DC_THRESHOLD              1313
#define SET_STR_PRECOMPUTE_THRESHOLD      2708

#define FAC_DSC_THRESHOLD                 1517
#define FAC_ODD_THRESHOLD                   28
