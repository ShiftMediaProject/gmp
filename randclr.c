/* gmp_randclear (state) -- Clear and deallocate random state STATE.

Copyright 1999, 2000, 2001  Free Software Foundation, Inc.

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
gmp_randclear (gmp_randstate_t rstate)
{
  mpz_clear (rstate->_mp_seed);

  switch (rstate->_mp_alg)
    {
    case GMP_RAND_ALG_LC:
      mpz_clear (rstate->_mp_algdata._mp_lc->_mp_a);
      if (rstate->_mp_algdata._mp_lc->_mp_m2exp == 0)
	mpz_clear (rstate->_mp_algdata._mp_lc->_mp_m);
      (*__gmp_free_func) (rstate->_mp_algdata._mp_lc, sizeof (*rstate->_mp_algdata._mp_lc));
      break;

#if 0
    case GMP_RAND_ALG_BBS:
      mpz_clear (rstate->algdata.bbs->bi);
      (*__gmp_free_func) (rstate->algdata.bbs, sizeof (*rstate->algdata.bbs));
      break;
#endif /* 0 */

    default:
      ASSERT (0);
      break;
    }
}
