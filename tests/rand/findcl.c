#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "gmp.h"
#include "gmpstat.h"

int g_debug = 0;

/* Input is a modulus (m).  We shall find multiplyer (a) and adder (c)
   conforming to the rules found in the first comment block in file
   mpz/urandom.c.

   Then run a spectral test on the generator and discard any
   multipliers not passing.  */

/* TODO:

   1) catch signals and print current 'a'.

   2) print low_merit and high_merit on startup.

   3) print "done" on exit.

*/

int
main (int argc, char *argv[])
{
  const char usage[] = "usage: findcl [-a start_a] m [low_merit [high_merit]]";
  int f;
  int lose, best;
  int c;
  int debug = 1;
  int have_start_a = 0;
  int cnt_high_merit;
  unsigned long int rem;
  mpz_t m, a;
  mpz_t ulim, z_tmp;
#define DIMS 6			/* dimensions run in spectral test */
  mpf_t v[DIMS-1];		/* spectral test result (there's no v
                                   for 1st dimension */
  mpf_t f_merit, low_merit, high_merit;

  mpz_init (m);
  mpz_init (a);
  mpz_init (ulim);
  mpz_init (z_tmp);
  for (f = 0; f < DIMS-1; f++)
    mpf_init (v[f]);
  mpf_init (f_merit);
  mpf_init_set_d (low_merit, .1);
  mpf_init_set_d (high_merit, .1);

  while ((c = getopt (argc, argv, "a:h")) != -1)
    switch (c)
      {
      case 'a':			/* start_a */
	mpz_set_str (a, optarg, 0);
	have_start_a = 1;
	break;

      case 'h':
      case '?':
      default:
	fputs (usage, stderr);
	exit (1);
      }

  argc -= optind;
  argv += optind;

  if (argc < 1)
    {
      fputs (usage, stderr);
      exit (1);
    }

  mpz_set_str (m, argv[0], 0);
  printf ("m = 0x");
  mpz_out_str (stdout, 16, m);
  puts ("");

  if (argc > 1)			/* have low_merit */
    mpf_set_str (low_merit, argv[1], 0);
  if (argc > 2)			/* have high_merit */
    mpf_set_str (high_merit, argv[2], 0);

  /* Search for 'a' in space .01m < a < .99m. */
  if (!have_start_a)
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
      for (f = 0, lose = 0, cnt_high_merit = DIMS-1; f < DIMS-1; f++)
	{
	  merit (f_merit, f + 2, v[f], m);

	  if (mpf_cmp_ui (v[f], 1 << (30 / (f + 2) + 1)) < 0
	      || mpf_cmp (f_merit, low_merit) < 0)
	    lose++;

	  if (mpf_cmp (f_merit, high_merit) >= 0)
	    cnt_high_merit--;
	}

      if (0 == lose)
	{
	  mpz_out_str (stdout, 10, a);
	  puts ("");
	  fflush (stdout);
	  if (0 == cnt_high_merit)
	    break;		/* leave loop */
	}

      if (lose < best)
	{
	  best = lose;
	  printf ("best (lose=%d): ", lose);
	  mpz_out_str (stdout, 10, a);
	  puts ("");
	  fflush (stdout);
	}
    }


  mpz_clear (m);
  mpz_clear (a);
  mpz_clear (ulim);
  mpz_clear (z_tmp);
  for (f = 0; f < DIMS-1; f++)
    mpf_clear (v[f]);
  mpf_clear (f_merit);
  mpf_clear (low_merit);
  mpf_clear (high_merit);

  printf ("done.\n");
  return 0;
}
