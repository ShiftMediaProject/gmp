/* mpf_get_str (digit_ptr, exp, base, n_digits, a) -- Convert the floating
  point number A to a base BASE number and store N_DIGITS raw digits at
  DIGIT_PTR, and the base BASE exponent in the word pointed to by EXP.  For
  example, the number 3.1416 would be returned as "31416" in DIGIT_PTR and
  1 in EXP.

Copyright (C) 1993, 1994, 1995, 1996, 1997, 2000 Free Software Foundation,
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

/*
  The conversion routine works like this:

  1. If U >= 1, compute U' = U / base**n, where n is chosen such that U' is
     the largest number smaller than 1.
  2. Else, if U < 1, compute U' = U * base**n, where n is chosen such that U'
     is the largest number smaller than 1.
  3. Convert U' (by repeatedly multiplying it by base).  This process can
     easily be interrupted when the needed number of digits are generated.
*/

char *
#if __STDC__
mpf_get_str (char *digit_ptr, mp_exp_t *exp, int base, size_t n_digits, mpf_srcptr u)
#else
mpf_get_str (digit_ptr, exp, base, n_digits, u)
     char *digit_ptr;
     mp_exp_t *exp;
     int base;
     size_t n_digits;
     mpf_srcptr u;
#endif
{
  mp_ptr up;
  mp_size_t usize;
  mp_exp_t uexp;
  mp_size_t prec;
  unsigned char *str;
  char *num_to_text;
  mp_ptr rp;
  mp_size_t rsize;
  mp_limb_t big_base;
  size_t digits_computed_so_far;
  int dig_per_u;
  unsigned char *tstr;
  mp_exp_t exp_in_base;
  int cnt;
  TMP_DECL (marker);

  TMP_MARK (marker);
  usize = u->_mp_size;
  uexp = u->_mp_exp;
  prec = u->_mp_prec + 1;

  if (base >= 0)
    {
      if (base == 0)
	base = 10;
      num_to_text = "0123456789abcdefghijklmnopqrstuvwxyz";
    }
  else
    {
      base = -base;
      num_to_text = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    }

  /* Don't compute more digits than U can accurately represent.
     Also, if 0 digits were requested, give *exactly* as many digits
     as can be accurately represented.  */
  {
    size_t max_digits = 2 + (size_t) (((prec - 2) * BITS_PER_MP_LIMB)
				      * __mp_bases[base].chars_per_bit_exactly);
    if (n_digits == 0 || n_digits > max_digits)
      n_digits = max_digits;
#if 0
/* This seems to work, but check it better before enabling it.  */
    else
      /* Limit the computation precision if only a limited digits are
	 desired.  We could probably decrease both this, and avoid the +1
	 for setting prec above.  */
      prec = 2 + (mp_size_t)
	(n_digits / (BITS_PER_MP_LIMB * __mp_bases[base].chars_per_bit_exactly));
#endif
  }

  if (digit_ptr == 0)
    {
      /* We didn't get a string from the user.  Allocate one (and return
	 a pointer to it) with space for `-' and terminating null.  */
      digit_ptr = (char *) (*_mp_allocate_func) (n_digits + 2);
    }

  if (usize == 0)
    {
      *exp = 0;
      *digit_ptr = 0;
      return digit_ptr;
    }

  str = (unsigned char *) digit_ptr;

  if (usize < 0)
    {
      *digit_ptr = '-';
      str++;
      usize = -usize;
    }

  up = PTR (u);

  if (uexp > 0)
    {
      /* U >= 1.  Compute U' = U / base**n, where n is chosen such that U' < 1.  */
      mp_size_t ralloc;
      mp_ptr tp;
      int i;

      /* Limit the number of digits to develop for small integers.  */
#if 0
      if (exp_in_base < n_digits)
	n_digits = exp_in_base;
#endif

      count_leading_zeros (cnt, up[usize - 1]);
      exp_in_base = (((double) uexp * BITS_PER_MP_LIMB - cnt)
		     * __mp_bases[base].chars_per_bit_exactly);
      exp_in_base += 1;

      ralloc = (prec + 1) * 2;
      rp = (mp_ptr) TMP_ALLOC (ralloc * BYTES_PER_MP_LIMB);
      tp = (mp_ptr) TMP_ALLOC (ralloc * BYTES_PER_MP_LIMB);

      rp[0] = base;
      rsize = 1;
      count_leading_zeros (cnt, exp_in_base);
      for (i = BITS_PER_MP_LIMB - cnt - 2; i >= 0; i--)
	{
	  mpn_mul_n (tp, rp, rp, rsize);
	  rsize = 2 * rsize;
	  rsize -= tp[rsize - 1] == 0;

	  if (rsize > prec)
	    {
	      MPN_COPY (rp, tp + rsize - prec, prec + 1);
	      rsize = prec;
	    }
	  else
	    MP_PTR_SWAP (rp, tp);

	  if (((exp_in_base >> i) & 1) != 0)
	    {
	      mp_limb_t cy;
	      cy = mpn_mul_1 (rp, rp, rsize, (mp_limb_t) base);
	      rp[rsize] = cy;
	      rsize += cy != 0;
	    }
	}

      count_leading_zeros (cnt, rp[rsize - 1]);
      if (cnt != 0)
	{
	  mpn_lshift (rp, rp, rsize, cnt);

	  if (usize < rsize)
	    {
	      /* Pad out U to the size of R while shifting it.
		 (Reuse temporary space at tp.)  */
	      mp_limb_t cy;

	      MPN_ZERO (tp, rsize - usize);
	      cy = mpn_lshift (tp + rsize - usize, up, usize, cnt);
	      up = tp;
	      usize = rsize;
	      if (cy)
		up[usize++] = cy;
	      ASSERT_ALWAYS (usize <= ralloc);	/* sufficient space? */
	    }
	  else
	    {
	      /* Copy U to temporary space.  */
	      /* FIXME: Allocate more space for tp above, and reuse it here.  */
	      mp_limb_t cy;
	      mp_ptr tup = (mp_ptr) TMP_ALLOC ((usize + 1) * BYTES_PER_MP_LIMB);

	      cy = mpn_lshift (tup, up, usize, cnt);
	      up = tup;
	      if (cy)
		up[usize++] = cy;
	    }
	}
      else
	{
	  if (usize < rsize)
	    {
	      /* Pad out U to the size of R.  (Reuse temporary space at tp.)  */
	      MPN_ZERO (tp, rsize - usize);
	      MPN_COPY (tp + rsize - usize, up, usize);
	      up = tp;
	      usize = rsize;
	    }
	  else
	    {
	      /* Copy U to temporary space.  */
	      mp_ptr tmp = (mp_ptr) TMP_ALLOC (usize * BYTES_PER_MP_LIMB);
	      MPN_COPY (tmp, up, usize);
	      up = tmp;
	    }
	}

      {
	mp_ptr qp;
	qp = (mp_ptr) TMP_ALLOC (prec * BYTES_PER_MP_LIMB);
	mpn_divrem (qp, prec - (usize - rsize), up, usize, rp, rsize);
	rsize = prec;
	rp = qp;
      }
    }
  else
    {
      /* U < 1.  Compute U' = U * base**n, where n is chosen such that U' is
	 the greatest number that still satisfies U' < 1.  */
      mp_size_t ralloc;
      mp_ptr tp;
      int i;

      uexp = -uexp;
      count_leading_zeros (cnt, up[usize - 1]);
      exp_in_base = (((double) uexp * BITS_PER_MP_LIMB + cnt - 1)
		     * __mp_bases[base].chars_per_bit_exactly);
      if (exp_in_base < 0)
	exp_in_base = 0;

      if (exp_in_base != 0)
	{
	  ralloc = (prec + 1) * 2;
	  rp = (mp_ptr) TMP_ALLOC (ralloc * BYTES_PER_MP_LIMB);
	  tp = (mp_ptr) TMP_ALLOC (ralloc * BYTES_PER_MP_LIMB);

	  rp[0] = base;
	  rsize = 1;
	  count_leading_zeros (cnt, exp_in_base);
	  for (i = BITS_PER_MP_LIMB - cnt - 2; i >= 0; i--)
	    {
	      mpn_mul_n (tp, rp, rp, rsize);
	      rsize = 2 * rsize;
	      rsize -= tp[rsize - 1] == 0;
	      if (rsize > prec)
		{
		  MPN_COPY (rp, tp + rsize - prec, prec + 1);
		  rsize = prec;
		}
	      else
		MP_PTR_SWAP (rp, tp);

	      if (((exp_in_base >> i) & 1) != 0)
		{
		  mp_limb_t cy;
		  cy = mpn_mul_1 (rp, rp, rsize, (mp_limb_t) base);
		  rp[rsize] = cy;
		  rsize += cy != 0;
		}
	    }

	  {
	    mp_limb_t cy;
	    tp = (mp_ptr) TMP_ALLOC ((rsize + usize) * BYTES_PER_MP_LIMB);
	    if (rsize > usize)
	      cy = mpn_mul (tp, rp, rsize, up, usize);
	    else
	      cy = mpn_mul (tp, up, usize, rp, rsize);
	    rsize += usize;
	    rsize -= cy == 0;
	    rp = tp;
	  }
	  exp_in_base = -exp_in_base;
	}
      else
	{
	  rp = (mp_ptr) TMP_ALLOC (usize * BYTES_PER_MP_LIMB);
	  MPN_COPY (rp, up, usize);
	  rsize = usize;
	}
    }

  big_base = __mp_bases[base].big_base;
  dig_per_u = __mp_bases[base].chars_per_limb;

  /* Hack for correctly (although not optimally) converting to bases that are
     powers of 2.  If we deem it important, we could handle powers of 2 by
     shifting and masking (just like mpn_get_str).  */
  if (big_base < 10)		/* logarithm of base when power of two */
    {
      int logbase = big_base;
      if (dig_per_u * logbase == BITS_PER_MP_LIMB)
	dig_per_u--;
      big_base = (mp_limb_t) 1 << (dig_per_u * logbase);
      /* fall out to general code... */
    }

  /* Now that we have normalized the number, develop the digits, essentially by
     multiplying it by BASE.  We initially develop at least 3 extra digits,
     since the two leading digits might become zero, and we need one extra for
     rounding the output properly.  */

  /* Allocate temporary digit space.  We can't put digits directly in the user
     area, since we generate more digits than requested.  (We allocate
     BITS_PER_MP_LIMB extra bytes because of the digit block nature of the
     conversion.)  */
  tstr = (unsigned char *) TMP_ALLOC (n_digits + BITS_PER_MP_LIMB + 3);

  for (digits_computed_so_far = 0; digits_computed_so_far < n_digits + 3;
       digits_computed_so_far += dig_per_u)
    {
      mp_limb_t cy;
      /* For speed: skip trailing zeroes.  */
      if (rp[0] == 0)
	{
	  rp++;
	  rsize--;
	  if (rsize == 0)
	    {
	      n_digits = digits_computed_so_far;
	      break;
	    }
	}

      cy = mpn_mul_1 (rp, rp, rsize, big_base);

      ASSERT_ALWAYS (! (digits_computed_so_far == 0 && cy == 0));

      /* Convert N1 from BIG_BASE to a string of digits in BASE
	 using single precision operations.  */
      {
	int i;
	unsigned char *s = tstr + digits_computed_so_far + dig_per_u;
	for (i = dig_per_u - 1; i >= 0; i--)
	  {
	    *--s = cy % base;
	    cy /= base;
	  }
      }
    }

  /* We can have at most two leading 0.  Remove them.  */
  if (tstr[0] == 0)
    {
      tstr++;
      digits_computed_so_far--;
      exp_in_base--;

      if (tstr[0] == 0)
	{
	  tstr++;
	  digits_computed_so_far--;
	  exp_in_base--;

	  if (tstr[0] == 0)
	    abort ();
	}
    }

  {
    size_t i;

    /* We should normally have computed too many digits.  Round the result
       at the point indicated by n_digits.  */
    if (digits_computed_so_far > n_digits)
      {
	/* Round the result.  */
	if (tstr[n_digits] * 2 >= base)
	  {
	    digits_computed_so_far = n_digits;
	    for (i = n_digits - 1;; i--)
	      {
		unsigned int x;
		x = ++(tstr[i]);
		if (x != base)
		  break;
		digits_computed_so_far--;
		if (i == 0)
		  {
		    /* We had something like `9999999...9d', where 2*d >= base.
		       This rounds up to `1', increasing the exponent.  */
		    tstr[0] = 1;
		    digits_computed_so_far = 1;
		    exp_in_base++;
		    break;
		  }
	      }
	  }
      }

    /* We might have fewer digits than requested as a result of rounding above,
       (i.e. 0.999999 => 1.0) or because we have a number that simply doesn't
       need many digits in this base (i.e., 0.125 in base 10).  */
    if (n_digits > digits_computed_so_far)
      n_digits = digits_computed_so_far;

    /* Remove trailing 0.  There can be many zeros.  */
    while (n_digits != 0 && tstr[n_digits - 1] == 0)
      n_digits--;

    /* Translate to ascii and null-terminate.  */
    for (i = 0; i < n_digits; i++)
      *str++ = num_to_text[tstr[i]];
  }
  *str = 0;
  *exp = exp_in_base;
  TMP_FREE (marker);
  return digit_ptr;
}
