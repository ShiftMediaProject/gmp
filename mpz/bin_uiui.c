#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"

void
mpz_bin_uiui (mpz_ptr r, unsigned long int n, unsigned long int k)
{
  if (n < k)
    /* This special case is a good idea since the unsigned variable n would
       else become negative.  It is not an exception that is mathematically
       necessary.  */
    mpz_set_ui (r, 0);
  else
    {
      unsigned long int i, j;
      mp_limb_t nacc, kacc;
      unsigned long int i2, j2;
      int i2c, j2c;
      unsigned long int cnt;

      /* Rewrite bin(n,k) as bin(n,n-k) if that is simpler to compute.  */
      if (k > n / 2)
	k = n - k;

      j = n - k + 1;
      mpz_set_ui (r, j);

      /* Initialize accumulators.  */
      nacc = 1;
      kacc = 1;

      cnt = 0;
      for (i = 2; i <= k; i++)
	{
	  mp_limb_t n1, n0, k1, k0;

	  j++;
#if 0
	  /* Remove common multiples of 2.  This will allow us to accumulate
	     more in nacc and kacc before we need a bignum step.  It would make
	     sense to cancel factors of 3, 5, etc too, but this would be best
	     handled by sieving out factors.  Alternatively, we could perform a
	     gcd of the accumulators just as they have overflown, and keep
	     accumulating until the gcd doesn't remove a significant factor.  */
	  while (((nacc | kacc) & 1) == 0)
	    {
	      nacc >>= 1;
	      kacc >>= 1;
	    }
#else
	  cnt = ((nacc | kacc) & 1) ^ 1;
	  nacc >>= cnt;
	  kacc >>= cnt;
#endif
	  /* Accumulate next multiples.  */
	  umul_ppmm (n1, n0, nacc, j);
	  umul_ppmm (k1, k0, kacc, i);
	  if (n1 != 0)
	    {
	      /* Accumulator overflow.  Perform bignum step.  */
	      mpz_mul_ui (r, r, nacc);
	      nacc = j;
	      mpz_tdiv_q_ui (r, r, kacc);
	      kacc = i;
	    }
	  else
	    {
	      if (k1 != 0) abort ();
	      /* Save new products in accumulators to keep accumulating.  */
	      nacc = n0;
	      kacc = k0;
	    }
	}

      /* Take care of whatever is left in accumulators.  */
      mpz_mul_ui (r, r, nacc);
      mpz_tdiv_q_ui (r, r, kacc);
    }
}
