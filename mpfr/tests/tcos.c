/* Test file for mpfr_cos.

Copyright 2001, 2002 Free Software Foundation, Inc.

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
#include <math.h>
#include "gmp.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#include "mpfr-test.h"

void check53 _PROTO ((double, double, mp_rnd_t));

void
check53 (double x, double cos_x, mp_rnd_t rnd_mode)
{
  mpfr_t xx, c;

  mpfr_init2 (xx, 53);
  mpfr_init2 (c, 53);
  mpfr_set_d (xx, x, rnd_mode); /* should be exact */
  mpfr_cos (c, xx, rnd_mode);
  if (mpfr_get_d1 (c) != cos_x && (!isnan(cos_x) || !mpfr_nan_p(c)))
    {
      fprintf (stderr, "mpfr_cos failed for x=%1.20e, rnd=%s\n", x,
	       mpfr_print_rnd_mode (rnd_mode));
      fprintf (stderr, "mpfr_cos gives cos(x)=%1.20e, expected %1.20e\n",
	       mpfr_get_d1 (c), cos_x);
      exit (1);
    }
  mpfr_clear (xx);
  mpfr_clear (c);
}

#define TEST_FUNCTION mpfr_cos
#include "tgeneric.c"

int
main (int argc, char *argv[])
{
  mpfr_t x, y;

  mpfr_init (x);
  mpfr_init (y);

  mpfr_set_prec (x, 53);
  mpfr_set_prec (y, 2);
  mpfr_set_d (x, 9.81333845856942e-1, GMP_RNDN);
  mpfr_cos (y, x, GMP_RNDN);

  mpfr_set_prec (x, 30);
  mpfr_set_prec (y, 30);
  mpfr_set_str_raw (x, "1.00001010001101110010100010101e-1");
  mpfr_cos (y, x, GMP_RNDU);
  mpfr_set_str_raw (x, "1.10111100010101011110101010100e-1");
  if (mpfr_cmp (y, x))
    {
      fprintf (stderr, "Error for prec=30, rnd=GMP_RNDU\n");
      printf ("expected "); mpfr_print_binary (x); putchar ('\n');
      printf ("     got "); mpfr_print_binary (y); putchar ('\n');
      exit (1);
    }

  mpfr_set_prec (x, 59);
  mpfr_set_prec (y, 59);
  mpfr_set_str_raw (x, "1.01101011101111010011111110111111111011011101100111100011e-3");
  mpfr_cos (y, x, GMP_RNDU);
  mpfr_set_str_raw (x, "1.1111011111110010001001001011100111101110100010000010010011e-1");
  if (mpfr_cmp (y, x))
    {
      fprintf (stderr, "Error for prec=59, rnd=GMP_RNDU\n");
      printf ("expected "); mpfr_print_binary (x); putchar ('\n');
      printf ("     got "); mpfr_print_binary (y); putchar ('\n');
      exit (1);
    }

  mpfr_set_prec (x, 5);
  mpfr_set_prec (y, 5);
  mpfr_set_str_raw (x, "1.1100e-2");
  mpfr_cos (y, x, GMP_RNDD);
  mpfr_set_str_raw (x, "1.1100e-1");
  if (mpfr_cmp (y, x))
    {
      fprintf (stderr, "Error for x=1.1100e-2, rnd=GMP_RNDD\n");
      printf ("expected 1.1100e-1, got "); mpfr_print_binary (y); putchar ('\n');
      exit (1);
    }

#ifdef HAVE_INFS
  check53 (DBL_NAN, DBL_NAN, GMP_RNDN);
  check53 (DBL_POS_INF, DBL_NAN, GMP_RNDN);
  check53 (DBL_NEG_INF, DBL_NAN, GMP_RNDN);
#endif

  /* worst case from PhD thesis of Vincent Lefe`vre: x=8980155785351021/2^54 */
  check53 (4.984987858808754279e-1, 8.783012931285841817e-1, GMP_RNDN);
  check53 (4.984987858808754279e-1, 8.783012931285840707e-1, GMP_RNDD);
  check53 (4.984987858808754279e-1, 8.783012931285840707e-1, GMP_RNDZ);
  check53 (4.984987858808754279e-1, 8.783012931285841817e-1, GMP_RNDU);
  check53 (1.00031274099908640274,  0.540039116973283217504, GMP_RNDN);
  check53 (1.00229256850978698523,  0.538371757797526551137, GMP_RNDZ);
  check53 (1.00288304857059840103,  0.537874062022526966409, GMP_RNDZ);
  check53 (1.00591265847407274059,  0.53531755997839769456,  GMP_RNDN);

  check53 (1.00591265847407274059, 0.53531755997839769456,  GMP_RNDN);

  test_generic (2, 100, 100);

  mpfr_clear (x);
  mpfr_clear (y);

  return 0;
}
