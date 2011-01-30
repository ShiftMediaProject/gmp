/* SPARC v9 32-bit gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 1999, 2000, 2001, 2002, 2004, 2009, 2010, 2011
Free Software Foundation, Inc.

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

#define DIVREM_1_NORM_THRESHOLD              3
#define DIVREM_1_UNNORM_THRESHOLD        MP_SIZE_T_MAX  /* never */
#define MOD_1_NORM_THRESHOLD                 4
#define MOD_1_UNNORM_THRESHOLD              13
#define MOD_1N_TO_MOD_1_1_THRESHOLD      MP_SIZE_T_MAX  /* never */
#define MOD_1U_TO_MOD_1_1_THRESHOLD         10
#define MOD_1_1_TO_MOD_1_2_THRESHOLD         0  /* never mpn_mod_1_1p */
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        28
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD  MP_SIZE_T_MAX  /* never */
#define USE_PREINV_DIVREM_1                  1
#define DIVEXACT_1_THRESHOLD                 0  /* always */
#define BMOD_1_TO_MOD_1_THRESHOLD           16

#define MUL_TOOM22_THRESHOLD                28
#define MUL_TOOM33_THRESHOLD                93
#define MUL_TOOM44_THRESHOLD               166
#define MUL_TOOM6H_THRESHOLD               204
#define MUL_TOOM8H_THRESHOLD               434

#define MUL_TOOM32_TO_TOOM43_THRESHOLD     102
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     160
#define MUL_TOOM42_TO_TOOM53_THRESHOLD     114
#define MUL_TOOM42_TO_TOOM63_THRESHOLD     101

#define SQR_BASECASE_THRESHOLD               8
#define SQR_TOOM2_THRESHOLD                 62
#define SQR_TOOM3_THRESHOLD                110
#define SQR_TOOM4_THRESHOLD                274
#define SQR_TOOM6_THRESHOLD                  0
#define SQR_TOOM8_THRESHOLD                567

#define MULMOD_BNM1_THRESHOLD               14
#define SQRMOD_BNM1_THRESHOLD               20

#define MUL_FFT_MODF_THRESHOLD             240  /* k = 5 */
#define MUL_FFT_TABLE3                                      \
  { {    240, 5}, {     19, 6}, {     17, 7}, {      9, 6}, \
    {     20, 7}, {     11, 6}, {     23, 7}, {     15, 6}, \
    {     31, 7}, {     25, 8}, {     15, 7}, {     33, 8}, \
    {     19, 7}, {     39, 8}, {     23, 7}, {     53, 8}, \
    {     27, 9}, {     15, 8}, {     31, 7}, {     65, 8}, \
    {     35, 7}, {     71, 8}, {     39, 9}, {    512,10}, \
    {   1024,11}, {   2048,12}, {   4096,13}, {   8192,14}, \
    {  16384,15}, {  32768,16} }
#define MUL_FFT_TABLE3_SIZE 30
#define MUL_FFT_THRESHOLD                 2112

#define SQR_FFT_MODF_THRESHOLD             248  /* k = 5 */
#define SQR_FFT_TABLE3                                      \
  { {    248, 5}, {      8, 4}, {     17, 5}, {     19, 6}, \
    {     10, 5}, {     21, 6}, {     20, 7}, {     11, 6}, \
    {     23, 7}, {     21, 8}, {     11, 7}, {     27, 8}, \
    {     15, 7}, {     33, 8}, {     19, 7}, {     39, 8}, \
    {     23, 7}, {     47, 8}, {     27, 9}, {     15, 8}, \
    {     31, 7}, {     63, 8}, {     35, 7}, {     73, 8}, \
    {    256, 9}, {    512,10}, {   1024,11}, {   2048,12}, \
    {   4096,13}, {   8192,14}, {  16384,15}, {  32768,16} }
#define SQR_FFT_TABLE3_SIZE 32
#define SQR_FFT_THRESHOLD                 1856

#define MULLO_BASECASE_THRESHOLD             0  /* always */
#define MULLO_DC_THRESHOLD                 106
#define MULLO_MUL_N_THRESHOLD              827

#define DC_DIV_QR_THRESHOLD                123
#define DC_DIVAPPR_Q_THRESHOLD             396
#define DC_BDIV_QR_THRESHOLD               111
#define DC_BDIV_Q_THRESHOLD                280

#define INV_MULMOD_BNM1_THRESHOLD           54
#define INV_NEWTON_THRESHOLD               351
#define INV_APPR_THRESHOLD                 351

#define BINV_NEWTON_THRESHOLD              324
#define REDC_1_TO_REDC_N_THRESHOLD          94

#define MU_DIV_QR_THRESHOLD                541
#define MU_DIVAPPR_Q_THRESHOLD             792
#define MUPI_DIV_QR_THRESHOLD              132
#define MU_BDIV_QR_THRESHOLD               606
#define MU_BDIV_Q_THRESHOLD                734

#define MATRIX22_STRASSEN_THRESHOLD         13
#define HGCD_THRESHOLD                     148
#define GCD_DC_THRESHOLD                   386
#define GCDEXT_DC_THRESHOLD                416
#define JACOBI_BASE_METHOD                   4

#define GET_STR_DC_THRESHOLD                12
#define GET_STR_PRECOMPUTE_THRESHOLD        18
#define SET_STR_DC_THRESHOLD               537
#define SET_STR_PRECOMPUTE_THRESHOLD      1576
