/* gmp_randinit_lc_2exp (state, a, c, m2exp) -- Initialize random
   state STATE for a linear congruential generator with multiplier A,
   adder C, and modulus 2 ^ M2EXP.

Copyright 2000, 2002 Free Software Foundation, Inc.

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
gmp_randinit_lc_2exp (gmp_randstate_t rstate,
		      mpz_srcptr a,
		      unsigned long int c,
		      unsigned long int m2exp)
{
  mp_limb_t  climb;

  ASSERT_ALWAYS (m2exp >= 2);

  mpz_init_set_ui (rstate->_mp_seed, 1);
  _mpz_realloc (rstate->_mp_seed, (m2exp + GMP_NUMB_BITS - 1) / GMP_NUMB_BITS);

  /* Allocate algorithm specific data. */
  rstate->_mp_algdata._mp_lc = (__gmp_randata_lc *)
    (*__gmp_allocate_func) (sizeof (__gmp_randata_lc));

  mpz_init_set (rstate->_mp_algdata._mp_lc->_mp_a, a);

  /* avoid negative a */
  mpz_fdiv_r_2exp (rstate->_mp_algdata._mp_lc->_mp_a,
                   rstate->_mp_algdata._mp_lc->_mp_a, m2exp);

  /* internally c < 2^m2exp
     FIXME: May need two limbs when using nails. */
  climb = c;
  if (m2exp < GMP_LIMB_BITS)
    climb &= (CNST_LIMB(1) << m2exp) - 1;

  rstate->_mp_algdata._mp_lc->_mp_c = climb;

  /* Cover weird case where m2exp is 0, which means that m is used
     instead of m2exp.
     FIXME: This is probably bogus. */
  if (m2exp == 0)
    mpz_init_set_ui (rstate->_mp_algdata._mp_lc->_mp_m, 0);
  rstate->_mp_algdata._mp_lc->_mp_m2exp = m2exp;

  rstate->_mp_alg = GMP_RAND_ALG_LC;
}
