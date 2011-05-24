/* tune-gcd-p

   Tune the choice for splitting p in divide-and-conquer gcd.

Copyright 2008, 2010, 2011 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.  */

#define TUNE_GCD_P 1

#include "../mpn/gcd.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "speed.h"


static int
compare_double(const void *ap, const void *bp)
{
  double a = * (const double *) ap;
  double b = * (const double *) bp;

  if (a < b)
    return -1;
  else if (a > b)
    return 1;
  else
    return 0;
}

static double
median (double *v, size_t n)
{
  qsort(v, n, sizeof(*v), compare_double);

  return v[n/2];
}

#define TIME(res, code) do {				\
  double time_measurement[5];				\
  unsigned time_i;					\
							\
  for (time_i = 0; time_i < 5; time_i++)		\
    {							\
      speed_starttime();				\
      code;						\
      time_measurement[time_i] = speed_endtime();	\
    }							\
  res = median(time_measurement, 5);			\
} while (0)

int
main(int argc, char **argv)
{
  gmp_randstate_t rands;
  mp_size_t n;
  mp_ptr ap;
  mp_ptr bp;
  mp_ptr up;
  mp_ptr vp;
  mp_ptr gp;
  TMP_DECL;

  /* Unbuffered so if output is redirected to a file it isn't lost if the
     program is killed part way through.  */
  setbuf (stdout, NULL);
  setbuf (stderr, NULL);

  gmp_randinit_default (rands);

  TMP_MARK;

  ap = TMP_ALLOC_LIMBS (P_TABLE_SIZE);
  bp = TMP_ALLOC_LIMBS (P_TABLE_SIZE);
  up = TMP_ALLOC_LIMBS (P_TABLE_SIZE);
  vp = TMP_ALLOC_LIMBS (P_TABLE_SIZE);
  gp = TMP_ALLOC_LIMBS (P_TABLE_SIZE);

  mpn_random (ap, P_TABLE_SIZE);
  mpn_random (bp, P_TABLE_SIZE);

  memset (p_table, 0, sizeof(p_table));

  for (n = 100; n++; n < P_TABLE_SIZE)
    {
      mp_size_t p;
      mp_size_t best_p;
      double best_time;
      double lehmer_time;

      if (ap[n-1] == 0)
	ap[n-1] = 1;

      if (bp[n-1] == 0)
	bp[n-1] = 1;

      p_table[n] = 0;
      TIME(lehmer_time, {
	  MPN_COPY (up, ap, n);
	  MPN_COPY (vp, bp, n);
	  mpn_gcd (gp, up, n, vp, n);
	});

      best_time = lehmer_time;
      best_p = 0;

      for (p = n * 0.48; p < n * 0.77; p++)
	{
	  double t;

	  p_table[n] = p;

	  TIME(t, {
	      MPN_COPY (up, ap, n);
	      MPN_COPY (vp, bp, n);
	      mpn_gcd (gp, up, n, vp, n);
	    });

	  if (t < best_time)
	    {
	      best_time = t;
	      best_p = p;
	    }
	}
      printf("%6d %6d %5.3g", n, best_p, (double) best_p / n);
      if (best_p > 0)
	{
	  double speedup = 100 * (lehmer_time - best_time) / lehmer_time;
	  printf(" %5.3g%%", speedup);
	  if (speedup < 1.0)
	    {
	      printf(" (ignored)");
	      best_p = 0;
	    }
	}
      printf("\n");

      p_table[n] = best_p;
    }
  TMP_FREE;
  gmp_randclear(rands);
  return 0;
}
