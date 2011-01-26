/* gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 2000, 2001, 2002, 2003, 2004, 2005, 2009, 2010 Free Software
Foundation, Inc.

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

/* 1300MHz Itanium2 (babe.fsffrance.org) */


#define MOD_1_NORM_THRESHOLD                 0  /* always */
#define MOD_1_UNNORM_THRESHOLD               0  /* always */
#define MOD_1N_TO_MOD_1_1_THRESHOLD          4
#define MOD_1U_TO_MOD_1_1_THRESHOLD          8
#define MOD_1_1_TO_MOD_1_2_THRESHOLD         0  /* never mpn_mod_1_1p */
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        21
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD     22
#define USE_PREINV_DIVREM_1                  1  /* native */
#define DIVEXACT_1_THRESHOLD                 0  /* always (native) */
#define BMOD_1_TO_MOD_1_THRESHOLD        MP_SIZE_T_MAX  /* never */

#define MUL_TOOM22_THRESHOLD                40
#define MUL_TOOM33_THRESHOLD               122
#define MUL_TOOM44_THRESHOLD               212
#define MUL_TOOM6H_THRESHOLD               318
#define MUL_TOOM8H_THRESHOLD               430

#define MUL_TOOM32_TO_TOOM43_THRESHOLD      93
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     146
#define MUL_TOOM42_TO_TOOM53_THRESHOLD     129
#define MUL_TOOM42_TO_TOOM63_THRESHOLD     151

#define SQR_BASECASE_THRESHOLD              11
#define SQR_TOOM2_THRESHOLD                 84
#define SQR_TOOM3_THRESHOLD                125
#define SQR_TOOM4_THRESHOLD                494
#define SQR_TOOM6_THRESHOLD                  0  /* never toom4 */
#define SQR_TOOM8_THRESHOLD                  0  /* never toom6 */

#define MULMOD_BNM1_THRESHOLD               23
#define SQRMOD_BNM1_THRESHOLD               25

#define MUL_FFT_MODF_THRESHOLD             444  /* k = 5 */
#define MUL_FFT_TABLE3                                      \
  { {    444, 5}, {     27, 6}, {     14, 5}, {     29, 6}, \
    {     35, 7}, {     18, 6}, {     37, 7}, {     19, 6}, \
    {     39, 7}, {     21, 6}, {     43, 7}, {     33, 8}, \
    {     17, 7}, {     37, 8}, {     19, 7}, {     39, 8}, \
    {     21, 7}, {     43, 8}, {     29, 9}, {     15, 8}, \
    {     37, 9}, {     19, 8}, {     43, 9}, {     23, 8}, \
    {     49, 9}, {     27, 8}, {     57, 9}, {     31, 8}, \
    {     63, 9}, {     35, 8}, {     71, 9}, {     43,10}, \
    {     23, 9}, {     59,10}, {     31, 9}, {     71,10}, \
    {     39, 9}, {     87,10}, {     47, 9}, {     99,10}, \
    {     55,11}, {     31,10}, {     87,11}, {     47,10}, \
    {    111,12}, {     31,11}, {     63,10}, {    143,11}, \
    {     79,10}, {    167,11}, {     95,10}, {    191,11}, \
    {    111,12}, {     63,11}, {    143,10}, {    287, 9}, \
    {    575,10}, {    303,11}, {    159,10}, {    319,12}, \
    {     95,11}, {    191,10}, {    399,11}, {    207,10}, \
    {    431,13}, {   8192,14}, {  16384,15}, {  32768,16}, \
    {  65536,17}, { 131072,18}, { 262144,19}, { 524288,20}, \
    {1048576,21}, {2097152,22}, {4194304,23}, {8388608,24} }
#define MUL_FFT_TABLE3_SIZE 76
#define MUL_FFT_THRESHOLD                 5760

#define SQR_FFT_MODF_THRESHOLD             440  /* k = 5 */
#define SQR_FFT_TABLE3                                      \
  { {    440, 5}, {     14, 4}, {     29, 5}, {     29, 6}, \
    {     15, 5}, {     31, 6}, {     35, 7}, {     18, 6}, \
    {     37, 7}, {     33, 8}, {     17, 7}, {     37, 8}, \
    {     19, 7}, {     40, 8}, {     37, 9}, {     19, 8}, \
    {     43, 9}, {     23, 8}, {     49, 9}, {     27, 8}, \
    {     57, 9}, {     43,10}, {     23, 9}, {     55,10}, \
    {     31, 9}, {     71,10}, {     39, 9}, {     83,10}, \
    {     47, 9}, {     99,10}, {     55,11}, {     31,10}, \
    {     87,11}, {     47,10}, {    111,12}, {     31,11}, \
    {     63,10}, {    135,11}, {     79,10}, {    167,11}, \
    {     95,10}, {    191,11}, {    111,12}, {     63,11}, \
    {    127,10}, {    255,11}, {    143,10}, {    303,11}, \
    {    159,10}, {    319,12}, {     95,11}, {    191,10}, \
    {    399,11}, {    207,10}, {    431,13}, {   8192,14}, \
    {  16384,15}, {  32768,16}, {  65536,17}, { 131072,18}, \
    { 262144,19}, { 524288,20}, {1048576,21}, {2097152,22}, \
    {4194304,23}, {8388608,24} }
#define SQR_FFT_TABLE3_SIZE 66
#define SQR_FFT_THRESHOLD                 4032

#define MULLO_BASECASE_THRESHOLD            29
#define MULLO_DC_THRESHOLD                  57
#define MULLO_MUL_N_THRESHOLD            11278

#define DC_DIV_QR_THRESHOLD                 59
#define DC_DIVAPPR_Q_THRESHOLD             222
#define DC_BDIV_QR_THRESHOLD                95
#define DC_BDIV_Q_THRESHOLD                264

#define INV_MULMOD_BNM1_THRESHOLD           82
#define INV_NEWTON_THRESHOLD                11
#define INV_APPR_THRESHOLD                  18

#define BINV_NEWTON_THRESHOLD              252
#define REDC_1_TO_REDC_2_THRESHOLD           0
#define REDC_2_TO_REDC_N_THRESHOLD         147

#define MU_DIV_QR_THRESHOLD               1142
#define MU_DIVAPPR_Q_THRESHOLD             998
#define MUPI_DIV_QR_THRESHOLD                0  /* always */
#define MU_BDIV_QR_THRESHOLD              1187
#define MU_BDIV_Q_THRESHOLD               1470

#define MATRIX22_STRASSEN_THRESHOLD         23
#define HGCD_THRESHOLD                     117
#define GCD_DC_THRESHOLD                   469
#define GCDEXT_DC_THRESHOLD                368
#define JACOBI_BASE_METHOD                   4

#define GET_STR_DC_THRESHOLD                13
#define GET_STR_PRECOMPUTE_THRESHOLD        21
#define SET_STR_DC_THRESHOLD              1204
#define SET_STR_PRECOMPUTE_THRESHOLD      3266
