#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"

#define TWO64 18446744073709551616.0

mp_limb_t
__udiv_qrnnd64 (mp_limb_t n1, mp_limb_t n0, mp_limb_t d, mp_limb_t *r)
{
  mp_limb_t q1, q2, q;
  mp_limb_t p1, p0;
  double di, dq;

  di = 1.0 / d;

  /* Generate upper 53 bits of quotient.  Be careful here; the `double'
     quotient may be rounded to 2^64 which we cannot safely convert back
     to a 64-bit integer.  */
  dq = (TWO64 * (double) n1 + (double) n0) * di;
  if (dq >= TWO64)
    q1 = 0xfffffffffffff800LL;
  else
    q1 = (mp_limb_t) dq;

  /* Multiply back in order to compare the product to the dividend.  */
  umul_ppmm (p1, p0, q1, d);

  /* Was the 53-bit quotient greater that our sought quotient?  Test the
     sign of the partial remainder to find out.  */
  if (n1 < p1 || (n1 == p1 && n0 < p0))
    {
      /* 53-bit quotient too large.  Partial remainder is negative.
	 Compute the absolute value of the remainder in n1,,n0.  */
      n1 = p1 - (n1 + (p0 < n0));
      n0 = p0 - n0;

      /* Now use the partial remainder as new dividend to compute more bits of
	 quotient.  This is an adjustment for the one we got previously.  */
      q2 = (mp_limb_t) ((TWO64 * (double) n1 + (double) n0) * di);
      umul_ppmm (p1, p0, q2, d);

      q = q1 - q2;
      if (n1 < p1 || (n1 == p1 && n0 <= p0))
	{
	  n0 = p0 - n0;
	}
      else
	{
	  n0 = p0 - n0;
	  n0 += d;
	  q--;
	}
    }
  else
    {
      n1 = n1 - (p1 + (n0 < p0));
      n0 = n0 - p0;

      q2 = (mp_limb_t) ((TWO64 * (double) n1 + (double) n0) * di);
      umul_ppmm (p1, p0, q2, d);

      q = q1 + q2;
      if (n1 < p1 || (n1 == p1 && n0 < p0))
	{
	  n0 = n0 - p0;
	  n0 += d;
	  q--;
	}
      else
	{
	  n0 = n0 - p0;
	  if (n0 >= d)
	    {
	      n0 -= d;
	      q++;
	    }
	}
    }

  *r = n0;
  return q;
}
