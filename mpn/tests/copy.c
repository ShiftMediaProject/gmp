/*
Copyright (C) 1999, 2000 Free Software Foundation, Inc.

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
MA 02111-1307, USA.
*/

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

#ifndef CLOCK
#if defined (__m88k__)
#define CLOCK 20 M
#elif defined (__i386__)
#define CLOCK (16666667)
#elif defined (__m68k__)
#define CLOCK (20 M)
#elif defined (_IBMR2)
#define CLOCK (25 M)
#elif defined (__sparc__)
#define CLOCK (20 M)
#elif defined (__sun__)
#define CLOCK (20 M)
#elif defined (__mips)
#define CLOCK (40 M)
#elif defined (__hppa__)
#define CLOCK (50 M)
#elif defined (__alpha)
#define CLOCK (133 M)
#else
#error "Don't know CLOCK of your machine"
#endif
#endif

#ifndef OPS
#define OPS (CLOCK/2)
#endif
#ifndef SIZE
#define SIZE 328
#endif
#ifndef TIMES
#define TIMES (OPS/SIZE)
#else
#undef OPS
#define OPS (SIZE*TIMES)
#endif


void
#if __STDC__
refmpn_copyi (mp_ptr rptr, mp_srcptr sptr, mp_size_t n)
#else
refmpn_copyi (rptr, sptr, n)
     register mp_ptr rptr;
     register mp_srcptr sptr;
     mp_size_t n;
#endif
{
  mp_size_t i;

  for (i = 0; i < n; i++)
    rptr[i] = sptr[i];
}

main (argc, argv)
     int argc;
     char **argv;
{
  mp_limb_t s1[SIZE];
  mp_limb_t dx[SIZE+2];
  mp_limb_t dy[SIZE+2];
  int i;
  long t0, t;
  int test;
  mp_size_t size;

  for (test = 0; ; test++)
    {
#if TIMES == 1 && ! defined (PRINT)
      if (test % (SIZE > 10000 ? 1 : 10000 / SIZE) == 0)
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

      dx[0] = 0x87654321;
      dy[0] = 0x87654321;
      dx[size+1] = 0x12345678;
      dy[size+1] = 0x12345678;

#if TIMES != 1
      mpn_random (s1, size);

#ifndef NOCHECK
      t0 = cputime();
      for (i = 0; i < TIMES; i++)
	refmpn_copyi (dx+1, s1, size);
      t = cputime() - t0;
      printf ("refmpn_copyi:   %ldms (%.2f cycles/limb)\n",
	      t, ((double) t * CLOCK) / (OPS * 1000.0));
#endif

      t0 = cputime();
      for (i = 0; i < TIMES; i++)
	MPN_COPY_INCR (dx+1, s1, size);
      t = cputime() - t0;
      printf ("MPN_COPY_INCR:   %ldms (%.2f cycles/limb)\n",
	      t, ((double) t * CLOCK) / (OPS * 1000.0));
#endif

#ifndef NOCHECK
      mpn_random2 (s1, size);

#ifdef PRINT
      mpn_print (s1, size);
#endif

      /* Put garbage in the destination.  */
      for (i = 0; i < size; i++)
	{
	  dx[i+1] = 0xdead;
	  dy[i+1] = 0xbeef;
	}

      refmpn_copyi (dx+1, s1, size);
      MPN_COPY_INCR (dy+1, s1, size);
#ifdef PRINT
      mpn_print (dx+1, size);
      mpn_print (dy+1, size);
#endif
      if (mpn_cmp (dx, dy, size+2) != 0
	  || dx[0] != 0x87654321 || dx[size+1] != 0x12345678)
	{
#ifndef PRINT
	  mpn_print (dx+1, size);
	  mpn_print (dy+1, size);
#endif
	  printf ("TEST NUMBER %d\n", test);
	  abort();
	}
#endif
    }
}

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
