/* Test file for mpfr_const_log2.

Copyright 1999, 2001 Free Software Foundation, Inc.

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
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

/* tlog2 [prec] [rnd] [0 = no print] */

void check _PROTO ((mp_prec_t, mp_prec_t));

void
check (mp_prec_t p0, mp_prec_t p1)
{
  mpfr_t x, y, z;
  mp_rnd_t rnd;

  mpfr_init (x);
  mpfr_init (y);
  mpfr_init2 (z, p1 + 10);
  mpfr_const_log2 (z, GMP_RNDN);
  __mpfr_const_log2_prec = 1;

  for (; p0<=p1; p0++)
    {
      mpfr_set_prec (x, p0);
      mpfr_set_prec (y, p0);
      for (rnd = 0; rnd < 4; rnd++)
        {
          mpfr_const_log2 (x, rnd);
          mpfr_set (y, z, rnd);
          if (mpfr_cmp (x, y) && mpfr_can_round (z, mpfr_get_prec(z), GMP_RNDN,
                                                 rnd, p0))
            {
              fprintf (stderr, "mpfr_const_log2 fails for prec=%u, rnd=%s\n",
                       (unsigned int) p0, mpfr_print_rnd_mode (rnd));
              fprintf (stderr, "expected ");
              mpfr_out_str (stderr, 2, 0, y, GMP_RNDN);
              fprintf (stderr, "\ngot      ");
              mpfr_out_str (stderr, 2, 0, x, GMP_RNDN);
              fprintf (stderr, "\n");
              exit (1);
            }
        }
    }

  mpfr_clear (x);
  mpfr_clear (y);
  mpfr_clear (z);
}

int
main (int argc, char *argv[])
{
  mpfr_t x;
  int p;
  mp_rnd_t rnd;

  p = (argc>1) ? atoi(argv[1]) : 53;
  rnd = (argc>2) ? atoi(argv[2]) : GMP_RNDZ;

  mpfr_init (x);

  check (2, 1000);

  /* check precision of 2 bits */
  mpfr_set_prec (x, 2);
  mpfr_const_log2 (x, GMP_RNDN);
  if (mpfr_get_d1 (x) != 0.75)
    {
      fprintf (stderr, "mpfr_const_log2 failed for prec=2, rnd=GMP_RNDN\n");
      fprintf (stderr, "expected 0.75, got %f\n", mpfr_get_d1 (x));
      exit (1);
    }

  if (argc>=2)
    {
      mpfr_set_prec (x, p);
      mpfr_const_log2 (x, rnd);
      printf ("log(2)=");
      mpfr_out_str (stdout, 10, 0, x, rnd);
      putchar('\n');
    }
  
  mpfr_set_prec (x, 53);
  mpfr_const_log2 (x, rnd);
  if (mpfr_get_d1 (x) != 6.9314718055994530941e-1)
    {
      fprintf (stderr, "mpfr_const_log2 failed for prec=53\n");
      exit (1);
    }

  mpfr_clear(x);

  return 0;
}
