/* Factoring with Pollard's rho method.

   Copyright (C) 1995, 1997 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; see the file COPYING.  If not, write to the Free
   Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <stdio.h>
#include "gmp.h"

int flag_mersenne = 0;

static unsigned add[] = {4, 2, 4, 2, 4, 6, 2, 6};

void
factor_using_division (t, limit)
     mpz_t t;
     unsigned int limit;
{
  mpz_t q, r;
  unsigned long int f;
  int i, ai;
  unsigned *addv = add;

  mpz_init (q);
  mpz_init (r);

  for (;;)
    {
      mpz_tdiv_qr_ui (q, r, t, 2L);
      if (mpz_cmp_ui (r, 0L) != 0)
	break;
      mpz_set (t, q);
      printf ("2 ");
      fflush (stdout);
    }

  for (;;)
    {
      mpz_tdiv_qr_ui (q, r, t, 3L);
      if (mpz_cmp_ui (r, 0L) != 0)
	break;
      mpz_set (t, q);
      printf ("3 ");
      fflush (stdout);
    }

  for (;;)
    {
      mpz_tdiv_qr_ui (q, r, t, 5L);
      if (mpz_cmp_ui (r, 0L) != 0)
	break;
      mpz_set (t, q);
      printf ("5 ");
      fflush (stdout);
    }

  f = 7;
  ai = 0;
  while (mpz_cmp_ui (t, 1) != 0)
    {
      mpz_tdiv_qr_ui (q, r, t, f);
      if (mpz_cmp_ui (r, 0L) != 0)
	{
	  f += addv[ai];
	  if (f > limit)
	    break;
	  if (mpz_cmp_ui (q, f) < 0)
	    break;
	  ai = (ai + 1) & 7;
	}
      else
	{
	  mpz_set (t, q);
	  printf ("%lu ", f);
	  fflush (stdout);
	}
    }

  mpz_clear (q);
  mpz_clear (r);
}

void
factor_using_pollard_rho (m, a_int, x0, p)
     mpz_t m;
     long a_int;
     long x0;
     unsigned long p;
{
  mpz_t x, y, q;
  mpz_t a;
  mpz_t d;
  mpz_t tmp;
  mpz_t n;
  int i = 1;
  int j = 1;

  mpz_init_set (n, m);

  mpz_init (d);
  mpz_init_set_ui (q, 1L);
  mpz_init (tmp);

  mpz_init_set_si (a, a_int);
  mpz_init_set_si (x, x0);
  mpz_init_set_si (y, x0);

  while (mpz_cmp_ui (n, 1L) != 0)
    {
      if (flag_mersenne)
	{
	  mpz_powm_ui (x, x, p, n);  mpz_add (x, x, a);
	  mpz_powm_ui (y, y, p, n);  mpz_add (y, y, a);
	  mpz_powm_ui (y, y, p, n);  mpz_add (y, y, a);
	}
      else
	{
	  mpz_mul (x, x, x);  mpz_add (x, x, a); mpz_mod (x, x, n);
	  mpz_mul (y, y, y);  mpz_add (y, y, a); mpz_mod (y, y, n);
	  mpz_mul (y, y, y);  mpz_add (y, y, a); mpz_mod (y, y, n);
	}

      if (mpz_cmp (x, y) > 0)
	mpz_sub (tmp, x, y);
      else
	mpz_sub (tmp, y, x);
      mpz_mul (q, q, tmp);
      mpz_mod (q, q, n);

      if (++i % j == 0)
	{
	  j += 1;
	  mpz_gcd (d, q, n);
	  if (mpz_cmp_ui (d, 1L) != 0)
	    {
	      if (!mpz_probab_prime_p (d, 25))
		factor_using_pollard_rho (d, (random () & 31) - 16,
					  (random () & 31), p);
	      else
		{
		  mpz_out_str (stdout, 10, d);
		  fflush (stdout);
		  fputc (' ', stdout);
		}
	      mpz_div (n, n, d);
	      mpz_mod (x, x, n);
	      mpz_mod (y, y, n);
	      if (mpz_probab_prime_p (n, 25))
		{
		  mpz_out_str (stdout, 10, n);
		  fflush (stdout);
		  fputc (' ', stdout);
		  break;
		}
	    }
	}
    }

  mpz_clear (n);
  mpz_clear (d);
  mpz_clear (q);
  mpz_clear (tmp);
  mpz_clear (a);
  mpz_clear (x);
  mpz_clear (y);
}

factor (t, a, x0, p)
     mpz_t t;
     long a;
     long x0;
     unsigned long p;
{
  factor_using_division (t, 1000000);
  if (mpz_cmp_ui (t, 1) != 0)
    {
      if (mpz_probab_prime_p (t, 25))
	mpz_out_str (stdout, 10, t);
      else
	factor_using_pollard_rho (t, a, x0, p);
    }
}

main (argc, argv)
     int argc;
     char *argv[];
{
  mpz_t t;
  long x0, a;
  unsigned long p;
  int i;

  for (i = 1; i < argc; i++)
    {
      if (!strncmp (argv[i], "-Mp", 3))
	{
	  p = atoi (argv[i] + 3);
	  mpz_init_set_ui (t, 1L);
	  mpz_mul_2exp (t, t, p);
	  mpz_sub_ui (t, t, 1L);
	  flag_mersenne = 1;
	}
      else
	{
	  p = 0;
	  mpz_init_set_str (t, argv[i], 0);
	}

      a = -1;
      x0 = 3;

      factor (t, a, x0, p);
      puts ("");
    }
}
