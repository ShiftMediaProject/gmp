/* Tests of mpz_scan0 and mpz_scan1.

Copyright 2000, 2001, 2002 Free Software Foundation, Inc.

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


unsigned long
refmpz_scan (mpz_srcptr z, unsigned long i, int sought)
{
  unsigned long  z_bits = (unsigned long) ABSIZ(z) * GMP_NUMB_BITS;

  do
    {
      if (mpz_tstbit (z, i) == sought)
        return i;
      i++;
    }
  while (i <= z_bits);

  return ULONG_MAX;
}

unsigned long
refmpz_scan0 (mpz_srcptr z, unsigned long starting_bit)
{
  return refmpz_scan (z, starting_bit, 0);
}

unsigned long
refmpz_scan1 (mpz_srcptr z, unsigned long starting_bit)
{
  return refmpz_scan (z, starting_bit, 1);
}


void
check_ref (void)
{
  mpz_t          z;
  int            test, size, neg, sought;
  unsigned long  i, got, want;

  mpz_init (z);
  for (test = 0; test < 10; test++)
    {
      for (size = 0; size < 5; size++)
        {
          mpz_random2 (z, size);

          for (neg = 0; neg <= 1; neg++)
            {
              if (neg)
                mpz_neg (z, z);

              for (i = 0; i < size * GMP_NUMB_BITS + 8; i++)
                {
                  for (sought = 0; sought <= 1; sought++)
                    {
                      if (sought == 0)
                        {
                          got = mpz_scan0 (z, i);
                          want = refmpz_scan0 (z, i);
                        }
                      else
                        {
                          got = mpz_scan1 (z, i);
                          want = refmpz_scan1 (z, i);
                        }

                      if (got != want)
                        {
                          printf ("wrong at test=%d, size=%d, neg=%d, i=%lu, sought=%d\n",
                                  test, size, neg, i, sought);
                          printf ("   z 0x");
                          mpz_out_str (stdout, -16, z);
                          printf ("\n");
                          printf ("   got=%lu, want=%lu\n", got, want);
                          exit (1);                  
                        }
                    }
                }
            }
        }
    }
  mpz_clear (z);
}


int
main (int argc, char *argv[])
{
  tests_start ();

  check_ref ();

  tests_end ();
  exit (0);
}
