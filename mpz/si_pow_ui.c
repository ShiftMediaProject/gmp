/* mpz_si_pow_ui -- long raised to ulong.

Copyright 2001 Free Software Foundation, Inc.

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
mpz_si_pow_ui (mpz_ptr r, long b, unsigned long e)
{
  mp_size_t  bsize = (b == 0 ? 0 : b >= 0 ? 1 : -1);
#ifdef _LONG_LONG_LIMB
  mp_limb_t  blimb = (b >= 0 ? b : - (mp_limb_t) b);
  mpz_n_pow_ui (r, &blimb, bsize, e);
#else
  b = ABS (b);
  mpz_n_pow_ui (r, &b, bsize, e);
#endif
}
