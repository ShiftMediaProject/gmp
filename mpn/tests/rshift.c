/*
Copyright (C) 1996, 1998, 1999 Free Software Foundation, Inc.

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
#define OPS (CLOCK/5)
#endif
#ifndef SIZE
#define SIZE 496
#endif
#ifndef TIMES
#define TIMES OPS/SIZE
#else
#undef OPS
#define OPS (SIZE*TIMES)
#endif

mp_limb_t
refmpn_rshift (wp, up, usize, cnt)
     register mp_ptr wp;
     register mp_srcptr up;
     mp_size_t usize;
     register unsigned int cnt;
{
  register mp_limb_t high_limb, low_limb;
  register unsigned sh_1, sh_2;
  register mp_size_t i;
  mp_limb_t retval;

#ifdef DEBUG
  if (usize == 0 || cnt == 0)
    abort ();
#endif

  sh_1 = cnt;
#if 0
  if (sh_1 == 0)
    {
      if (wp != up)
	{
	  /* Copy from low end to high end, to allow specified input/output
	     overlapping.  */
	  for (i = 0; i < usize; i++)
	    wp[i] = up[i];
	}
      return 0;
    }
#endif

  wp -= 1;
  sh_2 = BITS_PER_MP_LIMB - sh_1;
  high_limb = up[0];
  retval = high_limb << sh_2;
  low_limb = high_limb;

  for (i = 1; i < usize; i++)
    {
      high_limb = up[i];
      wp[i] = (low_limb >> sh_1) | (high_limb << sh_2);
      low_limb = high_limb;
    }
  low_limb >>= sh_1;
  wp[i] = low_limb;

  return retval;
}

#ifndef CNT
#define CNT 4
#endif

main (argc, argv)
     int argc;
     char **argv;
{
  mp_limb_t s1[SIZE];
  mp_limb_t dx[SIZE+2];
  mp_limb_t dy[SIZE+2];
  mp_limb_t cyx, cyy;
  int i;
  long t0, t;
  int test;
  int cnt = CNT;
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

#if TIMES == 1
      cnt = random () % (BITS_PER_MP_LIMB - 1) + 1;
#endif

#ifdef RANDOM
      size = random () % SIZE + 1;
#else
      size = SIZE;
#endif
      mpn_random2 (s1, size);

      dx[size+1] = 0x12345678;
      dy[size+1] = 0x12345678;
      dx[0] = 0x87654321;
      dy[0] = 0x87654321;

#if TIMES != 1
      mpn_random (s1, size);

#ifndef NOCHECK
      t0 = cputime();
      for (i = 0; i < TIMES; i++)
	refmpn_rshift (dx+1, s1, size, cnt);
      t = cputime() - t0;
      printf ("refmpn_rshift: %5ldms (%.2f cycles/limb)\n",
	      t, ((double) t * CLOCK) / (OPS * 1000.0));
#endif

      t0 = cputime();
      for (i = 0; i < TIMES; i++)
	mpn_rshift (dx+1, s1, size, cnt);
      t = cputime() - t0;
      printf ("mpn_rshift:    %5ldms (%.2f cycles/limb)\n",
	      t, ((double) t * CLOCK) / (OPS * 1000.0));
#endif

#ifndef NOCHECK
      mpn_random2 (s1, size);

#ifdef PRINT
      printf ("%-*d ", (int) (2 * sizeof(mp_limb_t)), cnt); mpn_print (s1, size);
#endif

      /* Put garbage in the destination.  */
      for (i = 0; i < size; i++)
	{
	  dx[i+1] = 0xdead;
	  dy[i+1] = 0xbeef;
	}

      cyx = refmpn_rshift (dx+1, s1, size, cnt);
      cyy = mpn_rshift (dy+1, s1, size, cnt);
#ifdef PRINT
      printf ("%*lX ", (int) (2 * sizeof(mp_limb_t)), cyx);
      mpn_print (dx+1, size);
      printf ("%*lX ", (int) (2 * sizeof(mp_limb_t)), cyy);
      mpn_print (dy+1, size);
#endif
      if (cyx != cyy || mpn_cmp (dx, dy, size+2) != 0
	  || dx[0] != 0x87654321 || dx[size+1] != 0x12345678)
	{
#ifndef PRINT
	  printf ("%*lX ", (int) (2 * sizeof(mp_limb_t)), cyx);
	  mpn_print (dx+1, size);
	  printf ("%*lX ", (int) (2 * sizeof(mp_limb_t)), cyy);
	  mpn_print (dy+1, size);
#endif
	  printf ("TEST NUMBER %d\n", test);
	  if (dy[size+1] != 0x12345678)
	    printf ("clobbered at high end\n");
	  if (dy[0] != 0x87654321)
	    printf ("clobbered at low end\n");
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
