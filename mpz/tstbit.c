/* mpz_tstbit -- test a specified bit.  Simulate 2's complement representation.

Copyright 1997, 2000 Free Software Foundation, Inc.

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

int
#if __STDC__
mpz_tstbit (mpz_srcptr d, unsigned long int bit_index)
#else
mpz_tstbit (d, bit_index)
     mpz_srcptr d;
     unsigned long int bit_index;
#endif
{
  mp_size_t dn;
  mp_ptr dp;
  mp_size_t limb_index;
  mp_limb_t limb;

  dn = SIZ(d);
  dp = PTR(d);

  limb_index = bit_index / BITS_PER_MP_LIMB;
  if (dn >= 0)
    {
      limb = 0;  /* Default if testing a bit outside of a positive number.  */
      if (limb_index < dn)
        limb = dp[limb_index];
    }
  else
    {
      mp_size_t zero_bound;

      dn = -dn;

      /* Locate the least significant non-zero limb.  */
      for (zero_bound = 0; dp[zero_bound] == 0; zero_bound++)
        ;

      if (limb_index > zero_bound)
        {
          limb = ~(mp_limb_t) 0; /* Default if testing a bit outside of a negative number.  */
          if (limb_index < dn)
            limb = ~dp[limb_index];
        }
      else
        {
          limb = 0;
          if (limb_index == zero_bound)
            limb = -dp[limb_index];
        }
    }
  return (limb >> (bit_index % BITS_PER_MP_LIMB)) & 1;
}
