/* Reference floating point routines.

Copyright 1996, 2001, 2004 Free Software Foundation, Inc.

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

#include <stdio.h>
#include <stdlib.h>

#include "gmp.h"
#include "gmp-impl.h"
#include "tests.h"


void
refmpf_add (mpf_ptr w, mpf_srcptr u, mpf_srcptr v)
{
  mp_size_t hi, lo, size;
  mp_ptr ut, vt, wt;
  int neg;
  mp_exp_t exp;
  mp_limb_t cy;
  TMP_DECL (mark);

  TMP_MARK (mark);

  if (SIZ (u) == 0)
    {
      size = ABSIZ (v);
      wt = (mp_ptr) TMP_ALLOC ((size+1) * BYTES_PER_MP_LIMB);
      MPN_COPY (wt, PTR (v), size);
      exp = EXP (v);
      neg = SIZ (v) < 0;
      goto done;
    }
  if (SIZ (v) == 0)
    {
      size = ABSIZ (u);
      wt = (mp_ptr) TMP_ALLOC ((size+1) * BYTES_PER_MP_LIMB);
      MPN_COPY (wt, PTR (u), size);
      exp = EXP (u);
      neg = SIZ (u) < 0;
      goto done;
    }
  if ((SIZ (u) ^ SIZ (v)) < 0)
    {
      mpf_t tmp;
      SIZ (tmp) = -SIZ (v);
      EXP (tmp) = EXP (v);
      PTR (tmp) = PTR (v);
      refmpf_sub (w, u, tmp);
      return;
    }
  neg = SIZ (u) < 0;

  /* Compute the significance of the hi and lo end of the result.  */
  hi = MAX (EXP (u), EXP (v));
  lo = MIN (EXP (u) - ABSIZ (u), EXP (v) - ABSIZ (v));
  size = hi - lo;
  ut = (mp_ptr) TMP_ALLOC ((size + 1) * BYTES_PER_MP_LIMB);
  vt = (mp_ptr) TMP_ALLOC ((size + 1) * BYTES_PER_MP_LIMB);
  wt = (mp_ptr) TMP_ALLOC ((size + 1) * BYTES_PER_MP_LIMB);
  MPN_ZERO (ut, size);
  MPN_ZERO (vt, size);
  {int off;
  off = size + (EXP (u) - hi) - ABSIZ (u);
  MPN_COPY (ut + off, PTR (u), ABSIZ (u));
  off = size + (EXP (v) - hi) - ABSIZ (v);
  MPN_COPY (vt + off, PTR (v), ABSIZ (v));
  }

  cy = mpn_add_n (wt, ut, vt, size);
  wt[size] = cy;
  size += cy;
  exp = hi + cy;

done:
  if (size > PREC (w))
    {
      wt += size - PREC (w);
      size = PREC (w);
    }
  MPN_COPY (PTR (w), wt, size);
  SIZ (w) = neg == 0 ? size : -size;
  EXP (w) = exp;
  TMP_FREE (mark);
}


/* Add 1 "unit in last place" (ie. in the least significant limb) to f.
   f cannot be zero, since that has no well-defined "last place".

   This routine is designed for use in cases where we pay close attention to
   the size of the data value and are using that (and the exponent) to
   indicate the accurate part of a result, or similar.  For this reason, if
   there's a carry out we don't store 1 and adjust the exponent, we just
   leave 100..00.  We don't even adjust if there's a carry out of prec+1
   limbs, but instead give up in that case (which we intend shouldn't arise
   in normal circumstances).  */

void
refmpf_add_ulp (mpf_ptr f)
{
  mp_ptr     fp = PTR(f);
  mp_size_t  fsize = SIZ(f);
  mp_size_t  abs_fsize = ABSIZ(f);
  mp_limb_t  c;

  if (fsize == 0)
    {
      printf ("Oops, refmpf_add_ulp called with f==0\n");
      abort ();
    }

  c = refmpn_add_1 (fp, fp, abs_fsize, CNST_LIMB(1));
  if (c != 0)
    {
      if (abs_fsize >= PREC(f) + 1)
        {
          printf ("Oops, refmpf_add_ulp carried out of prec+1 limbs\n");
          abort ();
        }

      fp[abs_fsize] = c;
      abs_fsize++;
      SIZ(f) = (fsize > 0 ? abs_fsize : - abs_fsize);
    }
}


/* Like mpf_set_prec, but taking a precision in limbs.
   PREC(f) ends up as the given "prec" value.  */
void
refmpf_set_prec_limbs (mpf_ptr f, unsigned long prec)
{
  mpf_set_prec (f, __GMPF_PREC_TO_BITS (prec));
}


void
refmpf_sub (mpf_ptr w, mpf_srcptr u, mpf_srcptr v)
{
  mp_size_t hi, lo, size;
  mp_ptr ut, vt, wt;
  int neg;
  mp_exp_t exp;
  TMP_DECL (mark);

  TMP_MARK (mark);

  if (SIZ (u) == 0)
    {
      size = ABSIZ (v);
      wt = (mp_ptr) TMP_ALLOC ((size+1) * BYTES_PER_MP_LIMB);
      MPN_COPY (wt, PTR (v), size);
      exp = EXP (v);
      neg = SIZ (v) > 0;
      goto done;
    }
  if (SIZ (v) == 0)
    {
      size = ABSIZ (u);
      wt = (mp_ptr) TMP_ALLOC ((size+1) * BYTES_PER_MP_LIMB);
      MPN_COPY (wt, PTR (u), size);
      exp = EXP (u);
      neg = SIZ (u) < 0;
      goto done;
    }
  if ((SIZ (u) ^ SIZ (v)) < 0)
    {
      mpf_t tmp;
      SIZ (tmp) = -SIZ (v);
      EXP (tmp) = EXP (v);
      PTR (tmp) = PTR (v);
      refmpf_add (w, u, tmp);
      if (SIZ (u) < 0)
	mpf_neg (w, w);
      return;
    }
  neg = SIZ (u) < 0;

  /* Compute the significance of the hi and lo end of the result.  */
  hi = MAX (EXP (u), EXP (v));
  lo = MIN (EXP (u) - ABSIZ (u), EXP (v) - ABSIZ (v));
  size = hi - lo;
  ut = (mp_ptr) TMP_ALLOC ((size + 1) * BYTES_PER_MP_LIMB);
  vt = (mp_ptr) TMP_ALLOC ((size + 1) * BYTES_PER_MP_LIMB);
  wt = (mp_ptr) TMP_ALLOC ((size + 1) * BYTES_PER_MP_LIMB);
  MPN_ZERO (ut, size);
  MPN_ZERO (vt, size);
  {int off;
  off = size + (EXP (u) - hi) - ABSIZ (u);
  MPN_COPY (ut + off, PTR (u), ABSIZ (u));
  off = size + (EXP (v) - hi) - ABSIZ (v);
  MPN_COPY (vt + off, PTR (v), ABSIZ (v));
  }

  if (mpn_cmp (ut, vt, size) >= 0)
    mpn_sub_n (wt, ut, vt, size);
  else
    {
      mpn_sub_n (wt, vt, ut, size);
      neg ^= 1;
    }
  exp = hi;
  while (size != 0 && wt[size - 1] == 0)
    {
      size--;
      exp--;
    }

done:
  if (size > PREC (w))
    {
      wt += size - PREC (w);
      size = PREC (w);
    }
  MPN_COPY (PTR (w), wt, size);
  SIZ (w) = neg == 0 ? size : -size;
  EXP (w) = exp;
  TMP_FREE (mark);
}
