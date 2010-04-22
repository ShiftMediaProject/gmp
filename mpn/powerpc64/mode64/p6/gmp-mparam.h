/* gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 1999, 2000, 2001, 2002, 2003, 2009, 2010 Free
Software Foundation, Inc.

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

/* POWER6 (kolga.bibsys.no) */

#define MOD_1_NORM_THRESHOLD                 5
#define MOD_1_UNNORM_THRESHOLD              13
#define MOD_1N_TO_MOD_1_1_THRESHOLD          6
#define MOD_1U_TO_MOD_1_1_THRESHOLD          5
#define MOD_1_1_TO_MOD_1_2_THRESHOLD         0
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        12
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD  MP_SIZE_T_MAX  /* never */
#define USE_PREINV_DIVREM_1                  0
#define DIVEXACT_1_THRESHOLD                 0  /* always (native) */
#define BMOD_1_TO_MOD_1_THRESHOLD        MP_SIZE_T_MAX  /* never */

#define MUL_TOOM22_THRESHOLD                18
#define MUL_TOOM33_THRESHOLD                33
#define MUL_TOOM44_THRESHOLD                82
#define MUL_TOOM6H_THRESHOLD               129
#define MUL_TOOM8H_THRESHOLD               163

#define MUL_TOOM32_TO_TOOM43_THRESHOLD      73
#define MUL_TOOM32_TO_TOOM53_THRESHOLD      63
#define MUL_TOOM42_TO_TOOM53_THRESHOLD      73
#define MUL_TOOM42_TO_TOOM63_THRESHOLD      66

#define SQR_BASECASE_THRESHOLD              10
#define SQR_TOOM2_THRESHOLD                 60
#define SQR_TOOM3_THRESHOLD                 69
#define SQR_TOOM4_THRESHOLD                138
#define SQR_TOOM6_THRESHOLD                191
#define SQR_TOOM8_THRESHOLD                272

#define MULMOD_BNM1_THRESHOLD               14
#define SQRMOD_BNM1_THRESHOLD               22

#define MUL_FFT_MODF_THRESHOLD             585  /* k = 5 */
#define MUL_FFT_TABLE3                                      \
  { {    585, 5}, {     25, 6}, {     29, 7}, {     15, 6}, \
    {     33, 7}, {     17, 6}, {     35, 7}, {     29, 8}, \
    {     15, 7}, {     33, 8}, {     17, 7}, {     36, 8}, \
    {     19, 7}, {     39, 8}, {     23, 7}, {     47, 8}, \
    {     29, 9}, {     15, 8}, {     35, 9}, {     19, 8}, \
    {     41, 9}, {     23, 8}, {     49, 9}, {     27,10}, \
    {     15, 9}, {     31, 8}, {     63, 9}, {     43,10}, \
    {     23, 9}, {     55,11}, {     15,10}, {     31, 9}, \
    {     71,10}, {     39, 9}, {     83,10}, {     47, 9}, \
    {     99,10}, {     55,11}, {     31,10}, {     63, 9}, \
    {    127,10}, {     79,11}, {     47,10}, {     95,12}, \
    {     31,11}, {     63,10}, {    135,11}, {     79,10}, \
    {    159,11}, {     95,10}, {    191,11}, {    111,12}, \
    {     63,11}, {    159,12}, {     95,11}, {    191,13}, \
    {   8192,14}, {  16384,15}, {  32768,16}, {  65536,17}, \
    { 131072,18}, { 262144,19}, { 524288,20}, {1048576,21}, \
    {2097152,22}, {4194304,23}, {8388608,24} }
#define MUL_FFT_TABLE3_SIZE 67
#define MUL_FFT_THRESHOLD                 7296

#define SQR_FFT_MODF_THRESHOLD             468  /* k = 5 */
#define SQR_FFT_TABLE3                                      \
  { {    468, 5}, {     27, 6}, {     29, 7}, {     29, 8}, \
    {     15, 7}, {     32, 8}, {     17, 7}, {     35, 8}, \
    {     19, 7}, {     39, 8}, {     29, 9}, {     15, 8}, \
    {     35, 9}, {     19, 8}, {     41, 9}, {     23, 8}, \
    {     47, 9}, {     27, 8}, {     55, 9}, {     31, 8}, \
    {     63, 9}, {     39,10}, {     23, 9}, {     55,10}, \
    {     31, 9}, {     67,10}, {     39, 9}, {     79,10}, \
    {     47, 9}, {     95,10}, {     55,11}, {     31,10}, \
    {     79,11}, {     47,10}, {     95,12}, {     31,11}, \
    {     63,10}, {    135,11}, {     79,10}, {    159,11}, \
    {     95,10}, {    191,12}, {     63,11}, {    127,10}, \
    {    255,11}, {    143,10}, {    287,11}, {    159,12}, \
    {     95,11}, {    191,13}, {   8192,14}, {  16384,15}, \
    {  32768,16}, {  65536,17}, { 131072,18}, { 262144,19}, \
    { 524288,20}, {1048576,21}, {2097152,22}, {4194304,23}, \
    {8388608,24} }
#define SQR_FFT_TABLE3_SIZE 61
#define SQR_FFT_THRESHOLD                 4160

#define MULLO_BASECASE_THRESHOLD             0  /* always */
#define MULLO_DC_THRESHOLD                 140
#define MULLO_MUL_N_THRESHOLD            13161

#define DC_DIV_QR_THRESHOLD                122
#define DC_DIVAPPR_Q_THRESHOLD             330
#define DC_BDIV_QR_THRESHOLD               125
#define DC_BDIV_Q_THRESHOLD                270

#define INV_MULMOD_BNM1_THRESHOLD          393
#define INV_NEWTON_THRESHOLD               315
#define INV_APPR_THRESHOLD                 318

#define BINV_NEWTON_THRESHOLD              345
#define REDC_1_TO_REDC_N_THRESHOLD          87

#define MU_DIV_QR_THRESHOLD                734
#define MU_DIVAPPR_Q_THRESHOLD             872
#define MUPI_DIV_QR_THRESHOLD              108
#define MU_BDIV_QR_THRESHOLD               734
#define MU_BDIV_Q_THRESHOLD                960

#define MATRIX22_STRASSEN_THRESHOLD         13
#define HGCD_THRESHOLD                     212
#define GCD_DC_THRESHOLD                   548
#define GCDEXT_DC_THRESHOLD                438
#define JACOBI_BASE_METHOD                   3

#define GET_STR_DC_THRESHOLD                19
#define GET_STR_PRECOMPUTE_THRESHOLD        33
#define SET_STR_DC_THRESHOLD              1607
#define SET_STR_PRECOMPUTE_THRESHOLD      3252

/* Tuneup completed successfully, took 446 seconds */
