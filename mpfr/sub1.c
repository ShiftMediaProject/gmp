/* mpfr_sub1 -- internal function to perform a "real" subtraction

Copyright 2001 Free Software Foundation.
Contributed by the Spaces project, INRIA Lorraine.

This file is part of the MPFR Library.

The MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

/* compute sign(b) * (|b| - |c|), with |b| > |c|, diff_exp = EXP(b) - EXP(c)
   Returns 0 iff result is exact,
   a negative value when the result is less than the exact value,
   a positive value otherwise.
*/

int
mpfr_sub1 (mpfr_ptr a, mpfr_srcptr b, mpfr_srcptr c, mp_rnd_t rnd_mode,
           int sub)
{
  int sign;
  mp_exp_unsigned_t diff_exp;
  mp_prec_t cancel, cancel1;
  mp_size_t cancel2, an, bn, cn, cn0;
  mp_limb_t *ap, *bp, *cp;
  mp_limb_t carry, bb, cc, borrow = 0;
  int inexact = 0, shift_b, shift_c, is_exact = 1, down = 0, add_exp = 0;
  int sh, k;
  TMP_DECL(marker);

  TMP_MARK(marker);
  ap = MPFR_MANT(a);
  an = 1 + (MPFR_PREC(a) - 1) / BITS_PER_MP_LIMB;

  sign = mpfr_cmp2 (b, c, &cancel);
  if (sign == 0)
    {
      if (rnd_mode == GMP_RNDD)
        MPFR_SET_NEG(a);
      else
        MPFR_SET_POS(a);
      MPFR_SET_ZERO(a);
      MPFR_RET(0);
    }

  /* If subtraction: sign(a) = sign * sign(b) */
  if (sub && MPFR_SIGN(a) != sign * MPFR_SIGN(b))
    MPFR_CHANGE_SIGN(a);

  if (sign < 0) /* swap b and c so that |b| > |c| */
    {
      mpfr_srcptr t;
      t = b; b = c; c = t;
    }

  /* If addition: sign(a) = sign of the larger argument in absolute value */
  if (!sub)
    MPFR_SET_SAME_SIGN(a, b);

  diff_exp = (mp_exp_unsigned_t) MPFR_EXP(b) - MPFR_EXP(c);

  /* reserve a space to store b aligned with the result, i.e. shifted by
     (-cancel) % BITS_PER_MP_LIMB to the right */
  bn = 1 + (MPFR_PREC(b) - 1) / BITS_PER_MP_LIMB;
  shift_b = cancel % BITS_PER_MP_LIMB;
  if (shift_b)
    shift_b = BITS_PER_MP_LIMB - shift_b;
  cancel1 = (cancel + shift_b) / BITS_PER_MP_LIMB;
  /* the high cancel1 limbs from b should not be taken into account */
  if (shift_b == 0)
    bp = MPFR_MANT(b); /* no need of an extra space */
  else
    {
      bp = TMP_ALLOC ((bn + 1) * BYTES_PER_MP_LIMB);
      bp[0] = mpn_rshift (bp + 1, MPFR_MANT(b), bn++, shift_b);
    }

  /* reserve a space to store c aligned with the result, i.e. shifted by
     (diff_exp-cancel) % BITS_PER_MP_LIMB to the right */
  cn = 1 + (MPFR_PREC(c) - 1) / BITS_PER_MP_LIMB;
  shift_c = diff_exp - (cancel % BITS_PER_MP_LIMB);
  shift_c = (shift_c + BITS_PER_MP_LIMB) % BITS_PER_MP_LIMB;
  if (shift_c == 0)
    cp = MPFR_MANT(c);
  else
    {
      cp = TMP_ALLOC ((cn + 1) * BYTES_PER_MP_LIMB);
      cp[0] = mpn_rshift (cp + 1, MPFR_MANT(c), cn++, shift_c);
    }

#ifdef DEBUG
  printf("shift_b=%u shift_c=%u\n", shift_b, shift_c);
#endif

  /* ensure ap != bp and ap != cp */
  if (ap == bp)
    {
      bp = (mp_ptr) TMP_ALLOC(bn * BYTES_PER_MP_LIMB);
      MPN_COPY (bp, ap, bn);
      /* ap == cp cannot occur since we would have b=c, which is detected
	 in mpfr_add or mpfr_sub */
    }
  else if (ap == cp)
    {
      cp = (mp_ptr) TMP_ALLOC (cn * BYTES_PER_MP_LIMB);
      MPN_COPY(cp, ap, cn);
    }
  
  /* here we have shift_c = (diff_exp - cancel) % BITS_PER_MP_LIMB,
     thus we want cancel2 = ceil((cancel - diff_exp) / BITS_PER_MP_LIMB) */

  cancel2 = (long int) (cancel - (diff_exp - shift_c)) / BITS_PER_MP_LIMB;
  /* the high cancel2 limbs from b should not be taken into account */
#ifdef DEBUG
  printf("cancel=%u cancel1=%u cancel2=%d\n", cancel, cancel1, cancel2);
#endif

  /*               ap[an-1]        ap[0]
             <----------------+-----------|---->
             <----------PREC(a)----------><-sh->
 cancel1
 limbs        bp[bn-cancel1-1]
 <--...-----><----------------+-----------+----------->
  cancel2
  limbs       cp[cn-cancel2-1]                                    cancel2 >= 0
    <--...--><----------------+----------------+---------------->
                (-cancel2)                                        cancel2 < 0
                   limbs      <----------------+---------------->
  */

  /* first part: put in ap[0..an-1] the value of high(b) - high(c),
     where high(b) consists of the high an+cancel1 limbs of b,
     and high(c) consists of the high an+cancel2 limbs of c.
   */

  /* copy high(b) into a */
  if (an + cancel1 <= bn) /* a: <----------------+-----------|---->
		         b: <-----------------------------------------> */
      MPN_COPY (ap, bp + bn - (an + cancel1), an);
  else  /* a: <----------------+-----------|---->
       b: <-------------------------> */
    if (cancel1 < bn) /* otherwise b does not overlap with a */
      {
	MPN_ZERO (ap, an + cancel1 - bn);
	MPN_COPY (ap + an + cancel1 - bn, bp, bn - cancel1);
      }
    else
      MPN_ZERO (ap, an);

#ifdef DEBUG
  printf("after copying high(b), a="); mpfr_print_binary(a); putchar('\n');
#endif

  /* subtract high(c) */
  if (an + cancel2 > 0) /* otherwise c does not overlap with a */
    {
      mp_limb_t *ap2;

      if (cancel2 >= 0)
	{
	  if (an + cancel2 <= cn) /* a: <----------------------------->
			      c: <-----------------------------------------> */
	    mpn_sub_n (ap, ap, cp + cn - (an + cancel2), an);
	  else /* a: <---------------------------->
	      c: <-------------------------> */
	    {
	      ap2 = ap + an + cancel2 - cn;
	      if (cn > cancel2)
		mpn_sub_n (ap2, ap2, cp, cn - cancel2);
	    }
	}
      else /* cancel2 < 0 */
	{
	  if (an + cancel2 <= cn) /* a: <----------------------------->
			                  c: <-----------------------------> */
	      borrow = mpn_sub_n (ap, ap, cp + cn - (an + cancel2), an + cancel2);
	  else /* a: <---------------------------->
	                c: <----------------> */
	    {
	      ap2 = ap + an + cancel2 - cn;
	      borrow = mpn_sub_n (ap2, ap2, cp, cn);
	    }
	  ap2 = ap + an + cancel2;
	  mpn_sub_1 (ap2, ap2, -cancel2, borrow);
	}
    }

#ifdef DEBUG
  printf("after subtracting high(c), a="); mpfr_print_binary(a); putchar('\n');
#endif

  /* now perform rounding */
  sh = an * BITS_PER_MP_LIMB - MPFR_PREC(a); /* last unused bits from a */
  carry = ap[0] & ((MP_LIMB_T_ONE << sh) - MP_LIMB_T_ONE);
  ap[0] -= carry;

  if (rnd_mode == GMP_RNDN)
    {
      if (sh)
	{
	  is_exact = (carry == 0);
	  /* can decide except when carry = 2^(sh-1) [middle]
	     or carry = 0 [truncate, but cannot decide inexact flag] */
	  down = (carry < (MP_LIMB_T_ONE << (sh - 1)));
	  if (carry > (MP_LIMB_T_ONE << (sh - 1)))
	    goto add_one_ulp;
	  else if ((0 < carry) && down)
	    {
	      inexact = -1; /* result if smaller than exact value */
	      goto truncate;
	    }
	}
    }
  else /* directed rounding: set rnd_mode to RNDZ iff towards zero */
    {
      if (((rnd_mode == GMP_RNDD) && (MPFR_SIGN(a) > 0)) ||
	  ((rnd_mode == GMP_RNDU) && (MPFR_SIGN(a) < 0)))
	rnd_mode = GMP_RNDZ;

      if (carry)
	{
	  if (rnd_mode == GMP_RNDZ)
	    {
	      inexact = -1;
	      goto truncate;
	    }
	  else /* round away */
	    goto add_one_ulp;
	}
    }

  /* we have to consider the low (bn - (an+cancel1)) limbs from b,
     and the (cn - (an+cancel2)) limbs from c. */
  bn -= an + cancel1;
  cn0 = cn;
  cn -= (long int) an + cancel2;
#ifdef DEBUG
  printf("last %d bits from a are %lu, bn=%ld, cn=%ld\n", sh, carry, bn, cn);
#endif

  for (k = 0; (bn > 0) || (cn > 0); k = 1)
    {
      bb = (bn > 0) ? bp[--bn] : 0;
      if ((cn > 0) && (cn-- <= cn0))
	cc = cp[cn];
      else
	cc = 0;

      if (down == 0)
	down = (bb < cc);

      if ((rnd_mode == GMP_RNDN) && !k && sh == 0)
	{
	  mp_limb_t half = GMP_LIMB_HIGHBIT;

	  is_exact = (bb == cc);

	  /* add one ulp if bb > cc + half
	     truncate if cc - half < bb < cc + half
	     sub one ulp if bb < cc - half
	  */

	  if (down)
	    {
	      if (cc >= half)
		cc -= half;
	      else
		bb += half;
	    }
	  else /* bb >= cc */
	    {
	      if (cc < half)
		cc += half;
	      else
		bb -= half;
	    }
	}

#ifdef DEBUG
      printf("    bb=%lu cc=%lu down=%d is_exact=%d\n", bb, cc, down, is_exact);
#endif
      if (bb < cc)
	{
	  if (rnd_mode == GMP_RNDZ)
	    goto sub_one_ulp;
	  else if (rnd_mode != GMP_RNDN) /* round away */
	    {
	      inexact = 1;
	      goto truncate;
	    }
	  else /* round to nearest */
	    {
	      if (is_exact && sh == 0)
		{
		  inexact = 0;
		  goto truncate;
		}
	      else if (down && sh == 0)
		goto sub_one_ulp;
	      else
		{
		  inexact = (is_exact) ? 1 : -1;
		  goto truncate;
		}
	    }
	}
      else if (bb > cc)
	{
	  if (rnd_mode == GMP_RNDZ)
	    {
	      inexact = -1;
	      goto truncate;
	    }
	  else if (rnd_mode != GMP_RNDN) /* round away */
	      goto add_one_ulp;
	  else /* round to nearest */
	    {
	      if (is_exact)
		{
		  inexact = -1;
		  goto truncate;
		}
	      else if (down)
		{
		  inexact = 1;
		  goto truncate;
		}
	      else
		goto add_one_ulp;
	    }
	}
    }

  if ((rnd_mode == GMP_RNDN) && !is_exact)
    {
      /* even rounding rule */
      if ((ap[0] >> sh) & 1)
	{
	  if (down) goto sub_one_ulp;
	  else goto add_one_ulp;
	}
      else
	inexact = (down) ? 1 : -1;
    }
  else
    inexact = 0;
  goto truncate;
  
 sub_one_ulp: /* add one unit in last place to a */
  mpn_sub_1 (ap, ap, an, MP_LIMB_T_ONE << sh);
  inexact = -1;
  goto end_of_sub;

 add_one_ulp: /* add one unit in last place to a */
  if (mpn_add_1 (ap, ap, an, MP_LIMB_T_ONE << sh)) /* result is a power of 2 */
    {
      ap[an-1] = GMP_LIMB_HIGHBIT;
      add_exp = 1;
    }
  inexact = 1; /* result larger than exact value */

 truncate:
  if ((ap[an-1] >> (BITS_PER_MP_LIMB - 1)) == 0) /* case 1 - epsilon */
    {
      ap[an-1] = GMP_LIMB_HIGHBIT;
      add_exp = 1;
    }

 end_of_sub:
  /* we have to set MPFR_EXP(a) to MPFR_EXP(b) - cancel + add_exp, taking
     care of underflows/overflows in that computation, and of the allowed
     exponent range */
  if (cancel)
    {
      mp_exp_t exp_b;

      cancel -= add_exp; /* still valid as unsigned long */
      exp_b = MPFR_EXP(b); /* save it in case a equals b */
      MPFR_EXP(a) = MPFR_EXP(b) - cancel;
      if ((MPFR_EXP(a) > exp_b) /* underflow in type mp_exp_t */
	  || (MPFR_EXP(a) < __mpfr_emin))
	{
	  TMP_FREE(marker); 
	  return mpfr_set_underflow (a, rnd_mode, MPFR_SIGN(a));
	}
    }
  else /* cancel = 0: MPFR_EXP(a) <- MPFR_EXP(b) + add_exp */
    {
      /* in case cancel = 0, add_exp can still be 1, in case b is just
	 below a power of two, c is very small, prec(a) < prec(b),
	 and rnd=away or nearest */
      if (add_exp && MPFR_EXP(b) == __mpfr_emax)
	{
	  TMP_FREE(marker);
	  return mpfr_set_overflow (a, rnd_mode, MPFR_SIGN(a));
	}
      MPFR_EXP(a) = MPFR_EXP(b) + add_exp;
    }
  TMP_FREE(marker);
#ifdef DEBUG
  printf ("result is a="); mpfr_print_binary(a); putchar('\n');
#endif
  /* check that result is msb-normalized */
  MPFR_ASSERTN(ap[an-1] > ~ap[an-1]);
  return inexact * MPFR_SIGN(a);
}
