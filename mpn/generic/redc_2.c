/* mpn_redc_2.  Set cp[] <- up[]/R^n mod mp[].  Clobber up[].
   mp[] is n limbs; up[] is 2n limbs.

   THIS IS AN INTERNAL FUNCTION WITH A MUTABLE INTERFACE.  IT IS ONLY
   SAFE TO REACH THIS FUNCTION THROUGH DOCUMENTED INTERFACES.

Copyright (C) 2000, 2001, 2002, 2004 Free Software Foundation, Inc.

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

#if defined (CHECK) || defined (TIMING)
#include <stdlib.h>
#include <stdio.h>
#endif

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"

void mpn_redc_1 (mp_ptr, mp_ptr, mp_srcptr, mp_size_t, mp_limb_t);
void mpn_redc_2 (mp_ptr, mp_ptr, mp_srcptr, mp_size_t, mp_srcptr);

#if GMP_NAIL_BITS != 0
you lose
#endif

/* For testing purposes, define our own mpn_addmul_2 if there is none already
   available.  */
#ifndef HAVE_NATIVE_mpn_addmul_2
mp_limb_t
mpn_addmul_2 (mp_ptr rp, mp_srcptr up, mp_size_t n, mp_srcptr vp)
{
  rp[n] = mpn_addmul_1 (rp, up, n, vp[0]);
  return mpn_addmul_1 (rp + 1, up, n, vp[1]);
}
#endif

#if defined (__ia64) && W_TYPE_SIZE == 64
#define umul2low(ph, pl, uh, ul, vh, vl) \
  do {									\
    mp_limb_t _ph, _pl;							\
    __asm__ ("xma.hu %0 = %3, %5, f0\n\t"				\
	     "xma.l %1 = %3, %5, f0\n\t"				\
	     ";;\n\t"							\
	     "xma.l %0 = %3, %4, %0\n\t"				\
	     ";;\n\t"							\
	     "xma.l %0 = %2, %5, %0"					\
	     : "=&f" (ph), "=&f" (pl)					\
	     : "f" (uh), "f" (ul), "f" (vh), "f" (vl));			\
  } while (0)
#endif

#ifndef umul2low
#define umul2low(ph, pl, uh, ul, vh, vl) \
  do {									\
    mp_limb_t _ph, _pl;							\
    umul_ppmm (_ph, _pl, ul, vl);					\
    (ph) = _ph + (ul) * (vh) + (uh) * (vl);				\
    (pl) = _pl;								\
  } while (0)
#endif

void
mpn_redc_2 (mp_ptr rp, mp_ptr up, mp_srcptr mp, mp_size_t n, mp_srcptr mip)
{
  mp_limb_t q[2];
  mp_size_t j;
  mp_limb_t upn;
  mp_limb_t cy;

  ASSERT_MPN (up, 2*n);

  if ((n & 1) != 0)
    {
      up[0] = mpn_addmul_1 (up, mp, n, (up[0] * mip[0]) & GMP_NUMB_MASK);
      up++;
    }

  for (j = n - 2; j >= 0; j -= 2)
    {
      umul2low (q[1], q[0], mip[1], mip[0], up[1], up[0]);
      upn = up[n];		/* mpn_addmul_2 overwrites this */
      up[1] = mpn_addmul_2 (up, mp, n, q);
      up[0] = up[n];
      up[n] = upn;
      up += 2;
    }
  cy = mpn_add_n (rp, up, up - n, n);
  if (cy != 0)
    mpn_sub_n (rp, rp, mp, n);
}

#if CHECK
void
dumpy (mp_srcptr p, mp_size_t n)
{
  mp_size_t i;
  if (n > 20)
    {
      for (i = n - 1; i >= n - 4; i--)
	{
	  printf ("%0*lx", (int) (2 * sizeof (mp_limb_t)), p[i]);
	  printf (" ");
	}
      printf ("... ");
      for (i = 3; i >= 0; i--)
	{
	  printf ("%0*lx", (int) (2 * sizeof (mp_limb_t)), p[i]);
	  printf (" " + (i == 0));
	}
    }
  else
    {
      for (i = n - 1; i >= 0; i--)
	{
	  printf ("%0*lx", (int) (2 * sizeof (mp_limb_t)), p[i]);
	  printf (" " + (i == 0));
	}
    }
  puts ("");
}

int
main (int argc, char *argv[])
{
  mp_size_t nmax, n, itch;
  mp_ptr scratch, up, ap, mp, rp1, rp2;
  mp_limb_t mip[2];
  unsigned long test;

  nmax = strtol (argv[1], 0, 0);
  itch = mpn_binvert_itch (2);
  scratch = malloc (itch * sizeof (mp_limb_t));
  up = malloc (nmax * 2 * sizeof (mp_limb_t));
  ap = malloc (nmax * 2 * sizeof (mp_limb_t));
  mp = malloc (nmax * sizeof (mp_limb_t));
  rp1 = malloc (nmax * sizeof (mp_limb_t));
  rp2 = malloc (nmax * sizeof (mp_limb_t));

  for (test = 0;; test++)
    {
      if (test % (726211 / nmax + 1) == 0)
	{ printf ("\r%lu", test); fflush (stdout); }

      n = random () % nmax + 1;

      mpn_random2 (up, 2 * n);
      mpn_random2 (mp, n); mp[0] |= 1;
      mpn_binvert (mip, mp, 2, scratch);
      mip[0] = -mip[0]; mip[1] = ~mip[1];

      MPN_COPY (ap, up, 2 * n);
      mpn_redc_1 (rp1, ap, mp, n, mip[0]);

      MPN_COPY (ap, up, 2 * n);
      mpn_redc_2 (rp2, ap, mp, n, mip);

      if (mpn_cmp (rp1, rp2, n) != 0)
	{
	  printf ("ERROR in test %lu\n", test);
	  printf ("N=   "); dumpy (up, 2 * n);
	  printf ("D=   "); dumpy (mp, n);
	  printf ("Q(1)="); dumpy (rp1, n);
	  printf ("Q(2)="); dumpy (rp2, n);
	}
    }
}
#endif

#if TIMING
#include "timing.h"
int
main (int argc, char *argv[])
{
  mp_size_t n, itch;
  mp_ptr scratch, up, ap, mp, rp1, rp2;
  mp_limb_t mip[2];
  double t;

  n = strtol (argv[1], 0, 0);
  itch = mpn_binvert_itch (2);
  scratch = malloc (itch * sizeof (mp_limb_t));
  up = malloc (n * 2 * sizeof (mp_limb_t));
  ap = malloc (n * 2 * sizeof (mp_limb_t));
  mp = malloc (n * sizeof (mp_limb_t));
  rp1 = malloc (n * sizeof (mp_limb_t));
  rp2 = malloc (n * sizeof (mp_limb_t));

  mpn_random2 (up, 2 * n);
  mpn_random2 (mp, n); mp[0] |= 1;
  mpn_binvert (mip, mp, 2, scratch);
  mip[0] = -mip[0]; mip[1] = ~mip[1];

  MPN_COPY (ap, up, 2 * n);
  TIME (t, mpn_redc_1 (rp1, ap, mp, n, mip[0]));
  printf ("mpn_redc_1:       %f", t);
#ifdef CLOCK
  printf ("  %.2f c/l", CLOCK * t / 1000 / (n * n));
#endif
  puts ("");

  MPN_COPY (ap, up, 2 * n);
  TIME (t, mpn_redc_2 (rp2, ap, mp, n, mip));
  printf ("mpn_redc_2:       %f", t);
#ifdef CLOCK
  printf ("  %.2f c/l", CLOCK * t / 1000 / (n * n));
#endif
  puts ("");

  TIME (t, mpn_mul_basecase (up, ap, n, mp, n));
  printf ("mpn_mul_basecase: %f", t);
#ifdef CLOCK
  printf ("  %.2f c/l", CLOCK * t / 1000 / (n * n));
#endif
  puts ("");

  return 0;
}
#endif
