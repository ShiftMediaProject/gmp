/* Test file for mpfr_out_str.

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
#include <time.h>
#include "gmp.h"
#include "mpfr.h"
#include "mpfr-impl.h"

FILE *fout;

#define check(d,r,b) check4(d,r,b,53)

void check4(d, rnd, base, prec) double d; unsigned char rnd; int base, prec;
{
  mpfr_t x;

  mpfr_init2(x, prec);
  mpfr_set_d(x, d, rnd);
  mpfr_set_machine_rnd_mode(rnd);
  fprintf(fout, "%1.19e base %d rnd %d:\n ", d, base, rnd);
  mpfr_out_str(fout, base, (base==2) ? prec : 0, x, rnd);
  fputc('\n', fout);
  mpfr_clear(x);
}

int
main(int argc, char **argv)
{
  int i,N=100,r,p; double d;

  /* with no argument: prints to /dev/null,
     tout_str N: prints N tests to stdout */
  if (argc==1) fout=fopen("/dev/null", "w");
  else { fout=stdout; N=atoi(argv[1]); }
  check(-1.5674376729569697500e+15, GMP_RNDN, 19);
  check(-5.71262771772792640000e-79, GMP_RNDU, 16);
  check(-0.0, GMP_RNDU, 7);
  check(-4.5306392613572974756e-308, GMP_RNDN, 8);
  check(-6.7265890111403371523e-165, GMP_RNDN, 4);
  check(-1.3242553591261807653e+156, GMP_RNDN, 16);
  check(-6.606499965302424244461355e233, GMP_RNDN, 10);
  check4(1.0, GMP_RNDN, 10, 120);
  check(1.0, GMP_RNDU, 10);
  check(4.059650008e-83, GMP_RNDN, 10);
  check(-7.4, GMP_RNDN, 10);
  check(0.997, GMP_RNDN, 10);
  check(-4.53063926135729747564e-308, GMP_RNDN, 10);
  check(2.14478198760196000000e+16, GMP_RNDN, 10);
  check(7.02293374921793516813e-84, GMP_RNDN, 10);

  /* random tests */
  srand(getpid());
  for (i=0;i<N;i++) {
    do { d = drand(); } while (isnan(d));
    r = rand()%4;
    p = 2 + rand()%35;
    check(d, r, p);
  }
  exit (0);
}



