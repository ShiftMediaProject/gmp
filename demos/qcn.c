/* Use mpz_kronecker_ui() to calculate an estimate for the quadratic
   class number h(d), for a given negative fundamental discriminant, using
   Dirichlet's analytic formula. */

/*
Copyright (C) 1999, 2000 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your option)
any later version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA.  */


/* Usage: qcn <discriminant>...

   A fundamental discriminant means one of the form D or 4*D with D
   square-free.  Each argument is checked to see it's congruent to 0 or 1
   mod 4 (as all discriminants must be), and that it's negative, but there's
   no check on D being square-free.

   This program is a bit of a toy, there are better methods for calculating
   the class number and class group structure.

   Reference:

   Daniel Shanks, "Class Number, A Theory of Factorization, and Genera",
   Proc. Symp. Pure Math., vol 20, 1970, pages 415-440.

*/

#include <math.h>
#include <stdio.h>

#include "gmp.h"


/* A simple but slow primality test.  */
int
prime_p (unsigned long n)
{
  unsigned long  i, limit;

  if (n == 2)
    return 1;
  if (n < 2 || !(n&1))
    return 0;

  limit = (unsigned long) floor (sqrt ((double) n));
  for (i = 3; i <= limit; i+=2)
    if ((n % i) == 0)
      return 0;

  return 1;
}


/* The formula is as follows, with d < 0.

               w * sqrt(-d)      inf      p
        h(d) = ------------ *  product --------
                  2 * pi         p=2   p - (d/p)
                             

   (d/p) is the Kronecker symbol and the product is over primes p.  w is 6
   when d=-3, 4 when d=-4, or 2 otherwise.
  
   Calculating the product up to p=infinity would take a long time, so for
   the estimate primes up to 132,000 are used.  Shanks found this giving an
   accuracy of about 1 part in 1000, in normal cases.  */

double
qcn_estimate (mpz_t d)
{
#define P_LIMIT  132000

  double  h;
  unsigned long  p;

  /* p=2 */
  h = sqrt (-mpz_get_d (d)) / M_PI
    * 2.0 / (2.0 - mpz_kronecker_ui (d, 2));

  if (mpz_cmp_si (d, -3) == 0)       h *= 3;
  else if (mpz_cmp_si (d, -4) == 0)  h *= 2;

  for (p = 3; p < P_LIMIT; p += 2)
    if (prime_p (p))
      h *= (double) p / (double) (p - mpz_kronecker_ui (d, p));

  return h;
}


void
qcn_str (char *num)
{
  mpz_t  z;

  mpz_init_set_str (z, num, 0);

  if (mpz_sgn (z) >= 0)
    {
      mpz_out_str (stdout, 0, z);
      printf (" is not supported (negatives only)\n");
    }
  else if (mpz_fdiv_ui (z, 4) != 0 && mpz_fdiv_ui (z, 4) != 1)
    {
      mpz_out_str (stdout, 0, z);
      printf (" is not a discriminant (must == 0 or 1 mod 4)\n");
    }
  else
    {
      printf ("h(");
      mpz_out_str (stdout, 0, z);
      printf (") approx %.1f\n", qcn_estimate (z));
    }
  mpz_clear (z);
}


int
main (int argc, char *argv[])
{
  int  i;

  if (argc < 2)
    {
      qcn_str ("-85702502803");           /* is 16259   */
      qcn_str ("-328878692999");          /* is 1499699 */
      qcn_str ("-928185925902146563");    /* is 52739552 */
      qcn_str ("-84148631888752647283");  /* is 496652272 */
    }
  else
    {
      for (i = 1; i < argc; i++)
        qcn_str (argv[i]);
    }

  return 0;
}
