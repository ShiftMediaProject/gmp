/* Test file for mpfr_set_si and mpfr_set_ui.

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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gmp.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#include "mpfr-test.h"

int
main (int argc, char *argv[])
{
  mpfr_t x;
  long k, z, d;
  unsigned long zl, dl, N;
  int inex;
  
  mpfr_init2(x, 100);

  SEED_RAND (time(NULL));

  N = (argc==1) ? 1000000 : atoi(argv[1]);

  for (k = 1; k <= N; k++)
    {
      z = random() - (1 << 30);      
      inex = mpfr_set_si(x, z, GMP_RNDZ);
      d = (long) mpfr_get_d1 (x);
      if (d != z) {
	fprintf(stderr, "Error in mpfr_set_si: expected %ld got %ld\n", z, d); exit(1);
      }
      if (inex)
      {
        fprintf(stderr,
                "Error in mpfr_set_si: inex value incorrect for %ld: %d\n",
                z, inex);
        exit(1);
      }
    }

  for (k = 1; k <= N; k++)
    {
      zl = random();
      inex = mpfr_set_ui (x, zl, GMP_RNDZ);
      dl = (unsigned long) mpfr_get_d1 (x);
      if (dl != zl) {
	fprintf(stderr, "Error in mpfr_set_ui: expected %lu got %lu\n", zl, dl); exit(1);
      }
      if (inex)
      {
        fprintf(stderr,
                "Error in mpfr_set_ui: inex value incorrect for %lu: %d\n",
                zl, inex);
        exit(1);
      }
    }

  mpfr_set_prec (x, 2);
  if (mpfr_set_si (x, 5, GMP_RNDZ) >= 0)
    {
      fprintf (stderr, "Wrong inexact flag for x=5, rnd=GMP_RNDZ\n");
      exit (1);
    }

  mpfr_set_prec (x, 2);
  if (mpfr_set_si (x, -5, GMP_RNDZ) <= 0)
    {
      fprintf (stderr, "Wrong inexact flag for x=-5, rnd=GMP_RNDZ\n");
      exit (1);
    }

  mpfr_set_prec (x, 3);
  inex = mpfr_set_si(x, 77617, GMP_RNDD); /* should be 65536 */
  if (MPFR_MANT(x)[0] != ((mp_limb_t)1 << (mp_bits_per_limb-1))
      || inex >= 0)
  {
    fprintf(stderr, "Error in mpfr_set_si(x:3, 77617, GMP_RNDD)\n");
    mpfr_print_binary(x); putchar('\n');
    exit(1);
  }
  inex = mpfr_set_ui(x, 77617, GMP_RNDD); /* should be 65536 */
  if (MPFR_MANT(x)[0] != ((mp_limb_t)1 << (mp_bits_per_limb-1))
      || inex >= 0)
  {
    fprintf(stderr, "Error in mpfr_set_ui(x:3, 77617, GMP_RNDD)\n");
    mpfr_print_binary(x); putchar('\n');
    exit(1);
  }

  mpfr_set_prec(x, 2);
  inex = mpfr_set_si(x, 33096, GMP_RNDU);
  if (mpfr_get_d1 (x) != 49152.0 || inex <= 0)
  {
    fprintf(stderr, "Error in mpfr_set_si, expected 49152, got %lu, inex %d\n",
	    (unsigned long) mpfr_get_d1 (x), inex);
    exit(1);
  }
  inex = mpfr_set_ui(x, 33096, GMP_RNDU);
  if (mpfr_get_d1 (x) != 49152.0)
  {
    fprintf(stderr, "Error in mpfr_set_ui, expected 49152, got %lu, inex %d\n",
	    (unsigned long) mpfr_get_d1 (x), inex);
    exit(1);
  }

  mpfr_set_si (x, -1, GMP_RNDN);
  mpfr_set_ui (x, 0, GMP_RNDN);
  if (MPFR_SIGN (x) < 0)
    {
      fprintf (stderr, "mpfr_set_ui (x, 0) gives -0\n");
      exit (1);
    }

  mpfr_set_si (x, -1, GMP_RNDN);
  mpfr_set_si (x, 0, GMP_RNDN);
  if (MPFR_SIGN (x) < 0)
    {
      fprintf (stderr, "mpfr_set_si (x, 0) gives -0\n");
      exit (1);
    }

  /* check potential bug in case mp_limb_t is unsigned */
  mpfr_set_emax (0);
  mpfr_set_si (x, -1, GMP_RNDN);
  if (mpfr_sgn (x) >= 0)
    {
      fprintf (stderr, "mpfr_set_si (x, -1) fails\n");
      exit (1);
    }

  mpfr_set_emax (5);
  mpfr_set_prec (x, 2);
  mpfr_set_si (x, -31, GMP_RNDN);
  if (mpfr_sgn (x) >= 0)
    {
      fprintf (stderr, "mpfr_set_si (x, -31) fails\n");
      exit (1);
    }

  mpfr_clear(x); 

  return 0;
}
