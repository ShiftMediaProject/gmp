#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"

/*
tege> Vilken är definitionen för negativa n?
teke> Den rekursiva definitionen funkar där också dvs
teke> 
teke>   k < 0   => bin(n,k) = 0
teke> 
teke>   k == 0  => bin(n,k) = 1
teke> 
teke>   1       => bin(n,k) = bin(n-1,k-1)*n/k
(teke=Torsten Ekedahl)
*/

/* This is a poor implementation.  Look at bin_uiui.c for improvement ideas.  */

void
#if __STDC__
mpz_bin_ui (mpz_ptr r, mpz_srcptr n, unsigned long int k)
#else
mpz_bin_ui (r, n, k)
     mpz_ptr r;
     mpz_srcptr n;
     unsigned long int k;
#endif
{
  mpz_t ni;
  unsigned long int i;
  unsigned long int kacc;
  mpz_t nacc;

  mpz_init (ni);
  mpz_sub_ui (ni, n, k);

  /* Rewrite bin(n,k) as bin(n,n-k) if that is simpler to compute.  */
  if (mpz_cmp_ui (ni, k) < 0)
    {
      k = mpz_get_ui (ni);
      mpz_sub_ui (ni, n, k);
    }

  mpz_set_ui (r, 1);

  kacc = 1;
  mpz_init_set_ui (nacc, 1);

  for (i = 1; i <= k; i++)
    {
      mp_limb_t k1, k0;

#if 0
      mp_limb_t nacclow;
      int c;

      nacclow = PTR(nacc)[0];
      for (c = 0; (((kacc | nacclow) & 1) == 0); c++)
	{
	  kacc >>= 1;
	  nacclow >>= 1;
	}
      mpz_div_2exp (nacc, nacc, c);
#endif

      mpz_add_ui (ni, ni, 1);
      mpz_mul (nacc, nacc, ni);
      umul_ppmm (k1, k0, kacc, i);
      if (k1 != 0)
	{
	  /* Accumulator overflow.  Perform bignum step.  */
	  mpz_mul (r, r, nacc);
	  mpz_set_ui (nacc, 1);
	  mpz_tdiv_q_ui (r, r, kacc);
	  kacc = i;
	}
      else
	{
	  /* Save new products in accumulators to keep accumulating.  */
	  kacc = k0;
	}
    }

  mpz_mul (r, r, nacc);
  mpz_set_ui (nacc, 1);
  mpz_tdiv_q_ui (r, r, kacc);

  mpz_clear (nacc);
  mpz_clear (ni);
}
