/* Test file for mpfr_agm.

Copyright 1999, 2001, 2002, 2003 Free Software Foundation, Inc.

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
#include <time.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#include "mpfr-test.h"

#define check(a,b,r) check4(a,b,r,0.0)

static void
check4 (double a, double b, mp_rnd_t rnd_mode, double res1)
{
  mpfr_t ta, tb, tres;
  double res2;

  mpfr_init2(ta, 53);
  mpfr_init2(tb, 53);
  mpfr_init2(tres, 53);

  mpfr_set_d(ta, a, rnd_mode);
  mpfr_set_d(tb, b, rnd_mode);

  mpfr_agm(tres, ta, tb, rnd_mode);

  res2 = mpfr_get_d1 (tres);

  if (res1!=res2 && !(Isnan(res1) && Isnan(res2)))
    {
      printf ("mpfr_agm failed for a=%1.20e, b=%1.20e, rnd_mode=%d\n",a,b,rnd_mode);
      printf ("expected result is %1.20e, got %1.20e (%d ulp)\n",res1,res2,
              ulp(res2,res1));
      exit (1);
  }
  mpfr_clear (ta);
  mpfr_clear (tb);
  mpfr_clear (tres);
}

static void
check_large (void)
{
  mpfr_t a, b, agm;

  mpfr_init2 (a, 82);
  mpfr_init2 (b, 82);
  mpfr_init2 (agm, 82);

  mpfr_set_ui (a, 1, GMP_RNDN);
  mpfr_set_str_binary (b, "0.1111101100001000000001011000110111101000001011111000100001000101010100011111110010E-39");
  mpfr_agm (agm, a, b, GMP_RNDN);
  mpfr_set_str_binary (a, "0.1110001000111101101010101010101101001010001001001011100101111011110101111001111100E-4");
  if (mpfr_cmp (agm, a))
    {
      printf ("mpfr_agm failed for precision 82\n");
      exit (1);
    }

  /* problem found by Damien Fischer <damien@maths.usyd.edu.au> 4 Aug 2003:
     produced a division by zero exception */
  mpfr_set_prec (a, 268);
  mpfr_set_prec (b, 268);
  mpfr_set_prec (agm, 268);
  mpfr_set_str (a, "703.93543315330225238487276503953366664991725089988315253092140138947103394917006", 10, GMP_RNDN);
  mpfr_set_str (b, "703.93543315330225238487279020523738740563816490895994499256063816906728642622316", 10, GMP_RNDN);
  mpfr_agm (agm, a, b, GMP_RNDN);

  mpfr_clear (a);
  mpfr_clear (b);
  mpfr_clear (agm);
}

static void
check_nans (void)
{
  mpfr_t  x, y, m;

  mpfr_init2 (x, 123L);
  mpfr_init2 (y, 123L);
  mpfr_init2 (m, 123L);

  /* agm(1,nan) == nan */
  mpfr_set_ui (x, 1L, GMP_RNDN);
  mpfr_set_nan (y);
  mpfr_agm (m, x, y, GMP_RNDN);
  ASSERT_ALWAYS (mpfr_nan_p (m));

  /* agm(1,+inf) == +inf */
  mpfr_set_ui (x, 1L, GMP_RNDN);
  mpfr_set_inf (y, 1);
  mpfr_agm (m, x, y, GMP_RNDN);
  ASSERT_ALWAYS (mpfr_inf_p (m));
  ASSERT_ALWAYS (mpfr_sgn (m) > 0);

  /* agm(+inf,+inf) == +inf */
  mpfr_set_inf (x, 1);
  mpfr_set_inf (y, 1);
  mpfr_agm (m, x, y, GMP_RNDN);
  ASSERT_ALWAYS (mpfr_inf_p (m));
  ASSERT_ALWAYS (mpfr_sgn (m) > 0);

  /* agm(-inf,+inf) == nan */
  mpfr_set_inf (x, -1);
  mpfr_set_inf (y, 1);
  mpfr_agm (m, x, y, GMP_RNDN);
  ASSERT_ALWAYS (mpfr_nan_p (m));

  mpfr_clear (x);
  mpfr_clear (y);
  mpfr_clear (m);
}

int
main (int argc, char* argv[])
{
   tests_start_mpfr ();

   check_nans ();

   if (argc == 2) /* tagm N: N tests with random double's */
     {
       int N, i;
       double a, b;

       N = atoi (argv[1]);
       for (i = 0; i < N; i++)
         {
           a = DBL_RAND ();
           b = DBL_RAND ();
           check(a, b, randlimb () % 4);
         }
       return 0;
     }
   else
     {
       check_large ();
       check4 (2.0, 1.0, GMP_RNDN, 1.45679103104690677029);
       check4 (6.0, 4.0, GMP_RNDN, 4.94936087247260925182);
       check4 (62.0, 61.0, GMP_RNDN, 6.14989837188450749750e+01);
       check4 (0.5, 1.0, GMP_RNDN, 7.28395515523453385143e-01);
       check4 (1.0, 2.0, GMP_RNDN, 1.45679103104690677029);
       check4 (234375765.0, 234375000.0, GMP_RNDN, 2.3437538249984395504e8);
       check4 (8.0, 1.0, GMP_RNDU, 3.615756177597362786);
       check4 (1.0, 44.0, GMP_RNDU, 1.33658354512981247808e1);
       check4 (1.0, 3.7252902984619140625e-9, GMP_RNDU, 7.55393356971199025907e-02);
     }

   tests_end_mpfr ();

   return 0;
}
