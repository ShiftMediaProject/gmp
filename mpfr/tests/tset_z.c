/* Test file for mpfr_set_z.

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
#include <limits.h>
#include <time.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#include "mpfr-test.h"


/* FIXME: It'd be better to examine the actual data in an mpfr_t to see that
   it's as expected.  Comparing mpfr_set_z with mpfr_cmp or against
   mpfr_get_si is a rather indirect test of a low level routine.  */


static void
check (long i, unsigned char rnd)
{
  mpfr_t f;
  mpz_t z;

  mpfr_init2 (f, 8 * sizeof(long));
  mpz_init (z);
  mpz_set_ui (z, i);
  mpfr_set_z (f, z, rnd);
  if (mpfr_get_si (f, GMP_RNDZ) != i)
    {
      printf ("Error in mpfr_set_z for i=%ld rnd_mode=%d\n", i, rnd);
      exit (1);
    }
  mpfr_clear (f);
  mpz_clear (z);
}

static void
check_large (void)
{
  mpz_t z;
  mpfr_t x, y;

  mpz_init (z);
  mpfr_init2 (x, 160);
  mpfr_init2 (y, 160);
  mpz_set_str (z, "77031627725494291259359895954016675357279104942148788042", 10);
  mpfr_set_z (x, z, GMP_RNDN);
  mpfr_set_str_binary (y, "0.1100100100001111110110101010001000100001011010001100001000110100110001001100011001100010100010111000000011011100000111001101000100101001000000100100111000001001E186");
  if (mpfr_cmp (x, y))
    {
      printf ("Error in mpfr_set_z on large input\n");
      exit (1);
    }
  mpz_clear (z);
  mpfr_clear (x);
  mpfr_clear (y);
}

int
main (int argc, char *argv[])
{
  long j;

  tests_start_mpfr ();

  check_large ();
  check (0, 0);
  for (j = 0; j < 200000; j++)
    check (randlimb () & LONG_MAX, randlimb () % 4);

  tests_end_mpfr ();
  return 0;
}
