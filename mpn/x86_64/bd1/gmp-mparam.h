/* AMD Bulldozer-1 gmp-mparam.h -- Compiler/machine parameter header file.

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

#define MOD_1_NORM_THRESHOLD                 0  /* always */
#define MOD_1_UNNORM_THRESHOLD               0  /* always */
#define MOD_1N_TO_MOD_1_1_THRESHOLD          4
#define MOD_1U_TO_MOD_1_1_THRESHOLD          4
#define MOD_1_1_TO_MOD_1_2_THRESHOLD        17
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        34
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD     10
#define USE_PREINV_DIVREM_1                  1  /* native */
#define DIV_QR_1_NORM_THRESHOLD              2
#define DIV_QR_1_UNNORM_THRESHOLD        MP_SIZE_T_MAX  /* never */
#define DIV_QR_2_PI2_THRESHOLD           MP_SIZE_T_MAX  /* never */
#define DIVEXACT_1_THRESHOLD                 0  /* always (native) */
#define BMOD_1_TO_MOD_1_THRESHOLD           22

#define MUL_TOOM22_THRESHOLD                16
#define MUL_TOOM33_THRESHOLD                57
#define MUL_TOOM44_THRESHOLD               154
#define MUL_TOOM6H_THRESHOLD               226
#define MUL_TOOM8H_THRESHOLD               466

#define MUL_TOOM32_TO_TOOM43_THRESHOLD      97
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     108
#define MUL_TOOM42_TO_TOOM53_THRESHOLD     105
#define MUL_TOOM42_TO_TOOM63_THRESHOLD     107
#define MUL_TOOM43_TO_TOOM54_THRESHOLD     148

#define SQR_BASECASE_THRESHOLD               0  /* always (native) */
#define SQR_TOOM2_THRESHOLD                 22
#define SQR_TOOM3_THRESHOLD                 86
#define SQR_TOOM4_THRESHOLD                218
#define SQR_TOOM6_THRESHOLD                303
#define SQR_TOOM8_THRESHOLD                430

#define MULMID_TOOM42_THRESHOLD             18

#define MULMOD_BNM1_THRESHOLD               11
#define SQRMOD_BNM1_THRESHOLD               14

#define MUL_FFT_MODF_THRESHOLD             376  /* k = 5 */
#define MUL_FFT_TABLE3                                      \
  { {    376, 5}, {     19, 6}, {     10, 5}, {     21, 6}, \
    {     11, 5}, {     23, 6}, {     21, 7}, {     11, 6}, \
    {     23, 7}, {     12, 6}, {     25, 7}, {     13, 6}, \
    {     27, 7}, {     15, 6}, {     31, 7}, {     21, 8}, \
    {     11, 7}, {     25, 8}, {     13, 7}, {     28, 8}, \
    {     15, 7}, {     32, 8}, {     17, 7}, {     35, 8}, \
    {     21, 9}, {     11, 7}, {     45, 8}, {     27, 9}, \
    {     15, 8}, {     35, 9}, {     19, 8}, {     41, 9}, \
    {     23, 8}, {     47, 9}, {     27,10}, {     15, 9}, \
    {     39,10}, {     23, 9}, {     51,11}, {     15,10}, \
    {     31, 9}, {     71,10}, {     39, 9}, {     83,10}, \
    {     47, 9}, {     99,10}, {     55,11}, {     31,10}, \
    {     87,11}, {     47,10}, {     95,12}, {     31,11}, \
    {     63,10}, {    127, 6}, {   2175, 7}, {   1151, 9}, \
    {    319,10}, {    167,11}, {     95,10}, {    191, 9}, \
    {    383,12}, {     63,10}, {    255, 9}, {    511,11}, \
    {    143,10}, {    287, 9}, {    575,10}, {    303,11}, \
    {    159,12}, {     95,11}, {    191,13}, {   8192,14}, \
    {  16384,15}, {  32768,16}, {  65536,17}, { 131072,18}, \
    { 262144,19}, { 524288,20}, {1048576,21}, {2097152,22}, \
    {4194304,23}, {8388608,24} }
#define MUL_FFT_TABLE3_SIZE 82
#define MUL_FFT_THRESHOLD                 4224

#define SQR_FFT_MODF_THRESHOLD             344  /* k = 5 */
#define SQR_FFT_TABLE3                                      \
  { {    344, 5}, {     19, 6}, {     10, 5}, {     21, 6}, \
    {     11, 5}, {     23, 6}, {     21, 7}, {     11, 6}, \
    {     25, 7}, {     13, 6}, {     27, 7}, {     21, 8}, \
    {     11, 7}, {     25, 8}, {     13, 7}, {     27, 8}, \
    {     15, 7}, {     31, 8}, {     17, 7}, {     35, 8}, \
    {     19, 7}, {     39, 8}, {     21, 9}, {     11, 8}, \
    {     27, 9}, {     15, 8}, {     35, 9}, {     19, 8}, \
    {     41, 9}, {     23, 8}, {     47, 9}, {     27,10}, \
    {     15, 9}, {     39,10}, {     23, 9}, {     51,11}, \
    {     15,10}, {     31, 9}, {     63,10}, {     39, 9}, \
    {     79,10}, {     47, 9}, {     95,11}, {     31,10}, \
    {     79,11}, {     47,10}, {     95,12}, {     31,11}, \
    {     63,10}, {    127, 9}, {    255,11}, {     79,10}, \
    {    159,11}, {     95,10}, {    191, 6}, {   3199, 7}, \
    {   1727, 8}, {    895, 9}, {    479,10}, {    255, 9}, \
    {    511,10}, {    271,11}, {    143,10}, {    287, 9}, \
    {    575,10}, {    303, 9}, {    607, 8}, {   1215,10}, \
    {    319,12}, {     95,11}, {    191,10}, {    383,11}, \
    {    207,13}, {   8192,14}, {  16384,15}, {  32768,16}, \
    {  65536,17}, { 131072,18}, { 262144,19}, { 524288,20}, \
    {1048576,21}, {2097152,22}, {4194304,23}, {8388608,24} }
#define SQR_FFT_TABLE3_SIZE 84
#define SQR_FFT_THRESHOLD                 3264

#define MULLO_BASECASE_THRESHOLD             0  /* always */
#define MULLO_DC_THRESHOLD                  33
#define MULLO_MUL_N_THRESHOLD             8397

#define DC_DIV_QR_THRESHOLD                 43
#define DC_DIVAPPR_Q_THRESHOLD             175
#define DC_BDIV_QR_THRESHOLD                43
#define DC_BDIV_Q_THRESHOLD                 76

#define INV_MULMOD_BNM1_THRESHOLD           34
#define INV_NEWTON_THRESHOLD               226
#define INV_APPR_THRESHOLD                 174

#define BINV_NEWTON_THRESHOLD              236
#define REDC_1_TO_REDC_2_THRESHOLD          51
#define REDC_2_TO_REDC_N_THRESHOLD           0  /* always */

#define MU_DIV_QR_THRESHOLD               1210
#define MU_DIVAPPR_Q_THRESHOLD            1334
#define MUPI_DIV_QR_THRESHOLD               89
#define MU_BDIV_QR_THRESHOLD              1120
#define MU_BDIV_Q_THRESHOLD               1308

#define POWM_SEC_TABLE  1,16,119,745,904,2315

#define MATRIX22_STRASSEN_THRESHOLD         19
#define HGCD_THRESHOLD                     109
#define HGCD_APPR_THRESHOLD                107
#define HGCD_REDUCE_THRESHOLD             2479
#define GCD_DC_THRESHOLD                   460
#define GCDEXT_DC_THRESHOLD                285
#define JACOBI_BASE_METHOD                   4

#define GET_STR_DC_THRESHOLD                13
#define GET_STR_PRECOMPUTE_THRESHOLD        20
#define SET_STR_DC_THRESHOLD               244
#define SET_STR_PRECOMPUTE_THRESHOLD      1160

#define FAC_DSC_THRESHOLD                  230
#define FAC_ODD_THRESHOLD                   24
