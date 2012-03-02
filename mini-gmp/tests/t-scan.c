#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#include "mini-random.h"

#define MAXBITS 400
#define COUNT 10000

static void
dump (const char *label, const mpz_t x)
{
  char *buf = mpz_get_str (NULL, 16, x);
  fprintf (stderr, "%s: %s\n", label, buf);
  free (buf);
}

int
main (int argc, char **argv)
{
  unsigned i;
  mpz_t a;
  mp_bitcnt_t b, res, ref;

  hex_random_init ();

  mpz_init (a);

  for (i = 0; i < COUNT; i++)
    {
      mini_random_scan_op (OP_SCAN0, MAXBITS, a, &b, &ref);
      res = mpz_scan0 (a, b);
      if (res != ref)
	{
	  fprintf (stderr, "mpz_scan0 failed:\n");
	  dump ("a", a);
	  fprintf (stderr, "b: %lu\n", b);
	  fprintf (stderr, "r: %lu\n", res);
	  fprintf (stderr, "ref: %lu\n", ref);
	  abort ();
	}
      mini_random_scan_op (OP_SCAN1, MAXBITS, a, &b, &ref);
      res = mpz_scan1 (a, b);
      if (res != ref)
	{
	  fprintf (stderr, "mpz_scan1 failed:\n");
	  dump ("a", a);
	  fprintf (stderr, "b: %lu\n", b);
	  fprintf (stderr, "r: %lu\n", res);
	  fprintf (stderr, "ref: %lu\n", ref);
	  abort ();
	}
    }
  mpz_clear (a);

  return 0;
}
