/* Naive implementation of nth root extraction.  It would probably be a
   better idea to use a division-free Newton iteration.  It is insane
   to use full precision from iteration 1.  The mpz_scan1 trick compensates
   to some extent, but is nothing I am proud of.  */

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"

int
mpz_root (mpz_ptr r, mpz_srcptr c, unsigned long int nth)
{
  mpz_t x, t0, t1, t2, t3;
  unsigned long int nbits;
  int exact;
  int i;
  unsigned long int lowz;
#if DEBUG
  int itercnt;
#endif

  mpz_init (x);
  mpz_init (t0);
  mpz_init (t1);
  mpz_init (t2);
  mpz_init (t3);

  nbits = mpz_sizeinbase (c, 2);
  mpz_set_ui (x, 1);
  nbits = (nbits - 1) / nth;
  mpz_mul_2exp (x, x, nbits);

  mpz_pow_ui (t1, x, nth);
  if (mpz_cmp (c, t1) < 0)
    abort ();
  mpz_mul_2exp (t2, x, 1);
  mpz_pow_ui (t1, t2, nth);
  if (mpz_cmp (c, t1) >= 0)
    abort ();

  /* Make the approximation better.  */
  for (i = 1; (nth >> i) != 0; i++)
    {
      if (nbits < i)
	break;

      mpz_setbit (x, nbits - i);
      mpz_tdiv_q_2exp (t0, x, nbits - i);
      mpz_pow_ui (t1, t0, nth);
      mpz_mul_2exp (t1, t1, (nbits - i) * nth);
      if (mpz_cmp (c, t1) < 0)
	mpz_clrbit (x, nbits - i);
    }
  if (nbits >= i)
    mpz_setbit (x, nbits - i);

#if DEBUG
  itercnt = 0;
#endif
  do
    {
#if DEBUG
      itercnt++;
#endif
      lowz = mpz_scan1 (x, 0);
      mpz_tdiv_q_2exp (t0, x, lowz);
      mpz_pow_ui (t1, t0, nth - 1);
      mpz_mul_2exp (t1, t1, lowz * (nth - 1));
      mpz_tdiv_q (t2, c, t1);
      mpz_sub (t2, x, t2);
      mpz_tdiv_q_ui (t3, t2, nth);
      mpz_sub (x, x, t3);
    }
  while (mpz_sgn (t3) != 0);

#if DEBUG
  {
    static char *ext[] = {"th","st","nd","rd","th","th","th","th","th","th"};
    printf ("Computed %lu%s root of a %ld limb number in %d iterations\n",
	    nth, ext[(nth - 10) % 100 < 10 ? 0 : nth % 10],
	    (long) mpz_size (c), itercnt);
  }
#endif

  lowz = mpz_scan1 (x, 0);
  mpz_tdiv_q_2exp (t0, x, lowz);
  mpz_pow_ui (t1, t0, nth);
  mpz_mul_2exp (t1, t1, lowz * nth);
  if (mpz_cmp (c, t1) < 0)
    {
      mpz_sub_ui (x, x, 1);
      lowz = mpz_scan1 (x, 0);
      mpz_tdiv_q_2exp (t0, x, lowz);
      mpz_pow_ui (t1, t0, nth);
      mpz_mul_2exp (t1, t1, lowz * nth);
    }

  exact = mpz_cmp (t1, c) == 0;

  if (r != NULL)
    mpz_set (r, x);

  mpz_clear (t3);
  mpz_clear (t2);
  mpz_clear (t1);
  mpz_clear (t0);
  mpz_clear (x);

  return exact;
}
