/* Create tuned thresholds for various algorithms. */

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

/* Usage: tune [-t] [-t] [-p precision]

   -t turns on some diagnostic traces, a second -t turns on more traces.

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
   This is probably because there's so little separating algorithms near
   their crossover, and on that basis it should make little or no difference
   to the final speed of the relevant routines, but nothing has been done to
   check that carefully.

   Limitations:
   
   The FFTs aren't subject to the same badness rule as the other thresholds,
   so each k is probably being brought on a touch early.  This isn't likely
   to make a difference, and the simpler probing means fewer tests.

*/

#define TUNE_PROGRAM_BUILD  1

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "gmp.h"
#include "gmp-impl.h"

#include "speed.h"
#include "sqr_basecase.h"

#if !HAVE_DECL_OPTARG
extern char *optarg;
extern int optind, opterr;
#endif


#define MAX_SIZE        1000  /* limbs */
#define STEP_FACTOR     0.01  /* how much to step sizes by (rounded down) */
#define MAX_TABLE       2     /* threshold entries */


#if WANT_FFT
mp_size_t  option_fft_max_size = 50000;  /* limbs */
#else
mp_size_t  option_fft_max_size = 0;
#endif
int        option_trace = 0;
int        option_fft_trace = 0;
struct speed_params  s;

struct dat_t {
  mp_size_t  size;
  double     d;
} *dat = NULL;
int  ndat = 0;
int  allocdat = 0;


/* Each "_threshold" array must be 1 bigger than the number of thresholds
   being tuned in a set, because one() stores an value in the entry above
   the one it's determining. */

mp_size_t  mul_threshold[MAX_TABLE+1] = { MP_SIZE_T_MAX };
mp_size_t  fft_modf_mul_threshold = MP_SIZE_T_MAX;
mp_size_t  sqr_threshold[MAX_TABLE+1] = { MP_SIZE_T_MAX };
mp_size_t  fft_modf_sqr_threshold = MP_SIZE_T_MAX;
mp_size_t  bz_threshold[2] = { MP_SIZE_T_MAX };
mp_size_t  fib_threshold[2] = { MP_SIZE_T_MAX };
mp_size_t  powm_threshold[2] = { MP_SIZE_T_MAX };
mp_size_t  gcd_accel_threshold[2] = { MP_SIZE_T_MAX };
mp_size_t  gcdext_threshold[2] = { MP_SIZE_T_MAX };


#ifndef KARATSUBA_SQR_MAX
#define KARATSUBA_SQR_MAX  0 /* meaning no limit */
#endif

struct param_t {
  const char  *name[MAX_TABLE];
  int         stop_since_change;
  mp_size_t   min_size;
  mp_size_t   max_size[MAX_TABLE];
};


/* Add an entry to the end of the dat[] array, reallocing to make it bigger
   if necessary.  */
void
add_dat (mp_size_t size, double d)
{
#define ALLOCDAT_STEP  500

  ASSERT_ALWAYS (ndat <= allocdat);

  if (ndat == allocdat)
    {
      dat = (struct dat_t *) _mp_allocate_or_reallocate
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
    {
      printf ("\n");
      printf ("x is the sum of the badness from setting thresh at given size\n");
      printf ("  (minimum x is sought)\n");
      printf ("i=%d size=%ld  first x=%.4f\n", i, dat[j].size, x);
    }

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


double
tuneup_measure (speed_function_t fun, struct speed_params *s)
{
  static mp_ptr  xp, yp;
  double   t;
  TMP_DECL (marker);

  TMP_MARK (marker);
  s->xp = SPEED_TMP_ALLOC_LIMBS (s->size, 0);
  s->yp = SPEED_TMP_ALLOC_LIMBS (s->size, 0);

  mpn_random (s->xp, s->size);
  mpn_random (s->yp, s->size);

  t = speed_measure (fun, s);

  TMP_FREE (marker);
  return t;
}  


void
print_define (const char *name, mp_size_t value)
{
  printf ("#ifndef %s\n", name);
  printf ("#define %-23s  ", name);
  if (value == MP_SIZE_T_MAX)
    printf ("MP_SIZE_T_MAX\n");
  else
    printf ("%5ld\n", value);
  printf ("#endif\n");
}


/* table[i+1] needs to be set to a sensible value when testing method i+1
   because mpn_mul_n uses TOOM3_MUL_THRESHOLD to size the temporary
   workspace for mpn_kara_mul_n. */

void
one (speed_function_t function, mp_size_t table[], size_t max_table,
     struct param_t *param)
{
  static struct param_t  dummy;
  int  i;

  if (param == NULL)  param = &dummy;

#define DEFAULT(x,n)  if (param->x == 0)  param->x = (n);

  DEFAULT (stop_since_change, 80);
  DEFAULT (min_size, 10);
  for (i = 0; i < numberof (param->max_size); i++)
    DEFAULT (max_size[i], MAX_SIZE);

  s.size = param->min_size;

  for (i = 0; i < max_table && s.size < MAX_SIZE; i++)
    {
      int  since_positive, since_thresh_change;
      int  thresh_idx, new_thresh_idx;

      ndat = 0;
      since_positive = 0;
      since_thresh_change = 0;
      thresh_idx = 0;

      if (option_trace >= 2)
        {
          printf ("             algorithm-A  algorithm-B   ratio  possible\n");
          printf ("              (seconds)    (seconds)    diff    thresh\n");
        }

      for ( ; s.size < MAX_SIZE; 
            s.size += MAX ((mp_size_t) floor (s.size * STEP_FACTOR), 1))
        {
          double   ti, tiplus1, d;

          /* If there's a size limit and it's reached then it should still
             be sensible to analyze the data since we want the threshold put
             either at or near the limit.  */
          if (s.size >= param->max_size[i])
            {
              if (option_trace)
                printf ("Reached maximum size (%ld) without otherwise stopping\n",
                        param->max_size[i]);
              break;
            }

          /*
            FIXME: check minimum size requirements are met, possibly by just
            checking for the -1 returns from the speed functions.
            if (s.size < MPN_TOOM_TABLE_TO_MINSIZE (i))
            continue;
          */

          /* using method i at this size */
          table[i] = s.size+1;
          table[i+1] = MAX_SIZE;
          ti = tuneup_measure (function, &s);
          if (ti == -1.0)
            abort ();

          /* using method i+1 at this size */
          table[i] = s.size;
          table[i+1] = s.size+1;
          tiplus1 = tuneup_measure (function, &s);
          if (tiplus1 == -1.0)
            abort ();

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
          if (thresh_idx != new_thresh_idx)
            since_thresh_change = 0, thresh_idx = new_thresh_idx;
          else
            if (++since_thresh_change > param->stop_since_change)
              {
                if (option_trace >= 1)
                  printf ("i=%d stopped due to since_thresh_change (%d)\n",
                          i, param->stop_since_change);
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
         crossover, without a specified param->max_size[i]. */
      if (s.size >= MAX_SIZE)
        {
          fprintf (stderr, "%s\n", param->name[i]);
          fprintf (stderr, "i=%d sizes %ld to %ld total %d measurements\n",
                   i, dat[0].size, dat[ndat-1].size, ndat);
          fprintf (stderr, "    max size reached before end of crossover\n");
          break;
        }

      if (option_trace >= 1)
        printf ("i=%d sizes %ld to %ld total %d measurements\n",
                i, dat[0].size, dat[ndat-1].size, ndat);

      if (ndat == 0)
        break;

      table[i] = dat[analyze_dat (i, 1)].size;

      print_define (param->name[i], table[i]);

      /* Look for the next threshold starting from the current one, but back
         a bit. */
      s.size = table[i]+1;
    }      
}


/* Special probing for the fft thresholds.  The size restrictions on the
   FFTs mean the graph of time vs size has a step effect.  See this for
   example using

       ./speed -s 4096-16384 -t 128 -P foo mpn_mul_fft.8 mpn_mul_fft.9
       gnuplot foo.gnuplot

   The current approach is to compare routines at the midpoint of relevant
   steps.  Arguably a more sophisticated system of threshold data is wanted
   if this step effect remains. */

struct fft_param_t {
  const char        *table_name;
  const char        *threshold_name;
  const char        *modf_threshold_name;
  mp_size_t         *p_threshold;
  mp_size_t         *p_modf_threshold;
  mp_size_t         first_size;
  mp_size_t         max_size;
  speed_function_t  function;
  speed_function_t  mul_function;
  mp_size_t         sqr;
};

/* mpn_mul_fft requires pl a multiple of 2^k limbs, but with
   N=pl*BIT_PER_MP_LIMB it internally also pads out so N/2^k is a multiple
   of 2^(k-1) bits. */

mp_size_t
fft_step_size (int k)
{
  if (2*k-1 > BITS_PER_INT)
    {
      printf ("Can't handle k=%d\n", k);
      abort ();
    }
  return (1<<k) * (MAX (1<<(k-1), BITS_PER_MP_LIMB)) / BITS_PER_MP_LIMB;
}

mp_size_t
fft_next_size (mp_size_t pl, int k)
{
  mp_size_t  m = fft_step_size (k);

/*    printf ("[k=%d %ld] %ld ->", k, m, pl); */

  if (pl == 0 || (pl & (m-1)) != 0)
    pl = (pl | (m-1)) + 1;

/*    printf (" %ld\n", pl); */
  return pl;
}

void
fft (struct fft_param_t *p)
{
  mp_size_t  size;
  int        i, k;

  for (i = 0; i < numberof (mpn_fft_table[p->sqr]); i++)
    mpn_fft_table[p->sqr][i] = MP_SIZE_T_MAX;

  *p->p_threshold = MP_SIZE_T_MAX;
  *p->p_modf_threshold = MP_SIZE_T_MAX;

  option_trace = MAX (option_trace, option_fft_trace);

  printf ("#ifndef %s\n", p->table_name);
  printf ("#define %s  {", p->table_name);
  if (option_trace >= 2)
    printf ("\n");

  k = FFT_FIRST_K;
  size = p->first_size;
  for (;;)
    {
      double  tk, tk1;

      size = fft_next_size (size+1, k+1);

      if (size >= p->max_size)
        break;
      if (k >= FFT_FIRST_K + numberof (mpn_fft_table[p->sqr]))
        break;

      usleep(10000);

      /* compare k to k+1 in the middle of the current k+1 step */
      s.size = size + fft_step_size (k+1) / 2;
      s.r = k;
      tk = tuneup_measure (p->function, &s);
      if (tk == -1.0)
        abort ();

      usleep(10000);

      s.r = k+1;
      tk1 = tuneup_measure (p->function, &s);
      if (tk1 == -1.0)
        abort ();

      if (option_trace >= 2)
        printf ("at %ld   size=%ld  k=%d  %.9lf   k=%d %.9lf\n",
                size, s.size, k, tk, k+1, tk1);

      /* declare the k+1 threshold as soon as it's faster at its midpoint */
      if (tk1 < tk)
        {
          mpn_fft_table[p->sqr][k-FFT_FIRST_K] = s.size;
          printf (" %ld,", s.size);
          if (option_trace >= 2) printf ("\n");
          k++;
        }
    }

  mpn_fft_table[p->sqr][k-FFT_FIRST_K] = 0;
  printf (" 0 }\n");
  printf ("#endif\n");


  size = p->first_size;
  
  /* Declare an FFT faster than a plain toom3 etc multiplication found as
     soon as one faster measurement obtained.  A multiplication in the
     middle of the FFT step is tested.  */
  for (;;)
    {
      int     modf = (*p->p_modf_threshold == MP_SIZE_T_MAX);
      double  tk, tm;

      /* k=7 should be the first FFT which can beat toom3 on a full
         multiply, so jump to that threshold and save some probing after the
         modf threshold is found.  */
      if (!modf && size < mpn_fft_table[p->sqr][2])
        {
          size = mpn_fft_table[p->sqr][2];
          if (option_trace >= 2)
            printf ("jump to size=%ld\n", size);
        }

      size = fft_next_size (size+1, mpn_fft_best_k (size, p->sqr));
      k = mpn_fft_best_k (size, p->sqr);

      if (size >= p->max_size)
        break;

      usleep(10000);

      s.size = size + fft_step_size (k) / 2;
      s.r = k;
      tk = tuneup_measure (p->function, &s);
      if (tk == -1.0)
        abort ();

      usleep(10000);

      if (!modf)  s.size /= 2;
      tm = tuneup_measure (p->mul_function, &s);
      if (tm == -1.0)
        abort ();

      if (option_trace >= 2)
        printf ("at %ld   size=%ld   k=%d  %.9lf   size=%ld %s mul %.9lf\n",
                size,
                size + fft_step_size (k) / 2, k, tk,
                s.size, modf ? "modf" : "full", tm);

      if (tk < tm)
        {
          if (modf)
            {
              *p->p_modf_threshold = s.size;
              print_define (p->modf_threshold_name, *p->p_modf_threshold);
            }
          else
            {
              *p->p_threshold = s.size;
              print_define (p->threshold_name,      *p->p_threshold);
              break;
            }
        }
    }

}


void
all (void)
{
  TMP_DECL (marker);

  TMP_MARK (marker);
  s.xp_block = SPEED_TMP_ALLOC_LIMBS (SPEED_BLOCK_SIZE, 0);
  s.yp_block = SPEED_TMP_ALLOC_LIMBS (SPEED_BLOCK_SIZE, 0);

  speed_time_init ();
  fprintf (stderr, "speed_precision %d, speed_unittime %.2e\n",
           speed_precision, speed_unittime);
  fprintf (stderr, "MAX_SIZE %ld, fft_max_size %ld, STEP_FACTOR %.3f\n",
           MAX_SIZE, option_fft_max_size, STEP_FACTOR);
  fprintf (stderr, "\n");

  {
    struct tm  *tp;
    time_t     t;
    time (&t);
    tp = localtime (&t);
    printf ("/* Generated by tuneup.c, %d-%02d-%02d. */\n\n",
            tp->tm_year+1900, tp->tm_mon+1, tp->tm_mday);
  }

  {
    static struct param_t  param;
    param.name[0] = "KARATSUBA_MUL_THRESHOLD";
    param.name[1] = "TOOM3_MUL_THRESHOLD";
    param.max_size[1] = TOOM3_MUL_THRESHOLD_LIMIT;
    one (speed_mpn_mul_n, mul_threshold, numberof(mul_threshold)-1, &param);
  }
  printf("\n");

  {
    static struct param_t  param;
    param.name[0] = "KARATSUBA_SQR_THRESHOLD";
    param.name[1] = "TOOM3_SQR_THRESHOLD";
    param.max_size[0] = KARATSUBA_SQR_MAX;
    one (speed_mpn_sqr_n, sqr_threshold, numberof(sqr_threshold)-1, &param);
  }
  printf("\n");

  {
    static struct param_t  param;
    param.name[0] = "BZ_THRESHOLD";
    one (speed_mpn_bz_tdiv_qr, bz_threshold, 1, &param);
  }
  printf("\n");

  {
    static struct param_t  param;
    param.name[0] = "FIB_THRESHOLD";
    one (speed_mpz_fib_ui, fib_threshold, 1, &param);
  }
  printf("\n");

  /* mpz_powm becomes slow before long, so stop soon after the determined
     threshold stops changing. */
  {
    static struct param_t  param;
    param.name[0] = "POWM_THRESHOLD";
    param.stop_since_change = 15;
    one (speed_mpz_powm, powm_threshold, 1, &param);
  }
  printf("\n");

  {
    static struct param_t  param;
    param.name[0] = "GCD_ACCEL_THRESHOLD";
    param.min_size = 1;
    one (speed_mpn_gcd, gcd_accel_threshold, 1, &param);
  }
  {
    static struct param_t  param;
    param.name[0] = "GCDEXT_THRESHOLD";
    param.min_size = 1;
    param.max_size[0] = 200;
    one (speed_mpn_gcdext, gcdext_threshold, 1, &param);
  }
  printf("\n");

  if (option_fft_max_size != 0)
    {
      {
        static struct fft_param_t  param;
        param.table_name          = "FFT_MUL_TABLE";
        param.threshold_name      = "FFT_MUL_THRESHOLD";
        param.p_threshold         = &FFT_MUL_THRESHOLD;
        param.modf_threshold_name = "FFT_MODF_MUL_THRESHOLD";
        param.p_modf_threshold    = &FFT_MODF_MUL_THRESHOLD;
        param.first_size          = TOOM3_MUL_THRESHOLD / 2;
        param.max_size            = option_fft_max_size;
        param.function            = speed_mpn_mul_fft;
        param.mul_function        = speed_mpn_mul_n;
        param.sqr = 0;
        fft (&param);
      }
      printf("\n");
      {
        static struct fft_param_t  param;
        param.table_name          = "FFT_SQR_TABLE";
        param.threshold_name      = "FFT_SQR_THRESHOLD";
        param.p_threshold         = &FFT_SQR_THRESHOLD;
        param.modf_threshold_name = "FFT_MODF_SQR_THRESHOLD";
        param.p_modf_threshold    = &FFT_MODF_SQR_THRESHOLD;
        param.first_size          = TOOM3_SQR_THRESHOLD / 2;
        param.max_size            = option_fft_max_size;
        param.function            = speed_mpn_mul_fft_sqr;
        param.mul_function        = speed_mpn_sqr_n;
        param.sqr = 0;
        fft (&param);
      }
      printf ("\n");
    }

  TMP_FREE (marker);
}


int
main (int argc, char *argv[])
{
  int  opt;

  /* Unbuffered so if output is redirected to a file it isn't lost if the
     program is killed part way through.  */
  setbuf (stdout, NULL);
  setbuf (stderr, NULL);

  while ((opt = getopt(argc, argv, "f:o:p:t")) != EOF)
    {
      switch (opt) {
      case 'f':
        if (optarg[0] == 't')
          option_fft_trace = 2;
        else
          option_fft_max_size = atol (optarg);
        break;
      case 'o':
        speed_option_set (optarg);
        break;
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
