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
  mpz_t a, res, ref;
  mp_bitcnt_t b;

  hex_random_init ();

  mpz_init (a);
  mpz_init (res);
  mpz_init (ref);

  for (i = 0; i < COUNT; i++)
    {
      mini_random_bit_op (OP_SETBIT, MAXBITS, a, &b, ref);
      mpz_set (res, a);
      mpz_setbit (res, b);
      if (mpz_cmp (res, ref))
	{
	  fprintf (stderr, "mpz_setbit failed:\n");
	  dump ("a", a);
	  fprintf (stderr, "b: %lu\n", b);
	  dump ("r", res);
	  dump ("ref", ref);
	  abort ();
	}
      if (!mpz_tstbit (res, b))
	{
	  fprintf (stderr, "mpz_tstbit failed (after mpz_setbit):\n");
	  dump ("res", a);
	  fprintf (stderr, "b: %lu\n", b);
	  abort ();
	}
      mini_random_bit_op (OP_CLRBIT, MAXBITS, a, &b, ref);
      mpz_set (res, a);
      mpz_clrbit (res, b);
      if (mpz_cmp (res, ref))
	{
	  fprintf (stderr, "mpz_clrbit failed:\n");
	  dump ("a", a);
	  fprintf (stderr, "b: %lu\n", b);
	  dump ("r", res);
	  dump ("ref", ref);
	  abort ();
	}
      if (mpz_tstbit (res, b))
	{
	  fprintf (stderr, "mpz_tstbit failed (after mpz_clrbit):\n");
	  dump ("res", a);
	  fprintf (stderr, "b: %lu\n", b);
	  abort ();
	}
      mini_random_bit_op (OP_COMBIT, MAXBITS, a, &b, ref);
      mpz_set (res, a);
      mpz_combit (res, b);
      if (mpz_cmp (res, ref))
	{
	  fprintf (stderr, "mpz_combit failed:\n");
	  dump ("a", a);
	  fprintf (stderr, "b: %lu\n", b);
	  dump ("r", res);
	  dump ("ref", ref);
	  abort ();
	}
      if (mpz_tstbit (res, b) == mpz_tstbit (a, b))
	{
	  fprintf (stderr, "mpz_tstbit failed (after mpz_combit):\n");
	  dump ("res", a);
	  fprintf (stderr, "b: %lu\n", b);
	  abort ();
	}
    }
  mpz_clear (a);
  mpz_clear (res);
  mpz_clear (ref);

  return 0;
}
