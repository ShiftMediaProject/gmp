/* mpz_bin_uiui - compute n over k.

Copyright (C) 1998, 1999, 2000 Free Software Foundation, Inc.

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
#include "longlong.h"

void
#if __STDC__
mpz_bin_uiui (mpz_ptr r, unsigned long int n, unsigned long int k)
#else
mpz_bin_uiui (r, n, k)
     mpz_ptr r;
     unsigned long int n;
     unsigned long int k;
#endif
{
  if (n < k)
    /* This special case is a good idea since the unsigned variable n would
       else become negative.  It is not an exception that is mathematically
       necessary.  */
    mpz_set_ui (r, 0);
  else
    {
      unsigned long int i, j;
      mp_limb_t nacc, kacc;
      unsigned long int cnt;

      /* Rewrite bin(n,k) as bin(n,n-k) if that is simpler to compute.  */
      if (k > n / 2)
	k = n - k;

      j = n - k + 1;
      mpz_set_ui (r, j);

      /* Initialize accumulators.  */
      nacc = 1;
      kacc = 1;

      cnt = 0;
      for (i = 2; i <= k; i++)
	{
	  mp_limb_t n1, n0, k1, k0;

	  j++;
#if 0
	  /* Remove common multiples of 2.  This will allow us to accumulate
	     more in nacc and kacc before we need a bignum step.  It would make
	     sense to cancel factors of 3, 5, etc too, but this would be best
	     handled by sieving out factors.  Alternatively, we could perform a
	     gcd of the accumulators just as they have overflown, and keep
	     accumulating until the gcd doesn't remove a significant factor.  */
	  while (((nacc | kacc) & 1) == 0)
	    {
	      nacc >>= 1;
	      kacc >>= 1;
	    }
#else
	  cnt = ((nacc | kacc) & 1) ^ 1;
	  nacc >>= cnt;
	  kacc >>= cnt;
#endif
	  /* Accumulate next multiples.  */
	  umul_ppmm (n1, n0, nacc, j);
	  umul_ppmm (k1, k0, kacc, i);
	  if (n1 != 0)
	    {
	      /* Accumulator overflow.  Perform bignum step.  */
	      mpz_mul_ui (r, r, nacc);
	      nacc = j;
	      mpz_tdiv_q_ui (r, r, kacc);
	      kacc = i;
	    }
	  else
	    {
	      if (k1 != 0) abort ();
	      /* Save new products in accumulators to keep accumulating.  */
	      nacc = n0;
	      kacc = k0;
	    }
	}

      /* Take care of whatever is left in accumulators.  */
      mpz_mul_ui (r, r, nacc);
      mpz_tdiv_q_ui (r, r, kacc);
    }
}
