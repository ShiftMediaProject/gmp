/* mpf_ui_div -- Divide an unsigned integer with a float.

Copyright 1993, 1994, 1995, 1996, 2000, 2001, 2002 Free Software Foundation,
Inc.

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

void
mpf_ui_div (mpf_ptr r, unsigned long int u, mpf_srcptr v)
{
  mp_srcptr vp;
  mp_ptr rp, tp;
  mp_size_t vsize;
  mp_size_t rsize, tsize;
  mp_size_t sign_quotient;
  mp_size_t prec;
  mp_limb_t q_limb;
  mp_exp_t rexp;
  TMP_DECL (marker);

  vsize = v->_mp_size;
  sign_quotient = vsize;
  vsize = ABS (vsize);
  prec = r->_mp_prec;

  if (vsize == 0)
    DIVIDE_BY_ZERO;

  if (u == 0)
    {
      r->_mp_size = 0;
      r->_mp_exp = 0;
      return;
    }

  TMP_MARK (marker);
  rexp = 1 - v->_mp_exp;

  rp = r->_mp_d;
  vp = v->_mp_d;

  if (vsize > prec)
    {
      vp += vsize - prec;
      vsize = prec;
    }

  tsize = vsize + prec;
  tp = (mp_ptr) TMP_ALLOC ((tsize + 1) * BYTES_PER_MP_LIMB);
  MPN_ZERO (tp, tsize);

  /* Normalize the divisor and the dividend.  */
  if ((vp[vsize - 1] & GMP_NUMB_HIGHBIT) == 0)
    {
      mp_ptr tmp;
      mp_limb_t dividend_high, dividend_low;
      unsigned normalization_steps;

      count_leading_zeros (normalization_steps, vp[vsize - 1]);
      normalization_steps -= GMP_NAIL_BITS;

      /* Shift up the divisor setting the most significant bit of
	 the most significant limb.  Use temporary storage not to clobber
	 the original contents of the divisor.  */
      tmp = (mp_ptr) TMP_ALLOC (vsize * BYTES_PER_MP_LIMB);
      mpn_lshift (tmp, vp, vsize, normalization_steps);
      vp = tmp;

      /* Shift up the dividend, possibly introducing a new most
	 significant word.  */
      dividend_high = (mp_limb_t) u >> (GMP_NUMB_BITS - normalization_steps);
      dividend_low = ((mp_limb_t) u << normalization_steps) & GMP_NUMB_MASK;

      tp[tsize - 1] = dividend_low;
      if (dividend_high != 0)
	{
#if GMP_NAIL_BITS != 0
	  if (dividend_high > vp[vsize - 1])
	    {
	      tp[tsize - 2] = dividend_low;
	      tp[tsize - 1] = dividend_high & GMP_NUMB_MASK;
	      tp[tsize] = dividend_high >> GMP_NUMB_BITS;
	      tsize++;
	      rexp += 2;
	    }
	  else
#endif
	    {
	      tp[tsize] = dividend_high;
	      tsize++;
	      rexp++;
	    }
	}
    }
  else
    {
      /* The divisor is already normalized, as required.
	 Copy it to temporary space if it overlaps with the quotient.  */
      if (vp - rp <= tsize - vsize)
	{
	  mp_ptr tmp = (mp_ptr) TMP_ALLOC (vsize * BYTES_PER_MP_LIMB);
	  MPN_COPY (tmp, vp, vsize);
	  vp = (mp_srcptr) tmp;
	}

      tp[tsize - 1] = u & GMP_NUMB_MASK;
#if GMP_NAIL_BITS != 0
      if (u > GMP_NUMB_MAX)
	{
	  tp[tsize] = u >> GMP_NUMB_BITS;
	  tsize++;
	  rexp++;
	}
#endif
    }

  q_limb = mpn_divmod (rp, tp, tsize, vp, vsize);
  rsize = tsize - vsize;
  if (q_limb)
    {
      rp[rsize] = q_limb;
      rsize++;
      rexp++;
    }

  r->_mp_size = sign_quotient >= 0 ? rsize : -rsize;
  r->_mp_exp = rexp;
  TMP_FREE (marker);
}
