/* Test file for mpfr_cbrt.

Copyright 2002, 2003 Free Software Foundation, Inc.

This file is part of the MPFR Library.

The MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <stdio.h>
#include <stdlib.h>
#include "gmp.h"
#include "mpfr.h"
#include "mpfr-test.h"

int
main (void)
{
  mpfr_t x;
  mp_rnd_t r;
  mp_prec_t p;

  tests_start_mpfr ();

  mpfr_init (x);

  mpfr_set_prec (x, 53);
  mpfr_set_d (x, 8.39005285514734966412e-01, GMP_RNDN);
  mpfr_cbrt (x, x, GMP_RNDN);
  if (mpfr_get_d1 (x) != 9.43166207799662426048e-01)
    {
      fprintf (stderr, "Error (1)\n");
      exit (1);
    }

  for (p=2; p<100; p++)
    {
      mpfr_set_prec (x, p);
      for (r=0; r<4; r++)
        {
          mpfr_set_ui (x, 1, GMP_RNDN);
          mpfr_cbrt (x, x, r);
          if (mpfr_cmp_ui (x, 1))
            {
              fprintf (stderr, "Error in mpfr_cbrt for x=1, rnd=%s\ngot ",
                       mpfr_print_rnd_mode (r));
              mpfr_out_str (stderr, 2, 0, x, GMP_RNDN);
              fprintf (stderr, "\n");
              exit (1);
            }
          mpfr_set_si (x, -1, GMP_RNDN);
          mpfr_cbrt (x, x, r);
          if (mpfr_cmp_si (x, -1))
            {
              fprintf (stderr, "Error in mpfr_cbrt for x=-1, rnd=%s\ngot ",
                       mpfr_print_rnd_mode (r));
              mpfr_out_str (stderr, 2, 0, x, GMP_RNDN);
              fprintf (stderr, "\n");
              exit (1);
            }

          if (p >= 5)
            {
              int i;
              for (i = -12; i <= 12; i++)
                {
                  mpfr_set_d (x, 27.0, GMP_RNDN);
                  mpfr_mul_2si (x, x, 3*i, GMP_RNDN);
                  mpfr_cbrt (x, x, GMP_RNDN);
                  if (mpfr_cmp_si_2exp (x, 3, i))
                    {
                      fprintf (stderr, "Error in mpfr_cbrt for "
                               "x = 27.0 * 2^(%d), rnd=%s\ngot ",
                               3*i, mpfr_print_rnd_mode (r));
                      mpfr_out_str (stderr, 2, 0, x, GMP_RNDN);
                      fprintf (stderr, "\ninstead of 3 * 2^(%d)\n", i);
                      exit (1);
                    }
                }
            }
        }
    }

  mpfr_clear (x);

  tests_end_mpfr ();
  return 0;
}
