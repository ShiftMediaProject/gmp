/* Text mpz_urandomm.

Copyright 2002 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "tests.h"

#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif

int
check_params (void)
{
  gmp_randstate_t r1, r2;
  mpz_t a, b, m;
  int i;
  int result;

  result = TRUE;

  mpz_init (a);
  mpz_init (b);
  mpz_init (m);

  if (result)
    {
      /* Test the consistency between urandomm and urandomb. */
      gmp_randinit_default (r1);
      gmp_randinit_default (r2);
      gmp_randseed_ui (r1, 85);
      gmp_randseed_ui (r2, 85);
      mpz_set_ui (m, 0);
      mpz_setbit (m, 80);
      for (i = 0; i < 100; i++)
	{
	  mpz_urandomm (a, r1, m);
	  mpz_urandomb (b, r2, 80);
	  if (mpz_cmp (a, b) != 0)
	    {
	      result = FALSE;
	      printf ("mpz_urandomm != mpz_urandomb\n");
	      break;
	    }
	}
      gmp_randclear (r1);
      gmp_randclear (r2);
    }

  if (result)
    {
      /* Test that mpz_urandomm returns the correct result with a
	 broken LC.  */
      mpz_set_ui (a, 0);
      gmp_randinit_lc_2exp (r1, a, 0xFF, 8);
      mpz_set_ui (m, 5);
      /* Warning: This code hangs in gmp 4.1 and below */
      for (i = 0; i < 100; i++)
	{
	  mpz_urandomm (a, r1, m);
	  if (mpz_cmp_ui (a, 2) != 0)
	    {
	      result = FALSE;
	      gmp_printf ("mpz_urandomm returns %Zd instead of 2\n", a);
	      break;
	    }
	}
      gmp_randclear (r1);
    }

  if (result)
    {
      /* Test that the results are always in range for either
         positive or negative values of m.  */
      gmp_randinit_default (r1);
      mpz_set_ui (m, 5);
      mpz_set_si (b, -5);
      for (i = 0; i < 100; i++)
	{
	  mpz_urandomm (a, r1, m);
	  if (mpz_cmp_ui (a, 5) >= 0 || mpz_sgn (a) < 0)
	    {
	      result = FALSE;
	      gmp_printf ("Out-of-range or non-positive value: %Zd\n", a);
	      break;
	    }
	  mpz_urandomm (a, r1, b);
	  if (mpz_cmp_si (a, -5) <= 0 || mpz_sgn (a) > 0)
	    {
	      result = FALSE;
	      gmp_printf ("Out-of-range or non-negative value: %Zd\n", a);
              break;
	    }
	}
      gmp_randclear (r1);
    }

  if (result)
    {
      /* Test that m=1 forces always result=0.  */
      gmp_randinit_default (r1);
      mpz_set_ui (m, 1);
      for (i = 0; i < 100; i++)
	{
	  mpz_urandomm (a, r1, m);
	  if (mpz_sgn (a) != 0)
	    {
	      result = FALSE;
	      gmp_printf ("mpz_urandomm fails with m=1 (result=%Zd)\n", a);
              break;
	    }
	}
      gmp_randclear (r1);
    }

  mpz_clear (a);
  mpz_clear (b);
  mpz_clear (m);
  return result;
}

int
main (int argc, char *argv[])
{
  int result = TRUE;

  tests_start ();

  if (result)
    if (!check_params ())
      result = FALSE;

  tests_end ();

  if (result)
    return 0; /* pass */
  else
    return 1; /* fail */
}
