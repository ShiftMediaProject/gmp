/* Test file for mpfr_rint, mpfr_trunc, mpfr_floor, mpfr_ceil, mpfr_round.

Copyright 2002, 2003 Free Software Foundation.

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
#include <string.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#include "mpfr-test.h"

#if __STDC_VERSION__ >= 199901L

static void
test_fct (double (*f)(double), int (*g)(), char *s, mp_rnd_t r)
{
  double d, y;
  mpfr_t dd, yy;

  mpfr_init2 (dd, 53);
  mpfr_init2 (yy, 53);
  for (d = -5.0; d <= 5.0; d += 0.25)
    {
      mpfr_set_d (dd, d, r);
      y = (*f)(d);
      if (g == &mpfr_rint)
        mpfr_rint (yy, dd, r);
      else
        (*g)(yy, dd);
      mpfr_set_d (dd, y, r);
      if (mpfr_cmp (yy, dd))
        {
          printf ("test_against_libc: incorrect result for %s, rnd = %s,"
                  " d = %g\ngot ", s, mpfr_print_rnd_mode (r), d);
          mpfr_out_str (stdout, 10, 0, yy, GMP_RNDN);
          printf (" instead of %g\n", y);
          exit (1);
        }
    }
  mpfr_clear (dd);
  mpfr_clear (yy);
}

#define TEST_FCT(F) test_fct (&F, &mpfr_##F, #F, r)

static void
test_against_libc (void)
{
  int r = 0;

  TEST_FCT (round);
  TEST_FCT (trunc);
  TEST_FCT (floor);
  TEST_FCT (ceil);
  for (r = 0; r < 4; r++)
    if (mpfr_set_machine_rnd_mode (r) == 0)
      test_fct (&nearbyint, &mpfr_rint, "rint", r);
}

#endif

static void
err (char *str, mp_size_t s, mpfr_t x, mpfr_t y, mp_prec_t p, mp_rnd_t r,
     int trint, int inexact)
{
  printf ("Error: %s\ns = %u, p = %u, r = %s, trint = %d, inexact = %d\nx = ",
          str, (unsigned int) s, (unsigned int) p, mpfr_print_rnd_mode (r),
          trint, inexact);
  mpfr_print_binary (x);
  printf ("\ny = ");
  mpfr_print_binary (y);
  printf ("\n");
  exit (1);
}

int
main (int argc, char *argv[])
{
  mp_size_t s;
  mpz_t z;
  mp_prec_t p;
  mpfr_t x, y, t, u, v;
  mp_rnd_t r;
  int inexact, sign_t;

  tests_start_mpfr ();

  mpfr_init (x);
  mpfr_init (y);
  mpz_init (z);
  mpfr_init (t);
  mpfr_init (u);
  mpfr_init (v);
  mpz_set_ui (z, 1);
  for (s = 2; s < 100; s++)
    {
      /* z has exactly s bits */

      mpz_mul_2exp (z, z, 1);
      if (randlimb () % 2)
        mpz_add_ui (z, z, 1);
      mpfr_set_prec (x, s);
      mpfr_set_prec (t, s);
      mpfr_set_prec (u, s);
      if (mpfr_set_z (x, z, GMP_RNDN))
        {
          printf ("Error: mpfr_set_z should be exact (s = %u)\n",
                  (unsigned int) s);
          exit (1);
        }
      if (randlimb () % 2)
        mpfr_neg (x, x, GMP_RNDN);
      if (randlimb () % 2)
        mpfr_div_2ui (x, x, randlimb () % s, GMP_RNDN);
      for (p = 2; p < 100; p++)
        {
          int trint;
          mpfr_set_prec (y, p);
          mpfr_set_prec (v, p);
          for (r = 0; r < 4; r++)
            for (trint = 0; trint < 2; trint++)
              {
                if (trint)
                  inexact = mpfr_rint (y, x, r);
                else if (r == GMP_RNDN)
                  inexact = mpfr_round (y, x);
                else if (r == GMP_RNDZ)
                  inexact = mpfr_trunc (y, x);
                else if (r == GMP_RNDU)
                  inexact = mpfr_ceil (y, x);
                else /* r = GMP_RNDD */
                  inexact = mpfr_floor (y, x);
                if (mpfr_sub (t, y, x, GMP_RNDN))
                  err ("subtraction 1 should be exact",
                       s, x, y, p, r, trint, inexact);
                sign_t = mpfr_cmp_ui (t, 0);
                if (((inexact == 0) && (sign_t != 0)) ||
                    ((inexact < 0) && (sign_t >= 0)) ||
                    ((inexact > 0) && (sign_t <= 0)))
                  err ("wrong inexact flag", s, x, y, p, r, trint, inexact);
                if (inexact == 0)
                  continue; /* end of the test for exact results */

                if (((r == GMP_RNDD || (r == GMP_RNDZ && MPFR_SIGN (x) > 0))
                     && inexact > 0) ||
                    ((r == GMP_RNDU || (r == GMP_RNDZ && MPFR_SIGN (x) < 0))
                     && inexact < 0))
                  err ("wrong rounding direction",
                       s, x, y, p, r, trint, inexact);
                if (inexact < 0)
                  {
                    mpfr_add_ui (v, y, 1, GMP_RNDU);
                    if (mpfr_cmp (v, x) <= 0)
                      err ("representable integer between x and its "
                           "rounded value", s, x, y, p, r, trint, inexact);
                  }
                else
                  {
                    mpfr_sub_ui (v, y, 1, GMP_RNDD);
                    if (mpfr_cmp (v, x) >= 0)
                      err ("representable integer between x and its "
                           "rounded value", s, x, y, p, r, trint, inexact);
                  }
                if (r == GMP_RNDN)
                  {
                    int cmp;
                    if (mpfr_sub (u, v, x, GMP_RNDN))
                      err ("subtraction 2 should be exact",
                           s, x, y, p, r, trint, inexact);
                    cmp = mpfr_cmp_abs (t, u);
                    if (cmp > 0)
                      err ("faithful rounding, but not the nearest integer",
                           s, x, y, p, r, trint, inexact);
                    if (cmp < 0)
                      continue;
                    /* |t| = |u|: x is the middle of two consecutive
                       representable integers. */
                    if (trint)
                      {
                        /* halfway case for mpfr_rint in GMP_RNDN rounding
                           mode: round to an even integer or mantissa. */
                        mpfr_div_2ui (y, y, 1, GMP_RNDZ);
                        if (!mpfr_integer_p (y))
                          err ("halfway case for mpfr_rint, result isn't an"
                               " even integer", s, x, y, p, r, trint, inexact);
                        /* If floor(x) and ceil(x) aren't both representable
                           integers, the mantissa must be even. */
                        mpfr_sub (v, v, y, GMP_RNDN);
                        mpfr_abs (v, v, GMP_RNDN);
                        if (mpfr_cmp_ui (v, 1) != 0)
                          {
                            mpfr_div_2si (y, y, MPFR_EXP (y) - MPFR_PREC (y)
                                          + 1, GMP_RNDN);
                            if (!mpfr_integer_p (y))
                              err ("halfway case for mpfr_rint, mantissa isn't"
                                   " even", s, x, y, p, r, trint, inexact);
                          }
                      }
                    else
                      { /* halfway case for mpfr_round: x must have been
                           rounded away from zero. */
                        if ((MPFR_SIGN (x) > 0 && inexact < 0) ||
                            (MPFR_SIGN (x) < 0 && inexact > 0))
                          err ("halfway case for mpfr_round, bad rounding"
                               " direction", s, x, y, p, r, trint, inexact);
                      }
                  }
              }
        }
    }
  mpfr_clear (x);
  mpfr_clear (y);
  mpz_clear (z);
  mpfr_clear (t);
  mpfr_clear (u);
  mpfr_clear (v);

  /* TODO: add hardcoded tests */
#if __STDC_VERSION__ >= 199901L
  if (argc > 1 && strcmp (argv[1], "-s") == 0)
    test_against_libc ();
#endif

  tests_end_mpfr ();
  return 0;
}
