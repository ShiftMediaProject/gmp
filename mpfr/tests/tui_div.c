/* Test file for mpfr_ui_div.

Copyright (C) 2000, 2001, 2002 Free Software Foundation, Inc.

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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "gmp.h"
#include "mpfr.h"
#ifdef TEST
#include "mpfr-test.h"
#endif

void check _PROTO((unsigned long, double, mp_rnd_t, double)); 
void check_inexact _PROTO((void));

/* checks that y/x gives the same results in double
   and with mpfr with 53 bits of precision */
void
check (unsigned long y, double x, mp_rnd_t rnd_mode, double z1)
{
  double z2; mpfr_t xx, zz;

  mpfr_init2(xx, 53);
  mpfr_init2(zz, 53);
  mpfr_set_d(xx, x, rnd_mode);
  mpfr_ui_div(zz, y, xx, rnd_mode);
#ifdef TEST
  mpfr_set_machine_rnd_mode(rnd_mode);
#endif
  if (z1==0.0) z1 = y/x;
  z2 = mpfr_get_d(zz);
  if (z1!=z2 && !(isnan(z1) && isnan(z2))) {
    printf("expected quotient is %1.20e, got %1.20e\n",z1,z2);
    printf("mpfr_ui_div failed for y=%lu x=%1.20e with rnd_mode=%s\n",
	   y, x, mpfr_print_rnd_mode(rnd_mode));
    exit(1);
  }
  mpfr_clear(xx);
  mpfr_clear(zz);
}

void
check_inexact (void)
{
  mpfr_t x, y, z;
  mp_prec_t px, py;
  int inexact, cmp;
  unsigned long int u;
  mp_rnd_t rnd;
  
  mpfr_init (x);
  mpfr_init (y);
  mpfr_init (z);

  for (px=2; px<300; px++)
    {
      mpfr_set_prec (x, px);
      mpfr_random (x);
      u = lrand48 ();
      for (py=2; py<300; py++)
	{
	  mpfr_set_prec (y, py);
	  mpfr_set_prec (z, py + px);
	  for (rnd=0; rnd<4; rnd++)
	    {
	      inexact = mpfr_ui_div (y, u, x, rnd);
	      if (mpfr_mul (z, y, x, rnd))
		{
		  fprintf (stderr, "z <- y * x should be exact\n");
		  exit (1);
		}
	      cmp = mpfr_cmp_ui (z, u);
	      if (((inexact == 0) && (cmp != 0)) ||
		  ((inexact > 0) && (cmp <= 0)) ||
		  ((inexact < 0) && (cmp >= 0)))
		{
		  fprintf (stderr, "Wrong inexact flag for u=%lu, rnd=%s\n", u,
			   mpfr_print_rnd_mode(rnd));
		  printf ("expected %d, got %d\n", cmp, inexact);
		  printf ("x="); mpfr_print_binary (x); putchar ('\n');
		  printf ("y="); mpfr_print_binary (y); putchar ('\n');
		  printf ("y*x="); mpfr_print_binary (z); putchar ('\n');
		  exit (1);
		}
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
#ifdef TEST
  double x; unsigned long y, N; int i,rnd_mode,rnd;
#ifdef __mips
    /* to get denormalized numbers on IRIX64 */
    union fpc_csr exp;
    exp.fc_word = get_fpc_csr();
    exp.fc_struct.flush = 0;
    set_fpc_csr(exp.fc_word);
#endif

  srand48(getpid());
  N = (argc<2) ? 1000000 : atoi(argv[1]);
  rnd_mode = (argc<3) ? -1 : atoi(argv[2]);
  for (i=0;i<1000000;i++) {
    x = drand(); 
    y = lrand48();
    if (ABS(x)>4e-286) {
      /* avoid denormalized numbers and overflows */
      rnd = (rnd_mode==-1) ? lrand48()%4 : rnd_mode;
      check(y, x, rnd, 0.0);
    }
  }
#endif
  check_inexact ();
  check(1, 1.0/0.0, GMP_RNDN, 0.0); 
  check(1, -1.0/0.0, GMP_RNDN, -0.0); 
  check(1, 0.0/0.0, GMP_RNDN, 0.0/0.0); 
  check(0, 0.0, GMP_RNDN, 0.0/0.0); 
  check(948002822, 1.22191250737771397120e+20, GMP_RNDN,
	7.758352715731357946e-12);
  check(1976245324, 1.25296395864546893357e+232, GMP_RNDZ,
	1.5772563211925444801e-223);
  check(740454110, 2.11496253355831863313e+183, GMP_RNDZ,
	3.5010270784996976041e-175);
  check(1690540942, 1.28278599852446657468e-276, GMP_RNDU,
	1.3178666932321966062e285);
  check(1476599377, -2.14191393656148625995e+305, GMP_RNDD,
	-6.8938315017943889615e-297);

  return 0;
}
