/* Test file for mpfr_pow and mpfr_pow_ui.

Copyright 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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
#include "mpfr-impl.h"
#include "mpfr-test.h"

static void
check_pow_ui (void)
{
  mpfr_t a, b;

  mpfr_init2 (a, 53);
  mpfr_init2 (b, 53);

  /* check in-place operations */
  mpfr_set_d (b, 0.6926773, GMP_RNDN);
  mpfr_pow_ui (a, b, 10, GMP_RNDN);
  mpfr_pow_ui (b, b, 10, GMP_RNDN);
  if (mpfr_cmp (a, b))
    {
      printf ("Error for mpfr_pow_ui (b, b, ...)\n");
      exit (1);
    }

  /* check large exponents */
  mpfr_set_d (b, 1, GMP_RNDN);
  mpfr_pow_ui (a, b, 4294967295UL, GMP_RNDN);

  mpfr_set_inf (a, -1);
  mpfr_pow_ui (a, a, 4049053855UL, GMP_RNDN);
  if (!mpfr_inf_p (a) || (mpfr_sgn (a) >= 0))
    {
      printf ("Error for (-Inf)^4049053855\n");
      exit (1);
    }

  mpfr_set_inf (a, -1);
  mpfr_pow_ui (a, a, (unsigned long) 30002752, GMP_RNDN);
  if (!mpfr_inf_p (a) || (mpfr_sgn (a) <= 0))
    {
      printf ("Error for (-Inf)^30002752\n");
      exit (1);
    }

  mpfr_clear (a);
  mpfr_clear (b);
}

static void
check_inexact (mp_prec_t p)
{
  mpfr_t x, y, z, t;
  unsigned long u;
  mp_prec_t q;
  int inexact, cmp;
  mp_rnd_t rnd;

  mpfr_init2 (x, p);
  mpfr_init (y);
  mpfr_init (z);
  mpfr_init (t);
  mpfr_random (x);
  u = randlimb () % 2;
  for (q=2; q<=p; q++)
    for (rnd=0; rnd<4; rnd++)
      {
        mpfr_set_prec (y, q);
        mpfr_set_prec (z, q + 10);
        mpfr_set_prec (t, q);
        inexact = mpfr_pow_ui (y, x, u, rnd);
        cmp = mpfr_pow_ui (z, x, u, rnd);
        if (mpfr_can_round (z, q + 10, rnd, rnd, q))
          {
            cmp = mpfr_set (t, z, rnd) || cmp;
            if (mpfr_cmp (y, t))
              {
                printf ("results differ for u=%lu rnd=%s\n",
                        u, mpfr_print_rnd_mode(rnd));
                printf ("x="); mpfr_print_binary (x); puts ("");
                printf ("y="); mpfr_print_binary (y); puts ("");
                printf ("t="); mpfr_print_binary (t); puts ("");
                printf ("z="); mpfr_print_binary (z); puts ("");
                exit (1);
              }
            if (((inexact == 0) && (cmp != 0)) ||
                ((inexact != 0) && (cmp == 0)))
              {
                printf ("Wrong inexact flag for p=%u, q=%u, rnd=%s\n",
                        (unsigned int) p, (unsigned int) q,
                        mpfr_print_rnd_mode (rnd));
                printf ("expected %d, got %d\n", cmp, inexact);
                printf ("u=%lu x=", u); mpfr_print_binary (x); puts ("");
                printf ("y="); mpfr_print_binary (y); puts ("");
                exit (1);
              }
          }
      }

  /* check exact power */
  mpfr_set_prec (x, p);
  mpfr_set_prec (y, p);
  mpfr_set_prec (z, p);
  mpfr_set_d (x, 4.0, GMP_RNDN);
  mpfr_set_d (y, 0.5, GMP_RNDN);
  mpfr_pow (z, x, y, GMP_RNDZ);

  mpfr_clear (x);
  mpfr_clear (y);
  mpfr_clear (z);
  mpfr_clear (t);
}

static void
special ()
{
  mpfr_t x, y, z, t;

  mpfr_init2 (x, 53);
  mpfr_init2 (y, 53);
  mpfr_init2 (z, 53);
  mpfr_init2 (t, 2);

  mpfr_set_prec (x, 64);
  mpfr_set_prec (y, 64);
  mpfr_set_prec (z, 64);
  mpfr_set_prec (t, 64);
  mpfr_set_str_binary (x, "0.111011000111100000111010000101010100110011010000011");
  mpfr_set_str_binary (y, "0.111110010100110000011101100011010111000010000100101");
  mpfr_set_str_binary (t, "0.1110110011110110001000110100100001001111010011111000010000011001");
    ;
  mpfr_pow (z, x, y, GMP_RNDN);
  if (mpfr_cmp (z, t))
    {
      printf ("Error in mpfr_pow for prec=64, rnd=GMP_RNDN\n");
      exit (1);
    }

  mpfr_set_prec (x, 53);
  mpfr_set_prec (y, 53);
  mpfr_set_prec (z, 53);
  mpfr_set_d (x, 5.68824667828621954868e-01, GMP_RNDN);
  mpfr_set_d (y, 9.03327850535952658895e-01, GMP_RNDN);
  mpfr_pow (z, x, y, GMP_RNDZ);
  if (mpfr_get_d1 (z) != 0.60071044650456473235)
    {
      printf ("Error in mpfr_pow for prec=53, rnd=GMP_RNDZ\n");
      exit (1);
    }

  mpfr_set_prec (t, 2);
  mpfr_set_prec (x, 30);
  mpfr_set_prec (y, 30);
  mpfr_set_prec (z, 30);
  mpfr_set_str (x, "1.00000000001010111110001111011e1", 2, GMP_RNDN);
  mpfr_set_d (t, -0.5, GMP_RNDN);
  mpfr_pow (z, x, t, GMP_RNDN);
  mpfr_set_str (y, "1.01101001111010101110000101111e-1", 2, GMP_RNDN);
  if (mpfr_cmp (z, y))
    {
      printf ("Error in mpfr_pow for prec=30, rnd=GMP_RNDN\n");
      exit (1);
    }

  mpfr_set_prec (x, 21);
  mpfr_set_prec (y, 21);
  mpfr_set_prec (z, 21);
  mpfr_set_str (x, "1.11111100100001100101", 2, GMP_RNDN);
  mpfr_pow (z, x, t, GMP_RNDZ);
  mpfr_set_str (y, "1.01101011010001100000e-1", 2, GMP_RNDN);
  if (mpfr_cmp (z, y))
    {
      printf ("Error in mpfr_pow for prec=21, rnd=GMP_RNDZ\n");
      printf ("Expected ");
      mpfr_out_str (stdout, 2, 0, y, GMP_RNDN);
      printf ("\nGot      ");
      mpfr_out_str (stdout, 2, 0, z, GMP_RNDN);
      printf ("\n");
      exit (1);
    }

  mpfr_clear (x);
  mpfr_clear (y);
  mpfr_clear (z);
  mpfr_clear (t);
}

static void
particular_cases (void)
{
  mpfr_t t[11], r;
  int i, j;
  int error = 0;

  for (i = 0; i < 11; i++)
    mpfr_init2 (t[i], 2);
  mpfr_init2 (r, 6);

  mpfr_set_nan (t[0]);
  mpfr_set_inf (t[1], 1);
  mpfr_set_ui (t[3], 0, GMP_RNDN);
  mpfr_set_ui (t[5], 1, GMP_RNDN);
  mpfr_set_ui (t[7], 2, GMP_RNDN);
  mpfr_div_2ui (t[9], t[5], 1, GMP_RNDN);
  for (i = 1; i < 11; i += 2)
    mpfr_neg (t[i+1], t[i], GMP_RNDN);

  for (i = 0; i < 11; i++)
    for (j = 0; j < 11; j++)
      {
        int p;
        static int q[11][11] = {
          /*          NaN +inf -inf  +0   -0   +1   -1   +2   -2  +0.5 -0.5 */
          /*  NaN */ { 0,   0,   0,  128, 128,  0,   0,   0,   0,   0,   0  },
          /* +inf */ { 0,   1,   2,  128, 128,  1,   2,   1,   2,   1,   2  },
          /* -inf */ { 0,   1,   2,  128, 128, -1,  -2,   1,   2,   1,   2  },
          /*  +0  */ { 0,   2,   1,  128, 128,  2,   1,   2,   1,   2,   1  },
          /*  -0  */ { 0,   2,   1,  128, 128, -2,  -1,   2,   1,   2,   1  },
          /*  +1  */ {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
          /*  -1  */ { 0,  128, 128, 128, 128,-128,-128, 128, 128,  0,   0  },
          /*  +2  */ { 0,   1,   2,  128, 128, 256,  64, 512,  32, 180,  90 },
          /*  -2  */ { 0,   1,   2,  128, 128,-256, -64, 512,  32,  0,   0  },
          /* +0.5 */ { 0,   2,   1,  128, 128,  64, 256,  32, 512,  90, 180 },
          /* -0.5 */ { 0,   2,   1,  128, 128, -64,-256,  32, 512,  0,   0  }
        };

        mpfr_pow (r, t[i], t[j], GMP_RNDN);
        p = mpfr_nan_p (r) ? 0 : mpfr_inf_p (r) ? 1 :
          mpfr_cmp_ui (r, 0) == 0 ? 2 :
          (int) (fabs (mpfr_get_d (r, GMP_RNDN)) * 128.0);
        if (p != 0 && MPFR_SIGN(r) < 0)
          p = -p;
        if (p != q[i][j])
          {
            printf ("Error in mpfr_pow for particular case (%d,%d):\n"
                    "got %d instead of %d\n", i, j, p, q[i][j]);
            error = 1;
          }
      }

  for (i = 0; i < 11; i++)
    mpfr_clear (t[i]);
  mpfr_clear (r);

  if (error)
    exit (1);
}

static void
underflows(void)
{
  mpfr_t x, y;
  int i;

  mpfr_init2 (x, 64);
  mpfr_init2 (y, 64);

  mpfr_set_ui (x, 1, GMP_RNDN);
  mpfr_set_exp (x, mpfr_get_emin());

  for (i = 3; i < 10; i++)
    {
      mpfr_set_ui (y, i, GMP_RNDN);
      mpfr_div_2ui (y, y, 1, GMP_RNDN);
      mpfr_pow (y, x, y, GMP_RNDN);
      if (!MPFR_IS_FP(y) || mpfr_cmp_ui (y, 0))
        {
          printf ("Error in mpfr_pow for ");
          mpfr_out_str (stdout, 2, 0, x, GMP_RNDN);
          printf (" ^ (%d/2)\nGot ", i);
          mpfr_out_str (stdout, 2, 0, y, GMP_RNDN);
          printf (" instead of 0.\n");
          exit (1);
        }
    }

  mpfr_clear (x);
  mpfr_clear (y);
}

int
main (void)
{
  mp_prec_t p;

  tests_start_mpfr ();

  special ();

  particular_cases ();

  check_pow_ui ();

  for (p=2; p<100; p++)
    check_inexact (p);

  /* underflows (); */

  tests_end_mpfr ();
  return 0;
}
