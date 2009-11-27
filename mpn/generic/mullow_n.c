/* mpn_mullow_n -- multiply two n-limb numbers and return the low n limbs
   of their products.

   THIS IS (FOR NOW) AN INTERNAL FUNCTION.  IT IS ONLY SAFE TO REACH THIS
   FUNCTION THROUGH DOCUMENTED INTERFACES.  IN FACT, IT IS ALMOST GUARANTEED
   THAT IT'LL CHANGE OR DISAPPEAR IN A FUTURE GNU MP RELEASE.

Copyright 2004, 2005 Free Software Foundation, Inc.

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


#ifndef MULLOW_BASECASE_THRESHOLD
#define MULLOW_BASECASE_THRESHOLD 0	/* never use mpn_mul_basecase */
#endif

#ifndef MULLOW_DC_THRESHOLD
#define MULLOW_DC_THRESHOLD 3*MUL_TOOM22_THRESHOLD
#endif

#ifndef MULLOW_MUL_N_THRESHOLD
#define MULLOW_MUL_N_THRESHOLD 10*MULLOW_DC_THRESHOLD
#endif

#if TUNE_PROGRAM_BUILD
#define MAYBE_range_basecase 1
#define MAYBE_range_toom22   1
#else
#define MAYBE_range_basecase                                           \
  ((MULLOW_DC_THRESHOLD == 0 ? MULLOW_BASECASE_THRESHOLD : MULLOW_DC_THRESHOLD) < 2 * MUL_TOOM22_THRESHOLD)
#define MAYBE_range_toom22                                             \
  ((MULLOW_DC_THRESHOLD == 0 ? MULLOW_BASECASE_THRESHOLD : MULLOW_DC_THRESHOLD) < MUL_TOOM33_THRESHOLD*36/(36-11) )
#endif

/* Avoid zero allocations when MULLOW_BASECASE_THRESHOLD is 0.  */
#define MUL_BASECASE_ALLOC \
 (MULLOW_BASECASE_THRESHOLD_LIMIT == 0 ? 1 : 2*MULLOW_BASECASE_THRESHOLD_LIMIT)

/*
  FIXME: This function should accept a temporary area.
  THINK: If mpn_mul_basecase is always faster than mpn_mullow_basecase
         (typically thanks to mpn_addmul_2) should we unconditionally use
         mpn_mul_n?
  THINK: The DC strategy uses different constatnts in different Toom's
	 ranges. Something smoother?
*/

/*
  Compute the least significant half of the product {xy,n}*{yp,n}, or
  formally {rp,n} = {xy,n}*{yp,n} Mod (B^n).

  Above the given threshold, the Divide and Conquer strategy is used.
  The operands are split in two, and a full product plus two mullow
  are used to obtain the final result. The more natural stategy is to
  split in two halves, but this is far from optimal when a
  sebquadratic multiplication is used.

  Mulders suggests an unbalanced split in favour of the full product,
  split n = n1 + n2, where an = n1 <= n2 = (1-a)n; i.e. 0 < a <= 1/2.

  To compute the value of a, we assume that the cost of mullow for a
  given size ML(n) is a fraction of the cost of a full product with
  same size M(n), and the cost M(n)=n^e for some exponent 1 < e <= 2;
  then we can write:

  ML(n) = 2*ML(an) + M((1-a)n) => k*M(n) = 2*k*M(n)*a^e + M(n)*(1-a)^e

  Given a value for e, want to minimise the value of k, i.e. the
  function k=(1-a)^e/(1-2*a^e).

  With e=2, the exponent for schoolbook multiplication, the minimum is
  given by the values a=1-a=1/2.

  With e=log(3)/log(2), the exponent for Karatsuba (aka toom22),
  Mulders compute (1-a) = 0.694... and we approximate a with 11/36.

  Other possible approximations follow:
  e=log(5)/log(3) [Toom-3] -> a ~= 9/40
  e=log(7)/log(4) [Toom-4] -> a ~= 7/39
  e=log(11)/log(6) [Toom-6] -> a ~= 1/8
  e=log(15)/log(8) [Toom-8] -> a ~= 1/10

  The values above where obtained with the following trivial commands
  in the gp-pari shell:

fun(e,a)=(1-a)^e/(1-2*a^e)
mul(a,b,c)={local(m,x,p);if(b-c<1/10000,(b+c)/2,m=1;x=b;forstep(p=c,b,(b-c)/8,if(fun(a,p)<m,m=fun(a,p);x=p));mul(a,(b+x)/2,(c+x)/2))}
contfracpnqn(contfrac(mul(log(2*2-1)/log(2),1/2,0),5))
contfracpnqn(contfrac(mul(log(3*2-1)/log(3),1/2,0),5))
contfracpnqn(contfrac(mul(log(4*2-1)/log(4),1/2,0),5))
contfracpnqn(contfrac(mul(log(6*2-1)/log(6),1/2,0),3))
contfracpnqn(contfrac(mul(log(8*2-1)/log(8),1/2,0),3))

  ,
  |\
  | \
  +----,
  |    |
  |    |
  |    |\
  |    | \
  +----+--`
  ^ n2 ^n1^

  For an actual impementation, the assumption that M(n)=n^e is
  incorrect, as a consequence also the assumption that ML(n)=k*M(n)
  with a constant k is wrong.

  But theory suggest us two things:
  - the best the multiplication product is (lower e), the more k
    approaches 1, and a approaches 0.

  - A value for a smaller than optimal is probably less bad than a
    bigger one: e.g. let e=log(3)/log(2), a=0.3058_ the optimal
    value, and k(a)=0.808_ the mul/mullow speed ratio. We get
    k(a+1/6)=0.929_ but k(a-1/6)=0.865_.
*/

void
mpn_mullow_n (mp_ptr rp, mp_srcptr xp, mp_srcptr yp, mp_size_t n)
{
  ASSERT (n >= 1);
  ASSERT (! MPN_OVERLAP_P (rp, n, xp, n));
  ASSERT (! MPN_OVERLAP_P (rp, n, yp, n));

  if (BELOW_THRESHOLD (n, MULLOW_BASECASE_THRESHOLD))
    {
      /* Allocate workspace of fixed size on stack: fast! */
      mp_limb_t ws[MUL_BASECASE_ALLOC];
      mpn_mul_basecase (ws, xp, n, yp, n);
      MPN_COPY (rp, ws, n);
    }
  else if (BELOW_THRESHOLD (n, MULLOW_DC_THRESHOLD))
    {
      mpn_mullow_basecase (rp, xp, yp, n);
    }
  else if (BELOW_THRESHOLD (n, MULLOW_MUL_N_THRESHOLD))
    {
      /* Divide-and-conquer */
      mp_size_t n2, n1;
      mp_ptr tp;
      TMP_SDECL;
      TMP_SMARK;
      /* We need fractional approximation of the value 1/2< b <1
	 giving the minimum in the function b^a/(1-2*(1-b)^a) .

	 Constants obtained with the following gp-pari commands:

       */
      if (MAYBE_range_basecase && BELOW_THRESHOLD (n, MUL_TOOM22_THRESHOLD*2) )
	n1 = n >> 1;
      else if (MAYBE_range_toom22 && BELOW_THRESHOLD (n, MUL_TOOM33_THRESHOLD*36/(36-11)))
	n1 = n * 11 / (size_t) 36;	/* n1 ~= n*(1-.694...) */
      else if (BELOW_THRESHOLD (n, MUL_TOOM44_THRESHOLD*40/(40-9)))
	n1 = n * 9 / (size_t) 40;	/* n1 ~= n*(1-.775...) */
      else
	n1 = n * 7 / (size_t) 39;	/* n1 ~= n*(1-.821...) */
      /* n1 = n * 4 / (size_t) 31;	// n1 ~= n*(1-.871...) [TOOM66] */
      /* n1 = n / (size_t) 10;		// n1 ~= n*(1-.899...) [TOOM88] */
      n2 = n - n1;
      ASSERT (n1 < 2 * n2);
      tp = TMP_SALLOC_LIMBS (2 * n2);

      /* Split as x = x1 2^(n2 GMP_NUMB_BITS) + x0,
                  y = y1 2^(n2 GMP_NUMB_BITS) + y0 */

      /* x0 * y0 */
      mpn_mul_n (tp, xp, yp, n2);
      MPN_COPY (rp, tp, n2);

      /* x1 * y0 * 2^(n2 GMP_NUMB_BITS) */
      mpn_mullow_n (rp + n2, xp + n2, yp, n1);
      mpn_add_n (rp + n2, rp + n2, tp + n2, n1);

      /* x0 * y1 * 2^(n2 GMP_NUMB_BITS) */
      mpn_mullow_n (tp, yp + n2, xp, n1);
      mpn_add_n (rp + n2, rp + n2, tp, n1);
      TMP_SFREE;
    }
  else
    {
      /* For really large operands, use plain mpn_mul_n but throw away upper n
	 limbs of result.  */
      mp_ptr tp;
      TMP_DECL;
      TMP_MARK;
      tp = TMP_ALLOC_LIMBS (2 * n);

#if !TUNE_PROGRAM_BUILD && WANT_FFT && (MULLOW_MUL_N_THRESHOLD >= MUL_FFT_THRESHOLD)
      mpn_mul_fft_full (tp, xp, n, yp, n);
#else
      mpn_mul_n (tp, xp, yp, n);
#endif
      MPN_COPY (rp, tp, n);
      TMP_FREE;
    }
}
