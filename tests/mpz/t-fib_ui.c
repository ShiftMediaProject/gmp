/* Test mpz_fib_ui.

Copyright 2000, 2001 Free Software Foundation, Inc.

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
#include <stdlib.h>
#include "gmp.h"
#include "gmp-impl.h"


/* Usage: t-fib_ui [x|num]

   Run with no arguments, testing goes up to the initial value of "limit"
   below.  With a number argument tests are carried up that far, or with a
   literal "x" tests are continued without limit (this being only meant for
   development purposes).

   The size tests performed are designed to partially replicate what will be
   going on in mpz_fac_ui.  There are some ASSERTs there, but they're not
   normally enabled.  */


#define MPZ_FIB_SIZE_FLOAT(n) \
  ((mp_size_t) ((n) * 0.6942419 / BITS_PER_MP_LIMB + 1))

int
main (int argc, char *argv[])
{
  unsigned long  n;
  unsigned long  limit = MAX (2000, 32*FIB_THRESHOLD);
  mpz_t          fn, fn1, r;

  if (argc > 1 && argv[1][0] == 'x')
    limit = ULONG_MAX;
  else if (argc > 1)
    limit = atoi (argv[1]);

  /* start at n==0 */
  mpz_init_set_ui (fn1, 1);  /* F[n-1] */
  mpz_init_set_ui (fn,  0);  /* F[n]   */
  mpz_init (r);

  for (n = 0; n < limit; n++)
    {
      if (MPZ_FIB_SIZE (n) < MPZ_FIB_SIZE_FLOAT (n))
        {
          printf ("MPZ_FIB_SIZE wrong at n=%lu\n", n);
          printf ("  MPZ_FIB_SIZE        %ld\n", MPZ_FIB_SIZE (n));
          printf ("  MPZ_FIB_SIZE_FLOAT  %ld\n", MPZ_FIB_SIZE_FLOAT (n));
          abort ();
        }

      mpz_fib_ui (r, n);

      if (MPZ_FIB_SIZE_FLOAT (n) < SIZ(r))
        {
          printf ("MPZ_FIB_SIZE_FLOAT wrong at n=%lu\n", n);
          printf ("  MPZ_FIB_SIZE_FLOAT  %ld\n", MPZ_FIB_SIZE_FLOAT (n));
          printf ("  SIZ(r)              %d\n", SIZ(r));
          abort ();
        }

      if (MPZ_FIB_SIZE (n) < SIZ(r))
        {
          printf ("MPZ_FIB_SIZE wrong at n=%lu\n", n);
          printf ("  MPZ_FIB_SIZE  %ld\n", MPZ_FIB_SIZE (n));
          printf ("  SIZ(r)        %d\n", SIZ(r));
          abort ();
        }

      MPZ_CHECK_FORMAT (r);

      if (mpz_cmp (r, fn) != 0)
        {
          printf ("mpz_fib_ui(%lu) wrong\n", n);
          printf ("  got  "); mpz_out_str (stdout, 10, r); printf("\n");
          printf ("  want "); mpz_out_str (stdout, 10, fn); printf("\n");
          abort ();
        }

      mpz_add (fn1, fn1, fn);  /* F[n+1] = F[n] + F[n-1] */
      mpz_swap (fn1, fn);
    }

  exit (0);
}
