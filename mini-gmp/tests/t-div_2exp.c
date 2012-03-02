#include <assert.h>
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

typedef void div_func (mpz_t, const mpz_t, mp_bitcnt_t);

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
      unsigned j;
      for (j = 0; j < 6; j++)
	{
	  static const enum hex_random_op ops[6] =
	    {
	      OP_CDIV_Q_2, OP_CDIV_R_2,
	      OP_FDIV_Q_2, OP_FDIV_R_2,
	      OP_TDIV_Q_2, OP_TDIV_R_2
	    };
	  static const char *name[6] =
	    {
	      "cdiv_q", "cdiv_r",
	      "fdiv_q", "fdiv_r",
	      "tdiv_q", "tdiv_r"
	    };
	  static div_func * const div [6] =
	    {
	      mpz_cdiv_q_2exp, mpz_cdiv_r_2exp,
	      mpz_fdiv_q_2exp, mpz_fdiv_r_2exp,
	      mpz_tdiv_q_2exp, mpz_tdiv_r_2exp
	    };

	  mini_random_bit_op (ops[j], MAXBITS, a, &b, ref);
	  div[j] (res, a, b);
	  if (mpz_cmp (ref, res))
	    {
	      fprintf (stderr, "mpz_%s_2exp failed:\n", name[j]);
	      dump ("a", a);
	      fprintf (stderr, "b: %lu\n", b);
	      dump ("r", res);
	      dump ("ref", ref);
	      abort ();
	    }
	}
    }
  mpz_clear (a);
  mpz_clear (res);
  mpz_clear (ref);

  return 0;
}
