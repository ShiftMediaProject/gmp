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

	  refcy = refmpn_addsub_n (ref1p, ref2p, s1p, s2p, n);
	  cy = mpn_addsub_n (r1p, r2p, s1p, s2p, n);

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
