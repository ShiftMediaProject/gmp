#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#include "mini-random.h"

#define GMP_LIMB_BITS (sizeof(mp_limb_t) * CHAR_BIT)

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
  mpz_t u, m, p, t;

  hex_random_init ();

  mpz_init (u);
  mpz_init (m);
  mpz_init (p);
  mpz_init (t);

  for (i = 0; i < COUNT; i++)
    {
      mini_urandomb (u, GMP_LIMB_BITS);
      mpz_setbit (u, GMP_LIMB_BITS -1);

      mpz_set_ui (m, mpn_invert_limb (u[0]._mp_d[0]));
      mpz_setbit (m, GMP_LIMB_BITS);

      mpz_mul (p, m, u);

      mpz_set_ui (t, 0);
      mpz_setbit (t, 2* GMP_LIMB_BITS);
      mpz_sub (t, t, p);

      /* Should have 0 < B^2 - m u <= u */
      if (mpz_sgn (t) <= 0 || mpz_cmp (t, u) > 0)
	{
	  fprintf (stderr, "mpn_invert_limb failed:\n");
	  dump ("u", u);
	  dump ("m", m);
	  dump ("p", p);
	  dump ("t", t);
	  abort ();
	}
    }

  for (i = 0; i < COUNT; i++)
    {
      mini_urandomb (u, 2*GMP_LIMB_BITS);
      mpz_setbit (u, 2*GMP_LIMB_BITS -1);

      mpz_set_ui (m, mpn_invert_3by2 (u[0]._mp_d[1], u[0]._mp_d[0]));

      mpz_setbit (m, GMP_LIMB_BITS);

      mpz_mul (p, m, u);

      mpz_set_ui (t, 0);
      mpz_setbit (t, 3 * GMP_LIMB_BITS);
      mpz_sub (t, t, p);

      /* Should have 0 < B^3 - m u <= u */
      if (mpz_sgn (t) <= 0 || mpz_cmp (t, u) > 0)
	{
	  fprintf (stderr, "mpn_invert_3by2 failed:\n");
	  dump ("u", u);
	  dump ("m", m);
	  dump ("p", p);
	  dump ("t", t);
	  abort ();
	}
    }

  mpz_clear (u);
  mpz_clear (m);
  mpz_clear (p);
  mpz_clear (t);

  return 0;
}
