/* Exercise the lc2exp random functions.

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
#include <stdlib.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "tests.h"


/* a=0 and c=0 produces zero results always. */
void
check_zero (unsigned long m2exp)
{
  gmp_randstate_t  r;
  mpz_t            a;
  unsigned long    c;
  int              i;

  mpz_init_set_ui (a, 0L);
  c = 0L;

  gmp_randinit_lc_2exp (r, a, c, m2exp);
  gmp_randseed_ui (r, 0L);

  for (i = 0; i < 5; i++)
    {
      mpz_urandomb (a, r, 123L);
      if (mpz_sgn (a) != 0)
        printf ("check_zero m2exp=%lu: didn't get zero\n", m2exp);
    }

  mpz_clear (a);
  gmp_randclear (r);
}

/* negative a */
void
check_nega (void)
{
  gmp_randstate_t  r;
  mpz_t            a;
  unsigned long    c, m2exp;
  int              i;

  mpz_init (a);
  mpz_setbit (a, 1000L);
  mpz_neg (a, a);
  c = 0L;
  m2exp = 45L;

  gmp_randinit_lc_2exp (r, a, c, m2exp);
  gmp_randseed_ui (r, 0L);

  for (i = 0; i < 5; i++)
    {
      mpz_urandomb (a, r, 123L);
      if (mpz_sgn (a) != 0)
        printf ("check_nega m2exp=%lu: didn't get zero\n", m2exp);
    }

  mpz_clear (a);
  gmp_randclear (r);
}

void
check_bigc (void)
{
  gmp_randstate_t  r;
  mpz_t            a;
  unsigned long    c, m2exp, bits;
  int              i;

  mpz_init_set_ui (a, 0L);
  c = ULONG_MAX;
  m2exp = 8;

  gmp_randinit_lc_2exp (r, a, c, m2exp);
  gmp_randseed_ui (r, 0L);

  for (i = 0; i < 20; i++)
    {
      bits = 123L;
      mpz_urandomb (a, r, bits);
      if (mpz_sgn (a) < 0 || mpz_sizeinbase (a, 2) > bits)
        {
          printf     ("check_bigc: mpz_urandomb out of range\n");
          printf     ("   m2exp=%lu\n", m2exp);
          gmp_printf ("   a=%#ZX\n", a);
          gmp_printf ("   sizeinbase2=%u\n", mpz_sizeinbase (a, 2));
        }
    }

  mpz_clear (a);
  gmp_randclear (r);
}

int
main (void)
{
  tests_start ();

  check_zero (2L);
  check_zero (7L);
  check_zero (32L);
  check_zero (64L);
  check_zero (1000L);

  check_nega ();
  check_bigc ();

  tests_end ();
  exit (0);
}
