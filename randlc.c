/* gmp_rand_init_lc (state, size, seed, a, c, m) -- Initialize a
   random state for a linear congruental generator with multiplier A,
   adder C, and modulus M.

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

#include <stdlib.h>		/* FIXME: For malloc(). */
#include "gmp.h"
#include "gmp-impl.h"

void
#if __STDC__
gmp_rand_init_lc (gmp_rand_state s,
		  unsigned long int size,
		  mpz_t seed,
		  mpz_t a,
		  unsigned long int c,
		  mpz_t m)
#else
gmp_rand_init_lc (s, size, seed, a, c, m)
     gmp_rand_state s;
     unsigned long int size;
     mpz_t seed;
     mpz_t a;
     unsigned long int c;
     mpz_t m;
#endif
{
  /* Allocate algorithm specific data. */
  /* FIXME: Use user supplied allocation func instead of malloc? */
  s->data.lc = (__gmp_rand_data_lc *) malloc (sizeof (__gmp_rand_data_lc));

  mpz_init_set (s->data.lc->a, a);
  mpz_init_set (s->data.lc->m, m);
  s->data.lc->c = c;

  __gmp_rand_init_common (s, GMP_RAND_ALG_LC, size, seed);
}
