#include <stdio.h>
#include <unistd.h>
#include "gmp.h"
#include "gmpstat.h"

int g_debug = 0;

/* Input is a modulus (m).  We shall find multiplyer (a) and adder (c)
   conforming to the rules found in the first comment block in file
   mpz/urandom.c.

   Then run a spectral test on the generator and discard any
   multipliers not passing.  */

int
main (int argc, char *argv[])
{
  const char usage[] = "usage: findcl m";
  int f;
  int lose, best;
  int debug = 1;
  unsigned long int rem;
  mpz_t m, a;
  mpz_t ulim, z_tmp;
#define DIMS 6			/* dimensions run in spectral test */
  mpf_t v[DIMS-1];		/* spectral test result (there's no v
                                   for 1st dimension */

  mpz_init (m);
  mpz_init (a);
  mpz_init (ulim);
  mpz_init (z_tmp);
  for (f = 0; f < DIMS-1; f++)
    mpf_init (v[f]);


  if (argc != 2)
    {
      fputs (usage, stderr);
      exit (1);
    }

  mpz_set_str (m, argv[1], 0);
  printf ("m = 0x");
  mpz_out_str (stdout, 16, m);
  puts ("");

  /* Search for 'a' in space .01m < a < .99m. */
  mpz_cdiv_q_ui (a, m, 100);
  mpz_mul_ui (ulim, m, 99);
  mpz_fdiv_q_ui (ulim, ulim, 100);

  if (debug)
    {
      /*      fprintf (stderr, "0x");*/
      mpz_out_str (stderr, 10, a);
      fprintf (stderr, " < a < ");
      mpz_out_str (stderr, 10, ulim);
      fputs ("\n", stderr);
    }

  /* Find 'a' where a % 8 == 5. */
  rem = mpz_mod_ui (z_tmp, a, 8);
  mpz_add_ui (a, a, 8 + (5 - rem));

  best = 2*(DIMS-1);
  for (; mpz_cmp (a, ulim) <= 0; mpz_add_ui (a, a, 8))
    {
      /* TODO: Reject 'a' with "binary or  decimal simple, regular pattern."  */

      /* run spectral test */
      spectral_test (v, DIMS, a, m);
      for (f = 0, lose = 0; f < DIMS-1; f++)
	if (mpf_cmp_ui (v[f], 1 << (30 / (f + 2) + 1)) < 0
	    || merit_u (f + 2, v[f], m) < .1)
	  lose++;

      if (0 == lose)
	{
	  mpz_out_str (stdout, 10, a);
	  puts ("");
	}

      if (lose < best)
	{
	  best = lose;
	  printf ("best (lose=%d): ", lose);
	  mpz_out_str (stdout, 10, a);
	  puts ("");
	}
    }


  mpz_clear (m);
  mpz_clear (a);
  mpz_clear (ulim);
  mpz_clear (z_tmp);

  return 0;
}
