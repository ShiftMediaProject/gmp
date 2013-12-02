/*
Copyright 1999-2001, 2007, 2009 Free Software Foundation, Inc.

This file is part of the GNU MP Library test suite.

The GNU MP Library test suite is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 3 of the License,
or (at your option) any later version.

The GNU MP Library test suite is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
Public License for more details.

You should have received a copy of the GNU General Public License along with
the GNU MP Library test suite.  If not, see https://www.gnu.org/licenses/.  */

#include <stdlib.h>
#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"

#if defined (USG) || defined (__SVR4) || defined (_UNICOS) || defined (__hpux)
#include <time.h>

int
cputime ()
{
  if (CLOCKS_PER_SEC < 100000)
    return clock () * 1000 / CLOCKS_PER_SEC;
  return clock () / (CLOCKS_PER_SEC / 1000);
}
#else
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

int
cputime ()
{
  struct rusage rus;

  getrusage (0, &rus);
  return rus.ru_utime.tv_sec * 1000 + rus.ru_utime.tv_usec / 1000;
}
#endif

#define M * 1000000

#ifndef OPS
#define OPS 10000000
#endif
#ifndef SIZE
#define SIZE 328
#endif
#ifndef TIMES
#define TIMES OPS/(SIZE+1)
#endif

void mpn_print (mp_ptr, mp_size_t);

int
main (int argc, char **argv)
{
  int test;
  mp_ptr a1, a2, a3, a4;
  mp_ptr s1p, s2p;
  mp_ptr ref1p, ref2p;
  mp_ptr r1p, r2p;
  int i;
  mp_size_t n;
  mp_limb_t refcy, cy;

  a1 = malloc ((SIZE + 2) * BYTES_PER_MP_LIMB);
  a2 = malloc ((SIZE + 2) * BYTES_PER_MP_LIMB);
  a3 = malloc ((SIZE + 2) * BYTES_PER_MP_LIMB);
  a4 = malloc ((SIZE + 2) * BYTES_PER_MP_LIMB);

  ref1p = malloc (SIZE * BYTES_PER_MP_LIMB);
  ref2p = malloc (SIZE * BYTES_PER_MP_LIMB);

  for (test = 0; ; test++)
    {
      if (test % (SIZE > 10000 ? 1 : 10000 / SIZE) == 0)
	{
	  printf ("\r%d", test);
	  fflush (stdout);
	}

#ifdef RANDOM
      n = (random () % SIZE + 1);
#else
      n = SIZE;
#endif

      for (i = 0; i <= 6; i++)
	{
	  mpn_random2 (a1, n + 2);
	  mpn_random2 (a2, n + 2);
	  mpn_random2 (a3, n + 2);
	  mpn_random2 (a4, n + 2);

	  switch (i)
	    {
	    case 0:
	      r1p = a1 + 1;
	      r2p = a2 + 1;
	      s1p = a3 + 1;
	      s2p = a4 + 1;
	      break;
	    case 1:
	      r1p = a1 + 1;
	      r2p = a2 + 1;
	      s1p = a1 + 1;
	      s2p = a4 + 1;
	      break;
	    case 2:
	      r1p = a1 + 1;
	      r2p = a2 + 1;
	      s1p = a3 + 1;
	      s2p = a2 + 1;
	      break;
	    case 3:
	      r1p = a1 + 1;
	      r2p = a2 + 1;
	      s1p = a2 + 1;
	      s2p = a4 + 1;
	      break;
	    case 4:
	      r1p = a1 + 1;
	      r2p = a2 + 1;
	      s1p = a3 + 1;
	      s2p = a1 + 1;
	      break;
	    case 5:
	      r1p = a1 + 1;
	      r2p = a2 + 1;
	      s1p = a1 + 1;
	      s2p = a2 + 1;
	      break;
	    case 6:
	      r1p = a1 + 1;
	      r2p = a2 + 1;
	      s1p = a2 + 1;
	      s2p = a1 + 1;
	      break;
	    }
	  r1p[-1] = 0x87654321;
	  r1p[n] = 0x12345678;
	  r2p[-1] = 0x87654321;
	  r2p[n] = 0x12345678;

	  refcy = refmpn_add_n_sub_n (ref1p, ref2p, s1p, s2p, n);
	  cy = mpn_add_n_sub_n (r1p, r2p, s1p, s2p, n);

	  if (refcy != cy
	      || mpn_cmp (ref1p, r1p, n) != 0
	      || mpn_cmp (ref2p, r2p, n) != 0
	      || r1p[-1] != 0x87654321 || r1p[n] != 0x12345678
	      || r2p[-1] != 0x87654321 || r2p[n] != 0x12345678)
	    {
	      printf ("\nTEST NUMBER %d/%d\n", test, i);
	      abort();
	    }
	}
    }

  exit (0);
}

void
mpn_print (mp_ptr p, mp_size_t size)
{
  mp_size_t i;

  for (i = size - 1; i >= 0; i--)
    {
#ifdef _LONG_LONG_LIMB
      printf ("%0*lX%0*lX", (int) (sizeof(mp_limb_t)),
	      (unsigned long) (p[i] >> (GMP_LIMB_BITS/2)),
              (int) (sizeof(mp_limb_t)), (unsigned long) (p[i]));
#else
      printf ("%0*lX", (int) (2 * sizeof(mp_limb_t)), p[i]);
#endif
#ifdef SPACE
      if (i != 0)
	printf (" ");
#endif
    }
  puts ("");
}
