/* gmp_urandomb_ui -- random bits returned in a ulong.

Copyright 2003 Free Software Foundation, Inc.

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


/* Currently bits>=BITS_PER_ULONG is quietly truncated to BITS_PER_ULONG,
   maybe this should raise an exception or something.  */

unsigned long
gmp_urandomb_ui (gmp_randstate_ptr rstate, unsigned long bits)
{
  mp_limb_t  a[LIMBS_PER_ULONG];

  /* start with zeros, since if bits==0 then _gmp_rand will store nothing at
     all, or if bits <= GMP_NUMB_BITS then it will store only a[0] */
  a[0] = 0;
#if GMP_NAIL_BITS != 0
  a[1] = 0;
#endif

  _gmp_rand (a, rstate, MIN (bits, BITS_PER_ULONG));

#if GMP_NAIL_BITS == 0
  return a[0];
#else
  return a[0] | (a[1] << GMP_NUMB_BITS);
#endif
}
