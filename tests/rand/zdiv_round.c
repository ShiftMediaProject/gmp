/* zdiv_round() -- divide integers, round to nearest */

#include "gmp.h"

void
zdiv_round (mpz_t rop, mpz_t n, mpz_t d)
{
  mpf_t f_n, f_d;

  mpf_init (f_n);
  mpf_init (f_d);

  mpf_set_z (f_d, d);
  mpf_set_z (f_n, n);

  mpf_div (f_n, f_n, f_d);
  mpf_set_d (f_d, .5);
  if (mpf_sgn (f_n) < 0)
    mpf_neg (f_d, f_d);
  mpf_add (f_n, f_n, f_d);
  mpz_set_f (rop, f_n);

  mpf_clear (f_n);
  mpf_clear (f_d);
  return;
}
