/* Test file for mpfr_sqrt.

Copyright 1999, 2001, 2002 Free Software Foundation, Inc.

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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#include "mpfr-test.h"

#define check(a,r) check3(a,r,-1.0)

int maxulp=0;

void check3 _PROTO((double, mp_rnd_t, double)); 
void check4 _PROTO((double, mp_rnd_t, char *)); 
void check24 _PROTO((float, mp_rnd_t, float)); 
void check_float _PROTO((void)); 
void special _PROTO((void));
void check_inexact _PROTO((mp_prec_t));
void check_nan _PROTO((void));

void
check3 (double a, mp_rnd_t rnd_mode, double Q)
{
  mpfr_t q; double Q2; int ck,u;

  ck = (Q!=-1.0); /* if ck=1, then Q is certified correct */
  mpfr_init2(q, 53);
  mpfr_set_d(q, a, rnd_mode);
#ifdef MPFR_HAVE_FESETROUND
  mpfr_set_machine_rnd_mode(rnd_mode);
#endif
  mpfr_sqrt(q, q, rnd_mode);
  if (ck==0) Q = sqrt(a);
  else {
    if (Q != sqrt(a) && (!isnan(Q) || !isnan(sqrt(a)))) {
      fprintf(stderr, "you've found a bug in your machine's sqrt for x=%1.20e\n", a);
      mpfr_clear(q); 
      exit(1);

    }
  }
  Q2 = mpfr_get_d1 (q);
  if (Q!=Q2 && (!isnan(Q) || !isnan(Q2))) {
    u = ulp(Q2,Q);
    if (ck) {
      printf("mpfr_sqrt failed for a=%1.20e, rnd_mode=%s\n",
	     a, mpfr_print_rnd_mode(rnd_mode));
      printf("expected sqrt is %1.20e, got %1.20e (%d ulp)\n",Q,Q2,u);
      mpfr_clear(q); 
      exit(1);
    }
    else if (u>maxulp || u<-maxulp) {
      maxulp = (u>maxulp) ? u : -u;
      printf("libm.a differs from mpfr_sqrt for a=%1.20e, rnd_mode=%s\n",
	     a, mpfr_print_rnd_mode(rnd_mode));
      printf("libm.a gives %1.20e, mpfr_sqrt gives %1.20e (%d ulp)\n",Q,Q2,u);
    }
  }
  mpfr_clear(q);
}

void
check4 (double a, mp_rnd_t rnd_mode, char *Q)
{
  mpfr_t q, res;

  mpfr_init2(q, 53); mpfr_init2(res, 53);
  mpfr_set_d(q, a, rnd_mode);
  mpfr_sqrt(q, q, rnd_mode);
  mpfr_set_str(res, Q, 16, GMP_RNDN);
  if (mpfr_cmp(q, res)) {
      printf("mpfr_sqrt failed for a=%1.20e, rnd_mode=%s\n",
	     a, mpfr_print_rnd_mode(rnd_mode));
      printf("expected "); mpfr_print_binary(res); putchar('\n');
      printf("got      "); mpfr_print_binary(q); putchar('\n');
      mpfr_clear(q); mpfr_clear(res); 
      exit(1);
  }
  mpfr_clear(res); 
  mpfr_clear(q);
}

void
check24 (float a, mp_rnd_t rnd_mode, float Q)
{
  mpfr_t q; float Q2;

  mpfr_init2(q, 24);
  mpfr_set_d(q, a, rnd_mode);
  mpfr_sqrt(q, q, rnd_mode);
  Q2 = mpfr_get_d1 (q);
  if (Q!=Q2) {
    printf("mpfr_sqrt failed for a=%1.10e, prec=24, rnd_mode=%s\n",
	   a, mpfr_print_rnd_mode(rnd_mode));
    printf("expected sqrt is %1.10e, got %1.10e\n",Q,Q2);
    exit(1);
  }
  mpfr_clear(q);
}

/* the following examples come from the paper "Number-theoretic Test 
   Generation for Directed Rounding" from Michael Parks, Table 3 */
void
check_float (void)
{
  float b = 8388608.0; /* 2^23 */

  check24(b*8388610.0, GMP_RNDN, 8.388609e6);
  check24(b*2.0*16777214.0, GMP_RNDN, 1.6777215e7);
  check24(b*8388612.0, GMP_RNDN, 8.388610e6);
  check24(b*2.0*16777212.0, GMP_RNDN, 1.6777214e7);
  check24(b*11946704.0, GMP_RNDN, 1.0010805e7);
  check24(b*14321479.0, GMP_RNDN, 1.0960715e7);
  check24(b*2.0*13689673.0, GMP_RNDN, 1.5155019e7);
  check24(b*8388614.0, GMP_RNDN, 8.388611e6);
  check24(b*2.0*16777210.0, GMP_RNDN, 1.6777213e7);
  check24(b*10873622.0, GMP_RNDN, 9.550631e6);

  check24(b*8388610.0, GMP_RNDZ, 8.388608e6);
  check24(b*2.0*16777214.0, GMP_RNDZ, 1.6777214e7);
  check24(b*8388612.0, GMP_RNDZ, 8.388609e6);
  check24(b*2.0*16777212.0, GMP_RNDZ, 1.6777213e7);
  check24(b*11946704.0, GMP_RNDZ, 1.0010805e7);
  check24(b*14321479.0, GMP_RNDZ, 1.0960715e7);
  check24(b*2.0*13689673.0, GMP_RNDZ, 1.5155019e7);
  check24(b*8388614.0, GMP_RNDZ, 8.38861e6);
  check24(b*2.0*16777210.0, GMP_RNDZ, 1.6777212e7);
  check24(b*10873622.0, GMP_RNDZ, 9.550631e6);

  check24(b*8388610.0, GMP_RNDU, 8.388609e6);
  check24(b*2.0*16777214.0, GMP_RNDU, 1.6777215e7);
  check24(b*8388612.0, GMP_RNDU, 8.388610e6);
  check24(b*2.0*16777212.0, GMP_RNDU, 1.6777214e7);
  check24(b*11946704.0, GMP_RNDU, 1.0010806e7);
  check24(b*14321479.0, GMP_RNDU, 1.0960716e7);
  check24(b*2.0*13689673.0, GMP_RNDU, 1.515502e7);
  check24(b*8388614.0, GMP_RNDU, 8.388611e6);
  check24(b*2.0*16777210.0, GMP_RNDU, 1.6777213e7);
  check24(b*10873622.0, GMP_RNDU, 9.550632e6);

  check24(b*8388610.0, GMP_RNDD, 8.388608e6);
  check24(b*2.0*16777214.0, GMP_RNDD, 1.6777214e7);
  check24(b*8388612.0, GMP_RNDD, 8.388609e6);
  check24(b*2.0*16777212.0, GMP_RNDD, 1.6777213e7);
  check24(b*11946704.0, GMP_RNDD, 1.0010805e7);
  check24(b*14321479.0, GMP_RNDD, 1.0960715e7);
  check24(b*2.0*13689673.0, GMP_RNDD, 1.5155019e7);
  check24(b*8388614.0, GMP_RNDD, 8.38861e6);
  check24(b*2.0*16777210.0, GMP_RNDD, 1.6777212e7);
  check24(b*10873622.0, GMP_RNDD, 9.550631e6);
}

void
special (void)
{
  mpfr_t x, z;
  int inexact;
  mp_prec_t p;

  mpfr_init (x);
  mpfr_init (z);

  mpfr_set_prec (x, 27);
  mpfr_set_str_raw (x, "0.110100111010101000010001011");
  if ((inexact = mpfr_sqrt (x, x, GMP_RNDZ)) >= 0)
    {
      fprintf (stderr, "Wrong inexact flag: expected -1, got %d\n", inexact);
      exit (1);
    }

  mpfr_set_prec (x, 2);
  for (p=2; p<1000; p++)
    {
      mpfr_set_prec (z, p);
      mpfr_set_ui (z, 1, GMP_RNDN);
      mpfr_add_one_ulp (z, GMP_RNDN);
      mpfr_sqrt (x, z, GMP_RNDU);
      if (mpfr_get_d1 (x) != 1.5)
	{
	  fprintf (stderr, "Error: sqrt(1+ulp(1), up) should give 1.5 (prec=%u)\n", (unsigned) p);
	  printf ("got "); mpfr_print_binary (x); putchar ('\n');
	  exit (1);
	}
    }

  /* check inexact flag */
  mpfr_set_prec (x, 5);
  mpfr_set_str_raw (x, "1.1001E-2");
  if ((inexact = mpfr_sqrt (x, x, GMP_RNDN)))
    {
      fprintf (stderr, "Wrong inexact flag: expected 0, got %d\n", inexact);
      exit (1);
    }

  mpfr_set_prec (x, 2);
  mpfr_set_prec (z, 2);

  /* checks the sign is correctly set */
  mpfr_set_d (x, 1.0, GMP_RNDN);
  mpfr_set_d (z, -1.0, GMP_RNDN);
  mpfr_sqrt (z, x, GMP_RNDN);
  if (mpfr_cmp_ui (z, 0) < 0) {
    fprintf (stderr, "Error: square root of %e gives %e\n", 
	     mpfr_get_d1 (x), mpfr_get_d1 (z));
    exit (1);
  }

  mpfr_set_prec (x, 192);
  mpfr_set_prec (z, 160);
  mpfr_set_str_raw (z, "0.1011010100000100100100100110011001011100100100000011000111011001011101101101110000110100001000100001100001011000E1");
  mpfr_set_prec (x, 160);
  mpfr_sqrt(x, z, GMP_RNDN);
  mpfr_sqrt(z, x, GMP_RNDN);

  mpfr_clear (x);
  mpfr_clear (z);
}

void
check_inexact (mp_prec_t p)
{
  mpfr_t x, y, z;
  mp_rnd_t rnd;
  int inexact, sign;

  mpfr_init2 (x, p);
  mpfr_init2 (y, p);
  mpfr_init2 (z, 2*p);
  mpfr_random (x);
  rnd = LONG_RAND() % 4;
  inexact = mpfr_sqrt (y, x, rnd);
  if (mpfr_mul (z, y, y, rnd)) /* exact since prec(z) = 2*prec(y) */
    {
      fprintf (stderr, "Error: multiplication should be exact\n");
      exit (1);
    }
  mpfr_sub (z, z, x, rnd); /* exact also */
  sign = mpfr_cmp_ui (z, 0);
  if (((inexact == 0) && (sign)) ||
      ((inexact > 0) && (sign <= 0)) ||
      ((inexact < 0) && (sign >= 0)))
    {
      fprintf (stderr, "Error: wrong inexact flag, expected %d, got %d\n",
	       sign, inexact);
      printf ("x=");
      mpfr_print_binary (x);
      printf (" rnd=%s\n", mpfr_print_rnd_mode (rnd));
      printf ("y="); mpfr_print_binary (y); putchar ('\n');
      exit (1);
    }
  mpfr_clear (x);
  mpfr_clear (y);
  mpfr_clear (z);
}

void
check_nan (void)
{
  mpfr_t  x, got;

  mpfr_init2 (x, 100L);
  mpfr_init2 (got, 100L);

  /* sqrt(NaN) == NaN */
  MPFR_CLEAR_FLAGS (x);
  MPFR_SET_NAN (x);
  ASSERT_ALWAYS (mpfr_sqrt (got, x, GMP_RNDZ) == 0); /* exact */
  ASSERT_ALWAYS (mpfr_nan_p (got));

  /* sqrt(-1) == NaN */
  mpfr_set_si (x, -1L, GMP_RNDZ);
  ASSERT_ALWAYS (mpfr_sqrt (got, x, GMP_RNDZ) == 0); /* exact */
  ASSERT_ALWAYS (mpfr_nan_p (got));

  /* sqrt(+inf) == +inf */
  MPFR_CLEAR_FLAGS (x);
  MPFR_SET_INF (x);
  MPFR_SET_POS (x);
  ASSERT_ALWAYS (mpfr_sqrt (got, x, GMP_RNDZ) == 0); /* exact */
  ASSERT_ALWAYS (mpfr_inf_p (got));

  /* sqrt(-inf) == NaN */
  MPFR_CLEAR_FLAGS (x);
  MPFR_SET_INF (x);
  MPFR_SET_NEG (x);
  ASSERT_ALWAYS (mpfr_sqrt (got, x, GMP_RNDZ) == 0); /* exact */
  ASSERT_ALWAYS (mpfr_nan_p (got));

  /* sqrt(-0) == 0 */
  mpfr_set_si (x, 0L, GMP_RNDZ);
  MPFR_SET_NEG (x);
  ASSERT_ALWAYS (mpfr_sqrt (got, x, GMP_RNDZ) == 0); /* exact */
  ASSERT_ALWAYS (mpfr_number_p (got));
  ASSERT_ALWAYS (mpfr_cmp_ui (got, 0L) == 0);

  mpfr_clear (x);
  mpfr_clear (got);
}

double five = 5.0;

int
main (void)
{
  double a;
  mp_prec_t p;
  int k;
#ifdef MPFR_HAVE_FESETROUND
  int i;

  mpfr_test_init ();

  /* On Debian potato glibc 2.1.3-18, sqrt() doesn't seem to respect
     fesetround. */
  {
    double  a, b;
    mpfr_set_machine_rnd_mode (GMP_RNDU);
    a = sqrt (five);
    mpfr_set_machine_rnd_mode (GMP_RNDD);
    b = sqrt (five);
    if (a == b)
      {
        printf ("Tests suppressed, mpfr_set_machine_rnd_mode doesn't affect sqrt()\n");
        goto nogood;
      }
  }

  SEED_RAND (time(NULL));
  for (i=0;i<100000;i++)
    {
      a = drand();
      if (a < 0.0) a = -a; /* ensures a is positive */
      check (a, LONG_RAND() % 4);
    }
 nogood:
#endif

  check_nan ();

  for (p=2; p<200; p++)
    for (k=0; k<200; k++)
      check_inexact (p);
  special ();
  check_float();
#ifdef HAVE_INFS
  check3 (DBL_NAN, GMP_RNDN, DBL_NAN); 
  check3 (-1.0, GMP_RNDN, DBL_NAN); 
  check3 (DBL_POS_INF, GMP_RNDN, DBL_POS_INF); 
  check3 (DBL_NEG_INF, GMP_RNDN, DBL_NAN);
#endif
  check3(-0.0, GMP_RNDN, 0.0); 
  check4(6.37983013646045901440e+32, GMP_RNDN, "5.9bc5036d09e0c@13");
  check4(1.0, GMP_RNDN, "1");
  check4(1.0, GMP_RNDZ, "1");
  check4(3.725290298461914062500000e-9, GMP_RNDN, "4@-4");
  check4(3.725290298461914062500000e-9, GMP_RNDZ, "4@-4");
  a=1190456976439861.0;
  check4(a, GMP_RNDZ, "2.0e7957873529a@6");
  check4(1024.0*a, GMP_RNDZ, "4.1cf2af0e6a534@7");
  /* the following examples are bugs in Cygnus compiler/system, found by
     Fabrice Rouillier while porting mpfr to Windows */
  check4(9.89438396044940256501e-134, GMP_RNDU, "8.7af7bf0ebbee@-56");
  check4(7.86528588050363751914e+31, GMP_RNDZ, "1.f81fc40f32062@13");
  check4(0.99999999999999988897, GMP_RNDN, "f.ffffffffffff8@-1");
  check4(1.00000000000000022204, GMP_RNDN, "1");
  /* the following examples come from the paper "Number-theoretic Test 
   Generation for Directed Rounding" from Michael Parks, Table 4 */
  a = 4503599627370496.0; /* 2^52 */

  check4(a*2.0*8732221479794286.0, GMP_RNDN, "1.f81fc40f32063@13");
  check4(a*8550954388695124.0, GMP_RNDN, "1.60c012a92fc65@13");
  check4(a*7842344481681754.0, GMP_RNDN, "1.51d17526c7161@13");
  check4(a*5935035262218600.0, GMP_RNDN, "1.25e19302f7e51@13");
  check4(a*5039650445085418.0, GMP_RNDN, "1.0ecea7dd2ec3d@13");
  check4(a*5039721545366078.0, GMP_RNDN, "1.0ecf250e8e921@13");
  check4(a*8005963117781324.0, GMP_RNDN, "1.5552f3eedcf33@13");
  check4(a*6703494707970582.0, GMP_RNDN, "1.3853ee10c9c99@13");
  check4(a*8010323124937260.0, GMP_RNDN, "1.556abe212b56f@13");
  check4(a*2.0*8010776873384260.0, GMP_RNDN, "1.e2d9a51977e6e@13");

  check4(a*2.0*8732221479794286.0, GMP_RNDZ, "1.f81fc40f32062@13");
  check4(a*8550954388695124.0, GMP_RNDZ, "1.60c012a92fc64@13");
  check4(a*7842344481681754.0, GMP_RNDZ, "1.51d17526c716@13");
  check4(a*5935035262218600.0, GMP_RNDZ, "1.25e19302f7e5@13");
  check4(a*5039650445085418.0, GMP_RNDZ, "1.0ecea7dd2ec3c@13");
  check4(a*5039721545366078.0, GMP_RNDZ, "1.0ecf250e8e92@13");
  check4(a*8005963117781324.0, GMP_RNDZ, "1.5552f3eedcf32@13");
  check4(a*6703494707970582.0, GMP_RNDZ, "1.3853ee10c9c98@13");
  check4(a*8010323124937260.0, GMP_RNDZ, "1.556abe212b56e@13");
  check4(a*2.0*8010776873384260.0, GMP_RNDZ, "1.e2d9a51977e6d@13");

  check4(a*2.0*8732221479794286.0, GMP_RNDU, "1.f81fc40f32063@13");
  check4(a*8550954388695124.0, GMP_RNDU, "1.60c012a92fc65@13");
  check4(a*7842344481681754.0, GMP_RNDU, "1.51d17526c7161@13");
  check4(a*5935035262218600.0, GMP_RNDU, "1.25e19302f7e51@13");
  check4(a*5039650445085418.0, GMP_RNDU, "1.0ecea7dd2ec3d@13");
  check4(a*5039721545366078.0, GMP_RNDU, "1.0ecf250e8e921@13");
  check4(a*8005963117781324.0, GMP_RNDU, "1.5552f3eedcf33@13");
  check4(a*6703494707970582.0, GMP_RNDU, "1.3853ee10c9c99@13");
  check4(a*8010323124937260.0, GMP_RNDU, "1.556abe212b56f@13");
  check4(a*2.0*8010776873384260.0, GMP_RNDU, "1.e2d9a51977e6e@13");

  check4(a*2.0*8732221479794286.0, GMP_RNDD, "1.f81fc40f32062@13");
  check4(a*8550954388695124.0, GMP_RNDD, "1.60c012a92fc64@13");
  check4(a*7842344481681754.0, GMP_RNDD, "1.51d17526c716@13");
  check4(a*5935035262218600.0, GMP_RNDD, "1.25e19302f7e5@13");
  check4(a*5039650445085418.0, GMP_RNDD, "1.0ecea7dd2ec3c@13");
  check4(a*5039721545366078.0, GMP_RNDD, "1.0ecf250e8e92@13");
  check4(a*8005963117781324.0, GMP_RNDD, "1.5552f3eedcf32@13");
  check4(a*6703494707970582.0, GMP_RNDD, "1.3853ee10c9c98@13");
  check4(a*8010323124937260.0, GMP_RNDD, "1.556abe212b56e@13");
  check4(a*2.0*8010776873384260.0, GMP_RNDD, "1.e2d9a51977e6d@13");

  return 0;
}
