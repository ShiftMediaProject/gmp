/* urandomb(rop, state) -- Generate a uniform pseudorandom real number
between zero and one, using `state' as the random state previously
initialized by a call to gmp_rand_init().

Copyright (C) 1999 Free Software Foundation, Inc.

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

/* mpf_urandomb() -- Generate a universally distributed real random
number 0 <= X < 1.  See file mpz/urandom.c for algorithms used. */

void
#if __STDC__
mpf_urandomb (mpf_t rop, gmp_rand_state *s)
#else
mpf_urandomb (rop, s)
     mpf_t rop;
     gmp_rand_state *s;
#endif
{
  mpz_t z_rand;
  mpf_t f_tmp;

  switch (s->alg)
    {
    case GMP_RAND_ALG_LC:
      mpz_init (z_rand);
      mpf_init (f_tmp);

      /* FIXME: mpz_urandomb() will most often do a division and a
	 multiplication in order to produce 0 <= z < 2^size-1.  We
	 only need the division.  Break up mpz_urandomb(). */

      mpz_urandomb (z_rand, s);
      mpf_set_z (rop, z_rand);
      mpf_set_z (f_tmp, s->scheme->m);
      mpf_div (rop, rop, f_tmp);

      mpz_clear (z_rand);
      mpf_clear (f_tmp);

      break;
    }
}
