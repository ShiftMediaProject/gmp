/* Test file for mpfr_cmp.

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
#include <math.h>
#include "gmp.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#include "mpfr-test.h"

int
main (void)
{
  double x, y;
  mpfr_t xx, yy;
  int i, c;
  mp_prec_t p;

  mpfr_init (xx);
  mpfr_init (yy);

  mpfr_set_prec (xx, 2);
  mpfr_set_prec (yy, 2);
  mpfr_set_str_raw(xx, "-0.10E0");
  mpfr_set_str_raw(yy, "-0.10E0");
  if (mpfr_cmp (xx, yy))
    {
      fprintf (stderr, "mpfr_cmp (xx, yy) returns non-zero for prec=2\n");
      exit (1);
    }

  mpfr_set_prec (xx, 65);
  mpfr_set_prec (yy, 65);
  mpfr_set_str_raw(xx, "0.10011010101000110101010000000011001001001110001011101011111011101E623");
  mpfr_set_str_raw(yy, "0.10011010101000110101010000000011001001001110001011101011111011100E623");
  p = 0;
  if (mpfr_cmp2(xx, yy, &p) <= 0 || p != 64)
    {
      printf("Error (1) in mpfr_cmp2\n");
      exit(1);
    }
  mpfr_set_str_raw(xx, "0.10100010001110110111000010001000010011111101000100011101000011100");
  mpfr_set_str_raw(yy, "0.10100010001110110111000010001000010011111101000100011101000011011");
  p = 0;
  if (mpfr_cmp2(xx, yy, &p) <= 0 || p != 64)
    {
      printf("Error (2) in mpfr_cmp2\n");
      exit(1);
    }

  mpfr_set_prec (xx, 160); mpfr_set_prec (yy, 160);
  mpfr_set_str_raw (xx, "0.1E1");
  mpfr_set_str_raw (yy, "0.1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111100000110001110100");
  p = 0;
  if (mpfr_cmp2 (xx, yy, &p) <= 0 || p != 144)
    {
      printf("Error (3) in mpfr_cmp2\n");
      exit(1);
    }

  mpfr_set_prec(xx, 53); mpfr_set_prec(yy, 200);
  mpfr_set_d(xx, 1.0, 0);
  mpfr_set_d(yy, 1.0, 0);
  if (mpfr_cmp(xx,yy)!=0) {
    printf("Error in mpfr_cmp: 1.0 != 1.0\n"); exit(1);
  }
  mpfr_set_prec(yy, 31);
  mpfr_set_d(xx, 1.0000000002, 0);
  mpfr_set_d(yy, 1.0, 0);
  if (!(mpfr_cmp(xx,yy)>0)) {
    printf("Error in mpfr_cmp: not 1.0000000002 > 1.0\n"); exit(1);
  }
  mpfr_set_prec(yy, 53);

  /* bug found by Gerardo Ballabio */
  mpfr_set_d(xx, 0.0, GMP_RNDN);
  mpfr_set_d(yy, 0.1, GMP_RNDN);
  if (mpfr_cmp(xx, yy) >= 0) {
    fprintf(stderr,
	    "Error in mpfr_cmp(0.0, 0.1), gives %d\n", mpfr_cmp(xx, yy));
    exit(1);
  }

  mpfr_set_inf (xx, 1);
  mpfr_set_d(yy, -23489745.0329, GMP_RNDN); 
  if (mpfr_cmp(xx, yy) <= 0) { 
    fprintf(stderr,
	    "Error in mpfr_cmp(Infp, 23489745.0329), gives %d\n", mpfr_cmp(xx, yy));
    exit(1);
  }

  mpfr_set_inf (xx, 1);
  mpfr_set_inf (yy, -1);
  if (mpfr_cmp(xx, yy) <= 0) { 
    fprintf(stderr,
	    "Error in mpfr_cmp(Infp, Infm), gives %d\n", mpfr_cmp(xx, yy));
    exit(1);
  }

  mpfr_set_inf (xx, -1);
  mpfr_set_inf (yy, 1);
  if (mpfr_cmp(xx, yy) >= 0) { 
    fprintf(stderr,
	    "Error in mpfr_cmp(Infm, Infp), gives %d\n", mpfr_cmp(xx, yy));
    exit(1);
  }

  mpfr_set_inf (xx, 1);
  mpfr_set_inf (yy, 1);
  if (mpfr_cmp(xx, yy) != 0) { 
    fprintf(stderr,
	    "Error in mpfr_cmp(Infp, Infp), gives %d\n", mpfr_cmp(xx, yy));
    exit(1);
  }

  mpfr_set_inf (xx, -1);
  mpfr_set_inf (yy, -1);
  if (mpfr_cmp(xx, yy) != 0) { 
    fprintf(stderr,
	    "Error in mpfr_cmp(Infm, Infm), gives %d\n", mpfr_cmp(xx, yy));
    exit(1);
  }

  mpfr_set_inf (xx, -1);
  mpfr_set_d(yy, 2346.09234, GMP_RNDN); 
  if (mpfr_cmp(xx, yy) >= 0) { 
    fprintf(stderr,
	    "Error in mpfr_cmp(Infm, 2346.09234), gives %d\n", mpfr_cmp(xx, yy));
    exit(1);
  }

  mpfr_set_d (xx, 0.0, GMP_RNDN);
  mpfr_set_d (yy, 1.0, GMP_RNDN);
  if ((i = mpfr_cmp3 (xx, yy, 1)) >= 0) {
    fprintf (stderr, "Error: mpfr_cmp3 (0, 1, 1) gives %d instead of a negative value\n", i);
    exit (1);
  }
  if ((i = mpfr_cmp3 (xx, yy, -1)) <= 0) {
    fprintf (stderr, "Error: mpfr_cmp3 (0, 1, -1) gives %d instead of a positive value\n", i);
    exit (1);
  }

  for (i=0;i<1000000;) {    x=drand(); y=drand();
    if (!isnan(x) && !isnan(y)) {
      i++;
      mpfr_set_d(xx, x, 0);
      mpfr_set_d(yy, y, 0);
      c = mpfr_cmp(xx,yy);
      if ((c>0 && x<=y) || (c==0 && x!=y) || (c<0 && x>=y)) {
	printf("Error in mpfr_cmp with x=%1.20e, y=%1.20e mpfr_cmp(x,y)=%d\n",
	       x,y,c); exit(1);
      }
    }
  }

  mpfr_clear(xx); mpfr_clear(yy);

  return 0;
}
