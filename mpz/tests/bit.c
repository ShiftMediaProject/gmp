/* Test mpz_setbit, mpz_clrbit, mpz_tstbit.

Copyright (C) 1997 Free Software Foundation, Inc.

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
#include "urandom.h"

void debug_mp ();

#ifndef SIZE
#define SIZE 4
#endif

main (argc, argv)
     int argc;
     char **argv;
{
  mpz_t x, s0, s1, s2, s3, m;
  mp_size_t xsize;
  int i;
  int reps = 100000;
  int bit0, bit1, bit2, bit3;
  unsigned long int bitindex;

  if (argc == 2)
     reps = atoi (argv[1]);

  mpz_init (x);
  mpz_init (s0);
  mpz_init (s1);
  mpz_init (s2);
  mpz_init (s3);
  mpz_init (m);

  for (i = 0; i < reps; i++)
    {
      xsize = urandom () % (2 * SIZE) - SIZE;
      mpz_random2 (x, xsize);
      bitindex = urandom () % SIZE;

      mpz_set (s0, x);
      bit0 = mpz_tstbit (x, bitindex);
      mpz_setbit (x, bitindex);
      MPZ_CHECK_FORMAT (x);

      mpz_set (s1, x);
      bit1 = mpz_tstbit (x, bitindex);
      mpz_clrbit (x, bitindex);
      MPZ_CHECK_FORMAT (x);

      mpz_set (s2, x);
      bit2 = mpz_tstbit (x, bitindex);
      mpz_setbit (x, bitindex);
      MPZ_CHECK_FORMAT (x);

      mpz_set (s3, x);
      bit3 = mpz_tstbit (x, bitindex);

      if (bit1 != 1 || bit2 != 0 || bit3 != 1)
	abort ();

      if (bit0 == 0)
	{
	  if (mpz_cmp (s0, s1) == 0 || mpz_cmp (s0, s2) != 0 || mpz_cmp (s0, s3) == 0)
	    abort ();
	}
      else
	{
	  if (mpz_cmp (s0, s1) != 0 || mpz_cmp (s0, s2) == 0 || mpz_cmp (s0, s3) != 0)
	    abort ();
	}

      if (mpz_cmp (s1, s2) == 0 || mpz_cmp (s1, s3) != 0)
	abort ();
      if (mpz_cmp (s2, s3) == 0)
	abort ();

      mpz_ui_pow_ui (m, 2L, bitindex);
      MPZ_CHECK_FORMAT (m);
      mpz_ior (x, s2, m);
      MPZ_CHECK_FORMAT (x);
      if (mpz_cmp (x, s3) != 0)
	abort ();

      mpz_com (m, m);
      MPZ_CHECK_FORMAT (m);
      mpz_and (x, s1, m);
      MPZ_CHECK_FORMAT (x);
      if (mpz_cmp (x, s2) != 0)
	abort ();
    }

  exit (0);
}

void
debug_mp (x, base)
     MP_INT *x;
{
  mpz_out_str (stderr, base, x); fputc ('\n', stderr);
}
