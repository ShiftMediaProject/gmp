/* gmp_rand_clear (state) -- Clear and deallocate random state STATE.

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
gmp_rand_clear (gmp_rand_state s)
#else
gmp_rand_clear (s)
     gmp_rand_state s;
#endif
{
  mpz_clear (s->seed);

  switch (s->alg)
    {
    case GMP_RAND_ALG_LC:
      mpz_clear (s->data.lc->a);
      if (s->data.lc->m2exp == 0)
	mpz_clear (s->data.lc->m);
      (*_mp_free_func) (s->data.lc, sizeof (*s->data.lc));
      break;

#if 0
    case GMP_RAND_ALG_BBS:
      mpz_clear (s->data.bbs->bi);
      (*_mp_free_func) (s->data.bbs, sizeof (*s->data.bbs));
      break;
#endif /* 0 */

    default:
      gmp_errno |= GMP_ERROR_UNSUPPORTED_ARGUMENT;
    }
}
