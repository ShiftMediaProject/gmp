/* Test mpz_pow_ui and mpz_ui_pow_ui.

Copyright (C) 1997, 1999 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "urandom.h"

void debug_mp ();
void ref_mpz_pow_ui ();

main (argc, argv)
     int argc;
     char **argv;
{
  mpz_t base;
  mpz_t result, ref_result;
  mp_size_t base_size;
  unsigned long int exp;
  int i;
  int reps = 50000;

  if (argc == 2)
     reps = atoi (argv[1]);

  mpz_init (base);
  mpz_init (result);
  mpz_init (ref_result);

  for (i = 0; i < reps; i++)
    {
      base_size = urandom () % 8 - 4;
      exp = urandom () % 16;

      mpz_random2 (base, base_size);

      ref_mpz_pow_ui (ref_result, base, exp);

      mpz_pow_ui (result, base, exp);
      if (mpz_cmp (result, ref_result))
	{
	  fprintf (stderr, "ERROR (mpz_pow_ui):\n");
	  debug_mp ("      base = ", base, -16);
	  fprintf (stderr, "       exp = %lu (0x%lX)\n", exp, exp);
	  debug_mp ("    result = ", result, -16);
	  debug_mp ("ref_result = ", ref_result, -16);
	  abort ();
	}

      if (mpz_cmp_ui (base, 0L) >= 0
	  && mpz_cmp_ui (base, ~(unsigned long int) 0) <= 0)
	{
	  mpz_ui_pow_ui (result, mpz_get_ui (base), exp);
	  if (mpz_cmp (result, ref_result))
	    {
	      fprintf (stderr, "ERROR (mpz_ui_pow_ui):\n");
	      debug_mp ("      base = ", base, -16);
	      fprintf (stderr, "       exp = %lu (0x%lX)\n", exp, exp);
	      debug_mp ("    result = ", result, -16);
	      debug_mp ("ref_result = ", ref_result, -16);
	      abort ();
	    }
	}
    }

  exit (0);
}

void
ref_mpz_pow_ui (w, u, e)
     mpz_t w;
     const mpz_t u;
     unsigned long int e;
{
  mpz_t s, t;
  unsigned long int i;

  mpz_init_set_ui (t, 1);
  mpz_init_set (s, u);

  if ((e & 1) != 0)
    mpz_mul (t, t, s);
      
  for (i = 2; i <= e; i <<= 1)
    {
      mpz_mul (s, s, s);
      if ((i & e) != 0)
	mpz_mul (t, t, s);
    }

  mpz_set (w, t);
  mpz_clear (s);
  mpz_clear (t);
}

void
debug_mp (str, x, base)
     char *str;
     mpz_t x;
     int base;
{
  fprintf (stderr, "%s", str);
  mpz_out_str (stderr, base, x); fputc ('\n', stderr);
}
