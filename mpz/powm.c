/* mpz_powm(res,base,exp,mod) -- Set RES to (base**exp) mod MOD.

   Contributed to the GNU project by Torbjorn Granlund.

Copyright 1991, 1993, 1994, 1996, 1997, 2000, 2001, 2002, 2005, 2008, 2009
Free Software Foundation, Inc.

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


#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#ifdef BERKELEY_MP
#include "mp.h"
#endif


/* TODO
 * Improve handling of buffers.
 */

/*
  b ^ e mod m   res
  0   0     0    ?
  0   e     0    ?
  0   0     m    ?
  0   e     m    0
  b   0     0    ?
  b   e     0    ?
  b   0     m    1 mod m
  b   e     m    b^e mod m
*/

#ifndef POWM_THRESHOLD
#define POWM_THRESHOLD  ((8 * SQR_KARATSUBA_THRESHOLD) / 3)
#endif

#define HANDLE_NEGATIVE_EXPONENT 1

void mpn_powm (mp_ptr, mp_srcptr, mp_size_t, mp_srcptr, mp_size_t, mp_srcptr, mp_size_t, mp_ptr);
void mpn_powlo  (mp_ptr, mp_srcptr, mp_srcptr, mp_size_t, mp_size_t, mp_ptr);

void
#ifndef BERKELEY_MP
mpz_powm (mpz_ptr r, mpz_srcptr b, mpz_srcptr e, mpz_srcptr m)
#else /* BERKELEY_MP */
pow (mpz_srcptr b, mpz_srcptr e, mpz_srcptr m, mpz_ptr r)
#endif /* BERKELEY_MP */
{
  mp_size_t n, nodd, ncnt;
  int cnt;
  mp_ptr rp, tp;
  mp_srcptr bp, ep, mp;
  mp_size_t rn, bn, es, en;
  TMP_DECL;

  n = ABSIZ(m);
  if (n == 0)
    DIVIDE_BY_ZERO;

  mp = PTR(m);

  TMP_MARK;

  es = SIZ(e);
  if (es <= 0)
    {
      mpz_t new_b;
      if (es == 0)
	{
	  /* b^0 mod m,  b is anything and m is non-zero.
	     Result is 1 mod m, i.e., 1 or 0 depending on if m = 1.  */
	  SIZ(r) = n != 1 || mp[0] != 1;
	  PTR(r)[0] = 1;
	  TMP_FREE;	/* we haven't really allocated anything here */
	  return;
	}
#if HANDLE_NEGATIVE_EXPONENT
      MPZ_TMP_INIT (new_b, n + 1);

      if (! mpz_invert (new_b, b, m))
	DIVIDE_BY_ZERO;
      b = new_b;
      es = -es;
#else
      DIVIDE_BY_ZERO;
#endif
    }
  en = es;

  bp = PTR(b);
  bn = ABSIZ(b);

  if (bn == 0)
    {
      SIZ(r) = 0;
      TMP_FREE;
      return;
    }

  ep = PTR(e);

  /* Handle (b^1 mod m) early, since mpn_pow* do not handle that case.  */
  if (en == 1 && ep[0] == 1)
    {
      MPZ_REALLOC (r, n);
      rp = PTR(r);
      if (bn >= n)
	{
	  mp_ptr qp = TMP_ALLOC_LIMBS (bn - n + 1);
	  mpn_tdiv_qr (qp, rp, 0L, bp, bn, mp, n);
	  rn = n;
	  MPN_NORMALIZE (rp, rn);

	  if (SIZ(b) < 0 && rn != 0)
	    {
	      mpn_sub (rp, mp, n, rp, rn);
	      rn = n;
	      MPN_NORMALIZE (rp, rn);
	    }
	}
      else
	{
	  if (SIZ(b) < 0)
	    {
	      mpn_sub (rp, mp, n, bp, bn);
	      rn = n;
	      rn -= (rp[rn - 1] == 0);
	    }
	  else
	    {
	      MPN_COPY (rp, bp, bn);
	      rn = bn;
	    }
	}
      SIZ(r) = rn;
      TMP_FREE;
      return;
    }

  /* Remove low zero limbs from M.  This loop will terminate for correctly
     represented mpz numbers.  */
  ncnt = 0;
  while (mp[0] == 0)
    {
      mp++;
      ncnt++;
    }
  nodd = n - ncnt;
  cnt = 0;
  if (mp[0] % 2 == 0)
    {
      mp_ptr new = TMP_ALLOC_LIMBS (nodd);
      count_trailing_zeros (cnt, mp[0]);
      mpn_rshift (new, mp, nodd, cnt);
      nodd -= new[nodd - 1] == 0;
      mp = new;
      ncnt++;
    }

  rp = TMP_ALLOC_LIMBS (n + 1);

  tp = TMP_ALLOC_LIMBS (4 * n + 1);
  tp[3 * n] = 0xfafa;
  mpn_powm (rp, bp, bn, ep, en, mp, nodd, tp);

  rn = n;

  if (ncnt != 0)
    {
      mp_ptr r2, xp, yp, odd_inv_2exp;
      unsigned long t;
      int bcnt;

      if (bn < ncnt)
	{
	  mp_ptr new = TMP_ALLOC_LIMBS (ncnt);
	  MPN_COPY (new, bp, bn);
	  MPN_ZERO (new + bn, ncnt - bn);
	  bp = new;
	}

      r2 = tp;

      if (bp[0] % 2 == 0)
	{
	  if (en > 1)
	    {
	      MPN_ZERO (r2, ncnt);
	      goto zero;
	    }

	  ASSERT (en == 1);
	  t = (ncnt - (cnt != 0)) * GMP_NUMB_BITS + cnt;

	  /* Count number of low zero bits in B, up to 3.  */
	  bcnt = (0x1213 >> ((bp[0] & 7) << 1)) & 0x3;
	  /* Note that ep[0] * bcnt might overflow, but that just results
	     in a missed optimization.  */
	  if (ep[0] * bcnt >= t)
	    {
	      MPN_ZERO (r2, ncnt);
	      goto zero;
	    }
	}

      mpn_powlo (r2, bp, ep, en, ncnt, tp + ncnt);

    zero:
      if (nodd < ncnt)
	{
	  mp_ptr new = TMP_ALLOC_LIMBS (ncnt);
	  MPN_COPY (new, mp, nodd);
	  MPN_ZERO (new + nodd, ncnt - nodd);
	  mp = new;
	}

      odd_inv_2exp = tp + n;
      mpn_binvert (odd_inv_2exp, mp, ncnt, tp + 2 * n);

      mpn_sub (r2, r2, ncnt, rp, nodd > ncnt ? ncnt : nodd);

      xp = tp + 2 * n;
      mpn_mullow_n (xp, odd_inv_2exp, r2, ncnt);

      if (cnt != 0)
	xp[ncnt - 1] &= (CNST_LIMB(1) << cnt) - 1;

      yp = tp;
      if (ncnt > nodd)
	mpn_mul (yp, xp, ncnt, mp, nodd);
      else
	mpn_mul (yp, mp, nodd, xp, ncnt);

      mpn_add (rp, yp, n, rp, nodd);

      ASSERT (nodd + ncnt >= n);
      ASSERT (nodd + ncnt <= n + 1);
    }

  MPN_NORMALIZE (rp, rn);

  if ((ep[0] & 1) && SIZ(b) < 0 && rn != 0)
    {
      mpn_sub (rp, PTR(m), ABSIZ(m), rp, rn);
      rn = n;
      MPN_NORMALIZE (rp, rn);
    }

  MPZ_REALLOC (r, rn);
  SIZ(r) = rn;
  MPN_COPY (PTR(r), rp, rn);

  TMP_FREE;
}
