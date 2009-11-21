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

fun(a,b)=b^a/(1-2*(1-b)^a)
mul(a,b,c)={local(m,x,p);if(b-c<1/10000,(b+c)/2,m=1;x=b;forstep(p=c,b,(b-c)/8,if(fun(a,p)<m,m=fun(a,p);x=p));mul(a,(b+x)/2,(c+x)/2))}
contfracpnqn(contfrac(1-mul(log(2*2-1)/log(2),1,1/2),5))
contfracpnqn(contfrac(1-mul(log(3*2-1)/log(3),1,1/2),5))
contfracpnqn(contfrac(1-mul(log(4*2-1)/log(4),1,1/2),5))
       */
      if (BELOW_THRESHOLD (n, MUL_TOOM33_THRESHOLD*36/(36-11)))
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
    { /* FIXME: directly call mpn_mul_fft_full */
      /* For really large operands, use plain mpn_mul_n but throw away upper n
	 limbs of result.  */
      mp_ptr tp;
      TMP_DECL;
      TMP_MARK;
      tp = TMP_ALLOC_LIMBS (2 * n);

      mpn_mul_n (tp, xp, yp, n);
      MPN_COPY (rp, tp, n);
      TMP_FREE;
    }
}
