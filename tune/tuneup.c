/* Create tuned thresholds for various algorithms. */

/*
Copyright 1999, 2000 Free Software Foundation, Inc.

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

#include "config.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"

#include "speed.h"

#if !HAVE_DECL_OPTARG
extern char *optarg;
extern int optind, opterr;
#endif


#define MAX_SIZE        1000  /* limbs */
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
   being tuned in a set, because one() stores a value in the entry above
   the one it's determining. */

mp_size_t  mul_threshold[MAX_TABLE+1] = { MP_SIZE_T_MAX };
mp_size_t  fft_modf_mul_threshold = MP_SIZE_T_MAX;
mp_size_t  sqr_threshold[MAX_TABLE+1] = { MP_SIZE_T_MAX };
mp_size_t  fft_modf_sqr_threshold = MP_SIZE_T_MAX;
mp_size_t  dc_threshold[2] = { MP_SIZE_T_MAX };
mp_size_t  fib_threshold[2] = { MP_SIZE_T_MAX };
mp_size_t  powm_threshold[2] = { MP_SIZE_T_MAX };
mp_size_t  gcd_accel_threshold[2] = { MP_SIZE_T_MAX };
mp_size_t  gcdext_threshold[2] = { MP_SIZE_T_MAX };
mp_size_t  divrem_1_norm_threshold[2] = { MP_SIZE_T_MAX };
mp_size_t  divrem_1_unnorm_threshold[2] = { MP_SIZE_T_MAX };
mp_size_t  mod_1_norm_threshold[2] = { MP_SIZE_T_MAX };
mp_size_t  mod_1_unnorm_threshold[2] = { MP_SIZE_T_MAX };


#ifndef TUNE_KARATSUBA_SQR_MAX
#define TUNE_KARATSUBA_SQR_MAX  0 /* meaning no limit */
#endif

struct param_t {
  const char        *name[MAX_TABLE];
  speed_function_t  function;
  speed_function_t  function2;
  double            step_factor;    /* how much to step sizes (rounded down) */
  double            function_fudge; /* multiplier for "function" speeds */
  int               stop_since_change;
  double            stop_factor;
  mp_size_t         min_size;
  int               min_is_always;
  mp_size_t         max_size[MAX_TABLE];
  mp_size_t         check_size;
  mp_size_t         size_extra;
  int               data_high_lt_r;
};


mp_limb_t
randlimb_norm (void)
{
  mp_limb_t  n;
  mpn_random (&n, 1);
  n |= MP_LIMB_T_HIGHBIT;
  return n;
}

#define MP_LIMB_T_HALFMASK  ((CNST_LIMB(1) << (BITS_PER_MP_LIMB/2)) - 1)

mp_limb_t
randlimb_half (void)
{
  mp_limb_t  n;
  mpn_random (&n, 1);
  n &= MP_LIMB_T_HALFMASK;
  n += (n==0);
  return n;
}


/* Add an entry to the end of the dat[] array, reallocing to make it bigger
   if necessary.  */
void
add_dat (mp_size_t size, double d)
{
#define ALLOCDAT_STEP  500

  ASSERT_ALWAYS (ndat <= allocdat);

  if (ndat == allocdat)
    {
      dat = (struct dat_t *) __gmp_allocate_or_reallocate
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
tuneup_measure (speed_function_t fun,
                const struct param_t *param,
                struct speed_params *s)
{
  static struct param_t  dummy;
  double   t;
  TMP_DECL (marker);

  if (! param)
    param = &dummy;

  s->size += param->size_extra;

  TMP_MARK (marker);
  s->xp = SPEED_TMP_ALLOC_LIMBS (s->size, 0);
  s->yp = SPEED_TMP_ALLOC_LIMBS (s->size, 0);

  mpn_random (s->xp, s->size);
  mpn_random (s->yp, s->size);

  if (param != NULL && param->data_high_lt_r)
    {
      s->xp[s->size-1] %= s->r;
      s->yp[s->size-1] %= s->r;
    }

  t = speed_measure (fun, s);

  s->size -= param->size_extra;

  TMP_FREE (marker);
  return t;
}  


void
print_define_start (const char *name)
{
  printf ("#define %-25s  ", name);
  if (option_trace)
    printf ("...\n");
}

void
print_define_end (const char *name, mp_size_t value)
{
  if (option_trace)
    printf ("#define %-23s  ", name);

  if (value == MP_SIZE_T_MAX)
    printf ("MP_SIZE_T_MAX\n");
  else
    printf ("%5ld\n", value);
}

void
print_define (const char *name, mp_size_t value)
{
  print_define_start (name);
  print_define_end (name, value);
}


/* table[i+1] needs to be set to a sensible value when testing method i+1
   because mpn_mul_n uses TOOM3_MUL_THRESHOLD to size the temporary
   workspace for mpn_kara_mul_n. */

void
one (mp_size_t table[], size_t max_table, struct param_t *param)
{
  int  i;

#define DEFAULT(x,n)  if (! (param->x))  param->x = (n);

  DEFAULT (function_fudge, 1.0);
  DEFAULT (function2, param->function);
  DEFAULT (step_factor, 0.01);  /* small steps by default */
  DEFAULT (stop_since_change, 80);
  DEFAULT (stop_factor, 1.2);
  DEFAULT (min_size, 10);
  for (i = 0; i < numberof (param->max_size); i++)
    DEFAULT (max_size[i], MAX_SIZE);

  if (param->check_size != 0)
    {
      double   t1, t2;
      s.size = param->check_size;

      table[0] = s.size+1;
      table[1] = MAX_SIZE;
      t1 = tuneup_measure (param->function, param, &s);

      table[0] = s.size;
      table[1] = s.size+1;
      t2 = tuneup_measure (param->function2, param, &s);
      if (t1 == -1.0 || t2 == -1.0)
        {
          printf ("Oops, can't run both functions at size %ld\n", s.size);
          abort ();
        }
      t1 *= param->function_fudge;

      /* ask that t2 is at least 4% below t1 */
      if (t1 < t2*1.04)
        {
          if (option_trace)
            printf ("function2 never enough faster: t1=%.9f t2=%.9f\n", t1, t2);
          table[0] = MP_SIZE_T_MAX;
          print_define (param->name[0], table[0]);
          return;
        }

      if (option_trace >= 2)
        printf ("function2 enough faster at size=%ld: t1=%.9f t2=%.9f\n",
                s.size, t1, t2);
    }

  s.size = param->min_size;

  for (i = 0; i < max_table && s.size < MAX_SIZE; i++)
    {
      int  since_positive, since_thresh_change;
      int  thresh_idx, new_thresh_idx;

      print_define_start (param->name[i]);

      ndat = 0;
      since_positive = 0;
      since_thresh_change = 0;
      thresh_idx = 0;

      if (option_trace >= 2)
        {
          printf ("             algorithm-A  algorithm-B   ratio  possible\n");
          printf ("              (seconds)    (seconds)    diff    thresh\n");
        }

      for (;
           s.size < MAX_SIZE; 
           s.size += MAX ((mp_size_t) floor (s.size * param->step_factor), 1))
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
          */

          /* using method i at this size */
          table[i] = s.size+1;
          table[i+1] = MAX_SIZE;
          ti = tuneup_measure (param->function, param, &s);
          if (ti == -1.0)
            abort ();
          ti *= param->function_fudge;

          /* using method i+1 at this size */
          table[i] = s.size;
          table[i+1] = s.size+1;
          tiplus1 = tuneup_measure (param->function2, param, &s);
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
            printf ("i=%d size=%ld  %.9f  %.9f  % .4f %c  %ld\n",
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
          if (ti >= tiplus1 * param->stop_factor)
            {
              if (option_trace >= 1)
                printf ("i=%d stopped due to ti >= tiplus1 * factor (%.1f)\n",
                        i, param->stop_factor);
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

      if (param->min_is_always && table[i] == param->min_size)
        table[i] = 0;

      print_define_end (param->name[i], table[i]);

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

      /* compare k to k+1 in the middle of the current k+1 step */
      s.size = size + fft_step_size (k+1) / 2;
      s.r = k;
      tk = tuneup_measure (p->function, NULL, &s);
      if (tk == -1.0)
        abort ();

      s.r = k+1;
      tk1 = tuneup_measure (p->function, NULL, &s);
      if (tk1 == -1.0)
        abort ();

      if (option_trace >= 2)
        printf ("at %ld   size=%ld  k=%d  %.9f   k=%d %.9f\n",
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

      s.size = size + fft_step_size (k) / 2;
      s.r = k;
      tk = tuneup_measure (p->function, NULL, &s);
      if (tk == -1.0)
        abort ();

      if (!modf)  s.size /= 2;
      tm = tuneup_measure (p->mul_function, NULL, &s);
      if (tm == -1.0)
        abort ();

      if (option_trace >= 2)
        printf ("at %ld   size=%ld   k=%d  %.9f   size=%ld %s mul %.9f\n",
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
  time_t  start_time, end_time;
  TMP_DECL (marker);

  TMP_MARK (marker);
  s.xp_block = SPEED_TMP_ALLOC_LIMBS (SPEED_BLOCK_SIZE, 0);
  s.yp_block = SPEED_TMP_ALLOC_LIMBS (SPEED_BLOCK_SIZE, 0);

  mpn_random (s.xp_block, SPEED_BLOCK_SIZE);
  mpn_random (s.yp_block, SPEED_BLOCK_SIZE);

  speed_time_init ();
  fprintf (stderr, "Using: %s\n", speed_time_string);

  if (speed_unittime == 1.0)
    fprintf (stderr, "speed_precision %d, speed_unittime 1 cycle\n",
             speed_precision);
  else
    fprintf (stderr, "speed_precision %d, speed_unittime %.2e secs\n",
             speed_precision, speed_unittime);

  fprintf (stderr, "MAX_SIZE %d, fft_max_size %ld\n",
           MAX_SIZE, option_fft_max_size);
  fprintf (stderr, "\n");

  time (&start_time);
  {
    struct tm  *tp;
    tp = localtime (&start_time);
    printf ("/* Generated by tuneup.c, %d-%02d-%02d. */\n\n",
            tp->tm_year+1900, tp->tm_mon+1, tp->tm_mday);
  }

  {
    static struct param_t  param;
    param.name[0] = "KARATSUBA_MUL_THRESHOLD";
    param.name[1] = "TOOM3_MUL_THRESHOLD";
    param.function = speed_mpn_mul_n;
    param.min_size = MPN_KARA_MUL_N_MINSIZE;
    param.max_size[1] = TOOM3_MUL_THRESHOLD_LIMIT;
    one (mul_threshold, numberof(mul_threshold)-1, &param);
  }
  printf("\n");

  {
    static struct param_t  param;
    param.name[0] = "KARATSUBA_SQR_THRESHOLD";
    param.name[1] = "TOOM3_SQR_THRESHOLD";
    param.function = speed_mpn_sqr_n;
    param.min_size = MPN_KARA_SQR_N_MINSIZE;
    param.max_size[0] = TUNE_KARATSUBA_SQR_MAX;
    one (sqr_threshold, numberof(sqr_threshold)-1, &param);
  }
  printf("\n");

  {
    static struct param_t  param;
    param.name[0] = "DC_THRESHOLD";
    param.function = speed_mpn_dc_tdiv_qr;
    one (dc_threshold, 1, &param);
  }

  /* This is an indirect determination, based on a comparison between redc
     and mpz_mod.  A fudge factor of 1.04 is applied to redc, to represent
     additional overheads it gets in mpz_powm.  */
  {
    static struct param_t  param;
    param.name[0] = "POWM_THRESHOLD";
    param.function = speed_redc;
    param.function2 = speed_mpz_mod;
    param.step_factor = 0.03;
    param.function_fudge = 1.04;
    one (powm_threshold, 1, &param);
  }
  printf("\n");

  {
    static struct param_t  param;
    param.name[0] = "FIB_THRESHOLD";
    param.function = speed_mpz_fib_ui;

    /* Bigcase odd and even sizes run at noticably different speeds, so
       always step by 1, and don't let stop_factor truncate the search.  */
    param.step_factor = 0.001;
    param.stop_factor = 5.0;

    /* start the search from a point after the table data */
    switch (BITS_PER_MP_LIMB) {
    case 32: param.min_size = 93; break;
    case 64: param.min_size = 186; break;
    default:
      printf ("Don't know FIB_THRESHOLD starting point for BITS_PER_MP_LIMB == %d\n",
              BITS_PER_MP_LIMB);
      abort ();
    }

    one (fib_threshold, 1, &param);
  }
  printf("\n");

  {
    static struct param_t  param;
    param.name[0] = "GCD_ACCEL_THRESHOLD";
    param.function = speed_mpn_gcd;
    param.min_size = 1;
    one (gcd_accel_threshold, 1, &param);
  }

  /* A comparison between the speed of a single limb step and a double limb
     step is made.  On a 32-bit limb the ratio is about 2.2 single steps to
     equal a double step, or on a 64-bit limb about 2.09.  (These were found
     from counting the steps on a 10000 limb gcdext.  */
  {
    static struct param_t  param;
    param.name[0] = "GCDEXT_THRESHOLD";
    param.function = speed_mpn_gcdext_one_single;
    param.function2 = speed_mpn_gcdext_one_double;
    switch (BITS_PER_MP_LIMB) {
    case 32: param.function_fudge = 2.2; break;
    case 64: param.function_fudge = 2.09; break;
    default: 
      printf ("Don't know GCDEXT_THERSHOLD factor for BITS_PER_MP_LIMB == %d\n",
              BITS_PER_MP_LIMB);
      abort ();
    }
    param.min_size = 5;
    param.min_is_always = 1;
    param.max_size[0] = 300;
    param.check_size = 300;
    one (gcdext_threshold, 1, &param);
  }
  printf("\n");

#if UDIV_PREINV_ALWAYS
  /* When udiv_qrnnd is implemented using udiv_qrnnd_preinv there's no need
     to tune, just always use the latter.  */
  printf ("#define DIVREM_1_NORM_THRESHOLD    0\n");
  printf ("#define DIVREM_1_UNNORM_THRESHOLD  0\n");
  printf ("#define MOD_1_NORM_THRESHOLD       0\n");
  printf ("#define MOD_1_UNNORM_THRESHOLD     0\n");

#else
  /* size_extra==1 reflects the fact that with high<divisor one division is
     always skipped.  Forcing high<divisor while testing ensures consistency
     while stepping through sizes, ie. that size-1 divides will be done each
     time.

     min_size==2 and min_is_always are used so that if plain division is
     only better at size==1 then don't bother including that code just for
     that case, instead go with preinv always and get a size saving.  */

#define DIV_1_PARAMS            \
  param.check_size = 256;       \
  param.min_size = 2;           \
  param.min_is_always = 1;      \
  param.data_high_lt_r = 1;     \
  param.size_extra = 1;         \
  param.stop_factor = 2.0;

  /* No support for tuning native assembler code, do that by hand and put
     the results in the .asm file. */
#if ! HAVE_NATIVE_mpn_divrem_1
  /* Tune for the integer part of divrem_1.  This will very possibly be a
     bit out for the fractional part, but that's too bad. */
  {
    static struct param_t  param;
    param.name[0] = "DIVREM_1_NORM_THRESHOLD";
    DIV_1_PARAMS;
    s.r = randlimb_norm ();
    param.function = speed_mpn_divrem_1;
    one (divrem_1_norm_threshold, 1, &param);
  }
  {
    static struct param_t  param;
    param.name[0] = "DIVREM_1_UNNORM_THRESHOLD";
    DIV_1_PARAMS;
    s.r = randlimb_half ();
    param.function = speed_mpn_divrem_1;
    one (divrem_1_unnorm_threshold, 1, &param);
  }
#endif
#if ! HAVE_NATIVE_mpn_mod_1
  {
    static struct param_t  param;
    param.name[0] = "MOD_1_NORM_THRESHOLD";
    DIV_1_PARAMS;
    s.r = randlimb_norm ();
    param.function = speed_mpn_mod_1;
    one (mod_1_norm_threshold, 1, &param);
  }
  {
    static struct param_t  param;
    param.name[0] = "MOD_1_UNNORM_THRESHOLD";
    DIV_1_PARAMS;
    s.r = randlimb_half ();
    param.function = speed_mpn_mod_1;
    one (mod_1_unnorm_threshold, 1, &param);
  }
#endif
#endif
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

  time (&end_time);
  printf ("/* Tuneup completed successfully, took %ld seconds */\n",
          end_time - start_time);

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
  exit (0);
}





/* The tune program uses a recompiled mpn/generic/divrem_1.c, which doesn't
   provide an mpn_divrem_1c.  If a native divrem_1 is instead being used
   then common.c will want an mpn_divrem_1c, so provide a dummy here.
   Without this the linker would drag in divrem_1.o from libgmp.la and
   mpn_divrem_1 there would conflict with the recompiled divrem_1.c.  */

mp_limb_t
mpn_divrem_1c (mp_ptr dst, mp_size_t xsize, mp_srcptr src, mp_size_t size,
               mp_limb_t divisor, mp_limb_t carry)
{
  abort ();
}

mp_limb_t
mpn_mod_1c (mp_srcptr src, mp_size_t size, mp_limb_t divisor, mp_limb_t carry)
{
  abort ();
}
