/* __gmp_rand_init_common -- Helper function for gmp_rand_init() and
   gmp_rand_init_lc().

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
#include "gmp-impl.h"

void
#if __STDC__
__gmp_rand_init_common (gmp_rand_state s,
			gmp_rand_algorithm alg,
			unsigned long int size,
			mpz_t seed)
#else
__gmp_rand_init_common (s, alg, size, seed)
     gmp_rand_state s;
     gmp_rand_algorithm alg;
     unsigned long int size;
     mpz_t seed;
#endif     
{
  s->alg = alg;
  s->size = size;
  mpz_init_set (s->seed, seed);
  mpz_init_set_ui (s->maxval, 1);
  mpz_mul_2exp (s->maxval, s->maxval, s->size);
}
