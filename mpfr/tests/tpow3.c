/* Test file for mpfr_pow.

Copyright 2001 Free Software Foundation.
Adapted from tarctan.c.

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
#include <limits.h>
#include <stdlib.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#include "mpfr-test.h"


int
main (int argc, char *argv[])
{
  mpfr_t x, y, z, ax;
  long int iy;
  mpfr_init (x);
  mpfr_init (ax);
  mpfr_init2 (y,sizeof(unsigned long int)*CHAR_BIT);
  mpfr_init (z);

  MPFR_SET_NAN(x);
  mpfr_random(y);
  mpfr_pow (z, x,y, GMP_RNDN);
  if(!MPFR_IS_NAN(z))
    {
      printf ("evaluation of function in x=NAN does not return NAN");
      exit (1);
    }

  MPFR_SET_NAN(y);
  mpfr_random(x);
  mpfr_pow (z, x,y, GMP_RNDN);
  if(!MPFR_IS_NAN(z))
    {
      printf ("evaluation of function in y=NAN does not return NAN");
      exit (1);
    }

  MPFR_CLEAR_FLAGS(z); 
  MPFR_CLEAR_FLAGS(y); 
  MPFR_CLEAR_FLAGS(x); 

  MPFR_SET_ZERO(y);
  mpfr_random(x);
  mpfr_pow (z, x,y, GMP_RNDN);
  if(mpfr_cmp_ui(z,1)!=0 && !(MPFR_IS_NAN(x)))
    {
      printf ("evaluation of function in y=0 does not return 1\n");
      printf ("x =");
      mpfr_out_str (stdout, 10, MPFR_PREC(x), x, GMP_RNDN);
      printf ("\n y =");
      mpfr_out_str (stdout, 10, MPFR_PREC(y), y, GMP_RNDN);
      printf ("\n result =");
      mpfr_out_str (stdout, 10, MPFR_PREC(z), z, GMP_RNDN);
      exit (1);
    }

  MPFR_CLEAR_FLAGS(z); 
  MPFR_CLEAR_FLAGS(y); 
  MPFR_CLEAR_FLAGS(x); 

  MPFR_SET_INF(y); 
  if (MPFR_SIGN(y) < 0)
    MPFR_CHANGE_SIGN(y);
  mpfr_random(x);
  mpfr_set_prec (ax, MPFR_PREC(x));
  mpfr_abs(ax,x,GMP_RNDN);
  mpfr_pow (z, x,y, GMP_RNDN);
  if( !MPFR_IS_INF(z) && (mpfr_cmp_ui(ax,1) > 0) )
    {
      printf ("evaluation of function in y=INF (|x|>1) does not return INF");
      exit (1);
    }
  if( !MPFR_IS_ZERO(z) && (mpfr_cmp_ui(ax,1) < 0) )
    {
      printf ("\nevaluation of function in y=INF (|x|<1) does not return 0");
      printf ("\nx =");
      mpfr_out_str (stdout, 10, MPFR_PREC(x), x, GMP_RNDN);
      printf ("\n y =");
      mpfr_out_str (stdout, 10, MPFR_PREC(y), y, GMP_RNDN);
      printf ("\n result =");
      mpfr_out_str (stdout, 10, MPFR_PREC(z), z, GMP_RNDN);
      putchar('\n');
      exit (1);
    }


  MPFR_CLEAR_FLAGS(z); 
  MPFR_CLEAR_FLAGS(y); 
  MPFR_CLEAR_FLAGS(x); 

  MPFR_SET_INF(y); 
  if (MPFR_SIGN(y) > 0)
    MPFR_CHANGE_SIGN(y);
  mpfr_random(x);
  mpfr_set_prec (ax, MPFR_PREC(x));
  mpfr_abs(ax,x,GMP_RNDN);
  mpfr_pow (z, x,y, GMP_RNDN);
  mpfr_pow (z, x,y, GMP_RNDN);
  if( !MPFR_IS_INF(z) && (mpfr_cmp_ui(ax,1) < 0) )
    {
      printf ("evaluation of function in y=INF (for |x| <0) does not return INF");
      exit (1);
    }
  if( !MPFR_IS_ZERO(z) && (mpfr_cmp_ui(ax,1) > 0) )
    {
      printf ("evaluation of function in y=INF (for |x| >0) does not return 0");
      exit (1);
    }

  MPFR_CLEAR_FLAGS(z); 
  MPFR_CLEAR_FLAGS(y); 
  MPFR_CLEAR_FLAGS(x); 

  MPFR_SET_INF(x);
  if (MPFR_SIGN(x) < 0)
    MPFR_CHANGE_SIGN(x);
  mpfr_random(y);
  mpfr_pow (z, x,y, GMP_RNDN);
  if(!MPFR_IS_INF(z) && (MPFR_SIGN(y) > 0))
    {
      printf ("evaluation of function in INF does not return INF");
      printf ("\nx =");
      mpfr_out_str (stdout, 10, MPFR_PREC(x), x, GMP_RNDN);
      printf ("\n y =");
      mpfr_out_str (stdout, 10, MPFR_PREC(y), y, GMP_RNDN);
      printf ("\n result =");
      mpfr_out_str (stdout, 10, MPFR_PREC(z), z, GMP_RNDN);
      putchar('\n');
      exit (1);
    }
  if(!MPFR_IS_ZERO(z) && (MPFR_SIGN(y) < 0))
    {
      printf ("evaluation of function in INF does not return INF");
      printf ("\nx =");
      mpfr_out_str (stdout, 10, MPFR_PREC(x), x, GMP_RNDN);
      printf ("\n y =");
      mpfr_out_str (stdout, 10, MPFR_PREC(y), y, GMP_RNDN);
      printf ("\n result =");
      mpfr_out_str (stdout, 10, MPFR_PREC(z), z, GMP_RNDN);
      putchar('\n');
      exit (1);
    }


  MPFR_CLEAR_FLAGS(z); 
  MPFR_CLEAR_FLAGS(y); 
  MPFR_CLEAR_FLAGS(x); 

  MPFR_SET_INF(x);
  if (MPFR_SIGN(x) > 0)
    MPFR_CHANGE_SIGN(x);
  mpfr_random(y);
  if (random() % 2)
    mpfr_neg (y, y, GMP_RNDN);
   mpfr_pow (z, x,y, GMP_RNDN);
  if(!MPFR_IS_INF(z) && (MPFR_SIGN(y) > 0) && (mpfr_isinteger(y)))
    {
      printf ("evaluation of function in x=-INF does not return INF");
      printf ("\nx =");
      mpfr_out_str (stdout, 10, MPFR_PREC(x), x, GMP_RNDN);
      printf ("\n y =");
      mpfr_out_str (stdout, 10, MPFR_PREC(y), y, GMP_RNDN);
      printf ("\n result =");
      mpfr_out_str (stdout, 10, MPFR_PREC(z), z, GMP_RNDN);
      putchar('\n');
      if(mpfr_isinteger(y))
        printf("y is an integer\n");
      else
        printf("y is not an integer\n");
        
      exit (1);
    }
  if(!MPFR_IS_ZERO(z) && (MPFR_SIGN(y) < 0) && (mpfr_isinteger(y)))
    {
      printf ("evaluation of function in x=-INF does not return 0");
      printf ("\nx =");
      mpfr_out_str (stdout, 10, MPFR_PREC(x), x, GMP_RNDN);
      printf ("\n y =");
      mpfr_out_str (stdout, 10, MPFR_PREC(y), y, GMP_RNDN);
      printf ("\n result =");
      mpfr_out_str (stdout, 10, MPFR_PREC(z), z, GMP_RNDN);
      putchar('\n');

      if(mpfr_isinteger(y))
        printf("y is an integer\n");
      else
        printf("y is not an integer\n");

      exit (1);
    }
  MPFR_CLEAR_FLAGS(z); 
  MPFR_CLEAR_FLAGS(y); 
  MPFR_CLEAR_FLAGS(x); 

  MPFR_SET_INF(x);
  if (MPFR_SIGN(x) > 0)
    MPFR_CHANGE_SIGN(x);

  iy=random();
  mpfr_random(y);
  if (random() % 2)
    iy=-iy;
  mpfr_set_d(y,iy,GMP_RNDN);
  mpfr_pow (z, x,y, GMP_RNDN);
  if(!MPFR_IS_INF(z) && (MPFR_SIGN(y) > 0) && (mpfr_isinteger(y)))
    {
      printf ("evaluation of function in x=-INF does not return INF");
      printf ("\nx =");
      mpfr_out_str (stdout, 10, MPFR_PREC(x), x, GMP_RNDN);
      printf ("\n y =");
      mpfr_out_str (stdout, 10, MPFR_PREC(y), y, GMP_RNDN);
      printf ("\n result =");
      mpfr_out_str (stdout, 10, MPFR_PREC(z), z, GMP_RNDN);
      putchar('\n');
      if(mpfr_isinteger(y))
        printf("y is an integer\n");
      else
        printf("y is not an integer\n");
        
      exit (1);
    }
  if(!MPFR_IS_ZERO(z) && (MPFR_SIGN(y) < 0) && (mpfr_isinteger(y)))
    {
      printf ("evaluation of function in x=-INF does not return 0");
      printf ("\nx =");
      mpfr_out_str (stdout, 10, MPFR_PREC(x), x, GMP_RNDN);
      printf ("\n y =");
      mpfr_out_str (stdout, 10, MPFR_PREC(y), y, GMP_RNDN);
      printf ("\n result =");
      mpfr_out_str (stdout, 10, MPFR_PREC(z), z, GMP_RNDN);
      putchar('\n');

      if(mpfr_isinteger(y))
        printf("y is an integer\n");
      else
        printf("y is not an integer\n");

      exit (1);
    }
  MPFR_CLEAR_FLAGS(z); 
  MPFR_CLEAR_FLAGS(y); 
  MPFR_CLEAR_FLAGS(x); 

  mpfr_set_ui(x,1,GMP_RNDN);
  MPFR_SET_INF(y);
  mpfr_pow (z, x,y, GMP_RNDN);
  if(!MPFR_IS_NAN(z))
    {
      printf ("evaluation of function in x=1, y=INF does not return NAN");
      printf ("\nx =");
      mpfr_out_str (stdout, 10, MPFR_PREC(x), x, GMP_RNDN);
      printf ("\n y =");
      mpfr_out_str (stdout, 10, MPFR_PREC(y), y, GMP_RNDN);
      printf ("\n result =");
      mpfr_out_str (stdout, 10, MPFR_PREC(z), z, GMP_RNDN);
      putchar('\n');

      exit (1);
    }

  MPFR_CLEAR_FLAGS(z); 
  MPFR_CLEAR_FLAGS(y); 
  MPFR_CLEAR_FLAGS(x); 

  MPFR_SET_ZERO(x);
  mpfr_random(y);
  if (random() % 2)
    mpfr_neg (y, y, GMP_RNDN);

  mpfr_pow (z, x,y, GMP_RNDN);

  if(!MPFR_IS_ZERO(z) && (MPFR_SIGN(y) < 0) && !(mpfr_isinteger(y)))
    {
      printf ("evaluation of function in y<0 does not return 0");
      printf ("\nx =");
      mpfr_out_str (stdout, 10, MPFR_PREC(x), x, GMP_RNDN);
      printf ("\n y =");
      mpfr_out_str (stdout, 10, MPFR_PREC(y), y, GMP_RNDN);
      printf ("\n result =");
      mpfr_out_str (stdout, 10, MPFR_PREC(z), z, GMP_RNDN);
      putchar('\n');

      exit (1);
    }
  if(!MPFR_IS_INF(z) && (MPFR_SIGN(y) < 0) && (mpfr_isinteger(y)))
    {
      printf ("evaluation of function in y<0 (y integer) does not return INF");
      printf ("\nx =");
      mpfr_out_str (stdout, 10, MPFR_PREC(x), x, GMP_RNDN);
      printf ("\n y =");
      mpfr_out_str (stdout, 10, MPFR_PREC(y), y, GMP_RNDN);
      printf ("\n result =");
      mpfr_out_str (stdout, 10, MPFR_PREC(z), z, GMP_RNDN);
      putchar('\n');
      exit (1);
    }
  if(!MPFR_IS_ZERO(z) && (MPFR_SIGN(y) > 0) && (mpfr_isinteger(y)))
    {
      printf ("evaluation of function in y<0 (y integer) does not return 0");
       printf ("\nx =");
      mpfr_out_str (stdout, 10, MPFR_PREC(x), x, GMP_RNDN);
      printf ("\n y =");
      mpfr_out_str (stdout, 10, MPFR_PREC(y), y, GMP_RNDN);
      printf ("\n result =");
      mpfr_out_str (stdout, 10, MPFR_PREC(z), z, GMP_RNDN);
      putchar('\n');
     exit (1);
    }

 
  {
  mp_prec_t prec, yprec;
  mpfr_t t, s;
  mp_rnd_t rnd;
  int inexact, compare, compare2;
  unsigned int n, err;

  int p0=2;
  int p1=100;
  int N=100;

  mpfr_init (s);
  mpfr_init (t);

  /* generic test */
  for (prec = p0; prec <= p1; prec++)
    {
      mpfr_set_prec (x, prec);
      mpfr_set_prec (s, sizeof(unsigned long int)*CHAR_BIT);
      mpfr_set_prec (z, prec);
      mpfr_set_prec (t, prec);
      yprec = prec + 10;

      for (n=0; n<N; n++)
	{
         
	  mpfr_random (x);

	  mpfr_random (s);
          if (random() % 2)
            mpfr_neg (s, s, GMP_RNDN);
	  rnd = random () % 4;
	  mpfr_set_prec (y, yprec);
	  compare = mpfr_pow (y, x, s, rnd);
	  err = (rnd == GMP_RNDN) ? yprec + 1 : yprec;
	  if (mpfr_can_round (y, err, rnd, rnd, prec))
	    {
	      mpfr_set (t, y, rnd);
	      inexact = mpfr_pow (z,x, s, rnd);	
	      if (mpfr_cmp (t, z))
		{
		  printf ("results differ for x=");
		  mpfr_out_str (stdout, 2, prec, x, GMP_RNDN);
                  printf (" values of the exponential=");
		  mpfr_out_str (stdout, 2, prec, s, GMP_RNDN);
		  printf (" prec=%u rnd_mode=%s\n", (unsigned) prec,
			  mpfr_print_rnd_mode (rnd));
		  printf ("got      ");
		  mpfr_out_str (stdout, 2, prec, z, GMP_RNDN);
		  putchar ('\n');
		  printf ("expected ");
		  mpfr_out_str (stdout, 2, prec, t, GMP_RNDN);
		  putchar ('\n');
		  printf ("approx  ");
		  mpfr_print_binary (y);
		  putchar ('\n');
		  exit (1);
		}
	      compare2 = mpfr_cmp (t, y);
	      /* if rounding to nearest, cannot know the sign of t - f(x)
		 because of composed rounding: y = o(f(x)) and t = o(y) */
	      if ((rnd != GMP_RNDN) && (compare * compare2 >= 0))
		compare = compare + compare2;
	      else
		compare = inexact; /* cannot determine sign(t-f(x)) */
	      if (((inexact == 0) && (compare != 0)) ||
		  ((inexact > 0) && (compare <= 0)) ||
		  ((inexact < 0) && (compare >= 0)))
		{
		  fprintf (stderr, "Wrong inexact flag for rnd=%s: expected %d, got %d\n",
			   mpfr_print_rnd_mode (rnd), compare, inexact);
		  printf ("x="); mpfr_print_binary (x); putchar ('\n');
		  printf ("y="); mpfr_print_binary (y); putchar ('\n');
		  printf ("t="); mpfr_print_binary (t); putchar ('\n');
		  exit (1);
		}
	    }
	}
    }

  mpfr_clear (s);
  mpfr_clear (t);

  }
  mpfr_clear (x);
  mpfr_clear (y);
  mpfr_clear (z);
  mpfr_clear (ax);

  return 0;
}
