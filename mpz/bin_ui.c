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


/* This is a poor implementation.  Look at bin_uiui.c for improvement ideas.
   In fact consider calling mpz_bin_uiui() when the arguments fit, leaving
   the code here only for big n.

   For the identity bin(n,k) = (-1)^k * bin(-n+k-1,k) see Knuth vol 1
   section 1.2.6 part G. */

void
#if __STDC__
mpz_bin_ui (mpz_ptr r, mpz_srcptr n, unsigned long int k)
#else
mpz_bin_ui (r, n, k)
     mpz_ptr r;
     mpz_srcptr n;
     unsigned long int k;
#endif
{
  mpz_t ni;
  unsigned long int i;
  unsigned long int kacc;
  mpz_t nacc;
  
  if (mpz_sgn (n) < 0)
    {
      /* bin(n,k) = (-1)^k * bin(-n+k-1,k), and set ni = -n+k-1 - k = -n-1 */
      mpz_init (ni);
      mpz_neg (ni, n);
      mpz_sub_ui (ni, ni, 1L);
      mpz_set_si (r, (long) (1 - ((k & 1) << 1)));  /* (-1)^k */
    }
  else
    {
      /* bin(n,k) == 0 if k>n
         (no test for this under the n<0 case, since -n+k-1 >= k there) */
      if (mpz_cmp_ui (n, k) < 0)
        {
          mpz_set_ui (r, 0L);
          return;
        }

      /* set ni = n-k */
      mpz_init (ni);
      mpz_sub_ui (ni, n, k);
      mpz_set_ui (r, 1L);
    }

  /* Now wanting bin(ni+k,k), with ni positive, and r is the sign (1 or -1). */

  /* Rewrite bin(n,k) as bin(n,n-k) if that is smaller.  In this case it's
     whether ni+k-k < k meaning ni<k, and if so change to denominator ni+k-k
     = ni, and new ni of ni+k-ni = k.  */
  if (mpz_cmp_ui (ni, k) < 0)
    {
      unsigned long  tmp;
      tmp = k;
      k = mpz_get_ui (ni);
      mpz_set_ui (ni, tmp);
    }

  kacc = 1;
  mpz_init_set_ui (nacc, 1);

  for (i = 1; i <= k; i++)
    {
      mp_limb_t k1, k0;

#if 0
      mp_limb_t nacclow;
      int c;

      nacclow = PTR(nacc)[0];
      for (c = 0; (((kacc | nacclow) & 1) == 0); c++)
	{
	  kacc >>= 1;
	  nacclow >>= 1;
	}
      mpz_div_2exp (nacc, nacc, c);
#endif

      mpz_add_ui (ni, ni, 1);
      mpz_mul (nacc, nacc, ni);
      umul_ppmm (k1, k0, kacc, i);
      if (k1 != 0)
	{
	  /* Accumulator overflow.  Perform bignum step.  */
	  mpz_mul (r, r, nacc);
	  mpz_set_ui (nacc, 1);
	  mpz_tdiv_q_ui (r, r, kacc);
	  kacc = i;
	}
      else
	{
	  /* Save new products in accumulators to keep accumulating.  */
	  kacc = k0;
	}
    }

  mpz_mul (r, r, nacc);
  mpz_set_ui (nacc, 1);
  mpz_tdiv_q_ui (r, r, kacc);

  mpz_clear (nacc);
  mpz_clear (ni);
}
