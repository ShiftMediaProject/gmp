/* urandomn (rop, state, n) -- Generate a uniform pseudorandom integer
   in the range 0,N), using STATE as the random state previously
   initialized by a call to gmp_rand_init().

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


void
#if __STDC__
mpz_urandomn (mpz_t rop, gmp_rand_state s, mpz_t n)
#else
mpz_urandomn (rop, s, n)
     mpz_t rop;
     gmp_rand_state s;
     mpz_t n;
#endif
{
#if 0
  mp_ptr rp;
  mp_size_t size, bsize;
  unsigned cnt;			/* FIXME: type? */

  rp = PTR (rop);
  size = SIZ (n);
  count_leading_zeros (cnt, rp[size - 1]);
  bsize = BITS_PER_MP_LIMP * size - cnt;

  mpn_rawrandom (tp, bsize + 20, s);
  MPN_NORMALIZE (tp, size);	
  SIZ (rop) = size;

  mpz_tdiv_r (rop, rop, n);	/* reduce to spec'd interval */
#endif
}
