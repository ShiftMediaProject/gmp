#include <stdlib.h>
#include <stdio.h>
#include "gmp.h"

/* Sieve table size */
#define ST_SIZE 30000
/* Largest prime to sieve with */
#define MAX_S_PRIME 1000

main (int argc, char **argv)
{
  char *progname = argv[0];
  mpz_t r0, r1;			/* range */
  mpz_t cur;
  unsigned char *st;
  unsigned long i, ii;
  unsigned long nprimes = 0;
  unsigned long last;
  int flag_print = 1;
  int flag_count = 0;

  st = malloc (ST_SIZE);

  while (argc != 1)
    {
      if (strcmp (argv[1], "-c") == 0)
	{
	  flag_count = 1;
	  argv++;
	  argc--;
	}
      else if (strcmp (argv[1], "-p") == 0)
	{
	  flag_print = 2;
	  argv++;
	  argc--;
	}
      else
	break;
    }

  if (flag_count)
    flag_print--;		/* clear unless an explicit -p  */

  mpz_init (r0);
  mpz_init (r1);
  mpz_init (cur);

  if (argc == 2)
    {
      mpz_set_ui (r0, 2);
      mpz_set_str (r1, argv[1], 0);
    }
  else if (argc == 3)
    {
      mpz_set_str (r0, argv[1], 0);
      if (argv[2][0] == '+')
	{
	  mpz_set_str (r1, argv[2] + 1, 0);
	  mpz_add (r1, r1, r0);
	}
      mpz_set_str (r1, argv[2], 0);
    }
  else
    {
      fprintf (stderr, "usage: %s [-c] [-g] [from [+]]to\n", progname);
      exit (1);
    }

  if (mpz_cmp_ui (r0, 2) < 0)
    mpz_set_ui (r0, 2);

  if ((mpz_get_ui (r0) & 1) == 0)
    {
      if (mpz_cmp_ui (r0, 2) == 0)
	{
	  if (flag_print)
	    puts ("2");
	  nprimes++;
	}
      mpz_add_ui (r0, r0, 1);
    }

  mpz_set (cur, r0);

  while (mpz_cmp (cur, r1) <= 0)
    {
      memset (st, 1, ST_SIZE);
      for (i = 3; i < MAX_S_PRIME; i += 2)
	{
	  unsigned long start;
	  start = mpz_tdiv_ui (cur, i);
	  if (start != 0)
	    start = i - start;
	  
	  if (mpz_cmp_ui (cur, i - start) == 0)
	    start += i;
	  for (ii = start; ii < ST_SIZE; ii += i)
	    st[ii] = 0;
	}
      last = 0;
      for (ii = 0; ii < ST_SIZE; ii += 2)
	{
	  if (st[ii] != 0)
	    {
	      mpz_add_ui (cur, cur, ii - last);
	      last = ii;
	      if (mpz_cmp (cur, r1) > 0)
		goto done;
	      if (mpz_probab_prime_p (cur, 3))
		{
		  nprimes++;
		  if (flag_print)
		    {
		      mpz_out_str (stdout, 10, cur); puts ("");
		    }
		}
	    }
	}
      mpz_add_ui (cur, cur, ST_SIZE - last);
    }
 done:
  if (flag_count)
    printf ("Pi(interval) = %lu\n", nprimes);

  exit (0);
}
