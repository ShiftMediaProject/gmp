#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#include "mini-random.h"

#define MAXBITS 400
#define COUNT 1000

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
  mpz_t b, e, m, res, ref;

  hex_random_init ();

  mpz_init (b);
  mpz_init (e);
  mpz_init (m);
  mpz_init (res);
  mpz_init (ref);

  for (i = 0; i < COUNT; i++)
    {
      mini_random_op4 (OP_POWM, MAXBITS, b, e, m, ref);
      mpz_powm (res, b, e, m);
      if (mpz_cmp (res, ref))
	{
	  fprintf (stderr, "mpz_powm failed:\n");
	  dump ("b", b);
	  dump ("e", e);
	  dump ("m", m);
	  dump ("r", res);
	  dump ("ref", ref);
	  abort ();
	}
    }
  mpz_clear (b);
  mpz_clear (e);
  mpz_clear (m);
  mpz_clear (res);
  mpz_clear (ref);

  return 0;
}
