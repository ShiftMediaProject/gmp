/* Test mpz_divexact_si and mpz_divexact_ui.

Copyright 1996, 2001 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <stdio.h>
#include <stdlib.h>

#include "gmp.h"
#include "gmp-impl.h"
#include "tests.h"


void
check_random (int argc, char *argv[])
{
  gmp_randstate_ptr rands = RANDS;
  int    reps = 5000;
  mpz_t  a, q, d, got;
  int    i, qneg, dneg;

  if (argc == 2)
    reps = atoi (argv[1]);

  mpz_init (a);
  mpz_init (q);
  mpz_init (d);
  mpz_init (got);

  for (i = 0; i < reps; i++)
    {
      mpz_erandomb_nonzero (d, rands, BITS_PER_ULONG);
      mpz_erandomb (q, rands, 512);
      mpz_mul (a, q, d);

      for (qneg = 0; qneg <= 1; qneg++)
        {
          for (dneg = 0; dneg <= 1; dneg++)
            {
              if (mpz_fits_ulong_p (d))
                {
                  unsigned long  du = mpz_get_ui (d);
                  mpz_divexact_ui (got, a, du);
                  MPZ_CHECK_FORMAT (got);
                  if (mpz_cmp (got, q) != 0)
                    {
                      printf    ("mpz_divexact_ui wrong\n");
                      printf    ("   du=%lu\n", du);
                    fail:
                      mpz_trace ("    a", a);
                      mpz_trace ("    d", d);
                      mpz_trace ("    q", q);
                      mpz_trace ("  got", got);
                      abort ();
                    }
                }
              if (mpz_fits_slong_p (d))
                {
                  unsigned long  ds = mpz_get_si (d);
                  mpz_divexact_si (got, a, ds);
                  MPZ_CHECK_FORMAT (got);
                  if (mpz_cmp (got, q) != 0)
                    {
                      printf ("mpz_divexact_si wrong\n");
                      printf ("   ds=%ld\n", ds);
                      goto fail;
                    }
                }

              mpz_neg (d, d);
              mpz_neg (a, a);
            }

          mpz_neg (q, q);
          mpz_neg (a, a);
        }

    }

  mpz_clear (a);
  mpz_clear (q);
  mpz_clear (d);
  mpz_clear (got);
}


int
main (int argc, char **argv)
{
  tests_start ();

  check_random (argc, argv);

  tests_end ();
  exit (0);
}
