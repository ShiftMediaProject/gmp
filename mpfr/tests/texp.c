/* Test file for mpfr_exp.

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

int maxu=0;

#define check(d, r) check3(d, r, 0.0)

/* returns the number of ulp of error */
static int
check3 (double d, mp_rnd_t rnd, double e)
{
  mpfr_t x, y;
  double f;
  int u = 0;

  mpfr_init2 (x, 53);
  mpfr_init2 (y, 53);
  mpfr_set_d (x, d, rnd);
  mpfr_exp (y, x, rnd);
  f = mpfr_get_d1 (y);
  if (f != e && !(Isnan(f) && Isnan(e)))
    {
      u = ulp (e, f);
      if (u < 0)
        {
          if (u == (mp_limb_t) 1 << (mp_bits_per_limb-1))
            u += 1;
          u = -u;
        }
      if (u != 0)
        {
          printf ("mpfr_exp failed for x=%1.20e, rnd=%s\n", d,
                  mpfr_print_rnd_mode (rnd));
          printf ("expected result is %1.20e, got %1.20e, dif=%d ulp\n",
                  e, f, u);
          exit (1);
        }
    }
  mpfr_clear (x);
  mpfr_clear (y);
  return u;
}

/* computes n bits of exp(d) */
static int
check_large (double d, int n, mp_rnd_t rnd)
{
  mpfr_t x, y;

  mpfr_init2 (x, n);
  mpfr_init2 (y, n);
  if (d == 0.0)
    { /* try exp(Pi*sqrt(163)/3)-640320 */
      mpfr_set_d (x, 163.0, rnd);
      mpfr_sqrt (x, x, rnd);
      mpfr_const_pi (y, rnd);
      mpfr_mul (x, x, y, rnd);
      mpfr_div_ui (x, x, 3, rnd);
    }
  else
    mpfr_set_d (x, d, rnd);
  mpfr_exp (y, x, rnd);
  if (d == 0.0)
    {
      mpfr_set_d (x, 640320.0, rnd);
      mpfr_sub (y, y, x, rnd);
      printf ("exp(Pi*sqrt(163)/3)-640320=");
    }
  else
    printf ("exp(%1.20e)=", d);
  mpfr_out_str (stdout, 10, 0, y, rnd);
  puts ("");
  printf (" =");
  mpfr_print_binary (y);
  puts ("");
  if (n == 53)
    printf (" =%1.20e\n", mpfr_get_d1 (y));

  mpfr_clear (x);
  mpfr_clear (y);

  return 0;
}

/* expx is the value of exp(X) rounded towards -infinity */
static int
check_worst_case (double X, double expx)
{
  mpfr_t x, y;

  mpfr_init2(x, 53); mpfr_init2(y, 53);
  mpfr_set_d(x, X, GMP_RNDN);
  mpfr_exp(y, x, GMP_RNDD);
  if (mpfr_get_d1 (y) != expx)
    {
      printf ("exp(x) rounded towards -infinity is wrong\n");
      exit(1);
    }
  mpfr_exp(x, x, GMP_RNDN);
  mpfr_set_d(x, X, GMP_RNDN);
  mpfr_exp(x, x, GMP_RNDU);
  mpfr_add_one_ulp(y, GMP_RNDN);
  if (mpfr_cmp(x,y))
    {
      printf ("exp(x) rounded towards +infinity is wrong\n");
      exit(1);
    }
  mpfr_clear(x); mpfr_clear(y);
  return 0;
}

/* worst cases communicated by Jean-Michel Muller and Vincent Lefevre */
static int
check_worst_cases (void)
{
  mpfr_t x; mpfr_t y;

  mpfr_init(x);
  mpfr_set_prec (x, 53);

  check_worst_case(4.44089209850062517562e-16, 1.00000000000000022204);
  check_worst_case(6.39488462184069720009e-14, 1.00000000000006372680);
  check_worst_case(1.84741111297455401935e-12, 1.00000000000184718907);
  check_worst_case(1.76177628026265550074e-10, 1.00000000017617751702);
  check3(1.76177628026265550074e-10, GMP_RNDN, 1.00000000017617773906);
  check_worst_case(7.54175277499595900852e-10, 1.00000000075417516676);
  check3(7.54175277499595900852e-10, GMP_RNDN, 1.00000000075417538881);
  /* bug found by Vincent Lefe`vre on December 8, 1999 */
  check3(-5.42410311287441459172e+02, GMP_RNDN, 2.7176584868845723e-236);
  /* further cases communicated by Vincent Lefe`vre on January 27, 2000 */
  check3(-1.32920285897904911589e-10, GMP_RNDN, 0.999999999867079769622);
  check3(-1.44037948245738330735e-10, GMP_RNDN, 0.9999999998559621072757);
  check3(-1.66795910430705305937e-10, GMP_RNDZ, 0.9999999998332040895832);
  check3(-1.64310953745426656203e-10, GMP_RNDN, 0.9999999998356891017792);
  check3(-1.38323574826034659172e-10, GMP_RNDZ, 0.9999999998616764251835);
  check3(-1.23621668465115401498e-10, GMP_RNDZ, 0.9999999998763783315425);

  mpfr_set_prec (x, 601);
  mpfr_set_str (x, "0.88b6ba510e10450edc258748bc9dfdd466f21b47ed264cdf24aa8f64af1f3fad9ec2301d43c0743f534b5aa20091ff6d352df458ef1ba519811ef6f5b11853534fd8fa32764a0a6d2d0dd20@0", 16, GMP_RNDZ);
  mpfr_init2 (y, 601);
  mpfr_exp_2 (y, x, GMP_RNDD);
  mpfr_exp3 (x, x, GMP_RNDD);
  if (mpfr_cmp (x, y))
    {
      printf ("mpfr_exp_2 and mpfr_exp3 differ for prec=601\n");
      printf ("mpfr_exp_2 gives ");
      mpfr_out_str (stdout, 2, 0, y, GMP_RNDN);
      printf ("\nmpfr_exp_3 gives ");
      mpfr_out_str (stdout, 2, 0, x, GMP_RNDN);
      printf ("\n");
      exit (1);
    }

  mpfr_clear (x);
  mpfr_clear (y);
  return 0;
}

static void
compare_exp2_exp3 (int n)
{
  mpfr_t x, y, z; int prec; mp_rnd_t rnd;

  mpfr_init (x);
  mpfr_init (y);
  mpfr_init (z);
  for (prec=20; prec<=n; prec++)
    {
      mpfr_set_prec (x, prec);
      mpfr_set_prec (y, prec);
      mpfr_set_prec (z, prec);
      mpfr_random (x);
      rnd = randlimb () % 4;
      mpfr_exp_2 (y, x, rnd);
      mpfr_exp3 (z, x, rnd);
      if (mpfr_cmp (y,z))
        {
          printf ("mpfr_exp_2 and mpfr_exp3 disagree for rnd=%s and\nx=",
                  mpfr_print_rnd_mode (rnd));
          mpfr_print_binary (x);
          puts ("");
          printf ("mpfr_exp_2 gives  ");
          mpfr_print_binary (y);
          puts ("");
          printf ("mpfr_exp3 gives ");
          mpfr_print_binary (z);
          puts ("");
          exit (1);
        }
  }
  mpfr_clear (x);
  mpfr_clear (y);
  mpfr_clear (z);
}

#define TEST_FUNCTION mpfr_exp
#include "tgeneric.c"

static void
check_special ()
{
  mpfr_t x, y, z;

  /* bug reported by Franky Backeljauw, 28 Mar 2003 */
  mpfr_init2 (x, 53);
  mpfr_init2 (y, 53);
  mpfr_set_str_binary (x, "1.1101011000111101011110000111010010101001101001110111e28");
  mpfr_exp (y, x, GMP_RNDN);

  mpfr_set_prec (x, 153);
  mpfr_init2 (z, 153);
  mpfr_set_str_binary (x, "1.1101011000111101011110000111010010101001101001110111e28");
  mpfr_exp (z, x, GMP_RNDN);
  mpfr_prec_round (z, 53, GMP_RNDN);

  if (mpfr_cmp (y, z))
    {
      printf ("Error in mpfr_exp for large argument\n");
      exit (1);
    }

  mpfr_clear (x);
  mpfr_clear (y);
  mpfr_clear (z);
}

int
main (int argc, char *argv[])
{
  double d;

  tests_start_mpfr ();

  check_special ();

  if (argc == 4)
    {
      check_large (atof(argv[1]), atoi(argv[2]), atoi(argv[3]));
      exit (0);
    }

  test_generic (2, 100, 100);

  compare_exp2_exp3(500);
  check_worst_cases();
  check3(0.0, GMP_RNDU, 1.0);
  check3(-8.88024741073346941839e-17, GMP_RNDU, 1.0);
  check3(8.70772839244701057915e-01, GMP_RNDN, 2.38875626491680437269);
  check3(1.0, GMP_RNDN, 2.71828182845904509080);
  check3(-3.42135637628104173534e-07, GMP_RNDZ, 0.999999657864420798958);
  /* worst case for argument reduction, very near from 5*log(2),
     thanks to Jean-Michel Muller
   */
  check3(3.4657359027997265421, GMP_RNDN, 32.0);
  check3(3.4657359027997265421, GMP_RNDU, 32.0);
  check3(3.4657359027997265421, GMP_RNDD, 31.999999999999996447);
  check3(2.26523754332090625496e+01, GMP_RNDD, 6.8833785261699581146e9);
  check3(1.31478962104089092122e+01, GMP_RNDZ, 5.12930793917860137299e+05);
  check3(4.25637507920002378103e-01, GMP_RNDU, 1.53056585656161181497e+00);
  check3(6.26551618962329307459e-16, GMP_RNDU, 1.00000000000000066613e+00);
  check3(-3.35589513871216568383e-03, GMP_RNDD, 9.96649729583626853291e-01);
  check3(1.95151388850007272424e+01, GMP_RNDU, 2.98756340674767792225e+08);
  check3(2.45045953503350730784e+01, GMP_RNDN, 4.38743344916128387451e+10);
  check3(2.58165606081678085104e+01, GMP_RNDD, 1.62925781879432281494e+11);
  check3(-2.36539020084338638128e+01, GMP_RNDZ, 5.33630792749924762447e-11);
  check3(2.39211946135858077866e+01, GMP_RNDU, 2.44817704330214385986e+10);
  check3(-2.78190533055889162029e+01, GMP_RNDZ, 8.2858803483596879512e-13);
  check3(2.64028186174889789584e+01, GMP_RNDD, 2.9281844652878973388e11);
  check3(2.92086338843268329413e+01, GMP_RNDZ, 4.8433797301907177734e12);
  check3(-2.46355324071459982349e+01, GMP_RNDZ, 1.9995129297760994791e-11);
  check3(-2.23509444608605427618e+01, GMP_RNDZ, 1.9638492867489702307e-10);
  check3(-2.41175390197331687148e+01, GMP_RNDD, 3.3564940885530624592e-11);
  check3(2.46363885231578088053e+01, GMP_RNDU, 5.0055014282693267822e10);
  d = 7819821913254249.0 / 70368744177664.0;
  check3(d, GMP_RNDN, 1.8262572323517295459e48);
  check3(-3.56196340354684821250e+02, GMP_RNDN, 2.0225297096141478156e-155);
  check3(6.59678273772710895173e+02, GMP_RNDU, 3.1234469273830195529e286);
  check3(5.13772529701934331570e+02, GMP_RNDD, 1.3445427121297197752e223);
  check3(3.57430211008718345056e+02, GMP_RNDD, 1.6981197246857298443e155);
  check3(3.82001814471465536371e+02, GMP_RNDU, 7.9667300591087367805e165);
  check3(5.92396038219384422518e+02, GMP_RNDD, 1.880747529554661989e257);
  check3(-5.02678550462488090034e+02, GMP_RNDU, 4.8919201895446217839e-219);
  check3(5.30015757134837031117e+02, GMP_RNDD, 1.5237672861171573939e230);
  check3(5.16239362447650933063e+02, GMP_RNDZ, 1.5845518406744492105e224);
  check3(6.00812634798592370977e-01, GMP_RNDN, 1.823600119339019443);

  tests_end_mpfr ();
  return 0;
}
