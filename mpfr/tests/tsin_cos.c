/* Test file for mpfr_sin_cos.

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

void large_test _PROTO ((int, int));
void check53 _PROTO ((double, double, double, mp_rnd_t));
void check53sin _PROTO ((double, double, mp_rnd_t));
void check53cos _PROTO ((double, double, mp_rnd_t));

void large_test (int prec, int N)
{
  int i;
  mpfr_t x, s, c;

  mpfr_init2 (x, prec);
  mpfr_init2 (s, prec);
  mpfr_init2 (c, prec);
  mpfr_set_d (x, 3.0, GMP_RNDN);
  mpfr_sqrt (x, x, GMP_RNDN);
  for (i=0; i<N; i++) mpfr_sin_cos (s, c, x, GMP_RNDN);
  mpfr_out_str (stdout, 10, 0, s, GMP_RNDN); putchar('\n');
  mpfr_clear (x);
  mpfr_clear (s);
  mpfr_clear (c);
}

void check53 (double x, double sin_x, double cos_x, mp_rnd_t rnd_mode)
{
  mpfr_t xx, s, c;

  mpfr_init2 (xx, 53);
  mpfr_init2 (s, 53);
  mpfr_init2 (c, 53);
  mpfr_set_d (xx, x, rnd_mode); /* should be exact */
  mpfr_sin_cos (s, c, xx, rnd_mode);
  if (mpfr_get_d1 (s) != sin_x && !(Isnan(sin_x) && mpfr_nan_p(s)))
    {
      fprintf (stderr, "mpfr_sin_cos failed for x=%1.20e, rnd=%s\n", x,
               mpfr_print_rnd_mode (rnd_mode));
      fprintf (stderr, "mpfr_sin_cos gives sin(x)=%1.20e, expected %1.20e\n",
               mpfr_get_d1 (s), sin_x);
      exit (1);
    }
  if (mpfr_get_d1 (c) != cos_x && !(Isnan(cos_x) && mpfr_nan_p(c)))
    {
      fprintf (stderr, "mpfr_sin_cos failed for x=%1.20e, rnd=%s\n", x,
               mpfr_print_rnd_mode (rnd_mode));
      fprintf (stderr, "mpfr_sin_cos gives cos(x)=%1.20e, expected %1.20e\n",
               mpfr_get_d1 (c), cos_x);
      exit (1);
    }
  mpfr_clear (xx);
  mpfr_clear (s);
  mpfr_clear (c);
}

void check53sin (double x, double sin_x, mp_rnd_t rnd_mode)
{
  mpfr_t xx, s, c;

  mpfr_init2 (xx, 53);
  mpfr_init2 (s, 53);
  mpfr_init2 (c, 53);
  mpfr_set_d (xx, x, rnd_mode); /* should be exact */
  mpfr_sin_cos (s, c, xx, rnd_mode);
  if (mpfr_get_d1 (s) != sin_x && !(Isnan(sin_x) && mpfr_nan_p(s)))
    {
      fprintf (stderr, "mpfr_sin_cos failed for x=%1.20e, rnd=%s\n", x,
               mpfr_print_rnd_mode (rnd_mode));
      fprintf (stderr, "mpfr_sin_cos gives sin(x)=%1.20e, expected %1.20e\n",
               mpfr_get_d1 (s), sin_x);
      exit (1);
    }
  mpfr_clear (xx);
  mpfr_clear (s);
  mpfr_clear (c);
}

void check53cos (double x, double cos_x, mp_rnd_t rnd_mode)
{
  mpfr_t xx, c, s;

  mpfr_init2 (xx, 53);
  mpfr_init2 (c, 53);
  mpfr_init2 (s, 53);
  mpfr_set_d (xx, x, rnd_mode); /* should be exact */
  mpfr_sin_cos (s, c, xx, rnd_mode);
  if (mpfr_get_d1 (c) != cos_x && !(Isnan(cos_x) && mpfr_nan_p(c)))
    {
      fprintf (stderr, "mpfr_sin_cos failed for x=%1.20e, rnd=%s\n", x,
               mpfr_print_rnd_mode (rnd_mode));
      fprintf (stderr, "mpfr_sin_cos gives cos(x)=%1.20e, expected %1.20e\n",
               mpfr_get_d1 (c), cos_x);
      exit (1);
    }
  mpfr_clear (xx);
  mpfr_clear (c);
  mpfr_clear (s);
}

/* tsin_cos prec [N] performs N tests with prec bits */
int
main(int argc, char *argv[])
{
  if (argc > 1)
    {
      large_test (atoi (argv[1]), (argc > 2) ? atoi (argv[2]) : 1);
    }

#ifdef HAVE_INFS
  check53 (DBL_NAN, DBL_NAN, DBL_NAN, GMP_RNDN);
  check53 (DBL_POS_INF, DBL_NAN, DBL_NAN, GMP_RNDN);
  check53 (DBL_NEG_INF, DBL_NAN, DBL_NAN, GMP_RNDN);
#endif
  /* worst case from PhD thesis of Vincent Lefe`vre: x=8980155785351021/2^54 */
  check53 (4.984987858808754279e-1, 4.781075595393330379e-1, 
	   8.783012931285841817e-1, GMP_RNDN);
  check53 (4.984987858808754279e-1, 4.781075595393329824e-1,
	   8.783012931285840707e-1, GMP_RNDD);
  check53 (4.984987858808754279e-1, 4.781075595393329824e-1,
	   8.783012931285840707e-1, GMP_RNDZ);
  check53 (4.984987858808754279e-1, 4.781075595393330379e-1,
	   8.783012931285841817e-1, GMP_RNDU);
  check53 (1.00031274099908640274,  8.416399183372403892e-1,
	   0.540039116973283217504, GMP_RNDN);
  check53 (1.00229256850978698523,  8.427074524447979442e-1,
	   0.538371757797526551137, GMP_RNDZ);
  check53 (1.00288304857059840103,  8.430252033025980029e-1,
	   0.537874062022526966409, GMP_RNDZ);
  check53 (1.00591265847407274059,  8.446508805292128885e-1,
	   0.53531755997839769456,  GMP_RNDN);

  /* check one argument only */
  check53sin (1.00591265847407274059,  8.446508805292128885e-1, GMP_RNDN);
  check53cos (1.00591265847407274059, 0.53531755997839769456,  GMP_RNDN);

  return 0;
}
