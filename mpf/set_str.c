/* mpf_set_str (dest, string, base) -- Convert the string STRING
   in base BASE to a float in dest.  If BASE is zero, the leading characters
   of STRING is used to figure out the base.

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

#include <string.h>
#include <ctype.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"


long int strtol _PROTO ((const char *, char **ptr, int));

static int
#if __STDC__
digit_value_in_base (int c, int base)
#else
digit_value_in_base (c, base)
     int c;
     int base;
#endif
{
  int digit;

  if (isdigit (c))
    digit = c - '0';
  else if (islower (c))
    digit = c - 'a' + 10;
  else if (isupper (c))
    digit = c - 'A' + 10;
  else
    return -1;

  if (digit < base)
    return digit;
  return -1;
}

int
#if __STDC__
mpf_set_str (mpf_ptr x, const char *str, int base)
#else
mpf_set_str (x, str, base)
     mpf_ptr x;
     char *str;
     int base;
#endif
{
  size_t str_size;
  char *s, *begs;
  size_t i;
  mp_size_t xsize;
  int c;
  int negative;
  char *dotpos = 0;
  int expflag;
  int decimal_exponent_flag;
  TMP_DECL (marker);

  TMP_MARK (marker);

  c = *str;

  /* Skip whitespace.  */
  while (isspace (c))
    c = *++str;

  negative = 0;
  if (c == '-')
    {
      negative = 1;
      c = *++str;
    }

  decimal_exponent_flag = base < 0;
  base = ABS (base);

  if (digit_value_in_base (c, base == 0 ? 10 : base) < 0
      && (c != '.' || digit_value_in_base (str[1], base == 0 ? 10 : base) < 0))
    return -1;			/* error if no digits */

  /* If BASE is 0, try to find out the base by looking at the initial
     characters.  */
  if (base == 0)
    {
      base = 10;
#if 0
      if (c == '0')
	{
	  base = 8;
	  if (str[1] == 'x' || str[1] == 'X')
	    {
	      base = 16;
	      str += 2;
	      c = *str;
	    }
	}
#endif
    }

  expflag = 0;
  str_size = strlen (str);
  for (i = 0; i < str_size; i++)
    {
      c = str[i];
      if (c == '@' || (base <= 10 && (c == 'e' || c == 'E')))
	{
	  expflag = 1;
	  str_size = i;
	  break;
	}

    }

  s = begs = (char *) TMP_ALLOC (str_size + 1);

  for (i = 0; i < str_size; i++)
    {
      c = *str;
      if (!isspace (c))
	{
	  int dig;

	  if (c == '.')
	    {
	      if (dotpos != 0)
		{
		  TMP_FREE (marker);
		  return -1;
		}
	      dotpos = s;
	    }
	  else
	    {
	      dig = digit_value_in_base (c, base);
	      if (dig < 0)
		{
		  TMP_FREE (marker);
		  return -1;
		}
	      *s++ = dig;
	    }
	}
      c = *++str;
    }

  str_size = s - begs;

  xsize = str_size / __mp_bases[base].chars_per_limb + 2;
  {
    long exp_in_base;
    mp_size_t ralloc, rsize, msize;
    int cnt, i;
    mp_ptr mp, tp, rp;
    mp_exp_t exp_in_limbs;
    mp_size_t prec = x->_mp_prec + 1;
    int divflag;
    mp_size_t madj, radj;

    mp = (mp_ptr) TMP_ALLOC (xsize * BYTES_PER_MP_LIMB);
    msize = mpn_set_str (mp, (unsigned char *) begs, str_size, base);

    if (msize == 0)
      {
	x->_mp_size = 0;
	x->_mp_exp = 0;
	TMP_FREE (marker);
	return 0;
      }

    madj = 0;
    /* Ignore excess limbs in MP,MSIZE.  */
    if (msize > prec)
      {
	madj = msize - prec;
	mp += msize - prec;
	msize = prec;
      }

    if (expflag != 0)
      exp_in_base = strtol (str + 1, (char **) 0,
			    decimal_exponent_flag ? 10 : base);
    else
      exp_in_base = 0;
    if (dotpos != 0)
      exp_in_base -= s - dotpos;
    divflag = exp_in_base < 0;
    exp_in_base = ABS (exp_in_base);

    if (exp_in_base == 0)
      {
	MPN_COPY (x->_mp_d, mp, msize);
	x->_mp_size = negative ? -msize : msize;
	x->_mp_exp = msize + madj;
	TMP_FREE (marker);
	return 0;
      }

    radj = 0;			/* counts number of ignored low limbs in r */

    ralloc = 2 * (prec + 1);
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
	radj <<= 1;

	if (rsize > prec)
	  {
	    radj += rsize - prec;
	    MPN_COPY_INCR (rp, tp + rsize - prec, prec);
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

    if (rsize > prec)
      {
	radj += rsize - prec;
	rp += rsize - prec;
	rsize = prec;
      }

    if (divflag)
      {
	mp_ptr qp;
	mp_limb_t qlimb;
	if (msize < rsize)
	  {
	    /* Pad out MP,MSIZE for current divrem semantics.  */
	    mp_ptr tmp = (mp_ptr) TMP_ALLOC ((rsize + 1) * BYTES_PER_MP_LIMB);
	    MPN_ZERO (tmp, rsize - msize);
	    MPN_COPY (tmp + rsize - msize, mp, msize);
	    mp = tmp;
	    madj -= rsize - msize;
	    msize = rsize;
	  }
	count_leading_zeros (cnt, rp[rsize - 1]);
	if (cnt != 0)
	  {
	    mp_limb_t cy;
	    mpn_lshift (rp, rp, rsize, cnt);
	    cy = mpn_lshift (mp, mp, msize, cnt);
	    if (cy)
	      mp[msize++] = cy;
	  }

	qp = (mp_ptr) TMP_ALLOC ((prec + 1) * BYTES_PER_MP_LIMB);
	qlimb = mpn_divrem (qp, prec - (msize - rsize), mp, msize, rp, rsize);
	tp = qp;
	exp_in_limbs = qlimb + (msize - rsize) + (madj - radj);
	rsize = prec;
	if (qlimb != 0)
	  {
	    tp[prec] = qlimb;
	    /* Skip the least significant limb not to overrun the destination
	       variable.  */
	    tp++;
	  }
      }
    else
      {
	tp = (mp_ptr) TMP_ALLOC ((rsize + msize) * BYTES_PER_MP_LIMB);
	if (rsize > msize)
	  mpn_mul (tp, rp, rsize, mp, msize);
	else
	  mpn_mul (tp, mp, msize, rp, rsize);
	rsize += msize;
	rsize -= tp[rsize - 1] == 0;
	exp_in_limbs = rsize + madj + radj;

	if (rsize > prec)
	  {
	    tp += rsize - prec;
	    rsize = prec;
	    exp_in_limbs += 0;
	  }
      }

    MPN_COPY (x->_mp_d, tp, rsize);
    x->_mp_size = negative ? -rsize : rsize;
    x->_mp_exp = exp_in_limbs;
    TMP_FREE (marker);
    return 0;
  }
}
