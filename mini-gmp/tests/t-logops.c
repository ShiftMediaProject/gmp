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
  mpz_t a, b, res, ref;

  hex_random_init ();

  mpz_init (a);
  mpz_init (b);
  mpz_init (res);
  mpz_init (ref);

  for (i = 0; i < COUNT; i++)
    {
      mini_random_op (OP_AND, MAXBITS, a, b, ref);
      mpz_and (res, a, b);
      if (mpz_cmp (res, ref))
	{
	  fprintf (stderr, "mpz_and failed:\n");
	  dump ("a", a);
	  dump ("b", b);
	  dump ("r", res);
	  dump ("ref", ref);
	  abort ();
	}

      mini_random_op (OP_IOR, MAXBITS, a, b, ref);
      mpz_ior (res, a, b);
      if (mpz_cmp (res, ref))
	{
	  fprintf (stderr, "mpz_ior failed:\n");
	  dump ("a", a);
	  dump ("b", b);
	  dump ("r", res);
	  dump ("ref", ref);
	  abort ();
	}

      mini_random_op (OP_XOR, MAXBITS, a, b, ref);
      mpz_xor (res, a, b);
      if (mpz_cmp (res, ref))
	{
	  fprintf (stderr, "mpz_xor failed:\n");
	  dump ("a", a);
	  dump ("b", b);
	  dump ("r", res);
	  dump ("ref", ref);
	  abort ();
	}
    }
  mpz_clear (a);
  mpz_clear (b);
  mpz_clear (res);
  mpz_clear (ref);

  return 0;
}
