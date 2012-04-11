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

/* Called when s is supposed to be floor(root(u,z)), and r = u - s^z */
static int
rootrem_valid_p (const mpz_t u, const mpz_t s, const mpz_t r, unsigned long z)
{
  mpz_t t;

  mpz_init (t);
  mpz_pow_ui (t, s, z);
  mpz_sub (t, u, t);
  if (mpz_sgn (t) != mpz_sgn(u) || mpz_cmp (t, r) != 0)
    {
      mpz_clear (t);
      return 0;
    }
  if (mpz_sgn (s) > 0)
    mpz_add_ui (t, s, 1);
  else
    mpz_sub_ui (t, s, 1);
  mpz_pow_ui (t, t, z);
  if (mpz_cmpabs (t, u) <= 0)
    {
      mpz_clear (t);
      return 0;
    }

  mpz_clear (t);
  return 1;
}

int
main (int argc, char **argv)
{
  unsigned i;
  unsigned long e;
  mpz_t u, s, r, bs;

  hex_random_init ();

  mpz_init (u);
  mpz_init (s);
  mpz_init (r);
  mpz_init (bs);

  for (i = 0; i < COUNT; i++)
    {
      mini_rrandomb (u, MAXBITS);
      mini_rrandomb (bs, 12);
      e = mpz_getlimbn (bs, 0) % mpz_sizeinbase (u, 2) + 2;
      if ((e & 1) && (mpz_getlimbn (bs, 0) & (1L<<10)))
	mpz_neg (u, u);
      mpz_rootrem (s, r, u, e);

      if (!rootrem_valid_p (u, s, r, e))
	{
	  fprintf (stderr, "mpz_rootrem(%lu-th) failed:\n", e);
	  dump ("u", u);
	  dump ("root", s);
	  dump ("rem", r);
	  abort ();
	}
    }
  mpz_clear (bs);
  mpz_clear (u);
  mpz_clear (s);
  mpz_clear (r);

  return 0;
}
