/* S/390-32 gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007,
2008, 2009, 2010, 2011 Free Software Foundation, Inc.

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

/* 1200 MHz IBM z990 running in 32-bit mode */

#define DIVREM_1_NORM_THRESHOLD              0  /* always */
#define DIVREM_1_UNNORM_THRESHOLD            3
#define MOD_1_1P_METHOD                      1
#define MOD_1_NORM_THRESHOLD                 0  /* always */
#define MOD_1_UNNORM_THRESHOLD               3
#define MOD_1N_TO_MOD_1_1_THRESHOLD         12
#define MOD_1U_TO_MOD_1_1_THRESHOLD          7
#define MOD_1_1_TO_MOD_1_2_THRESHOLD        15
#define MOD_1_2_TO_MOD_1_4_THRESHOLD         0  /* never mpn_mod_1s_2p */
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD     21
#define USE_PREINV_DIVREM_1                  1
#define DIV_QR_2_PI2_THRESHOLD           MP_SIZE_T_MAX  /* never */
#define DIVEXACT_1_THRESHOLD                 0  /* always */
#define BMOD_1_TO_MOD_1_THRESHOLD           50

#define MUL_TOOM22_THRESHOLD                16
#define MUL_TOOM33_THRESHOLD                66
#define MUL_TOOM44_THRESHOLD               169
#define MUL_TOOM6H_THRESHOLD               369
#define MUL_TOOM8H_THRESHOLD               517

#define MUL_TOOM32_TO_TOOM43_THRESHOLD     106
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     114
#define MUL_TOOM42_TO_TOOM53_THRESHOLD     114
#define MUL_TOOM42_TO_TOOM63_THRESHOLD     187

#define SQR_BASECASE_THRESHOLD               0  /* always (native) */
#define SQR_TOOM2_THRESHOLD                 28
#define SQR_TOOM3_THRESHOLD                 93
#define SQR_TOOM4_THRESHOLD                387
#define SQR_TOOM6_THRESHOLD                552
#define SQR_TOOM8_THRESHOLD                  0  /* always */

#define MULMID_TOOM42_THRESHOLD             38

#define MULMOD_BNM1_THRESHOLD               13
#define SQRMOD_BNM1_THRESHOLD               15

#define MUL_FFT_MODF_THRESHOLD             336  /* k = 5 */
#define MUL_FFT_TABLE3                                      \
  { {    336, 5}, {     19, 6}, {     11, 5}, {     23, 6}, \
    {     17, 7}, {      9, 6}, {     21, 7}, {     11, 6}, \
    {     24, 7}, {     13, 6}, {     27, 7}, {     15, 6}, \
    {     31, 7}, {     27, 8}, {     15, 7}, {     33, 8}, \
    {     19, 7}, {     39, 8}, {     23, 7}, {     47, 8}, \
    {     27, 9}, {     15, 8}, {     31, 7}, {     63, 8}, \
    {     39, 9}, {     23, 8}, {     51,10}, {     15, 9}, \
    {     31, 8}, {     63, 9}, {     39, 8}, {     79, 9}, \
    {     47,10}, {     31, 9}, {     79,10}, {     47,11}, \
    {   2048,12}, {   4096,13}, {   8192,14}, {  16384,15}, \
    {  32768,16} }
#define MUL_FFT_TABLE3_SIZE 41
#define MUL_FFT_THRESHOLD                 2752

#define SQR_FFT_MODF_THRESHOLD             308  /* k = 5 */
#define SQR_FFT_TABLE3                                      \
  { {    308, 5}, {     20, 6}, {     11, 5}, {     23, 6}, \
    {     21, 7}, {     11, 6}, {     24, 7}, {     15, 6}, \
    {     31, 7}, {     21, 8}, {     11, 7}, {     27, 8}, \
    {     15, 7}, {     33, 8}, {     19, 7}, {     39, 8}, \
    {     23, 7}, {     47, 8}, {     27, 9}, {     15, 8}, \
    {     39, 9}, {     23, 8}, {     47,10}, {     15, 9}, \
    {     31, 8}, {     63, 9}, {     47,10}, {     31, 9}, \
    {     79,10}, {     47,11}, {   2048,12}, {   4096,13}, \
    {   8192,14}, {  16384,15}, {  32768,16} }
#define SQR_FFT_TABLE3_SIZE 35
#define SQR_FFT_THRESHOLD                 2368

#define MULLO_BASECASE_THRESHOLD             6
#define MULLO_DC_THRESHOLD                  45
#define MULLO_MUL_N_THRESHOLD             5397

#define DC_DIV_QR_THRESHOLD                 40
#define DC_DIVAPPR_Q_THRESHOLD             152
#define DC_BDIV_QR_THRESHOLD                51
#define DC_BDIV_Q_THRESHOLD                136

#define INV_MULMOD_BNM1_THRESHOLD           46
#define INV_NEWTON_THRESHOLD               197
#define INV_APPR_THRESHOLD                 157

#define BINV_NEWTON_THRESHOLD              114
#define REDC_1_TO_REDC_N_THRESHOLD          55

#define MU_DIV_QR_THRESHOLD               1210
#define MU_DIVAPPR_Q_THRESHOLD            1334
#define MUPI_DIV_QR_THRESHOLD               81
#define MU_BDIV_QR_THRESHOLD               942
#define MU_BDIV_Q_THRESHOLD               1258

#define MATRIX22_STRASSEN_THRESHOLD         17
#define HGCD_THRESHOLD                     104
#define GCD_DC_THRESHOLD                   278
#define GCDEXT_DC_THRESHOLD                217
#define JACOBI_BASE_METHOD                   2

#define GET_STR_DC_THRESHOLD                16
#define GET_STR_PRECOMPUTE_THRESHOLD        30
#define SET_STR_DC_THRESHOLD               274
#define SET_STR_PRECOMPUTE_THRESHOLD       824

/* Tuneup completed successfully, took 108 seconds */
