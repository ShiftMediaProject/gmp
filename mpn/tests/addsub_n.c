/*
Copyright (C) 1999 Free Software Foundation, Inc.

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
#define CLOCK (16.666667 M)
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
#define OPS 10000000
#endif
#ifndef SIZE
#define SIZE 328
#endif
#ifndef TIMES
#define TIMES OPS/SIZE
#else
#undef OPS
#define OPS (SIZE*TIMES)
#endif


mp_limb_t
#if __STDC__
refmpn_addsub_n (mp_ptr r1p, mp_ptr r2p,
		 mp_srcptr s1p, mp_srcptr s2p, mp_size_t n)
#else
refmpn_addsub_n (r1p, r2p, s1p, s2p, n)
     register mp_ptr r1p;
     register mp_ptr r2p;
     register mp_srcptr s1p;
     register mp_srcptr s2p;
     mp_size_t n;
#endif
{
  mp_ptr p;
  mp_limb_t acy, scy;

  p = malloc (n * BYTES_PER_MP_LIMB);
  acy = mpn_add_n (p, s1p, s2p, n);
  scy = mpn_sub_n (r2p, s1p, s2p, n);
  MPN_COPY (r1p, p, n);
  free (p);
  return 2 * acy + scy;
}

main (argc, argv)
     int argc;
     char **argv;
{
  mp_limb_t s1[SIZE];
  mp_limb_t s2[SIZE];
  mp_limb_t d1x[SIZE+2];
  mp_limb_t d2x[SIZE+2];
  mp_limb_t d1y[SIZE+2];
  mp_limb_t d2y[SIZE+2];
  int cyx, cyy;
  int i;
  long t0, t;
  int test;
  mp_size_t size;

  for (test = 0; ; test++)
    {
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
	refmpn_addsub_n (d1x+1, d2x+1, s1, s2, size);
      t = cputime() - t0;
      printf ("refmpn_addsub_n:   %ldms (%.2f cycles/limb)\n",
	      t, ((double) t * CLOCK) / (OPS * 1000.0));

      t0 = cputime();
      for (i = 0; i < TIMES; i++)
	mpn_addsub_n (d1x+1, d2x+1, s1, s2, size);
      t = cputime() - t0;
      printf ("   mpn_addsub_n:   %ldms (%.2f cycles/limb)\n",
	      t, ((double) t * CLOCK) / (OPS * 1000.0));
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

      cyx = refmpn_addsub_n (d1x+1, d2x+1, s1, s2, size);
      cyy = mpn_addsub_n (d1y+1, d2y+1, s1, s2, size);
#ifdef PRINT
      printf ("%d ", cyx); mpn_print (d1x+1, size);
      printf ("%d ", cyx); mpn_print (d2x+1, size);
      printf ("%d ", cyy); mpn_print (d1y+1, size);
      printf ("%d ", cyy); mpn_print (d2y+1, size);
#endif
      if (cyx != cyy
	  || mpn_cmp (d1x, d1y, size+2) != 0
	  || mpn_cmp (d2x, d2y, size+2) != 0
	  || d1x[0] != 0x87654321 || d1x[size+1] != 0x12345678
	  || d2x[0] != 0x87654321 || d2x[size+1] != 0x12345678)
	{
#ifndef PRINT
	  printf ("%d ", cyx); mpn_print (d1x+1, size);
	  printf ("%d ", cyx); mpn_print (d2x+1, size);
	  printf ("%d ", cyy); mpn_print (d1y+1, size);
	  printf ("%d ", cyy); mpn_print (d2y+1, size);
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
