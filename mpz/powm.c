/* mpz_powm(res,base,exp,mod) -- Set RES to (base**exp) mod MOD.

Copyright 1991, 1993, 1994, 1996, 1997, 2000 Free Software Foundation, Inc.
Contributed by Paul Zimmermann.

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
#ifdef BERKELEY_MP
#include "mp.h"
#endif


/* Set c <- tp/R^n mod m.
   tp should have space for 2*n+1 limbs; clobber its most significant limb. */

void
mpn_redc (mp_ptr cp, mp_srcptr mp, mp_size_t n, mp_limb_t Nprim, mp_ptr tp)
{
  mp_limb_t cy;
  mp_limb_t q;
  size_t j;

  tp[2 * n] = 0;		/* carry guard */

  for (j = 0; j < n; j++)
    {
      q = tp[0] * Nprim;
      cy = mpn_addmul_1 (tp, mp, n, q);
      mpn_incr_u (tp + n, cy);
      tp++;
    }

  if (tp[n] != 0)
    mpn_sub_n (cp, tp, mp, n);
  else
    MPN_COPY (cp, tp, n);
}

/* average number of calls to redc for an exponent of n bits
   with the sliding window algorithm of base 2^k: the optimal is
   obtained for the value of k which minimizes 2^(k-1)+n/(k+1):

   n\k    4     5     6     7     8
   128    156*  159   171   200   261
   256    309   307*  316   343   403
   512    617   607*  610   632   688
   1024   1231  1204  1195* 1207  1256
   2048   2461  2399  2366  2360* 2396
   4096   4918  4787  4707  4665* 4670
*/

#ifndef BERKELEY_MP
void
#if __STDC__
mpz_powm (mpz_ptr res, mpz_srcptr base, mpz_srcptr e, mpz_srcptr mod)
#else
mpz_powm (res, base, e, mod)
     mpz_ptr res;
     mpz_srcptr base;
     mpz_srcptr e;
     mpz_srcptr mod;
#endif
#else /* BERKELEY_MP */
void
#if __STDC__
pow (mpz_srcptr base, mpz_srcptr e, mpz_srcptr mod, mpz_ptr res)
#else
pow (base, e, mod, res)
     mpz_srcptr base;
     mpz_srcptr e;
     mpz_srcptr mod;
     mpz_ptr res;
#endif
#endif /* BERKELEY_MP */
{
  mp_limb_t invm, *ep, c, mask;
  mpz_t xx, *g;
  mp_size_t n, i, K, j, l, k;
  int sh;
  int use_redc;
  mp_ptr tp;
  TMP_DECL (marker);

  TMP_MARK (marker);

  n = ABSIZ (mod);
  if (n == 0)
    DIVIDE_BY_ZERO;

  if (SIZ (e) <= 0)
    {
      /* Exponent is zero, result is 1 mod MOD, i.e., 1 or 0
         depending on if MOD equals 1.  */
      SIZ(res) = (ABSIZ (mod) == 1 && (PTR(mod))[0] == 1) ? 0 : 1;
      PTR(res)[0] = 1;
      return;
    }

  /* Use REDC instead of usual reduction for sizes < POWM_THRESHOLD.
     In REDC each modular multiplication costs about 2*n^2 limbs operations,
     whereas using usual reduction it costs 3*K(n), where K(n) is the cost of a
     multiplication using Karatsuba, and a division is assumed to cost 2*K(n),
     for example using Burnikel-Ziegler's algorithm. This gives a theoretical
     threshold of a*KARATSUBA_SQR_THRESHOLD, with a=(3/2)^(1/(2-ln(3)/ln(2))) ~
     2.66.  */
  /* For now, also disable REDC when MOD is even, as the inverse can't
     handle that.  */
  /* For use_redc, we use an ugly mixture of mpn and mpz.  The variables
     are in mpz, but we don't keep them normalized as mpz requires,
     except in a few places.  */

#ifndef POWM_THRESHOLD
#define POWM_THRESHOLD  ((8 * KARATSUBA_SQR_THRESHOLD) / 3)
#endif

  use_redc = n < POWM_THRESHOLD && PTR(mod)[0] % 2 != 0;
  if (use_redc)
    {
      /* invm = -1/m mod 2^BITS_PER_MP_LIMB, must have m odd */
      modlimb_invert (invm, PTR(mod)[0]);
      invm = -invm;
    }

  /* Determine optimal value of k, the number of exponent bits we look at
     at a time.  */
  count_leading_zeros (sh, PTR(e)[SIZ(e)-1]);
  l = SIZ (e) * BITS_PER_MP_LIMB - sh; /* number of bits of exponent */
  k = 1;
  K = 2;
  while (2 * l > K * (2 + k * (3 + k)))
    {
      k++;
      K *= 2;
    }

  tp = TMP_ALLOC_LIMBS (2 * n + 1);

  g = TMP_ALLOC_TYPE (K / 2, mpz_t);
  /* Compute x*R^n where R=2^BITS_PER_MP_LIMB.  */
  mpz_init (g[0]);
  mpz_mod (g[0], base, mod);
  if (use_redc)
    {
      mpz_mul_2exp (g[0], g[0], n * BITS_PER_MP_LIMB);
      mpz_mod (g[0], g[0], mod);
      _mpz_realloc (g[0], n);
      MPN_ZERO (PTR(g[0]) + SIZ(g[0]), n - SIZ(g[0]));
      SIZ(g[0]) = n;
    }

  /* Compute xx^i for odd g < 2^i.  */
  mpz_init (xx);
  _mpz_realloc (xx, n);

  if (use_redc)
    {
      mpn_sqr_n (tp, PTR(g[0]), n);
      mpn_redc (PTR(xx), PTR(mod), n, invm, tp); /* xx = x^2*R^n */
    }
  else
    {
      mpz_mul (xx, g[0], g[0]);
      mpz_mod (xx, xx, mod);
    }
  for (i = 1; i < K / 2; i++)
    {
      mpz_init (g[i]);
      if (use_redc)
	{
	  _mpz_realloc (g[i], n);
	  mpn_mul_n (tp, PTR(g[i - 1]), PTR(xx), n);
	  mpn_redc (PTR(g[i]), PTR(mod), n, invm, tp); /* g[i] = x^(2i+1)*R^n */
	  SIZ(g[i]) = n;
	}
      else
	{
	  mpz_mul (g[i], g[i - 1], xx);
	  mpz_mod (g[i], g[i], mod);
	}
    }

  /* Start the real stuff.  */
  mask = ((mp_limb_t) 1 << k) - 1;
  ep = PTR (e);
  i = SIZ (e) - 1;			/* current index */
  c = ep[i];				/* current limb */
  count_leading_zeros (sh, c);
  sh = BITS_PER_MP_LIMB - sh;		/* significant bits in ep[i] */
  sh -= k;				/* index of lower bit of ep[i] to take into account */
  if (sh < 0)
    {					/* k-sh extra bits are needed */
      if (i > 0)
	{
	  i--;
	  c = (c << (-sh)) | (ep[i] >> (BITS_PER_MP_LIMB + sh));
	  sh += BITS_PER_MP_LIMB;
	}
    }
  else
    c = c >> sh;

  j = 0;
  while (c % 2 == 0)
    {
      j++;
      c = c >> 1;
    }
  mpz_set (xx, g[c >> 1]);
  while (--j >= 0)
    {
      if (use_redc)
	{
	  mpn_sqr_n (tp, PTR(xx), n);
	  mpn_redc (PTR(xx), PTR(mod), n, invm, tp);
	}
      else
	{
	  mpz_mul (xx, xx, xx);
	  mpz_mod (xx, xx, mod);
	}
    }

  while (i > 0 || sh > 0)
    {
      c = ep[i];
      sh -= k;
      l = k;				/* number of bits treated */
      if (sh < 0)
	{
	  if (i > 0)
	    {
	      i--;
	      c = (c << (-sh)) | (ep[i] >> (BITS_PER_MP_LIMB + sh));
	      sh += BITS_PER_MP_LIMB;
	    }
	  else
	    {
	      l += sh;			/* may be less bits than k here */
	      c = c & (((mp_limb_t) 1 << l) - 1);
	    }
	}
      else
	c = c >> sh;
      c = c & mask;

      /* This while loop implements the sliding window improvement.  */
      while ((c & ((mp_limb_t) 1 << (k - 1))) == 0 && (i > 0 || sh > 0))
	{
	  if (use_redc)
	    {
	      mpn_sqr_n (tp, PTR(xx), n);
	      mpn_redc (PTR(xx), PTR(mod), n, invm, tp);
	    }
	  else
	    {
	      mpz_mul (xx, xx, xx);
	      mpz_mod (xx, xx, mod);
	    }
	  if (sh != 0)
	    {
	      sh--;
	      c = (c << 1) + ((ep[i] >> sh) & 1);
	    }
	  else
	    {
	      i--;
	      sh = BITS_PER_MP_LIMB - 1;
	      c = (c << 1) + (ep[i] >> sh);
	    }
	}

      /* Replace xx by xx^(2^k)*x^c.  */
      if (c != 0)
	{
	  j = 0;
	  while (c % 2 == 0)
	    {
	      j++;
	      c = c >> 1;
	    }
	  /* c0 = c * 2^j, i.e. xx^(2^k)*x^c = (A^(2^(k - j))*c)^(2^j) */
	  l -= j;
	  while (--l >= 0)
	    {
	      if (use_redc)
		{
		  mpn_sqr_n (tp, PTR(xx), n);
		  mpn_redc (PTR(xx), PTR(mod), n, invm, tp);
		}
	      else
		{
		  mpz_mul (xx, xx, xx);
		  mpz_mod (xx, xx, mod);
		}
	    }
	  if (use_redc)
	    {
	      mpn_mul_n (tp, PTR(xx), PTR(g[c >> 1]), n);
	      mpn_redc (PTR(xx), PTR(mod), n, invm, tp);
	    }
	  else
	    {
	      mpz_mul (xx, xx, g[c >> 1]);
	      mpz_mod (xx, xx, mod);
	    }
	}
      else
	j = l;				/* case c=0 */
      while (--j >= 0)
	{
	  if (use_redc)
	    {
	      mpn_sqr_n (tp, PTR(xx), n);
	      mpn_redc (PTR(xx), PTR(mod), n, invm, tp);
	    }
	  else
	    {
	      mpz_mul (xx, xx, xx);
	      mpz_mod (xx, xx, mod);
	    }
	}
    }

  /* Convert back xx to xx/R^n.  */
  if (use_redc)
    {
      MPN_COPY (tp, PTR(xx), n);
      MPN_ZERO (tp + n, n);
      mpn_redc (PTR(xx), PTR(mod), n, invm, tp);
      SIZ(xx) = n;
      MPN_NORMALIZE (PTR(xx), SIZ(xx));
      if (mpz_cmp (xx, mod) >= 0)
	mpz_sub (xx, xx, mod);
    }
  mpz_set (res, xx);

  mpz_clear (xx);
  for (i = 0; i < K / 2; i++)
    mpz_clear (g[i]);
  TMP_FREE (marker);
}
