/* urandomb (rop, state, size) -- Generate a uniform pseudorandom real
   number between 0 (inclusive) and 1 (exclusive) with SIZE number of
   bits, using STATE as the random state previously initialized by a
   call to gmp_rand_init().  If SIZE is 0, fill ROP.

Copyright (C) 1999, 2000  Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include "gmp.h"
#include "gmp-impl.h"

/* FIXME: Rename file to urandomb.c. */

/* mpf_urandomb() -- Generate a universally distributed real random
   number 0 <= X < 1.  See file mpn/generic/rawrandom.c for algorithms
   used. */

void
#if __STDC__
mpf_urandomb (mpf_t rop, gmp_rand_state s, unsigned long int nbits)
#else
mpf_urandomb (rop, s, nbits)
     mpf_t rop;
     gmp_rand_state s;
     unsigned long int nbits;
#endif
{
  mp_ptr rp;
  mp_size_t prec;

  /* FIXME: Use NBITS.  NBITS == 0 means fill ROP.  */

  rp = PTR (rop);
  prec = PREC (rop);

  gmp_rand_getraw (rp, s, prec * BITS_PER_MP_LIMB);
  MPN_NORMALIZE (rp, prec);
  SIZ (rop) = prec;
  EXP (rop) = 0;

  return;
}
