/* mpz_lcm_ui -- least common multiple of mpz and ulong.

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
#include "longlong.h"


void
mpz_lcm_ui (mpz_ptr r, mpz_srcptr u, unsigned long v)
{
  mp_size_t      usize;
  mp_srcptr      up;
  mp_ptr         rp;
  unsigned long  g;
  mp_limb_t      c;

  /* result zero if either operand zero */
  usize = SIZ(u);
  if (usize == 0 || v == 0)
    {
      SIZ(r) = 0;
      return;
    }
  usize = ABS(usize);

  MPZ_REALLOC (r, usize+1);

  up = PTR(u);
  g = (unsigned long) mpn_gcd_1 (up, usize, (mp_limb_t) v);
  v /= g;

  rp = PTR(r);
  c = mpn_mul_1 (rp, up, usize, (mp_limb_t) v);
  rp[usize] = c;
  usize += (c != 0);
  SIZ(r) = usize;
}
