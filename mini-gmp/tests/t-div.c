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

typedef void div_func (mpz_t, mpz_t, const mpz_t, const mpz_t);
typedef unsigned long div_ui_func (mpz_t, mpz_t, const mpz_t, unsigned long);

int
main (int argc, char **argv)
{
  unsigned i;
  mpz_t a, b, q, r, rq, rr;

  hex_random_init ();

  mpz_init (a);
  mpz_init (b);
  mpz_init (r);
  mpz_init (q);
  mpz_init (rr);
  mpz_init (rq);

  for (i = 0; i < COUNT; i++)
    {
      unsigned j;
      for (j = 0; j < 3; j++)
	{
	  static const enum hex_random_op ops[3] = { OP_CDIV, OP_FDIV, OP_TDIV };
	  static const char name[3] = { 'c', 'f', 't'};
	  static div_func * const div [3] =
	    {
	      mpz_cdiv_qr, mpz_fdiv_qr, mpz_tdiv_qr
	    };
	  static div_ui_func  *div_ui[3] =
	    {
	      mpz_cdiv_qr_ui, mpz_fdiv_qr_ui, mpz_tdiv_qr_ui
	    };

	  mini_random_op4 (ops[j], MAXBITS, a, b, rq, rr);
	  div[j] (q, r, a, b);
	  if (mpz_cmp (r, rr) || mpz_cmp (q, rq))
	    {
	      fprintf (stderr, "mpz_%cdiv_qr failed:\n", name[j]);
	      dump ("a", a);
	      dump ("b", b);
	      dump ("r   ", r);
	      dump ("rref", rr);
	      dump ("q   ", q);
	      dump ("qref", rq);
	      abort ();
	    }
	  if (mpz_fits_ulong_p (b))
	    {
	      mp_limb_t rl;
	      rl = div_ui[j] (q, r, a, mpz_get_ui (b));

	      if (rl != mpz_get_ui (rr)
		  || mpz_cmp (r, rr) || mpz_cmp (q, rq))
		{
		  fprintf (stderr, "mpz_%cdiv_qr_ui failed:\n", name[j]);
		  dump ("a", a);
		  dump ("b", b);
		  fprintf(stderr, "rl   = %lx\n", rl);
		  dump ("r   ", r);
		  dump ("rref", rr);
		  dump ("q   ", q);
		  dump ("qref", rq);
		  abort ();
		}
	    }
	}
    }
  mpz_clear (a);
  mpz_clear (b);
  mpz_clear (r);
  mpz_clear (q);
  mpz_clear (rr);
  mpz_clear (rq);

  return 0;
}
