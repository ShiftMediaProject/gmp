/* mpz_fib_ui(result, n) -- Set RESULT to the Nth Fibonacci number.

Copyright (C) 2000 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */


/* The lookup table makes small n instantaneous.  After that either a simple
   addition loop or a powering algorithm is used.

   Times vary a bit between odd and even sizes, due to the powering using a
   different last step for odd or even.  The even case does one multiply,
   the odd case two squares.

   Future:

   There might be some value in returning both F[n] and F[n-1], so that an
   application can then iterate from that pair.  Currently that would
   require a wasteful second call to get F[n-1].

   If the Lucas numbers L[n] were used as an auxiliary sequence, the
   "doubling" operation would be two squares rather than two multiplies.
   The formulas are a little more complicated, something like the following
   (not checked).

       F[2n] = ((F[n]+L[n])^2 - 6*F[n]^2 - 4*(-1)^n) / 2
       L[2n] = 5*F[n]^2 + 2*(-1)^n

       F[2n+1] = (F[2n] + L[2n]) / 2
       L[2n+1] = (5*F[2n] + L[2n]) / 2

   Lookup tables for L[] like the current F[] ones would be wanted.
   The L[] could even be optionally returned.

   Are there formulas with two squares using just F[n]?  Suspect not.  */


#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"


/* change this to "#define TRACE(x) x" for diagnostics */
#define TRACE(x)


/* table1[i] is F[i], table2[i] is F[i+numberof(table1)].
   table2[i][0] is the low limb, table2[i][1] the high limb.  */

#if BITS_PER_MP_LIMB == 2
static const mp_limb_t table1[] = {
 CNST_LIMB (0x0),  /* 0 */
 CNST_LIMB (0x1),  /* 1 */
 CNST_LIMB (0x1),  /* 2 */
 CNST_LIMB (0x2),  /* 3 */
 CNST_LIMB (0x3),  /* 4 */
};
static const mp_limb_t table2[][2] = {
 {CNST_LIMB(0x1), CNST_LIMB(0x1)}, /* 5 */
 {CNST_LIMB(0x0), CNST_LIMB(0x2)}, /* 6 */
 {CNST_LIMB(0x1), CNST_LIMB(0x3)}, /* 7 */
};
/* total 11 bytes if BITS_PER_LIMB==2 */
#endif

#if BITS_PER_MP_LIMB == 4
static const mp_limb_t table1[] = {
 CNST_LIMB (0x0),  /* 0 */
 CNST_LIMB (0x1),  /* 1 */
 CNST_LIMB (0x1),  /* 2 */
 CNST_LIMB (0x2),  /* 3 */
 CNST_LIMB (0x3),  /* 4 */
 CNST_LIMB (0x5),  /* 5 */
 CNST_LIMB (0x8),  /* 6 */
 CNST_LIMB (0xD),  /* 7 */
};
static const mp_limb_t table2[][2] = {
 {CNST_LIMB(0x5), CNST_LIMB(0x1)}, /* 8 */
 {CNST_LIMB(0x2), CNST_LIMB(0x2)}, /* 9 */
 {CNST_LIMB(0x7), CNST_LIMB(0x3)}, /* 10 */
 {CNST_LIMB(0x9), CNST_LIMB(0x5)}, /* 11 */
 {CNST_LIMB(0x0), CNST_LIMB(0x9)}, /* 12 */
 {CNST_LIMB(0x9), CNST_LIMB(0xE)}, /* 13 */
};
/* total 20 bytes if BITS_PER_LIMB==4 */
#endif

#if BITS_PER_MP_LIMB == 8
static const mp_limb_t table1[] = {
 CNST_LIMB (0x0),  /* 0 */
 CNST_LIMB (0x1),  /* 1 */
 CNST_LIMB (0x1),  /* 2 */
 CNST_LIMB (0x2),  /* 3 */
 CNST_LIMB (0x3),  /* 4 */
 CNST_LIMB (0x5),  /* 5 */
 CNST_LIMB (0x8),  /* 6 */
 CNST_LIMB (0xD),  /* 7 */
 CNST_LIMB (0x15),  /* 8 */
 CNST_LIMB (0x22),  /* 9 */
 CNST_LIMB (0x37),  /* 10 */
 CNST_LIMB (0x59),  /* 11 */
 CNST_LIMB (0x90),  /* 12 */
 CNST_LIMB (0xE9),  /* 13 */
};
static const mp_limb_t table2[][2] = {
 {CNST_LIMB(0x79), CNST_LIMB(0x1)}, /* 14 */
 {CNST_LIMB(0x62), CNST_LIMB(0x2)}, /* 15 */
 {CNST_LIMB(0xDB), CNST_LIMB(0x3)}, /* 16 */
 {CNST_LIMB(0x3D), CNST_LIMB(0x6)}, /* 17 */
 {CNST_LIMB(0x18), CNST_LIMB(0xA)}, /* 18 */
 {CNST_LIMB(0x55), CNST_LIMB(0x10)}, /* 19 */
 {CNST_LIMB(0x6D), CNST_LIMB(0x1A)}, /* 20 */
 {CNST_LIMB(0xC2), CNST_LIMB(0x2A)}, /* 21 */
 {CNST_LIMB(0x2F), CNST_LIMB(0x45)}, /* 22 */
 {CNST_LIMB(0xF1), CNST_LIMB(0x6F)}, /* 23 */
 {CNST_LIMB(0x20), CNST_LIMB(0xB5)}, /* 24 */
};
/* total 36 bytes if BITS_PER_LIMB==8 */
#endif

#if BITS_PER_MP_LIMB == 16
static const mp_limb_t table1[] = {
 CNST_LIMB (0x0),  /* 0 */
 CNST_LIMB (0x1),  /* 1 */
 CNST_LIMB (0x1),  /* 2 */
 CNST_LIMB (0x2),  /* 3 */
 CNST_LIMB (0x3),  /* 4 */
 CNST_LIMB (0x5),  /* 5 */
 CNST_LIMB (0x8),  /* 6 */
 CNST_LIMB (0xD),  /* 7 */
 CNST_LIMB (0x15),  /* 8 */
 CNST_LIMB (0x22),  /* 9 */
 CNST_LIMB (0x37),  /* 10 */
 CNST_LIMB (0x59),  /* 11 */
 CNST_LIMB (0x90),  /* 12 */
 CNST_LIMB (0xE9),  /* 13 */
 CNST_LIMB (0x179),  /* 14 */
 CNST_LIMB (0x262),  /* 15 */
 CNST_LIMB (0x3DB),  /* 16 */
 CNST_LIMB (0x63D),  /* 17 */
 CNST_LIMB (0xA18),  /* 18 */
 CNST_LIMB (0x1055),  /* 19 */
 CNST_LIMB (0x1A6D),  /* 20 */
 CNST_LIMB (0x2AC2),  /* 21 */
 CNST_LIMB (0x452F),  /* 22 */
 CNST_LIMB (0x6FF1),  /* 23 */
 CNST_LIMB (0xB520),  /* 24 */
};
static const mp_limb_t table2[][2] = {
 {CNST_LIMB(0x2511), CNST_LIMB(0x1)}, /* 25 */
 {CNST_LIMB(0xDA31), CNST_LIMB(0x1)}, /* 26 */
 {CNST_LIMB(0xFF42), CNST_LIMB(0x2)}, /* 27 */
 {CNST_LIMB(0xD973), CNST_LIMB(0x4)}, /* 28 */
 {CNST_LIMB(0xD8B5), CNST_LIMB(0x7)}, /* 29 */
 {CNST_LIMB(0xB228), CNST_LIMB(0xC)}, /* 30 */
 {CNST_LIMB(0x8ADD), CNST_LIMB(0x14)}, /* 31 */
 {CNST_LIMB(0x3D05), CNST_LIMB(0x21)}, /* 32 */
 {CNST_LIMB(0xC7E2), CNST_LIMB(0x35)}, /* 33 */
 {CNST_LIMB(0x4E7), CNST_LIMB(0x57)}, /* 34 */
 {CNST_LIMB(0xCCC9), CNST_LIMB(0x8C)}, /* 35 */
 {CNST_LIMB(0xD1B0), CNST_LIMB(0xE3)}, /* 36 */
 {CNST_LIMB(0x9E79), CNST_LIMB(0x170)}, /* 37 */
 {CNST_LIMB(0x7029), CNST_LIMB(0x254)}, /* 38 */
 {CNST_LIMB(0xEA2), CNST_LIMB(0x3C5)}, /* 39 */
 {CNST_LIMB(0x7ECB), CNST_LIMB(0x619)}, /* 40 */
 {CNST_LIMB(0x8D6D), CNST_LIMB(0x9DE)}, /* 41 */
 {CNST_LIMB(0xC38), CNST_LIMB(0xFF8)}, /* 42 */
 {CNST_LIMB(0x99A5), CNST_LIMB(0x19D6)}, /* 43 */
 {CNST_LIMB(0xA5DD), CNST_LIMB(0x29CE)}, /* 44 */
 {CNST_LIMB(0x3F82), CNST_LIMB(0x43A5)}, /* 45 */
 {CNST_LIMB(0xE55F), CNST_LIMB(0x6D73)}, /* 46 */
 {CNST_LIMB(0x24E1), CNST_LIMB(0xB119)}, /* 47 */
};
/* total 142 bytes if BITS_PER_LIMB==16 */
#endif

#if BITS_PER_MP_LIMB == 32
static const mp_limb_t table1[] = {
 CNST_LIMB (0x0),  /* 0 */
 CNST_LIMB (0x1),  /* 1 */
 CNST_LIMB (0x1),  /* 2 */
 CNST_LIMB (0x2),  /* 3 */
 CNST_LIMB (0x3),  /* 4 */
 CNST_LIMB (0x5),  /* 5 */
 CNST_LIMB (0x8),  /* 6 */
 CNST_LIMB (0xD),  /* 7 */
 CNST_LIMB (0x15),  /* 8 */
 CNST_LIMB (0x22),  /* 9 */
 CNST_LIMB (0x37),  /* 10 */
 CNST_LIMB (0x59),  /* 11 */
 CNST_LIMB (0x90),  /* 12 */
 CNST_LIMB (0xE9),  /* 13 */
 CNST_LIMB (0x179),  /* 14 */
 CNST_LIMB (0x262),  /* 15 */
 CNST_LIMB (0x3DB),  /* 16 */
 CNST_LIMB (0x63D),  /* 17 */
 CNST_LIMB (0xA18),  /* 18 */
 CNST_LIMB (0x1055),  /* 19 */
 CNST_LIMB (0x1A6D),  /* 20 */
 CNST_LIMB (0x2AC2),  /* 21 */
 CNST_LIMB (0x452F),  /* 22 */
 CNST_LIMB (0x6FF1),  /* 23 */
 CNST_LIMB (0xB520),  /* 24 */
 CNST_LIMB (0x12511),  /* 25 */
 CNST_LIMB (0x1DA31),  /* 26 */
 CNST_LIMB (0x2FF42),  /* 27 */
 CNST_LIMB (0x4D973),  /* 28 */
 CNST_LIMB (0x7D8B5),  /* 29 */
 CNST_LIMB (0xCB228),  /* 30 */
 CNST_LIMB (0x148ADD),  /* 31 */
 CNST_LIMB (0x213D05),  /* 32 */
 CNST_LIMB (0x35C7E2),  /* 33 */
 CNST_LIMB (0x5704E7),  /* 34 */
 CNST_LIMB (0x8CCCC9),  /* 35 */
 CNST_LIMB (0xE3D1B0),  /* 36 */
 CNST_LIMB (0x1709E79),  /* 37 */
 CNST_LIMB (0x2547029),  /* 38 */
 CNST_LIMB (0x3C50EA2),  /* 39 */
 CNST_LIMB (0x6197ECB),  /* 40 */
 CNST_LIMB (0x9DE8D6D),  /* 41 */
 CNST_LIMB (0xFF80C38),  /* 42 */
 CNST_LIMB (0x19D699A5),  /* 43 */
 CNST_LIMB (0x29CEA5DD),  /* 44 */
 CNST_LIMB (0x43A53F82),  /* 45 */
 CNST_LIMB (0x6D73E55F),  /* 46 */
 CNST_LIMB (0xB11924E1),  /* 47 */
};
static const mp_limb_t table2[][2] = {
 {CNST_LIMB(0x1E8D0A40), CNST_LIMB(0x1)}, /* 48 */
 {CNST_LIMB(0xCFA62F21), CNST_LIMB(0x1)}, /* 49 */
 {CNST_LIMB(0xEE333961), CNST_LIMB(0x2)}, /* 50 */
 {CNST_LIMB(0xBDD96882), CNST_LIMB(0x4)}, /* 51 */
 {CNST_LIMB(0xAC0CA1E3), CNST_LIMB(0x7)}, /* 52 */
 {CNST_LIMB(0x69E60A65), CNST_LIMB(0xC)}, /* 53 */
 {CNST_LIMB(0x15F2AC48), CNST_LIMB(0x14)}, /* 54 */
 {CNST_LIMB(0x7FD8B6AD), CNST_LIMB(0x20)}, /* 55 */
 {CNST_LIMB(0x95CB62F5), CNST_LIMB(0x34)}, /* 56 */
 {CNST_LIMB(0x15A419A2), CNST_LIMB(0x55)}, /* 57 */
 {CNST_LIMB(0xAB6F7C97), CNST_LIMB(0x89)}, /* 58 */
 {CNST_LIMB(0xC1139639), CNST_LIMB(0xDE)}, /* 59 */
 {CNST_LIMB(0x6C8312D0), CNST_LIMB(0x168)}, /* 60 */
 {CNST_LIMB(0x2D96A909), CNST_LIMB(0x247)}, /* 61 */
 {CNST_LIMB(0x9A19BBD9), CNST_LIMB(0x3AF)}, /* 62 */
 {CNST_LIMB(0xC7B064E2), CNST_LIMB(0x5F6)}, /* 63 */
 {CNST_LIMB(0x61CA20BB), CNST_LIMB(0x9A6)}, /* 64 */
 {CNST_LIMB(0x297A859D), CNST_LIMB(0xF9D)}, /* 65 */
 {CNST_LIMB(0x8B44A658), CNST_LIMB(0x1943)}, /* 66 */
 {CNST_LIMB(0xB4BF2BF5), CNST_LIMB(0x28E0)}, /* 67 */
 {CNST_LIMB(0x4003D24D), CNST_LIMB(0x4224)}, /* 68 */
 {CNST_LIMB(0xF4C2FE42), CNST_LIMB(0x6B04)}, /* 69 */
 {CNST_LIMB(0x34C6D08F), CNST_LIMB(0xAD29)}, /* 70 */
 {CNST_LIMB(0x2989CED1), CNST_LIMB(0x1182E)}, /* 71 */
 {CNST_LIMB(0x5E509F60), CNST_LIMB(0x1C557)}, /* 72 */
 {CNST_LIMB(0x87DA6E31), CNST_LIMB(0x2DD85)}, /* 73 */
 {CNST_LIMB(0xE62B0D91), CNST_LIMB(0x4A2DC)}, /* 74 */
 {CNST_LIMB(0x6E057BC2), CNST_LIMB(0x78062)}, /* 75 */
 {CNST_LIMB(0x54308953), CNST_LIMB(0xC233F)}, /* 76 */
 {CNST_LIMB(0xC2360515), CNST_LIMB(0x13A3A1)}, /* 77 */
 {CNST_LIMB(0x16668E68), CNST_LIMB(0x1FC6E1)}, /* 78 */
 {CNST_LIMB(0xD89C937D), CNST_LIMB(0x336A82)}, /* 79 */
 {CNST_LIMB(0xEF0321E5), CNST_LIMB(0x533163)}, /* 80 */
 {CNST_LIMB(0xC79FB562), CNST_LIMB(0x869BE6)}, /* 81 */
 {CNST_LIMB(0xB6A2D747), CNST_LIMB(0xD9CD4A)}, /* 82 */
 {CNST_LIMB(0x7E428CA9), CNST_LIMB(0x1606931)}, /* 83 */
 {CNST_LIMB(0x34E563F0), CNST_LIMB(0x23A367C)}, /* 84 */
 {CNST_LIMB(0xB327F099), CNST_LIMB(0x39A9FAD)}, /* 85 */
 {CNST_LIMB(0xE80D5489), CNST_LIMB(0x5D4D629)}, /* 86 */
 {CNST_LIMB(0x9B354522), CNST_LIMB(0x96F75D7)}, /* 87 */
 {CNST_LIMB(0x834299AB), CNST_LIMB(0xF444C01)}, /* 88 */
 {CNST_LIMB(0x1E77DECD), CNST_LIMB(0x18B3C1D9)}, /* 89 */
 {CNST_LIMB(0xA1BA7878), CNST_LIMB(0x27F80DDA)}, /* 90 */
 {CNST_LIMB(0xC0325745), CNST_LIMB(0x40ABCFB3)}, /* 91 */
 {CNST_LIMB(0x61ECCFBD), CNST_LIMB(0x68A3DD8E)}, /* 92 */
 {CNST_LIMB(0x221F2702), CNST_LIMB(0xA94FAD42)}, /* 93 */
};
/* total 560 bytes if BITS_PER_LIMB==32 */
#endif

#if BITS_PER_MP_LIMB == 64
static const mp_limb_t table1[] = {
 CNST_LIMB (0x0),  /* 0 */
 CNST_LIMB (0x1),  /* 1 */
 CNST_LIMB (0x1),  /* 2 */
 CNST_LIMB (0x2),  /* 3 */
 CNST_LIMB (0x3),  /* 4 */
 CNST_LIMB (0x5),  /* 5 */
 CNST_LIMB (0x8),  /* 6 */
 CNST_LIMB (0xD),  /* 7 */
 CNST_LIMB (0x15),  /* 8 */
 CNST_LIMB (0x22),  /* 9 */
 CNST_LIMB (0x37),  /* 10 */
 CNST_LIMB (0x59),  /* 11 */
 CNST_LIMB (0x90),  /* 12 */
 CNST_LIMB (0xE9),  /* 13 */
 CNST_LIMB (0x179),  /* 14 */
 CNST_LIMB (0x262),  /* 15 */
 CNST_LIMB (0x3DB),  /* 16 */
 CNST_LIMB (0x63D),  /* 17 */
 CNST_LIMB (0xA18),  /* 18 */
 CNST_LIMB (0x1055),  /* 19 */
 CNST_LIMB (0x1A6D),  /* 20 */
 CNST_LIMB (0x2AC2),  /* 21 */
 CNST_LIMB (0x452F),  /* 22 */
 CNST_LIMB (0x6FF1),  /* 23 */
 CNST_LIMB (0xB520),  /* 24 */
 CNST_LIMB (0x12511),  /* 25 */
 CNST_LIMB (0x1DA31),  /* 26 */
 CNST_LIMB (0x2FF42),  /* 27 */
 CNST_LIMB (0x4D973),  /* 28 */
 CNST_LIMB (0x7D8B5),  /* 29 */
 CNST_LIMB (0xCB228),  /* 30 */
 CNST_LIMB (0x148ADD),  /* 31 */
 CNST_LIMB (0x213D05),  /* 32 */
 CNST_LIMB (0x35C7E2),  /* 33 */
 CNST_LIMB (0x5704E7),  /* 34 */
 CNST_LIMB (0x8CCCC9),  /* 35 */
 CNST_LIMB (0xE3D1B0),  /* 36 */
 CNST_LIMB (0x1709E79),  /* 37 */
 CNST_LIMB (0x2547029),  /* 38 */
 CNST_LIMB (0x3C50EA2),  /* 39 */
 CNST_LIMB (0x6197ECB),  /* 40 */
 CNST_LIMB (0x9DE8D6D),  /* 41 */
 CNST_LIMB (0xFF80C38),  /* 42 */
 CNST_LIMB (0x19D699A5),  /* 43 */
 CNST_LIMB (0x29CEA5DD),  /* 44 */
 CNST_LIMB (0x43A53F82),  /* 45 */
 CNST_LIMB (0x6D73E55F),  /* 46 */
 CNST_LIMB (0xB11924E1),  /* 47 */
 CNST_LIMB (0x11E8D0A40),  /* 48 */
 CNST_LIMB (0x1CFA62F21),  /* 49 */
 CNST_LIMB (0x2EE333961),  /* 50 */
 CNST_LIMB (0x4BDD96882),  /* 51 */
 CNST_LIMB (0x7AC0CA1E3),  /* 52 */
 CNST_LIMB (0xC69E60A65),  /* 53 */
 CNST_LIMB (0x1415F2AC48),  /* 54 */
 CNST_LIMB (0x207FD8B6AD),  /* 55 */
 CNST_LIMB (0x3495CB62F5),  /* 56 */
 CNST_LIMB (0x5515A419A2),  /* 57 */
 CNST_LIMB (0x89AB6F7C97),  /* 58 */
 CNST_LIMB (0xDEC1139639),  /* 59 */
 CNST_LIMB (0x1686C8312D0),  /* 60 */
 CNST_LIMB (0x2472D96A909),  /* 61 */
 CNST_LIMB (0x3AF9A19BBD9),  /* 62 */
 CNST_LIMB (0x5F6C7B064E2),  /* 63 */
 CNST_LIMB (0x9A661CA20BB),  /* 64 */
 CNST_LIMB (0xF9D297A859D),  /* 65 */
 CNST_LIMB (0x19438B44A658),  /* 66 */
 CNST_LIMB (0x28E0B4BF2BF5),  /* 67 */
 CNST_LIMB (0x42244003D24D),  /* 68 */
 CNST_LIMB (0x6B04F4C2FE42),  /* 69 */
 CNST_LIMB (0xAD2934C6D08F),  /* 70 */
 CNST_LIMB (0x1182E2989CED1),  /* 71 */
 CNST_LIMB (0x1C5575E509F60),  /* 72 */
 CNST_LIMB (0x2DD8587DA6E31),  /* 73 */
 CNST_LIMB (0x4A2DCE62B0D91),  /* 74 */
 CNST_LIMB (0x780626E057BC2),  /* 75 */
 CNST_LIMB (0xC233F54308953),  /* 76 */
 CNST_LIMB (0x13A3A1C2360515),  /* 77 */
 CNST_LIMB (0x1FC6E116668E68),  /* 78 */
 CNST_LIMB (0x336A82D89C937D),  /* 79 */
 CNST_LIMB (0x533163EF0321E5),  /* 80 */
 CNST_LIMB (0x869BE6C79FB562),  /* 81 */
 CNST_LIMB (0xD9CD4AB6A2D747),  /* 82 */
 CNST_LIMB (0x16069317E428CA9),  /* 83 */
 CNST_LIMB (0x23A367C34E563F0),  /* 84 */
 CNST_LIMB (0x39A9FADB327F099),  /* 85 */
 CNST_LIMB (0x5D4D629E80D5489),  /* 86 */
 CNST_LIMB (0x96F75D79B354522),  /* 87 */
 CNST_LIMB (0xF444C01834299AB),  /* 88 */
 CNST_LIMB (0x18B3C1D91E77DECD),  /* 89 */
 CNST_LIMB (0x27F80DDAA1BA7878),  /* 90 */
 CNST_LIMB (0x40ABCFB3C0325745),  /* 91 */
 CNST_LIMB (0x68A3DD8E61ECCFBD),  /* 92 */
 CNST_LIMB (0xA94FAD42221F2702),  /* 93 */
};
static const mp_limb_t table2[][2] = {
 {CNST_LIMB(0x11F38AD0840BF6BF), CNST_LIMB(0x1)}, /* 94 */
 {CNST_LIMB(0xBB433812A62B1DC1), CNST_LIMB(0x1)}, /* 95 */
 {CNST_LIMB(0xCD36C2E32A371480), CNST_LIMB(0x2)}, /* 96 */
 {CNST_LIMB(0x8879FAF5D0623241), CNST_LIMB(0x4)}, /* 97 */
 {CNST_LIMB(0x55B0BDD8FA9946C1), CNST_LIMB(0x7)}, /* 98 */
 {CNST_LIMB(0xDE2AB8CECAFB7902), CNST_LIMB(0xB)}, /* 99 */
 {CNST_LIMB(0x33DB76A7C594BFC3), CNST_LIMB(0x13)}, /* 100 */
 {CNST_LIMB(0x12062F76909038C5), CNST_LIMB(0x1F)}, /* 101 */
 {CNST_LIMB(0x45E1A61E5624F888), CNST_LIMB(0x32)}, /* 102 */
 {CNST_LIMB(0x57E7D594E6B5314D), CNST_LIMB(0x51)}, /* 103 */
 {CNST_LIMB(0x9DC97BB33CDA29D5), CNST_LIMB(0x83)}, /* 104 */
 {CNST_LIMB(0xF5B15148238F5B22), CNST_LIMB(0xD4)}, /* 105 */
 {CNST_LIMB(0x937ACCFB606984F7), CNST_LIMB(0x158)}, /* 106 */
 {CNST_LIMB(0x892C1E4383F8E019), CNST_LIMB(0x22D)}, /* 107 */
 {CNST_LIMB(0x1CA6EB3EE4626510), CNST_LIMB(0x386)}, /* 108 */
 {CNST_LIMB(0xA5D30982685B4529), CNST_LIMB(0x5B3)}, /* 109 */
 {CNST_LIMB(0xC279F4C14CBDAA39), CNST_LIMB(0x939)}, /* 110 */
 {CNST_LIMB(0x684CFE43B518EF62), CNST_LIMB(0xEED)}, /* 111 */
 {CNST_LIMB(0x2AC6F30501D6999B), CNST_LIMB(0x1827)}, /* 112 */
 {CNST_LIMB(0x9313F148B6EF88FD), CNST_LIMB(0x2714)}, /* 113 */
 {CNST_LIMB(0xBDDAE44DB8C62298), CNST_LIMB(0x3F3B)}, /* 114 */
 {CNST_LIMB(0x50EED5966FB5AB95), CNST_LIMB(0x6650)}, /* 115 */
 {CNST_LIMB(0xEC9B9E4287BCE2D), CNST_LIMB(0xA58C)}, /* 116 */
 {CNST_LIMB(0x5FB88F7A983179C2), CNST_LIMB(0x10BDC)}, /* 117 */
 {CNST_LIMB(0x6E82495EC0AD47EF), CNST_LIMB(0x1B168)}, /* 118 */
 {CNST_LIMB(0xCE3AD8D958DEC1B1), CNST_LIMB(0x2BD44)}, /* 119 */
 {CNST_LIMB(0x3CBD2238198C09A0), CNST_LIMB(0x46EAD)}, /* 120 */
 {CNST_LIMB(0xAF7FB11726ACB51), CNST_LIMB(0x72BF2)}, /* 121 */
 {CNST_LIMB(0x47B51D498BF6D4F1), CNST_LIMB(0xB9A9F)}, /* 122 */
 {CNST_LIMB(0x52AD185AFE61A042), CNST_LIMB(0x12C691)}, /* 123 */
 {CNST_LIMB(0x9A6235A48A587533), CNST_LIMB(0x1E6130)}, /* 124 */
 {CNST_LIMB(0xED0F4DFF88BA1575), CNST_LIMB(0x3127C1)}, /* 125 */
 {CNST_LIMB(0x877183A413128AA8), CNST_LIMB(0x4F88F2)}, /* 126 */
 {CNST_LIMB(0x7480D1A39BCCA01D), CNST_LIMB(0x80B0B4)}, /* 127 */
 {CNST_LIMB(0xFBF25547AEDF2AC5), CNST_LIMB(0xD039A6)}, /* 128 */
 {CNST_LIMB(0x707326EB4AABCAE2), CNST_LIMB(0x150EA5B)}, /* 129 */
 {CNST_LIMB(0x6C657C32F98AF5A7), CNST_LIMB(0x2212402)}, /* 130 */
 {CNST_LIMB(0xDCD8A31E4436C089), CNST_LIMB(0x3720E5D)}, /* 131 */
 {CNST_LIMB(0x493E1F513DC1B630), CNST_LIMB(0x5933260)}, /* 132 */
 {CNST_LIMB(0x2616C26F81F876B9), CNST_LIMB(0x90540BE)}, /* 133 */
 {CNST_LIMB(0x6F54E1C0BFBA2CE9), CNST_LIMB(0xE98731E)}, /* 134 */
 {CNST_LIMB(0x956BA43041B2A3A2), CNST_LIMB(0x179DB3DC)}, /* 135 */
 {CNST_LIMB(0x4C085F1016CD08B), CNST_LIMB(0x263626FB)}, /* 136 */
 {CNST_LIMB(0x9A2C2A21431F742D), CNST_LIMB(0x3DD3DAD7)}, /* 137 */
 {CNST_LIMB(0x9EECB012448C44B8), CNST_LIMB(0x640A01D2)}, /* 138 */
 {CNST_LIMB(0x3918DA3387ABB8E5), CNST_LIMB(0xA1DDDCAA)}, /* 139 */
 {CNST_LIMB(0xD8058A45CC37FD9D), CNST_LIMB(0x105E7DE7C)}, /* 140 */
 {CNST_LIMB(0x111E647953E3B682), CNST_LIMB(0x1A7C5BB27)}, /* 141 */
 {CNST_LIMB(0xE923EEBF201BB41F), CNST_LIMB(0x2ADAD99A3)}, /* 142 */
 {CNST_LIMB(0xFA42533873FF6AA1), CNST_LIMB(0x4557354CA)}, /* 143 */
 {CNST_LIMB(0xE36641F7941B1EC0), CNST_LIMB(0x70320EE6E)}, /* 144 */
 {CNST_LIMB(0xDDA89530081A8961), CNST_LIMB(0xB58944339)}, /* 145 */
 {CNST_LIMB(0xC10ED7279C35A821), CNST_LIMB(0x125BB531A8)}, /* 146 */
 {CNST_LIMB(0x9EB76C57A4503182), CNST_LIMB(0x1DB44974E2)}, /* 147 */
 {CNST_LIMB(0x5FC6437F4085D9A3), CNST_LIMB(0x300FFEA68B)}, /* 148 */
 {CNST_LIMB(0xFE7DAFD6E4D60B25), CNST_LIMB(0x4DC4481B6D)}, /* 149 */
 {CNST_LIMB(0x5E43F356255BE4C8), CNST_LIMB(0x7DD446C1F9)}, /* 150 */
 {CNST_LIMB(0x5CC1A32D0A31EFED), CNST_LIMB(0xCB988EDD67)}, /* 151 */
 {CNST_LIMB(0xBB0596832F8DD4B5), CNST_LIMB(0x1496CD59F60)}, /* 152 */
 {CNST_LIMB(0x17C739B039BFC4A2), CNST_LIMB(0x21505647CC8)}, /* 153 */
 {CNST_LIMB(0xD2CCD033694D9957), CNST_LIMB(0x35E723A1C28)}, /* 154 */
 {CNST_LIMB(0xEA9409E3A30D5DF9), CNST_LIMB(0x573779E98F0)}, /* 155 */
 {CNST_LIMB(0xBD60DA170C5AF750), CNST_LIMB(0x8D1E9D8B519)}, /* 156 */
 {CNST_LIMB(0xA7F4E3FAAF685549), CNST_LIMB(0xE4561774E0A)}, /* 157 */
 {CNST_LIMB(0x6555BE11BBC34C99), CNST_LIMB(0x17174B500324)}, /* 158 */
 {CNST_LIMB(0xD4AA20C6B2BA1E2), CNST_LIMB(0x255CACC7512F)}, /* 159 */
 {CNST_LIMB(0x72A0601E26EEEE7B), CNST_LIMB(0x3C73F8175453)}, /* 160 */
 {CNST_LIMB(0x7FEB022A921A905D), CNST_LIMB(0x61D0A4DEA582)}, /* 161 */
 {CNST_LIMB(0xF28B6248B9097ED8), CNST_LIMB(0x9E449CF5F9D5)}, /* 162 */
 {CNST_LIMB(0x727664734B240F35), CNST_LIMB(0x1001541D49F58)}, /* 163 */
 {CNST_LIMB(0x6501C6BC042D8E0D), CNST_LIMB(0x19E59DECA992E)}, /* 164 */
 {CNST_LIMB(0xD7782B2F4F519D42), CNST_LIMB(0x29E6F209F3886)}, /* 165 */
 {CNST_LIMB(0x3C79F1EB537F2B4F), CNST_LIMB(0x43CC8FF69D1B5)}, /* 166 */
 {CNST_LIMB(0x13F21D1AA2D0C891), CNST_LIMB(0x6DB3820090A3C)}, /* 167 */
 {CNST_LIMB(0x506C0F05F64FF3E0), CNST_LIMB(0xB18011F72DBF1)}, /* 168 */
 {CNST_LIMB(0x645E2C209920BC71), CNST_LIMB(0x11F3393F7BE62D)}, /* 169 */
 {CNST_LIMB(0xB4CA3B268F70B051), CNST_LIMB(0x1D0B3A5EEEC21E)}, /* 170 */
 {CNST_LIMB(0x1928674728916CC2), CNST_LIMB(0x2EFE739E6AA84C)}, /* 171 */
 {CNST_LIMB(0xCDF2A26DB8021D13), CNST_LIMB(0x4C09ADFD596A6A)}, /* 172 */
 {CNST_LIMB(0xE71B09B4E09389D5), CNST_LIMB(0x7B08219BC412B6)}, /* 173 */
 {CNST_LIMB(0xB50DAC229895A6E8), CNST_LIMB(0xC711CF991D7D21)}, /* 174 */
 {CNST_LIMB(0x9C28B5D7792930BD), CNST_LIMB(0x14219F134E18FD8)}, /* 175 */
 {CNST_LIMB(0x513661FA11BED7A5), CNST_LIMB(0x2092BC0CDFF0CFA)}, /* 176 */
 {CNST_LIMB(0xED5F17D18AE80862), CNST_LIMB(0x34B45B202E09CD2)}, /* 177 */
 {CNST_LIMB(0x3E9579CB9CA6E007), CNST_LIMB(0x5547172D0DFA9CD)}, /* 178 */
 {CNST_LIMB(0x2BF4919D278EE869), CNST_LIMB(0x89FB724D3C046A0)}, /* 179 */
 {CNST_LIMB(0x6A8A0B68C435C870), CNST_LIMB(0xDF42897A49FF06D)}, /* 180 */
 {CNST_LIMB(0x967E9D05EBC4B0D9), CNST_LIMB(0x1693DFBC7860370D)}, /* 181 */
 {CNST_LIMB(0x108A86EAFFA7949), CNST_LIMB(0x248808541D00277B)}, /* 182 */
 {CNST_LIMB(0x978745749BBF2A22), CNST_LIMB(0x3B1BE81095605E88)}, /* 183 */
 {CNST_LIMB(0x988FEDE34BB9A36B), CNST_LIMB(0x5FA3F064B2608603)}, /* 184 */
 {CNST_LIMB(0x30173357E778CD8D), CNST_LIMB(0x9ABFD87547C0E48C)}, /* 185 */
 {CNST_LIMB(0xC8A7213B333270F8), CNST_LIMB(0xFA63C8D9FA216A8F)}, /* 186 */
};
/* total 2240 bytes if BITS_PER_LIMB==64 */
#endif


void
mpz_fib_ui (mpz_t r, unsigned long int n)
{
  mp_ptr         rp, rp_orig, tp, xp, yp;
  mp_size_t      ralloc, rsize, xsize;
  unsigned long  i, nfull;
  int            ni;
  TMP_DECL (marker);
  
  TRACE (printf ("\n", n);
         printf ("mpz_fib_ui n=%lu\n", n));

  if (n < numberof (table1))
    {
      mp_limb_t  l = table1[n];
      PTR(r)[0] = l;
      SIZ(r) = (l != 0);
      return;
    }

  if (n < numberof (table1) + numberof (table2))
    {
      mp_limb_t  hi, lo;
      MPZ_REALLOC (r, 2);
      lo = table2[n-numberof(table1)][0];
      hi = table2[n-numberof(table1)][1];
      PTR(r)[0] = lo;
      PTR(r)[1] = hi;
      SIZ(r) = 1 + (hi != 0);
      return;
    }

  TMP_MARK (marker);

  /* Variables are sized with +2 limbs because in bigcase, k limbs are
     doubled into a 2k+1 region, whereas the actual fib size may be only
     2k-1. */
  ralloc = MPZ_FIB_SIZE (n) + 2;
  MPZ_REALLOC (r, ralloc);

  rp = PTR(r);
  tp = TMP_ALLOC_LIMBS (ralloc);
  rp_orig = rp;

  nfull = n;
  ni = 0;
  if (n >= FIB_THRESHOLD)
    {
      /* find the base case to calculate */
      do {
        ni++, n /= 2;
      } while (n >= FIB_THRESHOLD);

      TRACE (printf ("new n=%lu ni=%d\n", n, ni));
      ASSERT (n >= FIB_THRESHOLD/2);

      /* Starting point for base case might come from just table lookups.
         The tests on FIB_THRESHOLD let the compiler eliminate some of this
         if FIB_THRESHOLD is big enough that one or both tables might never
         be in range. */

      if (FIB_THRESHOLD/2 < numberof (table1)
          && n < numberof (table1))
        {
          tp[0] = table1 [n-1];
          rp[0] = table1 [n];
          rsize = 1;
          goto bigcase;
        }
      if (FIB_THRESHOLD/2 <= numberof (table1)
          && n == numberof (table1))
        {
          tp[0] = table1 [n-1];
          tp[1] = 0;
          rp[0] = table2 [0][0];
          rp[1] = table2 [0][1];
          rsize = 2;
          goto bigcase;
        }
      if (FIB_THRESHOLD/2 < numberof (table1) + numberof (table2)
          && n < numberof (table1) + numberof (table2))
        {
          rp[0] = table2 [n - numberof (table1)][0];
          rp[1] = table2 [n - numberof (table1)][1];
          tp[0] = table2 [n-1 - numberof (table1)][0];
          tp[1] = table2 [n-1 - numberof (table1)][1];
          rsize = 2;
          goto bigcase;
        }
    }

  /* Simple loop starting from last two entries of table2[].
  
  rsize it maintained so that F[n] is normalized, meaning rp[rsize-1]!=0.
  rsize is also the size of tp=F[n-1], but it can have a high zero limb
  (sometimes).  rp and tp alternate between holding F[n] and F[n-1]. */

  ASSERT (n >= numberof (table1) + numberof (table2));
  i = n - (numberof (table1) + numberof (table2) - 1);
  
  rp[0] = table2 [numberof (table2) - 2][0];
  rp[1] = table2 [numberof (table2) - 2][1];
  tp[0] = table2 [numberof (table2) - 1][0];
  tp[1] = table2 [numberof (table2) - 1][1];
  rsize = 2;
  
  do
    {
      mp_limb_t  c;

      ASSERT (rsize < ralloc);
      c = mpn_add_n (rp, rp, tp, rsize);
      rp[rsize] = c;
      tp[rsize] = 0;
      rsize += c;

      if (--i == 0)
        goto no_swap;

      ASSERT (rsize < ralloc);
      c = mpn_add_n (tp, tp, rp, rsize);
      tp[rsize] = c;
      rp[rsize] = 0;
      rsize += c;
    }
  while (--i);

  MP_PTR_SWAP (rp, tp);

 no_swap:
  if (ni == 0)
    goto done;


 bigcase:

  /* Powering code for big N. 

  rp=F[n] and tp=F[n-1], and rsize is maintained so F[n] is normalized,
  meaning rp[rsize-1]!=0.  rsize is also the size of tp=F[n-1], but it
  can have a high zero limb.  */

  xp = TMP_ALLOC_LIMBS (ralloc);
  yp = TMP_ALLOC_LIMBS (ralloc);

  for (;;)
    {
      mp_limb_t  c;

      TRACE (printf ("n=%lu ni=%d rsize=%ld ralloc=%ld\n", 
                     nfull >> ni, ni, rsize, ralloc);
             mpn_trace ("rp", rp, rsize);
             mpn_trace ("tp", tp, rsize));

      ni--;
      
      /* if we want to stop on F[2n+1], break out for special last step code */
      if (ni == 0 && (nfull&1))
        break;

      /* xp,xsize = F[n]*(2F[n-1]+F[n]) = F[2n] */
      c = mpn_lshift (yp, tp, rsize, 1);    /* 2F[n-1] */
      c += mpn_add_n (yp, yp, rp, rsize);   /* (2F[n-1]+F[n]) */
      ASSERT (ralloc >= 2*rsize);
      mpn_mul_n (xp, yp, rp, rsize);
      TRACE (printf ("  c=%lu\n", c));
      if (c == 0)
        {
          /* this case is usual, the sum usually doesn't overflow */
          xsize = 2*rsize - (xp[2*rsize-1] == 0);
        }
      else 
        {
          ASSERT (ralloc >= 2*rsize+1);
          if (c == 1)
            {
              c = mpn_add_n (xp+rsize, xp+rsize, rp, rsize);
              xp[2*rsize] = c;
              xsize = 2*rsize + c;
            }
          else /* c == 2 */
            {
              c = mpn_lshift (yp, rp, rsize, 1);
              c += mpn_add_n (xp+rsize, xp+rsize, yp, rsize);
              xp[2*rsize] = c;
              xsize = 2*rsize + (c != 0);
            }
        }
      TRACE (printf ("xsize=%lu ", xsize);
             mpn_trace ("xp", xp, xsize));

      if (ni == 0)
        {
          /* stop now, wanting to finish on F[2n] */
          rp = xp;
          rsize = xsize;
          goto done;
        }

      /* yp = F[n-1]*(2F[n]-F[n-1]) = F[2n-2], in 2*rsize+1 limbs */
      ASSERT (ralloc >= 2*rsize+1);
      c = mpn_lshift (rp, rp, rsize, 1);    /* 2F[n] */
      c -= mpn_sub_n (rp, rp, tp, rsize);   /* 2F[n]-F[n-1] */
      ASSERT (c==0 || c==1);
      mpn_mul_n (yp, rp, tp, rsize);
      if (c)
        c = mpn_add_n (yp+rsize, yp+rsize, tp, rsize);
      yp[2*rsize] = c;

      TRACE (printf ("2*rsize+1=%lu yp[2*rsize]=%lu yp[2*rsize-1]=%lu\n",
                     2*rsize+1, yp[2*rsize], yp[2*rsize-1]);
             mpn_trace ("yp", yp, 2*rsize+1));


      /* When xsize < 2*rsize+1, yp[] should have zeros in the locations
         above xsize because F[2n-2] < F[2n]. */
      ASSERT (xsize > 2*rsize   || yp[2*rsize]   == 0);
      ASSERT (xsize > 2*rsize-1 || yp[2*rsize-1] == 0);
      rsize = xsize;

      /* At this point xp=F[2n], yp=F[2n-2], both rsize limbs */

      if (nfull & (1L << ni))
        {
          TRACE (printf ("  1 bit\n"));

          /* F[2n+1] = 2*F[2n]-F[2n-2] */
          c = mpn_lshift (rp, xp, rsize, 1);
          c -= mpn_sub_n (rp, rp, yp, rsize);
          ASSERT (c==0 || c==1);
          ASSERT (ralloc >= rsize+1);
          rp[rsize] = c;
          xp[rsize] = 0;
          rsize += (c != 0);
          MP_PTR_SWAP (tp, xp);   /* F[2n] */
        }
      else
        {
          TRACE (printf ("  0 bit\n"));

          ASSERT_NOCARRY (mpn_sub_n (yp, xp, yp, rsize));  /* F[2n-1] */
          MP_PTR_SWAP (rp, xp);    /* F[2n] */
          MP_PTR_SWAP (tp, yp);
        }
    }

  /* Here rp=F[n], tp=F[n-1] in rsize limbs.
     Want rp = F[n]^2 + (F[n]+F[n-1])^2 = F[2n+1] */
  {
    mp_size_t  ysize, tsize;
    mp_limb_t  c;

    ASSERT (ralloc >= 2*rsize);
    mpn_sqr_n (yp, rp, rsize);           /* F[n]^2 */
    ysize = 2*rsize - (yp[2*rsize-1] == 0);

    c = mpn_add_n (tp, tp, rp, rsize);   /* F[n]+F[n-1] */
    tp[rsize] = c;
    tsize = rsize + (c != 0);

    ASSERT (ralloc >= 2*tsize);
    mpn_sqr_n (rp, tp, tsize);           /* (F[n]+F[n-1])^2 */
    rsize = 2*tsize - (rp[2*tsize-1] == 0);

    ASSERT (ralloc >= rsize+1);
    c = mpn_add (rp, rp, rsize, yp, ysize);
    rp[rsize] = c;
    rsize += (c != 0);          
  }

 done:
  TRACE (printf ("done\n");
         mpn_trace ("rp", rp, rsize));
     
  if (rp != rp_orig)
    MPN_COPY (rp_orig, rp, rsize);
  SIZ(r) = rsize;

  ASSERT (rp_orig[rsize-1] != 0);
  ASSERT (rsize <= ralloc);

  TMP_FREE (marker);
}




/* ------------------------------------------------------------------------- */

#if GENERATE_FIB_TABLE
/* Generate the tables of fibonacci data.  This doesn't depend on the limb
   size of the host, and doesn't need mpz_fib_ui working. */

void
generate (int bpml)
{
  int    n;
  mpz_t  fn, fn1, hi, lo;
  unsigned long  bytes = 0;

  mpz_init (hi);
  mpz_init (lo);

  printf ("#if BITS_PER_MP_LIMB == %d\n", bpml);
  printf ("static const mp_limb_t table1[] = {\n");

  /* at n==0 */
  mpz_init_set_ui (fn1, 1);  /* F[n-1] */
  mpz_init_set_ui (fn,  0);  /* F[n]   */

  for (n = 0; mpz_sizeinbase(fn,2) <= 2*bpml; n++)
    {
      if (mpz_sizeinbase(fn,2) > bpml && mpz_sizeinbase(fn1,2) <= bpml)
        {
          printf ("};\n");
          printf ("static const mp_limb_t table2[][2] = {\n");
        }

      if (mpz_sizeinbase(fn,2) <= bpml)
        {
          printf (" CNST_LIMB (0x");
          mpz_out_str (stdout, -16, fn);
          printf("),  /* %d */\n", n);
          bytes += MAX (bpml, 8) / 8;
        }
      else
        {
          mpz_fdiv_q_2exp (hi, fn, bpml);
          mpz_fdiv_r_2exp (lo, fn, bpml);

          printf (" {CNST_LIMB(0x");
          mpz_out_str (stdout, -16, lo);
          printf("), CNST_LIMB(0x");
          mpz_out_str (stdout, -16, hi);
          printf(")}, /* %d */\n", n);
          bytes += 2 * MAX (bpml, 8) /8;
        }

      mpz_add (fn1, fn1, fn);  /* F[n+1] = F[n] + F[n-1] */
      mpz_swap (fn1, fn);
    }
  printf ("};\n");
  printf ("/* total %lu bytes if BITS_PER_LIMB==%d */\n", bytes, bpml);
  printf ("#endif\n");
  printf ("\n");
}

int
main (void)
{
  generate (2);
  generate (4);
  generate (8);
  generate (16);
  generate (32);
  generate (64);

  return 0;
}

#endif
