/* gmp_randinit_lc (state, a, c, m) -- Initialize a random state for a
   linear congruential generator with multiplier A, adder C, and
   modulus M.

Copyright 1999, 2000  Free Software Foundation, Inc.

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

void
gmp_randinit_lc (gmp_randstate_t rstate,
		 mpz_srcptr a,
		 unsigned long int c,
		 mpz_srcptr m)
{
  /* FIXME: Not finished.  We don't handle this in _gmp_rand() yet. */
  abort ();

  mpz_init_set_ui (rstate->_mp_seed, 1);
  _mpz_realloc (rstate->_mp_seed, ABSIZ (m));

  /* Allocate algorithm specific data. */
  rstate->_mp_algdata._mp_lc = (__gmp_randata_lc *)
    (*__gmp_allocate_func) (sizeof (__gmp_randata_lc));

  mpz_init_set (rstate->_mp_algdata._mp_lc->_mp_a, a);
  rstate->_mp_algdata._mp_lc->_mp_c = c;
  mpz_init_set (rstate->_mp_algdata._mp_lc->_mp_m, m);

  rstate->_mp_alg = GMP_RAND_ALG_LC;
}
