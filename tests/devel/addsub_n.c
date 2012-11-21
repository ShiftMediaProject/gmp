/*
Copyright 1999, 2000, 2001, 2007, 2009 Free Software Foundation, Inc.

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
the GNU MP Library test suite.  If not, see http://www.gnu.org/licenses/.  */

#include <stdlib.h>
#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "tests.h"

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
#define TIMES OPS/SIZE
#endif

void mpn_print (mp_ptr, mp_size_t);

int
main (int argc, char **argv)
{
  mp_ptr mem;
  mp_ptr s1;
  mp_ptr s2;
  mp_ptr d1x;
  mp_ptr d2x;
  mp_ptr d1y;
  mp_ptr d2y;
  int cyx, cyy;
  int i;
  long t0, t;
  int test;
  mp_size_t size;
  unsigned int ntests;

  mem = malloc ((6 * SIZE + 8) * sizeof (mp_limb_t));

  s1 = mem + 0;
  s2 = mem + SIZE;
  d1x = mem + 2 * SIZE;
  d2x = mem + 3 * SIZE + 2;
  d1y = mem + 4 * SIZE + 4;
  d2y = mem + 5 * SIZE + 6;

  ntests = ~(unsigned) 0;
  if (argc == 2)
    ntests = strtol (argv[1], 0, 0);

  for (test = 1; test <= ntests; test++)
    {
#if TIMES == 1 && ! defined (PRINT)
      if (test % (SIZE > 100000 ? 1 : 100000 / SIZE) == 0)
	{
	  printf ("\r%d", test);
	  fflush (stdout);
	}
#endif

#ifdef RANDOM
      size = (random () % SIZE + 1);
#else
      size = SIZE;
#endif

      d1x[0] = 0x87654321;
      d1y[0] = 0x87654321;
      d1x[size+1] = 0x12345678;
      d1y[size+1] = 0x12345678;
      d2x[0] = 0x87654321;
      d2y[0] = 0x87654321;
      d2x[size+1] = 0x12345678;
      d2y[size+1] = 0x12345678;

#if TIMES != 1
      mpn_random (s1, size);
      mpn_random (s2, size);

      t0 = cputime();
      for (i = 0; i < TIMES; i++)
	mpn_add_n_sub_n (d1x+1, d2x+1, s1, s2, size);
      t = cputime() - t0;
      printf ("   mpn_add_n_sub_n:   %ldms (%.3f cycles/limb)\n",
	      t, ((double) t * CLOCK) / (TIMES * size * 1000.0));
#endif

#ifndef NOCHECK
      mpn_random2 (s1, size);
      mpn_random2 (s2, size);

#ifdef PRINT
      mpn_print (s1, size);
      mpn_print (s2, size);
#endif

      /* Put garbage in the destination.  */
      for (i = 0; i < size; i++)
	{
	  d1x[i+1] = 0xdead;
	  d1y[i+1] = 0xbeef;
	  d2x[i+1] = 0xdeed;
	  d2y[i+1] = 0xdeaf;
	}

      cyx = refmpn_add_n_sub_n (d1x+1, d2x+1, s1, s2, size);
      cyy = mpn_add_n_sub_n (d1y+1, d2y+1, s1, s2, size);
#ifdef PRINT
      printf ("ref add %d ", cyx); mpn_print (d1x+1, size);
      printf ("    add %d ", cyy); mpn_print (d1y+1, size);
      printf ("ref sub %d ", cyx); mpn_print (d2x+1, size);
      printf ("    sub %d ", cyy); mpn_print (d2y+1, size);
#endif
      if (cyx != cyy
	  || mpn_cmp (d1x, d1y, size+2) != 0
	  || mpn_cmp (d2x, d2y, size+2) != 0
	  || d1x[0] != 0x87654321 || d1x[size+1] != 0x12345678
	  || d2x[0] != 0x87654321 || d2x[size+1] != 0x12345678)
	{
#ifndef PRINT
	  printf ("ref add %d ", cyx); mpn_print (d1x+1, size);
	  printf ("    add %d ", cyy); mpn_print (d1y+1, size);
	  printf ("ref sub %d ", cyx); mpn_print (d2x+1, size);
	  printf ("    sub %d ", cyy); mpn_print (d2y+1, size);
#endif
	  printf ("\n");
	  if (d1y[0] != 0x87654321)
	    printf ("add clobbered at low end\n");
	  if (d1y[size+1] != 0x12345678)
	    printf ("add clobbered at high end\n");
	  if (d2y[0] != 0x87654321)
	    printf ("sub clobbered at low end\n");
	  if (d2y[size+1] != 0x12345678)
	    printf ("sub clobbered at high end\n");
	  printf ("TEST NUMBER %d\n", test);
	  abort();
	}
#endif
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
