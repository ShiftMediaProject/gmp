/* Test mpz_popcount.

Copyright 2001 Free Software Foundation, Inc.

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



void
check_onebit (void)
{
  mpz_t          n;
  unsigned long  i, got;

  mpz_init (n);
  for (i = 0; i < 5 * BITS_PER_MP_LIMB; i++)
    {
      mpz_setbit (n, i);
      got = mpz_popcount (n);
      if (got != 1)
        {
          printf ("mpz_popcount wrong on single bit at %lu\n", i); 
          printf ("   got %lu, want 1\n", got);
          abort();                                    
        }
      mpz_clrbit (n, i);
    }
  mpz_clear (n);
}


void
check_data (void)
{
  static const struct {
    const char     *n;
    unsigned long  want;
  } data[] = {
    { "-1", ~ (unsigned long) 0 },
    { "-12345678", ~ (unsigned long) 0 },
    { "0", 0 },
    { "1", 1 },
    { "3", 2 },
    { "5", 2 },
    { "0xFFFF", 16 },
    { "0xFFFFFFFF", 32 },
    { "0xFFFFFFFFFFFFFFFF", 64 },
    { "0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", 128 },
  };

  unsigned long   got;
  int    i;
  mpz_t  n;

  mpz_init (n);
  for (i = 0; i < numberof (data); i++)
    {
      mpz_set_str_or_abort (n, data[i].n, 0);
      got = mpz_popcount (n);
      if (got != data[i].want)
        {
          printf ("mpz_popcount wrong at data[%d]\n", i); 
          printf ("   n     \"%s\"\n", data[i].n);
          printf ("         ");   mpz_out_str (stdout, 10, n); printf ("\n");
          printf ("         0x"); mpz_out_str (stdout, 16, n); printf ("\n");
          printf ("   got   %lu\n", got);
          printf ("   want  %lu\n", data[i].want);
          abort();                                    
        }
    }
  mpz_clear (n);
}


int
main (void)
{
  tests_start ();

  check_onebit ();
  check_data ();

  tests_end ();
  exit (0);
}
