/* mpn_mul_1 for Cray vector computers utilizing IEEE fp.

Copyright 2000 Free Software Foundation, Inc.

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
MA 02111-1307, USA.  */


#include <intrinsics.h>
#include "gmp.h"
#include "gmp-impl.h"

mp_limb_t
mpn_mul_1 (mp_ptr rp, mp_srcptr up, mp_size_t n, mp_limb_t vl)
{
  mp_limb_t p0[n], p1[n];
  mp_limb_t cyl;
  mp_size_t i;

#pragma _CRI ivdep
  for (i = 0; i < n; i++)
    {
      mp_limb_t ul = up[i];
      p0[i] = ul * vl;
      p1[i] = _int_mult_upper (ul, vl);
    }
  rp[0] = p0[0];
  cyl = mpn_add_n (rp + 1, p0 + 1, p1, n - 1);
  cyl += p1[n - 1];

  return cyl;
}
