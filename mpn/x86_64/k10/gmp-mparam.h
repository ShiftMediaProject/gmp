/* AMD K10 gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 2000-2012 Free Software Foundation, Inc.

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

#if 0
#undef mpn_sublsh_n
#define mpn_sublsh_n(rp,up,vp,n,c)					\
  (((rp) == (up)) ? mpn_submul_1 (rp, vp, n, CNST_LIMB(1) << (c))	\
   : MPN(mpn_sublsh_n)(rp,up,vp,n,c))
#endif

#define MOD_1_NORM_THRESHOLD                 0  /* always */
#define MOD_1_UNNORM_THRESHOLD               0  /* always */
#define MOD_1N_TO_MOD_1_1_THRESHOLD          3
#define MOD_1U_TO_MOD_1_1_THRESHOLD          2
#define MOD_1_1_TO_MOD_1_2_THRESHOLD        19
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        22
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD      7
#define USE_PREINV_DIVREM_1                  1  /* native */
#define DIV_QR_2_PI2_THRESHOLD           MP_SIZE_T_MAX  /* never */
#define DIVEXACT_1_THRESHOLD                 0  /* always (native) */
#define BMOD_1_TO_MOD_1_THRESHOLD           15

#define MUL_TOOM22_THRESHOLD                28
#define MUL_TOOM33_THRESHOLD                81
#define MUL_TOOM44_THRESHOLD               242
#define MUL_TOOM6H_THRESHOLD               369
#define MUL_TOOM8H_THRESHOLD               482

#define MUL_TOOM32_TO_TOOM43_THRESHOLD      97
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     155
#define MUL_TOOM42_TO_TOOM53_THRESHOLD     149
#define MUL_TOOM42_TO_TOOM63_THRESHOLD     163
#define MUL_TOOM43_TO_TOOM54_THRESHOLD     226

#define SQR_BASECASE_THRESHOLD               0  /* always (native) */
#define SQR_TOOM2_THRESHOLD                 34
#define SQR_TOOM3_THRESHOLD                113
#define SQR_TOOM4_THRESHOLD                336
#define SQR_TOOM6_THRESHOLD                446
#define SQR_TOOM8_THRESHOLD                597

#define MULMID_TOOM42_THRESHOLD             36

#define MULMOD_BNM1_THRESHOLD               15
#define SQRMOD_BNM1_THRESHOLD               18

#define MUL_FFT_MODF_THRESHOLD             570  /* k = 5 */
#define MUL_FFT_TABLE3                                      \
  { {    570, 5}, {     25, 6}, {     13, 5}, {     27, 6}, \
    {     27, 7}, {     14, 6}, {     29, 7}, {     15, 6}, \
    {     31, 7}, {     16, 6}, {     33, 7}, {     29, 8}, \
    {     15, 7}, {     33, 8}, {     17, 7}, {     36, 8}, \
    {     19, 7}, {     39, 8}, {     21, 7}, {     43, 8}, \
    {     23, 7}, {     47, 8}, {     25, 7}, {     51, 8}, \
    {     29, 9}, {     15, 8}, {     35, 9}, {     19, 8}, \
    {     43, 9}, {     23, 8}, {     51, 9}, {     27, 8}, \
    {     55,10}, {     15, 9}, {     31, 8}, {     65, 9}, \
    {     43,10}, {     23, 9}, {     55,10}, {     31, 9}, \
    {     67,10}, {     39, 9}, {     83,10}, {     47, 9}, \
    {     95,10}, {     55,11}, {     31,10}, {     87,11}, \
    {     47,10}, {    111,12}, {     31,11}, {     63,10}, \
    {    135,11}, {     79,10}, {    167,11}, {     95,10}, \
    {    191,11}, {    111,12}, {     63,11}, {    143,10}, \
    {    287, 9}, {    575,11}, {    175,12}, {     95,11}, \
    {    207,13}, {   8192,14}, {  16384,15}, {  32768,16}, \
    {  65536,17}, { 131072,18}, { 262144,19}, { 524288,20}, \
    {1048576,21}, {2097152,22}, {4194304,23}, {8388608,24} }
#define MUL_FFT_TABLE3_SIZE 76
#define MUL_FFT_THRESHOLD                 7808

#define SQR_FFT_MODF_THRESHOLD             472  /* k = 5 */
#define SQR_FFT_TABLE3                                      \
  { {    472, 5}, {     23, 6}, {     12, 5}, {     25, 6}, \
    {     13, 5}, {     27, 6}, {     29, 7}, {     15, 6}, \
    {     31, 7}, {     16, 6}, {     33, 7}, {     29, 8}, \
    {     15, 7}, {     32, 8}, {     17, 7}, {     35, 8}, \
    {     19, 7}, {     39, 8}, {     21, 7}, {     43, 8}, \
    {     23, 7}, {     47, 8}, {     29, 9}, {     15, 8}, \
    {     35, 9}, {     19, 8}, {     43, 9}, {     23, 8}, \
    {     51, 9}, {     27, 8}, {     55,10}, {     15, 9}, \
    {     31, 8}, {     65, 9}, {     43,10}, {     23, 9}, \
    {     55,11}, {     15,10}, {     31, 9}, {     67,10}, \
    {     39, 9}, {     83,10}, {     47, 9}, {     95,10}, \
    {     55,11}, {     31,10}, {     79,11}, {     47,10}, \
    {    103,12}, {     31,11}, {     63,10}, {    135,11}, \
    {     79,10}, {    159,11}, {    111,12}, {     63,10}, \
    {    255,11}, {    143,10}, {    287,11}, {    159,12}, \
    {     95,11}, {    191,10}, {    383, 9}, {    767,10}, \
    {    399,11}, {    207,13}, {   8192,14}, {  16384,15}, \
    {  32768,16}, {  65536,17}, { 131072,18}, { 262144,19}, \
    { 524288,20}, {1048576,21}, {2097152,22}, {4194304,23}, \
    {8388608,24} }
#define SQR_FFT_TABLE3_SIZE 77
#define SQR_FFT_THRESHOLD                 5568

#define MULLO_BASECASE_THRESHOLD             0  /* always */
#define MULLO_DC_THRESHOLD                  61
#define MULLO_MUL_N_THRESHOLD            15150

#define DC_DIV_QR_THRESHOLD                 56
#define DC_DIVAPPR_Q_THRESHOLD             230
#define DC_BDIV_QR_THRESHOLD                52
#define DC_BDIV_Q_THRESHOLD                 44

#define INV_MULMOD_BNM1_THRESHOLD           54
#define INV_NEWTON_THRESHOLD               222
#define INV_APPR_THRESHOLD                 214

#define BINV_NEWTON_THRESHOLD              324
#define REDC_1_TO_REDC_2_THRESHOLD          51
#define REDC_2_TO_REDC_N_THRESHOLD          71

#define MU_DIV_QR_THRESHOLD               1718
#define MU_DIVAPPR_Q_THRESHOLD            1652
#define MUPI_DIV_QR_THRESHOLD              102
#define MU_BDIV_QR_THRESHOLD              1528
#define MU_BDIV_Q_THRESHOLD               1589

#define POWM_SEC_TABLE  2,23,130,572,1358

#define MATRIX22_STRASSEN_THRESHOLD         17
#define HGCD_THRESHOLD                     135
#define HGCD_APPR_THRESHOLD                171
#define HGCD_REDUCE_THRESHOLD             3524
#define GCD_DC_THRESHOLD                   622
#define GCDEXT_DC_THRESHOLD                469
#define JACOBI_BASE_METHOD                   4

#define GET_STR_DC_THRESHOLD                13
#define GET_STR_PRECOMPUTE_THRESHOLD        29
#define SET_STR_DC_THRESHOLD               248
#define SET_STR_PRECOMPUTE_THRESHOLD      1648

#define FAC_DSC_THRESHOLD                 1075
#define FAC_ODD_THRESHOLD                   23
