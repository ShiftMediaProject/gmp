/* Test file for mpfr_const_pi.

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
#include "mpfr.h"

/* tpi [prec] [rnd] [0 = no print] */

int
main (int argc, char *argv[])
{
  mpfr_t x; int p; unsigned char rnd;

  p = (argc>1) ? atoi(argv[1]) : 53;
  rnd = (argc>2) ? atoi(argv[2]) : GMP_RNDZ;
  mpfr_init2(x, p);
  mpfr_const_pi(x, rnd);
  if (argc>=2) {
    printf("Pi="); mpfr_out_str(stdout, 10, 0, x, rnd); putchar('\n');
  }
  else if (mpfr_get_d1 (x) != 3.141592653589793116) {
    fprintf(stderr, "mpfr_const_pi failed for prec=53\n"); exit(1);
  }
  mpfr_clear(x);

  return 0;
}
