/* gmp_rand_init_ui (state, alg, size, seed) -- Initialize a random
state.

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

/* gmp_rand_init_ui() -- Initialize a gmp_rand_state.  Return 0 on
   success and 1 on failure. */

int
#if __STDC__
gmp_rand_init_ui (gmp_rand_state s,
		  gmp_rand_algorithm alg,
		  unsigned long int size,
		  unsigned long int seed)
#else
gmp_rand_init_ui (s, alg, size, seed)
     gmp_rand_state s;
     gmp_rand_algorithm alg;
     unsigned long int size;
     unsigned long int seed;
#endif
{
  mpz_t z_seed;
  int ret;

  mpz_init_set_ui (z_seed, seed);
  ret = gmp_rand_init (s, alg, size, z_seed);
  mpz_clear (z_seed);
  
  return ret;
}
