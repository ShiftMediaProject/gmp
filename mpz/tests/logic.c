/* Test mpz_com, mpz_and, mpz_ior, and mpz_xor.

Copyright (C) 1993, 1994, 1996, 1997 Free Software Foundation, Inc.

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

#ifndef SIZE
#define SIZE 16
#endif

main (argc, argv)
     int argc;
     char **argv;
{
  mpz_t x, y, r1, r2;
  mpz_t t1, t2, t3, t4;
  mp_size_t xsize, ysize;
  int i;
  int reps = 100000;

  if (argc == 2)
     reps = atoi (argv[1]);

  mpz_init (x);
  mpz_init (y);
  mpz_init (r1);
  mpz_init (r2);
  mpz_init (t1);
  mpz_init (t2);
  mpz_init (t3);
  mpz_init (t4);

  for (i = 0; i < reps; i++)
    {
      xsize = urandom () % SIZE - SIZE/2;
      mpz_random2 (x, xsize);

      ysize = urandom () % SIZE - SIZE/2;
      mpz_random2 (y, ysize);

      mpz_com (r1, x);
      MPZ_CHECK_FORMAT (r1);
      mpz_com (r1, r1);
      MPZ_CHECK_FORMAT (r1);
      if (mpz_cmp (r1, x) != 0)
	dump_abort ();

      mpz_com (r1, y);
      MPZ_CHECK_FORMAT (r1);
      mpz_com (r2, r1);
      MPZ_CHECK_FORMAT (r2);
      if (mpz_cmp (r2, y) != 0)
	dump_abort ();

      mpz_com (t1, x);
      MPZ_CHECK_FORMAT (t1);
      mpz_com (t2, y);
      MPZ_CHECK_FORMAT (t2);
      mpz_and (t3, t1, t2);
      MPZ_CHECK_FORMAT (t3);
      mpz_com (r1, t3);
      MPZ_CHECK_FORMAT (r1);
      mpz_ior (r2, x, y);
      MPZ_CHECK_FORMAT (r2);
      if (mpz_cmp (r1, r2) != 0)
	dump_abort ();

      mpz_com (t1, x);
      MPZ_CHECK_FORMAT (t1);
      mpz_com (t2, y);
      MPZ_CHECK_FORMAT (t2);
      mpz_ior (t3, t1, t2);
      MPZ_CHECK_FORMAT (t3);
      mpz_com (r1, t3);
      MPZ_CHECK_FORMAT (r1);
      mpz_and (r2, x, y);
      MPZ_CHECK_FORMAT (r2);
      if (mpz_cmp (r1, r2) != 0)
	dump_abort ();

      mpz_ior (t1, x, y);
      MPZ_CHECK_FORMAT (t1);
      mpz_and (t2, x, y);
      MPZ_CHECK_FORMAT (t2);
      mpz_com (t3, t2);
      MPZ_CHECK_FORMAT (t3);
      mpz_and (r1, t1, t3);
      MPZ_CHECK_FORMAT (r1);
      mpz_xor (r2, x, y);
      MPZ_CHECK_FORMAT (r2);
      if (mpz_cmp (r1, r2) != 0)
	dump_abort ();
    }

  exit (0);
}

dump_abort ()
{
  abort();
}

void
debug_mp (x, base)
     MP_INT *x;
{
  mpz_out_str (stderr, base, x); fputc ('\n', stderr);
}
