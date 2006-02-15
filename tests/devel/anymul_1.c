/*
Copyright 1996, 1997, 1998, 1999, 2000, 2001, 2002 Free Software Foundation,
Inc.

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
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301, USA.
*/

#include <stdlib.h>
#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#include "tests.h"

#ifdef OPERATION_mul_1
#define func __gmpn_mul_1
#define reffunc refmpn_mul_1
#define funcname "mpn_mul_1"
#endif

#ifdef OPERATION_addmul_1
#define func __gmpn_addmul_1
#define reffunc refmpn_addmul_1
#define funcname "mpn_addmul_1"
#endif

#ifdef OPERATION_submul_1
#define func __gmpn_submul_1
#define reffunc refmpn_submul_1
#define funcname "mpn_submul_1"
#endif

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

static void mpn_print (mp_ptr, mp_size_t);

#define M * 1000000

#ifndef CLOCK
#error "Don't know CLOCK of your machine"
#endif

#ifndef OPS
#define OPS (CLOCK/5)
#endif
#ifndef SIZE
#define SIZE 496
#endif
#ifndef TIMES
#define TIMES OPS/(SIZE+1)
#endif

main (int argc, char **argv)
{
  mp_ptr s1, dx, dy;
  mp_limb_t cyx, cyy;
  int i;
  long t0, t;
  unsigned int test;
  mp_limb_t xlimb;
  mp_size_t size;
  double cyc;
  unsigned int ntests;

  s1 = malloc (SIZE * sizeof (mp_limb_t));
  dx = malloc ((SIZE + 2) * sizeof (mp_limb_t));
  dy = malloc ((SIZE + 2) * sizeof (mp_limb_t));

  ntests = ~(unsigned) 0;
  if (argc == 2)
    ntests = strtol (argv[1], 0, 0);

  for (test = 1; test <= ntests; test++)
    {
#if TIMES == 1 && ! defined (PRINT)
      if (test % (SIZE > 10000 ? 1 : 10000 / SIZE) == 0)
	{
	  printf ("\r%u", test);
	  fflush (stdout);
	}
#endif

#ifdef RANDOM
      size = random () % SIZE + 1;
#else
      size = SIZE;
#endif

      dy[0] = 0x87654321;
      dy[size+1] = 0x12345678;

#ifdef FIXED_XLIMB
      xlimb = FIXED_XLIMB;
#else
      mpn_random2 (&xlimb, 1);
#endif

#if TIMES != 1
      mpn_random (s1, size);
      mpn_random (dy+1, size);

      MPN_COPY (dx, dy, size+2);
      t0 = cputime();
      for (i = 0; i < TIMES; i++)
	func (dx+1, s1, size, xlimb);
      t = cputime() - t0;
      cyc = ((double) t * CLOCK) / (TIMES * size * 1000.0);
      printf (funcname ":    %5ldms (%.3f cycles/limb) [%.2f Gb/s]\n",
	      t, cyc,
	      CLOCK/cyc*BITS_PER_MP_LIMB*BITS_PER_MP_LIMB/1e9);
#endif

#ifndef NOCHECK
      mpn_random2 (s1, size);
#ifdef ZERO
      memset (dy+1, 0, size * sizeof *s1);
#else
      mpn_random2 (dy+1, size);
#endif
#if defined (PRINT) || defined (XPRINT)
      printf ("xlimb=");
      mpn_print (&xlimb, 1);
#endif
#ifdef PRINT
#ifndef OPERATION_mul_1
      printf ("%*s ", (int) (2 * sizeof(mp_limb_t)), "");
      mpn_print (dy+1, size);
#endif
      printf ("%*s ", (int) (2 * sizeof(mp_limb_t)), "");
      mpn_print (s1, size);
#endif

      MPN_COPY (dx, dy, size+2);
      cyx = reffunc (dx+1, s1, size, xlimb);
      cyy = func (dy+1, s1, size, xlimb);

#ifdef PRINT
      mpn_print (&cyx, 1);
      mpn_print (dx+1, size);
      mpn_print (&cyy, 1);
      mpn_print (dy+1, size);
#endif

      if (cyx != cyy || mpn_cmp (dx, dy, size+2) != 0
	  || dx[0] != 0x87654321 || dx[size+1] != 0x12345678)
	{
#ifndef PRINT
	  mpn_print (&cyx, 1);
	  mpn_print (dx+1, size);
	  mpn_print (&cyy, 1);
	  mpn_print (dy+1, size);
#endif
	  printf ("\n");
	  printf ("%*s ", (int) (2 * sizeof(mp_limb_t)), "DIFF:");
	  for (i = size; i != 0; i--)
	    {
	      mp_limb_t diff = dy[i] ^ dx[i];
	      if (diff != 0)
		printf ("%*lX", (int) (2 * sizeof(mp_limb_t)), diff);
	      else
		printf ("%*s", (int) (2 * sizeof(mp_limb_t)), "");
#ifdef SPACE
	      if (i != 0)
		printf (" ");
#endif
	    }
	  printf ("\n");
	  if (dy[0] != 0x87654321)
	    printf ("clobbered at low end\n");
	  if (dy[size+1] != 0x12345678)
	    printf ("clobbered at high end\n");
	  printf ("TEST NUMBER %u\n", test);
	  abort();
	}
#endif
    }
  exit (0);
}

static void
mpn_print (mp_ptr p, mp_size_t size)
{
  mp_size_t i;

  for (i = size - 1; i >= 0; i--)
    {
#ifdef _LONG_LONG_LIMB
      printf ("%0*lX%0*lX", (int) (sizeof(mp_limb_t)),
	      (unsigned long) (p[i] >> (BITS_PER_MP_LIMB/2)),
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
