/* Test file for mpfr_get_str.

Copyright (C) 1999 PolKA project, Inria Lorraine and Loria

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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gmp.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#include <time.h>

void check(d, rnd) double d; unsigned char rnd;
{
  mpfr_t x; char *str, str2[30]; mp_exp_t e;

  mpfr_init2(x, 53);
  mpfr_set_d(x, d, rnd);
  str = mpfr_get_str(NULL, &e, 10, 5, x, rnd);
  mpfr_set_machine_rnd_mode(rnd);
  sprintf(str2, "%1.4e", d);
  mpfr_clear(x);
  free(str);
}

int
main(int argc, char **argv)
{
  int i; double d;

  srand(getpid());
  /* printf seems to round towards nearest in all cases, at least with gcc */
  check(4.059650008e-83, 0);
  check(-6.606499965302424244461355e233, 0);
  check(-7.4, 0);
  check(0.997, 0);
  check(-4.53063926135729747564e-308, 0);
  check(2.14478198760196000000e+16, 0);
  check(7.02293374921793516813e-84, 0);
  check(-6.7274500420134077e-87,0); 
  for (i=0;i<100000;i++) {
    do { d = drand(); } while (isnan(d));
    check(d, 0);
  }
  exit (0);
}



