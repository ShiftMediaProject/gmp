/* Test file for mpfr_set_str.

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
#include <string.h>
#include <time.h>
#include "gmp.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#include "mpfr-test.h"

int
main (int argc, char *argv[])
{
  mpfr_t x, y;
  unsigned long k, bd, nc, i;
  char *str, *str2;
  mp_exp_t e;
  int base, logbase, prec, baseprec;

  if (argc>=2) /* tset_str <string> <prec> */
    {
      prec = (argc>=3) ? atoi(argv[2]) : 53;
      mpfr_init2 (x, prec);
      mpfr_set_str_raw (x, argv[1]);
      mpfr_out_str (stdout, 10, 0, x, GMP_RNDN);
      putchar ('\n');
      mpfr_clear (x);
      return 0;
    }

  SEED_RAND (time(NULL));
  
  nc = (argc > 1) ? atoi(argv[1]) : 53;
  if (nc < 100)
    nc = 100;

  bd = LONG_RAND() & 8;
  
  str2 = str = (char *) malloc (nc * sizeof(char));

  if (bd)
    {
      for(k = 1; k <= bd; k++)
	*(str2++) = (LONG_RAND() & 1) + '0';
    }
  else
    *(str2++) = '0';

  *(str2++) = '.'; 

  for (k = 1; k < nc - 17 - bd; k++)
    *(str2++) = '0' + (LONG_RAND() & 1);

  *(str2++) = 'e'; 
  sprintf (str2, "%d", (int) LONG_RAND() - (1 << 30)); 

  mpfr_init2 (x, nc + 10); 
  mpfr_set_str_raw (x, str);

  mpfr_set_prec (x, 54);
  mpfr_set_str_raw (x, "0.100100100110110101001010010101111000001011100100101010E-529");
  mpfr_init2 (y, 54);
  mpfr_set_str (y, "4.936a52bc17254@-133", 16, GMP_RNDN);
  if (mpfr_cmp (x, y))
    {
      fprintf (stderr, "Error in mpfr_set_str\n");
      mpfr_print_binary(x);
      putchar('\n');
      mpfr_print_binary(y);
      putchar('\n');
      mpfr_clear(x);
      mpfr_clear(y); 
      exit(1);
    }

  free(str);

  mpfr_set_prec (x, 53);
  mpfr_set_str_raw (x, "+110101100.01010000101101000000100111001000101011101110E00");

  mpfr_set_str_raw (x, "1.0");
  if (mpfr_get_d1 (x) != 1.0)
    {
      fprintf (stderr, "Error in mpfr_set_str_raw for s=1.0\n"); 
      mpfr_clear(x);
      mpfr_clear(y); 
      exit(1);
    }

  mpfr_set_str_raw (x, "+0000");
  mpfr_set_str_raw (x, "+0000E0");
  mpfr_set_str_raw (x, "0000E0");
  if (mpfr_get_d1 (x) != 0.0)
    {
      fprintf (stderr, "Error in mpfr_set_str_raw for s=0.0\n"); 
      mpfr_clear (x);
      mpfr_clear (y);
      exit (1);
    }

  mpfr_set_str (x, "+243495834958.53452345E1", 10, GMP_RNDN);
  mpfr_set_str (x, "9007199254740993", 10, GMP_RNDN);
  mpfr_set_str (x, "9007199254740992", 10, GMP_RNDU);
  mpfr_set_str (x, "9007199254740992", 10, GMP_RNDD);
  mpfr_set_str (x, "9007199254740992", 10, GMP_RNDZ);

  /* check a random number printed and read is not modified */
  prec = 53;
  mpfr_set_prec (x, prec);
  mpfr_set_prec (y, prec);
  for (i=0;i<100000;i++)
    {
      mpfr_random (x);
      k = LONG_RAND() % 4;
      logbase = (LONG_RAND() % 5) + 1;
      base = 1 << logbase;
      /* Warning: the number of bits needed to print exactly a number of 
	 'prec' bits in base 2^logbase may be greater than ceil(prec/logbase),
	 for example 0.11E-1 in base 2 cannot be written exactly with only
	 one digit in base 4 */
      if (base == 2)
	baseprec = prec;
      else
	baseprec = 1 + (prec - 2 + logbase) / logbase;
      str = mpfr_get_str (NULL, &e, base, baseprec, x, k);
      mpfr_set_str (y, str, base, k);
      MPFR_EXP(y) += logbase * (e - strlen (str));
      if (mpfr_cmp (x, y))
	{
	  fprintf (stderr, "mpfr_set_str o mpfr_get_str <> id for rnd_mode=%s\n",
		   mpfr_print_rnd_mode (k));
	  printf ("x=");
	  mpfr_print_binary (x);
	  putchar('\n');
	  printf ("s=%s, exp=%d, base=%d\n", str, (int) e, base);
	  printf ("y=");
	  mpfr_print_binary (y);
	  putchar('\n');
	  mpfr_clear (x);
	  mpfr_clear (y);
	  exit (1);
	}
      free (str);
    }

  if (mpfr_set_str (x, "NaN", 10, GMP_RNDN) != 0 || !mpfr_nan_p(x))
    {
      fprintf (stderr, "mpfr_set_str failed on NaN\n");
      exit (1);
    }

  if (mpfr_set_str (x, "Inf", 10, GMP_RNDN) != 0 || !mpfr_inf_p(x) ||
      MPFR_SIGN(x) < 0)
    {
      fprintf (stderr, "mpfr_set_str failed on Inf\n");
      exit (1);
    }

  if (mpfr_set_str (x, "-Inf", 10, GMP_RNDN) != 0 || !mpfr_inf_p(x) ||
      MPFR_SIGN(x) > 0)
    {
      fprintf (stderr, "mpfr_set_str failed on -Inf\n");
      exit (1);
    }

  if (mpfr_set_str (x, "+Inf", 10, GMP_RNDN) != 0 || !mpfr_inf_p(x) ||
      MPFR_SIGN(x) < 0)
    {
      fprintf (stderr, "mpfr_set_str failed on +Inf\n");
      exit (1);
    }

  mpfr_clear (x);
  mpfr_clear (y);

  return 0;
}
