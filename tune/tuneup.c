/* Create tuned thresholds for various algorithms. */

/*
Copyright (C) 1999, 2000 Free Software Foundation, Inc.

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

/* Usage: tune [-t] [-t] [-p precision]

   -t turns on diagnostic traces, a second -t enables extra tracing.

   The thresholds are determined as follows.  A crossover may not be a
   single size but rather a range where it oscillates between method A or
   method B faster.  If the threshold is set making B used where A is faster
   (or vice versa) that's bad.  Badness is the percentage time lost and
   total badness is the sum of this over all sizes measured.  The threshold
   is set to minimize total badness.

   Suppose, as sizes increase, method B becomes faster than method A.  The
   effect of the rule is that, as you look at increasing sizes, isolated
   points where B is faster are ignored, but when it's consistently faster,
   or faster on balance, then the threshold is set there.  The same result
   is obtained thinking in the other direction of A becoming faster at
   smaller sizes.

   In practice the thresholds tend to be chosen to bring on the next
   algorithm fairly quickly.

   This rule is attractive because it's got a basis in reason and is fairly
   easy to implement, but no work has been done to actually compare it in
   absolute terms to other possibilities.

   Sometimes running the program twice produces slightly different results.
   This's probably because there's so little separating algorithms near
   their crossover, and on that basis it should make little or no difference
   to the final speed of mpn_mul_n() etc, but nothing has been done to check
   that carefully.

   Limitations:
   
   It's assumed that each algorithm is optimal in some range.  There's no
   support for, say, going straight from N-1 to N+1.  Arguably this
   shouldn't happen, though it might be worth checking it doesn't.

   Enhancements:

   Go backwards a bit from the N-way threshold when looking for the N+1-way
   threshold.

*/

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "gmp.h"
#include "gmp-impl.h"

#include "speed.h"
#include "sqr_basecase.h"


#define MAX_SIZE        1000  /* limbs */
#define MAX_TABLE       2     /* threshold entries (use 3 when FFT exists) */
#define STEP_FACTOR     0.01  /* how much to step sizes by (rounded down) */


#define numberof(x)            (sizeof (x) / sizeof ((x)[0]))
#define SIGNED_TYPE_MAX(type)  (~(((type) 1) << (8*sizeof(type)-1)))
#define MP_SIZE_T_MAX          SIGNED_TYPE_MAX (mp_size_t)


int     option_trace = 0;
struct speed_params  s;

struct {
  mp_size_t  size;
  double     d;
} *dat = NULL;
int  ndat = 0;
int  allocdat = 0;


mp_size_t  tune_mul_threshold[MAX_TABLE+1] = { MP_SIZE_T_MAX };
mp_size_t  tune_sqr_threshold[MAX_TABLE+1] = { MP_SIZE_T_MAX };
mp_size_t  bz_threshold[2] = { MP_SIZE_T_MAX };


/* Add an entry to the end of the dat[] array, reallocing to make it bigger
   if necessary.  */
void
add_dat (mp_size_t size, double d)
{
#define ALLOCDAT_STEP  500

  ASSERT_ALWAYS (ndat <= allocdat);

  if (ndat == allocdat)
    {
      dat = _mp_allocate_or_reallocate
        (dat, allocdat * sizeof(dat[0]),
         (allocdat+ALLOCDAT_STEP) * sizeof(dat[0]));
      allocdat += ALLOCDAT_STEP;
    }

  dat[ndat].size = size;
  dat[ndat].d = d;
  ndat++;
}


/* Return the threshold size based on the data accumulated. */
mp_size_t
analyze_dat (int i, int final)
{
  double  x, min_x;
  int     j, min_j;

  /* If the threshold is set at dat[0].size, any positive values are bad. */
  x = 0.0;
  for (j = 0; j < ndat; j++)
    if (dat[j].d > 0.0)
      x += dat[j].d;

  if (option_trace >= 2 && final)
    printf ("i=%d size=%ld  first x %.4f\n", i, dat[j].size, x);

  min_x = x;
  min_j = 0;


  /* When stepping to the next dat[j].size, positive values are no longer
     bad (so subtracted), negative values become bad (so add the absolute
     value, meaning subtract). */
  for (j = 0; j < ndat; x -= dat[j].d, j++)
    {
      if (option_trace >= 2 && final)
        printf ("i=%d size=%ld  x=%.4f\n", i, dat[j].size, x);

      if (x < min_x)
        {
          min_x = x;
          min_j = j;
        }
    }
   
  return min_j;
}


/* table[i+1] needs to be set to a sensible value when testing method i+1
   because mpn_mul_n uses TOOM3_MUL_THRESHOLD to size the temporary
   workspace for the mpn_kara_mul_n. */

void
one (speed_function_t function, mp_size_t table[], size_t max_table,
     const char *name)
{
  int  i;

  s.size = 10;
  for (i = 0; i < max_table && s.size < MAX_SIZE; i++)
    {
      int  since_positive, since_thresh_change;
      int  thresh_idx, new_thresh_idx;

      ndat = 0;
      since_positive = 0;
      since_thresh_change = 0;
      thresh_idx = 0;

      for ( ; s.size < MAX_SIZE; 
            s.size += MAX (floor (s.size * STEP_FACTOR), 1))
        {
          double   ti, tiplus1, d;

          if (max_table != 1 && i == 0 && s.size > 63)
            break;

          /*
            FIXME: check minimum size requirements are met, possibly by just
            checking for the -1 returns from the speed functions.
            if (s.size < MPN_TOOM_TABLE_TO_MINSIZE (i))
            continue;
          */

          /* using method i at this size */
          table[i] = s.size+1;
          table[i+1] = MAX_SIZE;
          ti = speed_measure (function, &s);
          if (ti == -1.0)
            abort();

          /* using method i+1 at this size */
          table[i] = s.size;
          table[i+1] = s.size+1;
          tiplus1 = speed_measure (function, &s);
          if (tiplus1 == -1.0)
            abort();

          /* Calculate the fraction by which the one or the other routine is
             slower.  */
          if (tiplus1 >= ti)
            d = (tiplus1 - ti) / tiplus1;  /* negative */
          else
            d = (tiplus1 - ti) / ti;       /* positive */

          add_dat (s.size, d);

          new_thresh_idx = analyze_dat (i, 0);

          if (option_trace >= 2)
            printf ("i=%d size=%ld  %.9f  %.9f  % .4f %c  %d\n",
                    i, s.size, ti, tiplus1, d,
                    ti > tiplus1 ? '#' : ' ',
                    dat[new_thresh_idx].size);

          /* Stop if the last time method i was faster was more than a
             certain number of measurements ago.  */
#define STOP_SINCE_POSITIVE  200
          if (d >= 0)
            since_positive = 0;            
          else
            if (++since_positive > STOP_SINCE_POSITIVE)
              {
                if (option_trace >= 1)
                  printf ("i=%d stopped due to since_positive (%d)\n",
                          i, STOP_SINCE_POSITIVE);
                break;
              }

          /* Stop if method i has become slower by a certain factor. */
#define STOP_FACTOR   1.2
          if (ti >= tiplus1 * STOP_FACTOR)
            {
              if (option_trace >= 1)
                printf ("i=%d stopped due to ti >= tiplus1 * factor (%.1f)\n",
                        i, STOP_FACTOR);
              break;
            }

          /* Stop if the threshold implied hasn't changed in a certain
             number of measurements.  (It's this condition that ususally
             stops the loop.) */
#define STOP_SINCE_CHANGE   80
          if (thresh_idx != new_thresh_idx)
            since_thresh_change = 0, thresh_idx = new_thresh_idx;
          else
            if (++since_thresh_change > STOP_SINCE_CHANGE)
              {
                if (option_trace >= 1)
                  printf ("i=%d stopped due to since_thresh_change (%d)\n",
                          i, STOP_SINCE_CHANGE);
                break;
              }

          /* Stop if the threshold implied is more than a certain number of
             measurements ago.  */
#define STOP_SINCE_AFTER   500
          if (ndat - thresh_idx > STOP_SINCE_AFTER)
            {
              if (option_trace >= 1)
                printf ("i=%d stopped due to ndat - thresh_idx > amount (%d)\n",
                        i, STOP_SINCE_AFTER);
              break;
            }
        }

      /* Stop when the size limit is reached before the end of the
         crossover. */
      if (s.size >= MAX_SIZE)
        {
          if (option_trace >= 1)
            printf ("i=%d sizes %ld to %ld total %d measurements\n"
                    "    max size reached before end of crossover\n",
                    i, dat[0].size, dat[ndat-1].size, ndat);
          break;
        }

      if (option_trace >= 1)
        printf ("i=%d sizes %ld to %ld total %d measurements\n",
                i, dat[0].size, dat[ndat-1].size, ndat);

      if (ndat == 0)
        break;

      table[i] = dat[analyze_dat (i, 1)].size;

      {
        const char *s;
        char  buf[80];
        if (max_table == 1)
          s = "";
        else
          {
            switch (i) {
            case 0: s = "KARATSUBA_"; break;
            case 1: s = "TOOM3_"; break;
            case 2: s = "FFT_"; break;
            default: abort ();
            }
          }
        sprintf (buf, "%s%s_THRESHOLD", s, name);
        printf ("\
#ifndef %s\n\
#define %-23s %4ld\n\
#endif\n",
                buf, buf, table[i]);
      }

      /* Look for the next threshold starting from the current one, but back
         a bit. */
      s.size = table[i]+1;
    }      
}

void
all (void)
{
  s.xp = _mp_allocate_func_aligned
    (MAX_SIZE * sizeof (mp_limb_t), CACHE_LINE_SIZE);
  s.yp = _mp_allocate_func_aligned
    (MAX_SIZE * sizeof (mp_limb_t), CACHE_LINE_SIZE);
  mpn_random (s.xp, MAX_SIZE);
  mpn_random (s.yp, MAX_SIZE);

  speed_time_init ();

  printf("\
/* Generated by tune.c.\n\
   speed_precision %d, speed_unittime %.2e\n\
   MAX_SIZE %d, STEP_FACTOR %.3f  */\n\
\n",
         speed_precision, speed_unittime  *1e6, 
         MAX_SIZE, STEP_FACTOR);

  one (speed_mpn_mul_n, tune_mul_threshold, numberof(tune_mul_threshold)-1,
       "MUL");
  printf("\n");

  one (speed_mpn_sqr_n, tune_sqr_threshold, numberof(tune_sqr_threshold)-1,
       "SQR");
  printf("\n");
  
  one (speed_mpn_bz_tdiv_qr, bz_threshold, 1, "BZ");
}


int
main (int argc, char *argv[])
{
  int  opt;

  /* Unbuffered so if output is redirected to a file it isn't lost if the
     program is killed part way through.  */
  setbuf (stdout, NULL);

  while ((opt = getopt(argc, argv, "p:t")) != EOF)
    {
      switch (opt) {
      case 'p':
        speed_precision = atoi (optarg);
        break;
      case 't':
        option_trace++;
        break;
      case '?':
        exit(1);
      }
    }
		
  all ();
  return 0;
}
