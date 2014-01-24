/* Alpha EV5 gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 1999-2002, 2004, 2005, 2008-2010 Free Software
Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published
by the Free Software Foundation; either version 3 of the License, or (at
your option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library.  If not, see https://www.gnu.org/licenses/.  */

#define GMP_LIMB_BITS 64
#define GMP_LIMB_BYTES 8


/* 600 MHz 21164A */

#define DIVREM_1_NORM_THRESHOLD              0  /* preinv always */
#define DIVREM_1_UNNORM_THRESHOLD            0  /* always */
#define MOD_1_1P_METHOD                      2
#define MOD_1_NORM_THRESHOLD                 0  /* always */
#define MOD_1_UNNORM_THRESHOLD               0  /* always */
#define MOD_1N_TO_MOD_1_1_THRESHOLD          3
#define MOD_1U_TO_MOD_1_1_THRESHOLD          7
#define MOD_1_1_TO_MOD_1_2_THRESHOLD        20
#define MOD_1_2_TO_MOD_1_4_THRESHOLD         0  /* never mpn_mod_1s_2p */
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD     14
#define USE_PREINV_DIVREM_1                  1  /* preinv always */
#define DIV_QR_1N_PI1_METHOD                 1
#define DIV_QR_1_NORM_THRESHOLD          MP_SIZE_T_MAX  /* never */
#define DIV_QR_1_UNNORM_THRESHOLD        MP_SIZE_T_MAX  /* never */
#define DIV_QR_2_PI2_THRESHOLD           MP_SIZE_T_MAX  /* never */
#define DIVEXACT_1_THRESHOLD                 0  /* always */
#define BMOD_1_TO_MOD_1_THRESHOLD           80

#define MUL_TOOM22_THRESHOLD                14
#define MUL_TOOM33_THRESHOLD                45
#define MUL_TOOM44_THRESHOLD               118
#define MUL_TOOM6H_THRESHOLD               157
#define MUL_TOOM8H_THRESHOLD               236

#define MUL_TOOM32_TO_TOOM43_THRESHOLD      57
#define MUL_TOOM32_TO_TOOM53_THRESHOLD      77
#define MUL_TOOM42_TO_TOOM53_THRESHOLD      81
#define MUL_TOOM42_TO_TOOM63_THRESHOLD      56
#define MUL_TOOM43_TO_TOOM54_THRESHOLD      66

#define SQR_BASECASE_THRESHOLD               4
#define SQR_TOOM2_THRESHOLD                 22
#define SQR_TOOM3_THRESHOLD                 74
#define SQR_TOOM4_THRESHOLD                171
#define SQR_TOOM6_THRESHOLD                  0  /* always */
#define SQR_TOOM8_THRESHOLD                357

#define MULMID_TOOM42_THRESHOLD             18

#define MULMOD_BNM1_THRESHOLD               10
#define SQRMOD_BNM1_THRESHOLD               12

#define MUL_FFT_MODF_THRESHOLD             284  /* k = 5 */
#define MUL_FFT_TABLE3                                      \
  { {    284, 5}, {     11, 6}, {      6, 5}, {     13, 6}, \
    {      7, 5}, {     15, 6}, {     13, 7}, {      7, 6}, \
    {     15, 7}, {      8, 6}, {     17, 7}, {     13, 8}, \
    {      7, 7}, {     17, 8}, {      9, 7}, {     20, 8}, \
    {     11, 7}, {     23, 8}, {     13, 9}, {      7, 8}, \
    {     19, 9}, {     11, 8}, {     25,10}, {      7, 9}, \
    {     15, 8}, {     33, 9}, {     19, 8}, {     39, 9}, \
    {     23, 8}, {     47,10}, {     15, 9}, {     39,10}, \
    {     23, 9}, {     47,11}, {     15,10}, {     31, 9}, \
    {     67,10}, {     39, 9}, {     79,10}, {     47, 9}, \
    {     95,10}, {     55,11}, {     31,10}, {     63, 8}, \
    {    255, 7}, {    511,10}, {     71, 9}, {    143, 8}, \
    {    287, 7}, {    575, 9}, {    159, 8}, {    319,11}, \
    {     47,12}, {     31,11}, {     63, 9}, {    255, 8}, \
    {    511,10}, {    143, 9}, {    287,11}, {     79,10}, \
    {    159, 9}, {    319, 8}, {    639,10}, {    175, 9}, \
    {    351, 8}, {    703,10}, {    191, 9}, {    383,10}, \
    {    207, 9}, {    415,12}, {   4096,13}, {   8192,14}, \
    {  16384,15}, {  32768,16}, {  65536,17}, { 131072,18}, \
    { 262144,19}, { 524288,20}, {1048576,21}, {2097152,22}, \
    {4194304,23}, {8388608,24} }
#define MUL_FFT_TABLE3_SIZE 82
#define MUL_FFT_THRESHOLD                 3008

#define SQR_FFT_MODF_THRESHOLD             236  /* k = 5 */
#define SQR_FFT_TABLE3                                      \
  { {    236, 5}, {     11, 6}, {      6, 5}, {     13, 6}, \
    {      7, 5}, {     15, 6}, {     17, 7}, {      9, 6}, \
    {     19, 7}, {     17, 8}, {      9, 7}, {     20, 8}, \
    {     11, 7}, {     23, 8}, {     13, 9}, {      7, 8}, \
    {     19, 9}, {     11, 8}, {     25,10}, {      7, 9}, \
    {     15, 8}, {     33, 9}, {     19, 8}, {     39, 9}, \
    {     23, 8}, {     47, 9}, {     27,10}, {     15, 9}, \
    {     39,10}, {     23, 9}, {     47,11}, {     15,10}, \
    {     31, 9}, {     67,10}, {     39, 9}, {     79,10}, \
    {     47,11}, {     31,10}, {     63, 9}, {    127, 8}, \
    {    255,10}, {     71, 9}, {    143, 8}, {    287,10}, \
    {     79,11}, {     47,10}, {     95,12}, {     31,11}, \
    {     63,10}, {    127, 9}, {    255, 8}, {    511,10}, \
    {    143, 9}, {    287,11}, {     79,10}, {    159, 9}, \
    {    319, 8}, {    639,10}, {    175, 9}, {    351,11}, \
    {     95,10}, {    191, 9}, {    383,10}, {    207, 9}, \
    {    415,11}, {    111,10}, {    223,12}, {   4096,13}, \
    {   8192,14}, {  16384,15}, {  32768,16}, {  65536,17}, \
    { 131072,18}, { 262144,19}, { 524288,20}, {1048576,21}, \
    {2097152,22}, {4194304,23}, {8388608,24} }
#define SQR_FFT_TABLE3_SIZE 79
#define SQR_FFT_THRESHOLD                 2240

#define MULLO_BASECASE_THRESHOLD             0  /* always */
#define MULLO_DC_THRESHOLD                  45
#define MULLO_MUL_N_THRESHOLD             5558

#define DC_DIV_QR_THRESHOLD                 51
#define DC_DIVAPPR_Q_THRESHOLD             168
#define DC_BDIV_QR_THRESHOLD                48
#define DC_BDIV_Q_THRESHOLD                108

#define INV_MULMOD_BNM1_THRESHOLD           26
#define INV_NEWTON_THRESHOLD               179
#define INV_APPR_THRESHOLD                 180

#define BINV_NEWTON_THRESHOLD              182
#define REDC_1_TO_REDC_N_THRESHOLD          51

#define MU_DIV_QR_THRESHOLD                998
#define MU_DIVAPPR_Q_THRESHOLD            1078
#define MUPI_DIV_QR_THRESHOLD               90
#define MU_BDIV_QR_THRESHOLD               855
#define MU_BDIV_Q_THRESHOLD                942

#define POWM_SEC_TABLE  2,17,91,453,1168

#define MATRIX22_STRASSEN_THRESHOLD         15
#define HGCD_THRESHOLD                      94
#define HGCD_APPR_THRESHOLD                102
#define HGCD_REDUCE_THRESHOLD             1615
#define GCD_DC_THRESHOLD                   309
#define GCDEXT_DC_THRESHOLD                201
#define JACOBI_BASE_METHOD                   3

#define GET_STR_DC_THRESHOLD                14
#define GET_STR_PRECOMPUTE_THRESHOLD        28
#define SET_STR_DC_THRESHOLD               426
#define SET_STR_PRECOMPUTE_THRESHOLD      1505

#define FAC_DSC_THRESHOLD                 1404
#define FAC_ODD_THRESHOLD                    0  /* always */
