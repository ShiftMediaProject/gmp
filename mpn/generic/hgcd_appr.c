/* hgcd_appr.c.

   THE FUNCTIONS IN THIS FILE ARE INTERNAL WITH MUTABLE INTERFACES.  IT IS ONLY
   SAFE TO REACH THEM THROUGH DOCUMENTED INTERFACES.  IN FACT, IT IS ALMOST
   GUARANTEED THAT THEY'LL CHANGE OR DISAPPEAR IN A FUTURE GNU MP RELEASE.

Copyright 2011 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.  */

#include <stdio.h>

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"

#define TRACE 0

/* Scratch need: Copies of inputs, and n limbs for hgcd_appr_step,
   total 3*n limbs. */
mp_size_t
mpn_hgcd_appr_itch (mp_size_t n)
{
  return 3*n;
}

int
mpn_hgcd_appr (mp_srcptr ap, mp_srcptr bp, mp_size_t n,
	       struct hgcd_matrix *M, mp_ptr tp)
{
  unsigned extra_bits;
  mp_size_t s;
  mp_limb_t mask;
  int count;
  mp_ptr up, vp;
  int success = 0;

  ASSERT (n > 0);
  mask = ap[n-1] | bp[n-1];

  ASSERT (mask > 0);

  if (n <= 2)
    /* Implies s = n. A fairly uninteresting case but exercised by the
       random inputs of the testsuite. */
    return 0;
    
  /* We aim for reduction of to GMP_NUMB_BITS * s bits. But each time
     we discard some of the least significant limbs, we must keep one
     additional bit to account for the truncation error. We maintain
     the GMP_NUMB_BITS * s - extra_bits as the current target size. */
     
  s = n/2 + 1;
  extra_bits = 0;

  /* FIXME: Could avoid copying now, and do it when applying the first
     hgcd2 matrix. */
  up = tp; vp = tp + n; tp = tp + 2*n;
  MPN_COPY (up, ap, n);
  MPN_COPY (vp, bp, n);

#if TRACE
  fprintf (stderr, "hgcd_appr: In: n = %u, s = %u\n",
	   (unsigned) n, (unsigned) s);
#endif
  while (n > 2)
    {
      mp_size_t nn;

#if TRACE
      fprintf (stderr, "loop: n = %u\n", (unsigned) n);
#endif
      ASSERT (n > s);
      ASSERT (n <= 2*s);

      nn = mpn_hgcd_step (n, up, vp, s, M, tp);
      if (!nn)
	break;

      n = nn;
      success = 1;

      /* We can truncate and discard the lower p bits whenever nbits <=
	 2*sbits - p. To account for the truncation error, we must
	 adjust

	   sbits <-- sbits + 1 - p,

	 rather than just sbits <-- sbits - p. This adjustment makes
	 the produced matrix sligthly smaller than it could be. */

      if (GMP_NUMB_BITS * (n + 1) + 2 * extra_bits <= 2*GMP_NUMB_BITS * s)
	{
	  mp_size_t p = (GMP_NUMB_BITS * (2*s - n) - 2*extra_bits) / GMP_NUMB_BITS;

#if TRACE
	  fprintf (stderr, "before: n = %u, s = %u, e = %u, p = %u\n",
		   (unsigned) n, (unsigned) s, (unsigned) extra_bits,
		   (unsigned) p);
#endif
	  if (extra_bits == 0)
	    {
	      /* We cross a limb boundary and bump s. We can't do that
		 if the result is that it makes makes min(U, V)
		 smaller than 2^{GMP_NUMB_BITS} s. */
	      if (s + 1 == n
		  || mpn_zero_p (up + s + 1, n - s - 1)
		  || mpn_zero_p (vp + s + 1, n - s - 1))
		continue;

	      extra_bits = GMP_NUMB_BITS - 1;
	      s++;
	    }
	  else
	    {
	      extra_bits--;
	    }

	  /* Drop the p least significant limbs */
	  up += p; vp += p; n -= p; s -= p;

#if TRACE
	  fprintf (stderr, "after: n = %u, s = %u, e = %u\n",
		   (unsigned) n, (unsigned) s, (unsigned) extra_bits);
#endif
	}
    }

  ASSERT (s > 0);
#if TRACE
  fprintf (stderr, "final: n = %u, s = %u, e = %u\n",
	   (unsigned) n, (unsigned) s, (unsigned) extra_bits);
#endif

  if (extra_bits > 0)
    {
      /* We can get here only of we have dropped at least one of the
	 least significant bits, so we can decrement up and vp. We can
	 then shift left extra bits using mpn_shiftr. */
      /* NOTE: In the unlikely case that n is large, it would be
	 preferable to do an initial subdiv step to reduce the size
	 before shifting, but that would mean duplicating
	 mpn_gcd_subdiv_step with a bit count rather than a limb
	 count. */
      up--; vp--;
      up[0] = mpn_rshift (up+1, up+1, n, GMP_NUMB_BITS - extra_bits);
      vp[0] = mpn_rshift (vp+1, vp+1, n, GMP_NUMB_BITS - extra_bits);
      n += (up[n] | vp[n]) > 0;

      ASSERT (success);

      while (n > 2)
	{
	  mp_size_t nn;

	  ASSERT (n > s);
	  ASSERT (n <= 2*s);

#if TRACE
      fprintf (stderr, "extra loop: n = %u\n", (unsigned) n);
#endif
	  nn = mpn_hgcd_step (n, up, vp, s, M, tp);
#if TRACE
	  fprintf (stderr, "extra bit reduction: %s\n",
		   nn ? "success" : "fail");
#endif
	  
	  if (!nn)
	    return 1;

	  n = nn;
	}
    }

  if (n == 2)
    {
      struct hgcd_matrix1 M1;
      ASSERT (s == 1);

      if (mpn_hgcd2 (up[1], up[0], vp[1], vp[0], &M1))
	{
#if TRACE
	  fprintf (stderr, "final hgcd2 succeeded\n");
#endif
	  /* Multiply M <- M * M1 */
	  mpn_hgcd_matrix_mul_1 (M, &M1, tp);
	  success = 1;
	}
    }

  return success;
}
