/* Test file for in-place operations.

Copyright (C) 2000 Free Software Foundation.

This file is part of the MPFR Library.

The MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include "gmp.h"
#include "mpfr.h"
#include "mpfr-impl.h"

void (*testfunc) () = NULL;
void test3 (char *, mp_prec_t, mp_rnd_t);
void test3a (char *, mp_prec_t, mp_rnd_t);
void test2ui (char *, mp_prec_t, mp_rnd_t);
void testui2 (char *, mp_prec_t, mp_rnd_t);
void test2 (char *, mp_prec_t, mp_rnd_t);
void test2a (char *, mp_prec_t);
int mpfr_compare (mpfr_t, mpfr_t);

/* same than mpfr_cmp, but returns 0 for both NaN's */
int mpfr_compare (mpfr_t a, mpfr_t b)
{
  return (MPFR_IS_NAN(a)) ? !MPFR_IS_NAN(b) : mpfr_cmp(a, b);
}

void test3 (char *foo, mp_prec_t prec, mp_rnd_t rnd)
{
  mpfr_t ref1, ref2, ref3;
  mpfr_t res1;
  int i;

#ifdef DEBUG
  printf("checking %s\n", foo);
#endif
  mpfr_init2 (ref1, prec);
  mpfr_init2 (ref2, prec);
  mpfr_init2 (ref3, prec);
  mpfr_init2 (res1, prec);

  /* for each variable, consider each of the following 6 possibilities:
     NaN, +Infinity, -Infinity, +0, -0 or a random number */
  for (i=0; i<36; i++) {
    if (i%6==0) MPFR_SET_NAN(ref2);
    if (i%6==1) mpfr_set_d (ref2, 1.0/0.0, GMP_RNDN);
    if (i%6==2) mpfr_set_d (ref2, -1.0/0.0, GMP_RNDN);
    if (i%6==3) mpfr_set_d (ref2, 0.0, GMP_RNDN);
    if (i%6==4) mpfr_set_d (ref2, -0.0, GMP_RNDN);
    if (i%6==5) mpfr_random (ref2);

    if (i/6==0) MPFR_SET_NAN(ref3);
    if (i/6==1) mpfr_set_d (ref3, 1.0/0.0, GMP_RNDN);
    if (i/6==2) mpfr_set_d (ref3, -1.0/0.0, GMP_RNDN);
    if (i/6==3) mpfr_set_d (ref3, 0.0, GMP_RNDN);
    if (i/6==4) mpfr_set_d (ref3, -0.0, GMP_RNDN);
    if (i/6==5) mpfr_random (ref3);

    /* reference call: foo(a, b, c) */
    testfunc (ref1, ref2, ref3, rnd);

    /* foo(a, a, c) */
    mpfr_set (res1, ref2, rnd); /* exact operation */
    testfunc (res1, res1, ref3, rnd);
    if (mpfr_compare (res1, ref1)) {
      fprintf (stderr, "Error for %s(a, a, c) for a=%e, c=%e\n", foo,
	       mpfr_get_d (ref2), mpfr_get_d (ref3));
      fprintf (stderr, "expected %e, got %e\n", mpfr_get_d (ref1),
	       mpfr_get_d (res1));
      exit (1);
    }

    /* foo(a, b, a) */
    mpfr_set (res1, ref3, rnd);
    testfunc (res1, ref2, res1, rnd);
    if (mpfr_compare (res1, ref1)) {
      fprintf (stderr, "Error for %s(a, b, a) for b=%e, a=%e\n", foo,
	       mpfr_get_d (ref2), mpfr_get_d (ref3));
      fprintf (stderr, "expected %e, got %e\n", mpfr_get_d (ref1),
	       mpfr_get_d (res1));
      exit (1);
    }

    /* foo(a, a, a) */
    mpfr_set (ref3, ref2, rnd);
    testfunc (ref1, ref2, ref3, rnd);
    mpfr_set (res1, ref2, rnd);
    testfunc (res1, res1, res1, rnd);
    if (mpfr_compare (res1, ref1)) {
      fprintf (stderr, "Error for %s(a, a, a) for a=%e\n", foo,
	       mpfr_get_d (ref2));
      fprintf (stderr, "expected %e, got %e\n", mpfr_get_d (ref1),
	       mpfr_get_d (res1));
      exit (1);
    }
  }

  mpfr_clear (ref1);
  mpfr_clear (ref2);
  mpfr_clear (ref3);
  mpfr_clear (res1);
}

void test2ui (char *foo, mp_prec_t prec, mp_rnd_t rnd)
{
  mpfr_t ref1, ref2;
  unsigned int ref3;
  mp_limb_t c[1];
  mpfr_t res1;
  int i;

#ifdef DEBUG  
  printf("checking %s\n", foo);
#endif
  mpfr_init2 (ref1, prec);
  mpfr_init2 (ref2, prec);
  mpfr_init2 (res1, prec);

  /* ref2 can be NaN, +Inf, -Inf, +0, -0 or any number
     ref3 can be 0 or any number */
  for (i=0; i<12; i++) {
    if (i%6==0) MPFR_SET_NAN(ref2);
    if (i%6==1) mpfr_set_d (ref2, 1.0/0.0, GMP_RNDN);
    if (i%6==2) mpfr_set_d (ref2, -1.0/0.0, GMP_RNDN);
    if (i%6==3) mpfr_set_d (ref2, 0.0, GMP_RNDN);
    if (i%6==4) mpfr_set_d (ref2, -0.0, GMP_RNDN);
    if (i%6==5) mpfr_random (ref2);

    if (i/6==0) ref3=0;
    else {
      mpn_random (c, 1);
      ref3 = (unsigned int) c[0];
    }

    /* reference call: foo(a, b, c) */
    testfunc (ref1, ref2, ref3, rnd);

    /* foo(a, a, c) */
    mpfr_set (res1, ref2, rnd); /* exact operation */
    testfunc (res1, res1, ref3, rnd);
    if (mpfr_compare (res1, ref1)) {
      fprintf (stderr, "Error for %s(a, a, c) for a=%e c=%u\n", foo,
	       mpfr_get_d (ref2), ref3);
      fprintf (stderr, "expected %e, got %e\n", mpfr_get_d (ref1),
	       mpfr_get_d (res1));
      exit (1);
    }
  }

  mpfr_clear (ref1);
  mpfr_clear (ref2);
  mpfr_clear (res1);
}

void testui2 (char *foo, mp_prec_t prec, mp_rnd_t rnd)
{
  mpfr_t ref1, ref3;
  unsigned int ref2;
  mp_limb_t c[1];
  mpfr_t res1;
  int i;

#ifdef DEBUG  
  printf("checking %s\n", foo);
#endif
  mpfr_init2 (ref1, prec);
  mpfr_init2 (ref3, prec);
  mpfr_init2 (res1, prec);
  mpfr_random (ref3);
  mpn_random (c, 1);
  ref2 = (unsigned int) c[0];

  for (i=0; i<12; i++) {
    if (i%6==0) MPFR_SET_NAN(ref3);
    if (i%6==1) mpfr_set_d (ref3, 1.0/0.0, GMP_RNDN);
    if (i%6==2) mpfr_set_d (ref3, -1.0/0.0, GMP_RNDN);
    if (i%6==3) mpfr_set_d (ref3, 0.0, GMP_RNDN);
    if (i%6==4) mpfr_set_d (ref3, -0.0, GMP_RNDN);
    if (i%6==5) mpfr_random (ref3);

    if (i/6==0) ref2=0;
    else {
      mpn_random (c, 1);
      ref2 = (unsigned int) c[0];
    }

    /* reference call: foo(a, b, c) */
    testfunc (ref1, ref2, ref3, rnd);

    /* foo(a, b, a) */
    mpfr_set (res1, ref3, rnd); /* exact operation */
    testfunc (res1, ref2, res1, rnd);
    if (mpfr_compare (res1, ref1)) {
      fprintf (stderr, "Error for %s(a, b, a) for b=%u a=%e\n", foo,
	       ref2, mpfr_get_d (ref3));
      fprintf (stderr, "expected %e, got %e\n", mpfr_get_d (ref1),
	       mpfr_get_d (res1));
      exit (1);
    }
  }

  mpfr_clear (ref1);
  mpfr_clear (ref3);
  mpfr_clear (res1);
}

/* foo(mpfr_ptr, mpfr_srcptr, mp_rndt) */
void test2 (char *foo, mp_prec_t prec, mp_rnd_t rnd)
{
  mpfr_t ref1, ref2;
  mpfr_t res1;
  int i;

#ifdef DEBUG  
  printf("checking %s\n", foo);
#endif
  mpfr_init2 (ref1, prec);
  mpfr_init2 (ref2, prec);
  mpfr_init2 (res1, prec);
  mpfr_random (ref2);

  for (i=0; i<6; i++) {
    if (i==0) MPFR_SET_NAN(ref2);
    if (i==1) mpfr_set_d (ref2, 1.0/0.0, GMP_RNDN);
    if (i==2) mpfr_set_d (ref2, -1.0/0.0, GMP_RNDN);
    if (i==3) mpfr_set_d (ref2, 0.0, GMP_RNDN);
    if (i==4) mpfr_set_d (ref2, -0.0, GMP_RNDN);
    if (i==5) mpfr_random (ref2);

    /* reference call: foo(a, b) */
    testfunc (ref1, ref2, rnd);

    /* foo(a, a) */
    mpfr_set (res1, ref2, rnd); /* exact operation */
    testfunc (res1, res1, rnd);
    if (mpfr_compare (res1, ref1)) {
      fprintf (stderr, "Error for %s(a, a) for a=%e\n", foo, mpfr_get_d (ref2));
      fprintf (stderr, "expected %e, got %e\n", mpfr_get_d (ref1),
	       mpfr_get_d (res1));
      exit (1);
    }
  }

  mpfr_clear (ref1);
  mpfr_clear (ref2);
  mpfr_clear (res1);
}

/* foo(mpfr_ptr, mpfr_srcptr) */
void test2a (char *foo, mp_prec_t prec)
{
  mpfr_t ref1, ref2;
  mpfr_t res1;
  int i;

#ifdef DEBUG  
  printf("checking %s\n", foo);
#endif
  mpfr_init2 (ref1, prec);
  mpfr_init2 (ref2, prec);
  mpfr_init2 (res1, prec);
  mpfr_random (ref2);

  for (i=0; i<6; i++) {
    if (i==0) MPFR_SET_NAN(ref2);
    if (i==1) mpfr_set_d (ref2, 1.0/0.0, GMP_RNDN);
    if (i==2) mpfr_set_d (ref2, -1.0/0.0, GMP_RNDN);
    if (i==3) mpfr_set_d (ref2, 0.0, GMP_RNDN);
    if (i==4) mpfr_set_d (ref2, -0.0, GMP_RNDN);
    if (i==5) mpfr_random (ref2);

    /* reference call: foo(a, b) */
    testfunc (ref1, ref2);

    /* foo(a, a) */
    mpfr_set (res1, ref2, GMP_RNDN); /* exact operation */
    testfunc (res1, res1);
    if (mpfr_compare (res1, ref1)) {
      fprintf (stderr, "Error for %s(a, a) for a=%e\n", foo, mpfr_get_d (ref2));
      fprintf (stderr, "expected %e, got %e\n", mpfr_get_d (ref1),
	       mpfr_get_d (res1));
      exit (1);
    }
  }

  mpfr_clear (ref1);
  mpfr_clear (ref2);
  mpfr_clear (res1);
}

/* one operand, two results */
void test3a (char *foo, mp_prec_t prec, mp_rnd_t rnd)
{
  mpfr_t ref1, ref2, ref3;
  mpfr_t res1, res2;
  int i;

#ifdef DEBUG  
  printf("checking %s\n", foo);
#endif
  mpfr_init2 (ref1, prec);
  mpfr_init2 (ref2, prec);
  mpfr_init2 (ref3, prec);
  mpfr_init2 (res1, prec);
  mpfr_init2 (res2, prec);
  mpfr_random (ref3);

  for (i=0; i<6; i++) {
    if (i==0) MPFR_SET_NAN(ref3);
    if (i==1) mpfr_set_d (ref3, 1.0/0.0, GMP_RNDN);
    if (i==2) mpfr_set_d (ref3, -1.0/0.0, GMP_RNDN);
    if (i==3) mpfr_set_d (ref3, 0.0, GMP_RNDN);
    if (i==4) mpfr_set_d (ref3, -0.0, GMP_RNDN);
    if (i==5) mpfr_random (ref3);

     /* reference call: foo(a, b, c) */
     testfunc (ref1, ref2, ref3, rnd);

     /* foo(a, b, a) */
     mpfr_set (res1, ref3, rnd); /* exact operation */
     testfunc (res1, res2, res1, rnd);
     if (mpfr_compare (res1, ref1) || mpfr_compare (res2, ref2)) {
       fprintf (stderr, "Error for %s(a, b, a) for a=%e\n", foo, mpfr_get_d (ref3));
       fprintf (stderr, "expected (%e,%e), got (%e,%e)\n", mpfr_get_d (ref1),
		mpfr_get_d (ref2), mpfr_get_d (res1), mpfr_get_d (res2));
       exit (1);
     }

     /* foo(a, b, b) */
     mpfr_set (res2, ref3, rnd); /* exact operation */
     testfunc (res1, res2, res2, rnd);
     if (mpfr_compare (res1, ref1) || mpfr_compare (res2, ref2)) {
       fprintf (stderr, "Error for %s(a, b, b) for b=%e\n", foo, mpfr_get_d (ref3));
       fprintf (stderr, "expected (%e,%e), got (%e,%e)\n", mpfr_get_d (ref1),
		mpfr_get_d (ref2), mpfr_get_d (res1), mpfr_get_d (res2));
       exit (1);
     }
  }

  mpfr_clear (ref1);
  mpfr_clear (ref2);
  mpfr_clear (ref3);
  mpfr_clear (res1);
  mpfr_clear (res2);
}

int main ()
{
  testfunc = mpfr_add; test3 ("mpfr_add", 53, GMP_RNDN);
  testfunc = mpfr_add_ui; test2ui ("mpfr_add_ui", 53, GMP_RNDN);
  testfunc = mpfr_agm; test3 ("mpfr_agm", 53, GMP_RNDN);
  testfunc = mpfr_ceil; test2 ("mpfr_ceil", 53, GMP_RNDN);
  testfunc = mpfr_div; test3 ("mpfr_div", 53, GMP_RNDN);
  testfunc = mpfr_div_2exp; test2ui ("mpfr_div_2exp", 53, GMP_RNDN);
  testfunc = (void*) mpfr_div_ui; test2ui ("mpfr_div_ui", 53, GMP_RNDN);
  testfunc = (void*) mpfr_exp; test2 ("mpfr_exp", 53, GMP_RNDN);
  testfunc = mpfr_floor; test2 ("mpfr_floor", 53, GMP_RNDN);
  testfunc = (void*) mpfr_log; test2 ("mpfr_log", 53, GMP_RNDN);
  testfunc = mpfr_mul; test3 ("mpfr_mul", 53, GMP_RNDN);
  testfunc = mpfr_mul_2exp; test2ui ("mpfr_mul_2exp", 53, GMP_RNDN);
  testfunc = mpfr_mul_ui; test2ui ("mpfr_mul_ui", 53, GMP_RNDN);
  testfunc = mpfr_neg; test2 ("mpfr_neg", 53, GMP_RNDN);
  testfunc = (void*) mpfr_pow_ui; test2ui ("mpfr_pow_ui", 53, GMP_RNDN);
  testfunc = mpfr_reldiff; test3 ("mpfr_reldiff", 53, GMP_RNDN);
  testfunc = (void*) mpfr_sin_cos; test3a ("mpfr_sin_cos", 53, GMP_RNDN);
  testfunc = mpfr_sub; test3 ("mpfr_sub", 53, GMP_RNDN);
  testfunc = mpfr_sub_ui; test2ui ("mpfr_sub_ui", 53, GMP_RNDN);
  testfunc = (void*) mpfr_sqrt; test2 ("mpfr_sqrt", 53, GMP_RNDN);
  testfunc = mpfr_ui_div; testui2 ("mpfr_ui_div", 53, GMP_RNDN);
  testfunc = mpfr_ui_sub; testui2 ("mpfr_ui_sub", 53, GMP_RNDN);
  testfunc = mpfr_trunc; test2 ("mpfr_trunc", 53, GMP_RNDN);
  return 0;
}
