/* mpn_popcount, mpn_hamdist -- population count and hamming distance.

Copyright 1994, 1996, 2000 Free Software Foundation, Inc.

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

#include "gmp.h"
#include "gmp-impl.h"


#ifdef OPERATION_popcount
#define FUNCTION   mpn_popcount
#define ARGUMENTS  register mp_srcptr p, register mp_size_t size
#define OPERAND    (p[i])
#endif

#ifdef OPERATION_hamdist
#define FUNCTION   mpn_hamdist
#define ARGUMENTS  mp_srcptr up, mp_srcptr vp, mp_size_t size
#define OPERAND    (up[i] ^ vp[i])
#endif

#ifndef FUNCTION
Error, need OPERATION_popcount or OPERATION_hamdist
#endif


#if defined __GNUC__
/* No processor claiming to be SPARC v9 compliant seems to
   implement the POPC instruction.  Disable pattern for now.  */
#if 0 && defined __sparc_v9__ && BITS_PER_MP_LIMB == 64
#define popc_limb(a) \
  ({									\
    DItype __res;							\
    asm ("popc %1,%0" : "=r" (__res) : "rI" (a));			\
    __res;								\
  })
#endif
#endif


#ifndef popc_limb

/* Cool population count of a mp_limb_t.
   You have to figure out how this works, I won't tell you!  */

static inline unsigned int
#if __STDC__
popc_limb (mp_limb_t x)
#else
popc_limb (x)
     mp_limb_t x;
#endif
{
#if BITS_PER_MP_LIMB == 64
  mp_limb_t cnst;
  cnst = CNST_LIMB(0xaaaaaaaaaaaaaaaa);
  x -= (x & cnst) >> 1;
  cnst = CNST_LIMB(0x3333333333333333);
  x = ((x & ~cnst) >> 2) + (x & cnst);
  cnst = CNST_LIMB(0x0f0f0f0f0f0f0f0f);
  x = ((x >> 4) + x) & cnst;
  x = ((x >> 8) + x);
  x = ((x >> 16) + x);
  x = ((x >> 32) + x) & 0xff;
#endif
#if BITS_PER_MP_LIMB == 32
  x -= (x & 0xaaaaaaaaL) >> 1;
  x = ((x >> 2) & 0x33333333L) + (x & 0x33333333L);
  x = ((x >> 4) + x) & 0x0f0f0f0fL;
  x = ((x >> 8) + x);
  x = ((x >> 16) + x) & 0xff;
#endif
#if BITS_PER_MP_LIMB == 16
  x -= (x & 0xaaaa) >> 1;
  x = ((x >> 2) & 0x3333) + (x & 0x3333);
  x = ((x >> 4) + x) & 0x0f0f;
  x = ((x >> 8) + x) & 0xff;
#endif
#if BITS_PER_MP_LIMB == 8
  x -= (x & 0xaa) >> 1;
  x = ((x >> 2) & 0x33) + (x & 0x33);
  x = ((x >> 4) + x) & 0xf;
#endif
#if BITS_PER_MP_LIMB == 4
  x = (x & 1) + ((x >> 1) & 1) + ((x >> 2) & 1) + ((x >> 3) & 1);
#endif
  return x;
}
#endif


unsigned long int
FUNCTION (ARGUMENTS)
{
  unsigned long int  result = 0;
  mp_size_t  i;

  /* mpz/popcount.c and mpz/hamdist.c pass size==0 though this isn't a
     documented feature. */
  ASSERT (size >= 0);

  for (i = 0; i < size; i++)
    result += popc_limb (OPERAND);

  return result;
}
