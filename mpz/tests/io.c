/* Test conversion and I/O using mpz_out_str and mpz_inp_str.

Copyright (C) 1993, 1994, 1996, 2000 Free Software Foundation, Inc.

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

#define FILENAME  "io.tmp"

#ifndef SIZE
#define SIZE 16
#endif

void
debug_mp (mpz_t x, int base)
{
  mpz_out_str (stderr, base, x); fputc ('\n', stderr);
}

int
main (int argc, char **argv)
{
  mpz_t  op1, op2;
  mp_size_t size;
  int i;
  int reps = 10000;
  FILE *fp;
  int base;

  if (argc == 2)
    reps = atoi (argv[1]);

  mpz_init (op1);
  mpz_init (op2);

  fp = fopen (FILENAME, "w+"); 

  for (i = 0; i < reps; i++)
    {
      size = urandom () % SIZE - SIZE/2;

      mpz_random2 (op1, size);
      base = urandom () % 36 + 1;
      if (base == 1)
	base = 0;

      rewind (fp);  
      if (mpz_out_str (fp, base, op1) == 0
          || putc (' ', fp) == EOF
          || fflush (fp) != 0)
        {
          fprintf (stderr, "mpz_out_str write error\n");
          abort ();
        }

      rewind (fp);  
      if (mpz_inp_str (op2, fp, base) == 0)
        {
	  fprintf (stderr, "mpz_inp_str read error\n");
	  abort ();
	}

      if (mpz_cmp (op1, op2))
	{
	  fprintf (stderr, "ERROR\n");
	  fprintf (stderr, "op1  = "); debug_mp (op1, -16);
	  fprintf (stderr, "op2  = "); debug_mp (op2, -16);
	  fprintf (stderr, "base = %d\n", base);
	  abort ();
	}
    }

  fclose (fp);

  unlink (FILENAME);

  exit (0);
}
