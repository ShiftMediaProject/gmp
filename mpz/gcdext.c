/* mpz_gcdext(g, s, t, a, b) -- Set G to gcd(a, b), and S and T such that
   g = as + bt.

Copyright 1991, 1993, 1994, 1995, 1996, 1997, 2000, 2001, 2005, 2011 Free Software
Foundation, Inc.

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

#include <stdio.h> /* for NULL */
#include "gmp.h"
#include "gmp-impl.h"

void
mpz_gcdext (mpz_ptr g, mpz_ptr s, mpz_ptr t, mpz_srcptr a, mpz_srcptr b)
{
  mp_size_t asize, bsize;
  mp_srcptr ap, bp;
  mp_ptr tmp_ap, tmp_bp;
  mp_size_t gsize, ssize, tmp_ssize;
  mp_ptr gp, sp, tmp_gp, tmp_sp;
  __mpz_struct stmp, gtmp;
  TMP_DECL;

  /* mpn_gcdext requires that U >= V.  Therefore, we often have to swap U and
     V.  This in turn leads to a lot of complications.  The computed cofactor
     will be the wrong one, so we have to fix that up at the end.  */

  asize = ABS (SIZ (a));
  bsize = ABS (SIZ (b));
  ap = PTR (a);
  bp = PTR (b);

  if (asize < bsize || (asize == bsize && mpn_cmp (ap, bp, asize) < 0))
    {
      MPZ_SRCPTR_SWAP (a, b);
      MPN_SRCPTR_SWAP (ap, asize, bp, bsize);
      MPZ_PTR_SWAP (s, t);
    }

  if (bsize == 0)    
    {
      /* g = |a|, s = sign(a), t = 0. */
      ssize = SIZ (a) >= 0 ? 1 : -1;

      if (ALLOC (g) < asize)
	_mpz_realloc (g, asize);
      gp = PTR (g);
      MPN_COPY (gp, ap, asize);
      SIZ (g) = asize;

      if (t != NULL)
	SIZ (t) = 0;
      if (s != NULL)
	{
	  SIZ (s) = ssize;
	  PTR (s)[0] = 1;
	}
      return;
    }

  TMP_MARK;

  tmp_ap = TMP_ALLOC_LIMBS (asize);
  tmp_bp = TMP_ALLOC_LIMBS (bsize);
  MPN_COPY (tmp_ap, ap, asize);
  MPN_COPY (tmp_bp, bp, bsize);
  
  tmp_gp = TMP_ALLOC_LIMBS (bsize);
  tmp_sp = TMP_ALLOC_LIMBS (bsize);

  gsize = mpn_gcdext (tmp_gp, tmp_sp, &tmp_ssize, tmp_ap, asize, tmp_bp, bsize);

  ssize = ABS (tmp_ssize);

  PTR (&gtmp) = tmp_gp;
  SIZ (&gtmp) = gsize;

  PTR (&stmp) = tmp_sp;
  SIZ (&stmp) = (tmp_ssize ^ SIZ (a)) >= 0 ? ssize : -ssize;

  if (t != NULL)
    {
      mpz_t x;
      MPZ_TMP_INIT (x, ssize + asize + 1);
      mpz_mul (x, &stmp, a);
      mpz_sub (x, &gtmp, x);
      mpz_divexact (t, x, b);
    }

  if (s != NULL)
    {
      if (ALLOC (s) < ssize)
	_mpz_realloc (s, ssize);
      sp = PTR (s);
      MPN_COPY (sp, tmp_sp, ssize);
      SIZ (s) = SIZ (&stmp);
    }

  if (ALLOC (g) < gsize)
    _mpz_realloc (g, gsize);
  gp = PTR (g);
  MPN_COPY (gp, tmp_gp, gsize);
  SIZ (g) = gsize;

  TMP_FREE;
}
