/* POWER4/PowerPC970 gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 2008, 2009, 2010 Free Software Foundation, Inc.

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


#define MOD_1_NORM_THRESHOLD                 0  /* always */
#define MOD_1_UNNORM_THRESHOLD               0  /* always */
#define MOD_1N_TO_MOD_1_1_THRESHOLD          7
#define MOD_1U_TO_MOD_1_1_THRESHOLD          6
#define MOD_1_1_TO_MOD_1_2_THRESHOLD         9
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        26
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD     14
#define USE_PREINV_DIVREM_1                  0
#define DIVEXACT_1_THRESHOLD                 0  /* always (native) */
#define BMOD_1_TO_MOD_1_THRESHOLD           43

#define MUL_TOOM22_THRESHOLD                14
#define MUL_TOOM33_THRESHOLD                54
#define MUL_TOOM44_THRESHOLD               154
#define MUL_TOOM6H_THRESHOLD               206
#define MUL_TOOM8H_THRESHOLD               309

#define MUL_TOOM32_TO_TOOM43_THRESHOLD      89
#define MUL_TOOM32_TO_TOOM53_THRESHOLD      99
#define MUL_TOOM42_TO_TOOM53_THRESHOLD      97
#define MUL_TOOM42_TO_TOOM63_THRESHOLD      97

#define SQR_BASECASE_THRESHOLD               5
#define SQR_TOOM2_THRESHOLD                 36
#define SQR_TOOM3_THRESHOLD                 61
#define SQR_TOOM4_THRESHOLD                154
#define SQR_TOOM6_THRESHOLD                206
#define SQR_TOOM8_THRESHOLD                309

#define MULMOD_BNM1_THRESHOLD               12
#define SQRMOD_BNM1_THRESHOLD               14

#define MUL_FFT_MODF_THRESHOLD             654  /* k = 5 */
#define MUL_FFT_TABLE3                                      \
  { {    654, 5}, {     21, 6}, {     11, 5}, {     23, 6}, \
    {     13, 5}, {     27, 6}, {     21, 7}, {     11, 6}, \
    {     23, 7}, {     12, 6}, {     25, 7}, {     13, 6}, \
    {     27, 7}, {     21, 8}, {     11, 7}, {     25, 8}, \
    {     13, 7}, {     27, 8}, {     15, 7}, {     31, 8}, \
    {     21, 9}, {     11, 8}, {     27, 9}, {     15, 8}, \
    {     35, 9}, {     19, 8}, {     39, 9}, {     23, 8}, \
    {     47, 9}, {     27,10}, {     15, 9}, {     39,10}, \
    {     23, 9}, {     55,11}, {     15,10}, {     31, 9}, \
    {     71,10}, {     39, 9}, {     83,10}, {     47, 9}, \
    {     95,10}, {     55,11}, {     31,10}, {     63, 9}, \
    {    127,10}, {     79,11}, {     47,10}, {    103,12}, \
    {     31,11}, {     63,10}, {    135, 8}, {    543,11}, \
    {     79,10}, {    159, 8}, {    639,10}, {    167, 9}, \
    {    335,11}, {     95, 9}, {    383, 8}, {    767,10}, \
    {    199, 9}, {    415,11}, {    111,12}, {     63,11}, \
    {    127, 9}, {    511, 8}, {   1023, 9}, {    543,11}, \
    {    143, 9}, {    575, 8}, {   1151, 9}, {    607,11}, \
    {    159, 9}, {    639,10}, {    351,12}, {     95,10}, \
    {    383, 9}, {    767,10}, {    415, 9}, {    831,13}, \
    {     63,12}, {    127,10}, {    511, 9}, {   1023,10}, \
    {    543, 9}, {   1087, 8}, {   2175,10}, {    575, 9}, \
    {   1151,10}, {    607, 9}, {   1215,12}, {    159,10}, \
    {    639,11}, {    335,10}, {    671,11}, {    351,10}, \
    {    703, 9}, {   1407,11}, {    383,10}, {    767,11}, \
    {    415,10}, {    831, 9}, {   1663,11}, {    447,10}, \
    {    895,13}, {    127,11}, {    511,10}, {   1023,11}, \
    {    543,10}, {   1087, 9}, {   2175,11}, {    575,10}, \
    {   1151,11}, {    607,10}, {   1215, 9}, {   2431,11}, \
    {    639,10}, {   1279,11}, {    671,12}, {    351,11}, \
    {    703,10}, {   1407,12}, {    383,11}, {    767,12}, \
    {    415,11}, {    831,10}, {   1663,12}, {    447,11}, \
    {    895,12}, {    479,11}, {    959,14}, {    127,12}, \
    {    511,11}, {   1023,12}, {    543,11}, {   1087,10}, \
    {   2175,12}, {    575,11}, {   1151,12}, {    607,11}, \
    {   1215,10}, {   2431,12}, {    639,11}, {   1279,12}, \
    {    671,11}, {   1343,12}, {    703,11}, {   1407,12}, \
    {    735,13}, {    383,12}, {    767,11}, {   1535,12}, \
    {    799,11}, {   1599,12}, {    831,11}, {   1663,13}, \
    {    447,12}, {    959,11}, {   1919,13}, {    511,12}, \
    {   1087,11}, {   2175,13}, {    575,12}, {   1215,11}, \
    {   2431,13}, {    639,12}, {   1343,11}, {   2687,13}, \
    {    703,12}, {   1407,14}, {    383,13}, {    767,12}, \
    {   1599,13}, {    831,12}, {   1663,13}, {    895,12}, \
    {   1791,13}, {    959,12}, {   1919,11}, {   3839,14}, \
    {    511,13}, {   1087,12}, {   2175,13}, {   1215,12}, \
    {   2431,14}, {    639,13}, {   1343,12}, {   2687,13}, \
    {   1471,12}, {   2943,14}, {    767,13}, {   1599,12}, \
    {   3199,13}, {   1663,14}, {    895,13}, {   8192,14}, \
    {  16384,15}, {  32768,16}, {  65536,17}, { 131072,18}, \
    { 262144,19}, { 524288,20}, {1048576,21}, {2097152,22}, \
    {4194304,23}, {8388608,24} }
#define MUL_FFT_TABLE3_SIZE 206
#define MUL_FFT_THRESHOLD                 9472

#define SQR_FFT_MODF_THRESHOLD             618  /* k = 6 */
#define SQR_FFT_TABLE3                                      \
  { {    618, 6}, {     21, 7}, {     11, 6}, {     23, 7}, \
    {     21, 8}, {     11, 7}, {     25, 8}, {     13, 7}, \
    {     27, 8}, {     15, 7}, {     31, 8}, {     21, 9}, \
    {     11, 8}, {     27, 9}, {     15, 8}, {     33, 9}, \
    {     19, 8}, {     39, 9}, {     23, 8}, {     47, 9}, \
    {     27,10}, {     15, 9}, {     39,10}, {     23, 9}, \
    {     51,11}, {     15,10}, {     31, 9}, {     67,10}, \
    {     39, 9}, {     79,10}, {     47, 9}, {     95,10}, \
    {     55,11}, {     31,10}, {     79,11}, {     47,10}, \
    {     95, 8}, {    383,12}, {     31,11}, {     63,10}, \
    {    127, 8}, {    511,10}, {    135, 8}, {    543,11}, \
    {     79, 9}, {    319, 8}, {    639, 9}, {    351,11}, \
    {     95, 9}, {    383, 8}, {    767, 9}, {    415,12}, \
    {     63,11}, {    127, 9}, {    511, 8}, {   1023, 9}, \
    {    543, 8}, {   1087,11}, {    143, 9}, {    575, 8}, \
    {   1151, 9}, {    607,10}, {    319, 9}, {    639,10}, \
    {    351,12}, {     95,10}, {    383, 9}, {    767,10}, \
    {    415, 9}, {    831,13}, {     63,12}, {    127,10}, \
    {    511, 9}, {   1023,10}, {    543, 9}, {   1087, 8}, \
    {   2175,10}, {    575, 9}, {   1151,10}, {    607,11}, \
    {    319,10}, {    639,11}, {    351,10}, {    703, 9}, \
    {   1407,11}, {    383,10}, {    767,11}, {    415,10}, \
    {    831, 9}, {   1663,11}, {    447,10}, {    895,11}, \
    {    479,10}, {    959,13}, {    127,11}, {    511,10}, \
    {   1023,11}, {    543,10}, {   1087, 9}, {   2175,11}, \
    {    575,10}, {   1151,11}, {    607,12}, {    319,11}, \
    {    639,12}, {    351,11}, {    703,10}, {   1407,12}, \
    {    383,11}, {    767,12}, {    415,11}, {    831,10}, \
    {   1663,12}, {    447,11}, {    895,12}, {    479,11}, \
    {    959,10}, {   1919,14}, {    127,12}, {    511,11}, \
    {   1023,12}, {    543,11}, {   1087,10}, {   2175,12}, \
    {    575,11}, {   1151,12}, {    607,13}, {    319,12}, \
    {    639,11}, {   1279,12}, {    671,11}, {   1343,12}, \
    {    703,11}, {   1407,13}, {    383,12}, {    767,11}, \
    {   1535,12}, {    831,11}, {   1663,13}, {    447,12}, \
    {    959,11}, {   1919,10}, {   3839,13}, {    511,12}, \
    {   1087,11}, {   2175,13}, {    575,12}, {   1215,11}, \
    {   2431,13}, {    639,12}, {   1343,13}, {    703,12}, \
    {   1407,14}, {    383,13}, {    767,12}, {   1535,13}, \
    {    831,12}, {   1663,13}, {    895,12}, {   1791,13}, \
    {    959,12}, {   1919,11}, {   3839,14}, {    511,13}, \
    {   1087,12}, {   2175,13}, {   1215,12}, {   2431,14}, \
    {    639,13}, {   1343,12}, {   2687,13}, {   1407,12}, \
    {   2815,13}, {   1471,14}, {    767,13}, {   1663,14}, \
    {    895,13}, {   8192,14}, {  16384,15}, {  32768,16}, \
    {  65536,17}, { 131072,18}, { 262144,19}, { 524288,20}, \
    {1048576,21}, {2097152,22}, {4194304,23}, {8388608,24} }
#define SQR_FFT_TABLE3_SIZE 188
#define SQR_FFT_THRESHOLD                 7040

#define MULLO_BASECASE_THRESHOLD             5
#define MULLO_DC_THRESHOLD                  34
#define MULLO_MUL_N_THRESHOLD            18629

#define DC_DIV_QR_THRESHOLD                 30
#define DC_DIVAPPR_Q_THRESHOLD             103
#define DC_BDIV_QR_THRESHOLD                48
#define DC_BDIV_Q_THRESHOLD                120

#define INV_MULMOD_BNM1_THRESHOLD          107
#define INV_NEWTON_THRESHOLD               170
#define INV_APPR_THRESHOLD                 117

#define BINV_NEWTON_THRESHOLD              206
#define REDC_1_TO_REDC_N_THRESHOLD          56

#define MU_DIV_QR_THRESHOLD               1470
#define MU_DIVAPPR_Q_THRESHOLD            1334
#define MUPI_DIV_QR_THRESHOLD               57
#define MU_BDIV_QR_THRESHOLD              1099
#define MU_BDIV_Q_THRESHOLD               1308

#define MATRIX22_STRASSEN_THRESHOLD         17
#define HGCD_THRESHOLD                      86
#define GCD_DC_THRESHOLD                   233
#define GCDEXT_DC_THRESHOLD                221
#define JACOBI_BASE_METHOD                   1

#define GET_STR_DC_THRESHOLD                11
#define GET_STR_PRECOMPUTE_THRESHOLD        24
#define SET_STR_DC_THRESHOLD               795
#define SET_STR_PRECOMPUTE_THRESHOLD      1790
