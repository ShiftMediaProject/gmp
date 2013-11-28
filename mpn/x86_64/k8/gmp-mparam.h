/* AMD K8 gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 2000-2012 Free Software Foundation, Inc.

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

// #undef mpn_sublsh_n
// #define mpn_sublsh_n(rp,up,vp,n,c)					\
//    (((rp) == (up)) ? mpn_submul_1 (rp, vp, n, CNST_LIMB(1) << (c))	\
//      : MPN(mpn_sublsh_n)(rp,up,vp,n,c))

#define MOD_1_NORM_THRESHOLD                 0  /* always */
#define MOD_1_UNNORM_THRESHOLD               0  /* always */
#define MOD_1N_TO_MOD_1_1_THRESHOLD          6
#define MOD_1U_TO_MOD_1_1_THRESHOLD          2
#define MOD_1_1_TO_MOD_1_2_THRESHOLD        13
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        25
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD      9
#define USE_PREINV_DIVREM_1                  1  /* native */
#define DIV_QR_2_PI2_THRESHOLD           MP_SIZE_T_MAX  /* never */
#define DIVEXACT_1_THRESHOLD                 0  /* always (native) */
#define BMOD_1_TO_MOD_1_THRESHOLD           17

#define MUL_TOOM22_THRESHOLD                28
#define MUL_TOOM33_THRESHOLD                81
#define MUL_TOOM44_THRESHOLD               166
#define MUL_TOOM6H_THRESHOLD               446
#define MUL_TOOM8H_THRESHOLD               482

#define MUL_TOOM32_TO_TOOM43_THRESHOLD      97
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     153
#define MUL_TOOM42_TO_TOOM53_THRESHOLD     116
#define MUL_TOOM42_TO_TOOM63_THRESHOLD     187
#define MUL_TOOM43_TO_TOOM54_THRESHOLD     166

#define SQR_BASECASE_THRESHOLD               0  /* always (native) */
#define SQR_TOOM2_THRESHOLD                 34
#define SQR_TOOM3_THRESHOLD                121
#define SQR_TOOM4_THRESHOLD                532
#define SQR_TOOM6_THRESHOLD                562
#define SQR_TOOM8_THRESHOLD                592

#define MULMID_TOOM42_THRESHOLD             36

#define MULMOD_BNM1_THRESHOLD               18
#define SQRMOD_BNM1_THRESHOLD               19

#define MUL_FFT_MODF_THRESHOLD             786  /* k = 5 */
#define MUL_FFT_TABLE3                                      \
  { {    786, 5}, {     27, 6}, {     14, 5}, {     29, 6}, \
    {     15, 5}, {     31, 6}, {     29, 7}, {     15, 6}, \
    {     31, 7}, {     18, 6}, {     37, 7}, {     19, 6}, \
    {     39, 7}, {     29, 8}, {     15, 7}, {     32, 8}, \
    {     17, 7}, {     37, 8}, {     19, 7}, {     40, 8}, \
    {     21, 7}, {     44, 8}, {     23, 7}, {     47, 8}, \
    {     25, 7}, {     51, 8}, {     43, 9}, {     23, 8}, \
    {     53, 9}, {     27, 8}, {     57, 9}, {     31, 8}, \
    {     67, 9}, {     35, 8}, {     71, 9}, {     39, 8}, \
    {     81, 9}, {     43,10}, {     23, 9}, {     59,10}, \
    {     31, 9}, {     71,10}, {     39, 9}, {     87,10}, \
    {     47, 9}, {     99,10}, {     55, 9}, {    115,11}, \
    {     31,10}, {     63, 9}, {    127,10}, {     71, 9}, \
    {    143,10}, {     87,11}, {     47,10}, {    111,12}, \
    {     31,11}, {     63,10}, {    135, 9}, {    271,10}, \
    {    143,11}, {     79,10}, {    167,11}, {     95,10}, \
    {    199,11}, {    111,12}, {     63,11}, {    127,10}, \
    {    255,11}, {    175,12}, {     95,11}, {    207,10}, \
    {    415,11}, {    223,13}, {   8192,14}, {  16384,15}, \
    {  32768,16}, {  65536,17}, { 131072,18}, { 262144,19}, \
    { 524288,20}, {1048576,21}, {2097152,22}, {4194304,23}, \
    {8388608,24} }
#define MUL_FFT_TABLE3_SIZE 85
#define MUL_FFT_THRESHOLD                11520

#define SQR_FFT_MODF_THRESHOLD             666  /* k = 5 */
#define SQR_FFT_TABLE3                                      \
  { {    666, 5}, {     27, 6}, {     14, 5}, {     29, 6}, \
    {     15, 5}, {     31, 6}, {     16, 5}, {     33, 6}, \
    {     29, 7}, {     15, 6}, {     33, 7}, {     17, 6}, \
    {     35, 7}, {     33, 8}, {     17, 7}, {     37, 8}, \
    {     19, 7}, {     40, 8}, {     21, 7}, {     43, 8}, \
    {     23, 7}, {     47, 8}, {     25, 7}, {     51, 8}, \
    {     43, 9}, {     23, 8}, {     51, 9}, {     27, 8}, \
    {     57, 9}, {     31, 8}, {     65, 9}, {     35, 8}, \
    {     71, 9}, {     39, 8}, {     79, 9}, {     43,10}, \
    {     23, 9}, {     55,10}, {     31, 9}, {     71,10}, \
    {     39, 9}, {     87,10}, {     47, 9}, {     99,10}, \
    {     55, 9}, {    111,11}, {     31,10}, {     63, 9}, \
    {    127,10}, {     71, 9}, {    143,10}, {     87,11}, \
    {     47,10}, {    111,12}, {     31,11}, {     63,10}, \
    {    143,11}, {     79,10}, {    167,11}, {     95,10}, \
    {    191, 9}, {    383,10}, {    199, 9}, {    399,11}, \
    {    111,12}, {     63,11}, {    143,10}, {    287,12}, \
    {     95,11}, {    207,13}, {   8192,14}, {  16384,15}, \
    {  32768,16}, {  65536,17}, { 131072,18}, { 262144,19}, \
    { 524288,20}, {1048576,21}, {2097152,22}, {4194304,23}, \
    {8388608,24} }
#define SQR_FFT_TABLE3_SIZE 81
#define SQR_FFT_THRESHOLD                 6592

#define MULLO_BASECASE_THRESHOLD             0  /* always */
#define MULLO_DC_THRESHOLD                  63
#define MULLO_MUL_N_THRESHOLD            22239

#define DC_DIV_QR_THRESHOLD                 43
#define DC_DIVAPPR_Q_THRESHOLD             252
#define DC_BDIV_QR_THRESHOLD                38
#define DC_BDIV_Q_THRESHOLD                 88

#define INV_MULMOD_BNM1_THRESHOLD           64
#define INV_NEWTON_THRESHOLD               228
#define INV_APPR_THRESHOLD                 226

#define BINV_NEWTON_THRESHOLD              258
#define REDC_1_TO_REDC_2_THRESHOLD          35
#define REDC_2_TO_REDC_N_THRESHOLD          84

#define MU_DIV_QR_THRESHOLD               2089
#define MU_DIVAPPR_Q_THRESHOLD            1895
#define MUPI_DIV_QR_THRESHOLD              108
#define MU_BDIV_QR_THRESHOLD              1787
#define MU_BDIV_Q_THRESHOLD               1895

#define POWM_SEC_TABLE  2,23,195,643,2080

#define MATRIX22_STRASSEN_THRESHOLD         17
#define HGCD_THRESHOLD                     157
#define HGCD_APPR_THRESHOLD                 50
#define HGCD_REDUCE_THRESHOLD             4455
#define GCD_DC_THRESHOLD                   535
#define GCDEXT_DC_THRESHOLD                505
#define JACOBI_BASE_METHOD                   4

#define GET_STR_DC_THRESHOLD                17
#define GET_STR_PRECOMPUTE_THRESHOLD        32
#define SET_STR_DC_THRESHOLD               268
#define SET_STR_PRECOMPUTE_THRESHOLD      1787

#define FAC_DSC_THRESHOLD                 1474
#define FAC_ODD_THRESHOLD                   24
