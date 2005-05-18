/* mpn_mullow_n -- multiply two n-limb nunbers and return the low n limbs
   of their products.

   THIS IS (FOR NOW) AN INTERNAL FUNCTION.  IT IS ONLY SAFE TO REACH THIS
   FUNCTION THROUGH DOCUMENTED INTERFACES.  IN FACT, IT IS ALMOST GUARANTEED
   THAT IT'LL CHANGE OR DISAPPEAR IN A FUTURE GNU MP RELEASE.

Copyright 2004 Free Software Foundation, Inc.

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


#ifndef MULLOW_BASECASE_THRESHOLD
#define MULLOW_BASECASE_THRESHOLD 0	/* never use mpn_mul_basecase */
#endif

#ifndef MULLOW_DC_THRESHOLD
#define MULLOW_DC_THRESHOLD 3*MUL_KARATSUBA_THRESHOLD
#endif

#ifndef MULLOW_MUL_N_THRESHOLD
#define MULLOW_MUL_N_THRESHOLD 10*MULLOW_DC_THRESHOLD
#endif


/*
  FIXME: This function should accept a temporary area.
  FIXME: Perhaps call mpn_kara_mul_n instead of mpn_mul_n?
  FIXME: Call mpn_mul_basecase for really small operands, it might be faster
         than mpn_mullow_basecase when the former is in assembly, or when we
         have mpn_admul_2.
  THINK: If mpn_mul_basecase is always faster than mpn_mullow_basecase
         (typically thanks for mpn_addmul_2) should we unconditionally use
         mpn_mul_n?
  FIXME: The recursive calls to mpn_mullow_n use sizes n/2 (one uses floor(n/2)
         and the other ceil(n/2)).  Depending on the values of the various
         _THRESHOLDs, this may never trigger MULLOW_BASECASE_THRESHOLD.
	 Should we worry abuot this overhead?
*/

void
mpn_mullow_n (mp_ptr rp, mp_srcptr xp, mp_srcptr yp, mp_size_t n)
{
  if (BELOW_THRESHOLD (n, MULLOW_BASECASE_THRESHOLD))
    {
      mp_ptr tp;
      TMP_DECL;
      TMP_MARK;
      tp = TMP_ALLOC_LIMBS (2 * n);

      mpn_mul_basecase (tp, xp, n, yp, n);
      MPN_COPY (rp, tp, n);
      TMP_FREE;
    }
  else if (BELOW_THRESHOLD (n, MULLOW_DC_THRESHOLD))
    {
      mpn_mullow_basecase (rp, xp, yp, n);
    }
  else if (BELOW_THRESHOLD (n, MULLOW_MUL_N_THRESHOLD))
    {
      /* Divide-and-conquer */
      mp_size_t n1 = (n+1) >> 1;	/* ceil(n/2) */
      mp_size_t n2 = n - n1;		/* floor(n/2) */
      mp_ptr tp;
      TMP_DECL;
      TMP_MARK;
      tp = TMP_ALLOC_LIMBS (n1);

      /* Split as x = x1 2^(n1 GMP_NUMB_BITS) + x0,
                  y = y1 2^(n2 GMP_NUMB_BITS) + y0 */

      /* x0 * y0 */
      mpn_mul_n (rp, xp, yp, n2);
      if (n1 != n2)
	rp[2 * n2] = mpn_addmul_1 (rp + n2, yp, n2, xp[n2]);

      /* x1 * y0 * 2^(n1 GMP_NUMB_BITS) */
      mpn_mullow_n (tp, xp + n1, yp, n2);
      mpn_add_n (rp + n1, rp + n1, tp, n2);

      /* x0 * y1 * 2^(n2 GMP_NUMB_BITS) */
      mpn_mullow_n (tp, yp + n2, xp, n1);
      mpn_add_n (rp + n2, rp + n2, tp, n1);
      TMP_FREE;
    }
  else
    {
      /* For really large operands, use plain mpn_mul_n but through away upper
	 n limbs of result.  */
      mp_ptr tp;
      TMP_DECL;
      TMP_MARK;
      tp = TMP_ALLOC_LIMBS (2 * n);

      mpn_mul_n (tp, xp, yp, n);
      MPN_COPY (rp, tp, n);
      TMP_FREE;
    }
}
