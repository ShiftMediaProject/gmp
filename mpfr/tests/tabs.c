/* Test file for mpfr_abs.

Copyright 2000, 2001, 2002 Free Software Foundation, Inc.

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

void check_inexact _PROTO((void));

void
check_inexact (void)
{
  mp_prec_t p, q;
  mpfr_t x, y, absx;
  mp_rnd_t rnd;
  int inexact, cmp;

  mpfr_init (x);
  mpfr_init (y);
  mpfr_init (absx);
  
  for (p=2; p<500; p++)
    {
      mpfr_set_prec (x, p);
      mpfr_set_prec (absx, p);
      mpfr_random (x);
      if (LONG_RAND () % 2)
	{
	  mpfr_set (absx, x, GMP_RNDN);
	  mpfr_neg (x, x, GMP_RNDN);
	}
      else
	mpfr_set (absx, x, GMP_RNDN);
      for (q=2; q<2*p; q++)
	{
	  mpfr_set_prec (y, q);
	  for (rnd=0; rnd<4; rnd++)
	    {
	      inexact = mpfr_abs (y, x, rnd);
	      cmp = mpfr_cmp (y, absx);
	      if (((inexact == 0) && (cmp != 0)) ||
		  ((inexact > 0) && (cmp <= 0)) ||
		  ((inexact < 0) && (cmp >= 0)))
		{
		  fprintf (stderr, "Wrong inexact flag: expected %d, got %d\n", cmp, inexact);
		  printf ("x="); mpfr_print_binary (x); putchar ('\n');
		  printf ("absx="); mpfr_print_binary (absx); putchar ('\n');
		  printf ("y="); mpfr_print_binary (y); putchar ('\n');
		  exit (1);
		}
	    }
	}
    }

  mpfr_clear (x);
  mpfr_clear (y);
  mpfr_clear (absx);
}

int
main (int argc, char *argv[])
{
   mpfr_t x;
   int n, k, rnd;
   double d, absd, dd;

   mpfr_test_init ();

   check_inexact ();

   mpfr_init2(x, 53);

   mpfr_set_d(x, 1.0, GMP_RNDN);
   mpfr_abs(x, x, GMP_RNDN);
   if (mpfr_get_d1 (x) != 1.0) {
     fprintf(stderr, "Error in mpfr_abs(1.0)\n"); exit(1);
   }

   mpfr_set_d(x, -1.0, GMP_RNDN);
   mpfr_abs(x, x, GMP_RNDN);
   if (mpfr_get_d1 (x) != 1.0) {
     fprintf(stderr, "Error in mpfr_abs(-1.0)\n"); exit(1);
   }

   mpfr_set_inf (x, 1);
   mpfr_abs (x, x, GMP_RNDN);
   if (!mpfr_inf_p(x) || (mpfr_sgn(x) <= 0))
     {
       fprintf (stderr, "Error in mpfr_abs(Inf).\n");
       exit (1);
     }

   mpfr_set_inf (x, -1);
   mpfr_abs (x, x, GMP_RNDN);
   if (!mpfr_inf_p(x) || (mpfr_sgn(x) <= 0))
     {
       fprintf (stderr, "Error in mpfr_abs(-Inf).\n");
       exit (1);
     }

   n = (argc==1) ? 1000000 : atoi(argv[1]);
   for (k = 1; k <= n; k++)
     {
       do
	 {
	   d = drand ();
	   absd = ABS(d);
	 }
#ifdef HAVE_DENORMS
       while (0);
#else
       while (absd <= 2.2e-307);
#endif
       rnd = LONG_RAND() % 4;
       mpfr_set_d (x, d, 0);
       mpfr_abs (x, x, rnd);
       dd = mpfr_get_d1 (x);
       if (!isnan(d) && (dd != absd))
	 { 
	   fprintf(stderr, 
		   "Mismatch on d = %.20e\n", d);
	   fprintf(stderr, "dd=%.20e\n", dd);
	   mpfr_print_binary(x); putchar('\n');
	   exit(1);
	 } 
     }

   mpfr_clear(x);

   return 0;
}
