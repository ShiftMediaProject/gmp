/* Time routines for speed measurments.

Copyright 1999, 2000, 2001, 2002 Free Software Foundation, Inc.

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


/* speed_time_init() - initialize timing things.  speed_starttime() calls
   this if it hasn't been done yet, so you only need to call this explicitly
   if you want to use the global variables before the first measurement.
  
   speed_starttime() - start a time measurment.

   speed_endtime() - end a time measurment, return time taken (seconds or
   cycles).

   speed_time_string - a string describing the time method in use.

   speed_unittime - global variable with the unit of time measurement
   accuracy (seconds or cycles).

   speed_precision - global variable which is the intended accuracy of time
   measurements.  speed_measure() for instance runs target routines with
   enough repetitions so it takes at least speed_unittime*speed_precision.
   A program can provide an option so the user can set this, otherwise it
   gets a default based on the measuring method chosen.

   speed_cycletime - the time in seconds for each CPU cycle, for example on
   a 100 MHz CPU this would be 1.0e-8.  If the CPU frequency is unknown this
   is 0.0 if the time base is in seconds, or 1.0 if it's in cycles.


   speed_endtime() and speed_unittime are normally in seconds, but if a
   cycle counter is being used to measure and the CPU frequency is unknown,
   then speed_endtime() returns cycles and speed_cycletime and
   speed_unittime are 1.0.

   Notice that speed_unittime*speed_precision is the target duration for
   speed_endtime(), irrespective of whether that's in seconds or cycles.

   Call speed_cycletime_need_seconds() to demand that speed_endtime() is in
   seconds and not perhaps in cycles.

   Call speed_cycletime_need_cycles() to demand that speed_cycletime is
   non-zero, so that speed_endtime()/speed_cycletime will work to give times
   in cycles.


   Notes:

   Various combinations of cycle counter, read_real_time(), getrusage(),
   gettimeofday() and times() can arise, according to which are available
   and their precision.


   Allowing speed_endtime() to return either seconds or cycles is only a
   slight complication and makes it possible for the speed program to do
   some sensible things without demanding the CPU frequency.  If seconds are
   being measured then it can always print seconds, and if cycles are being
   measured then it can always print them without needing to know how long
   they are.  Also the tune program doesn't care at all what the units are.

   GMP_CPU_FREQUENCY can always be set when the automated methods in freq.c
   fail.  This will be needed if times in seconds are wanted but a cycle
   counter is being used, or if times in cycles are wanted but getrusage or
   another seconds based timer is in use.

   If the measuring method uses a cycle counter but supplements it with
   getrusage or the like, then knowing the CPU frequency is mandatory since
   the code compares values from the two.


   Solaris gethrtime() seems no more than a slow way to access the Sparc V9
   cycle counter.  gethrvtime() seems to be relevant only to LWP, it doesn't
   for instance give nanosecond virtual time.  So neither of these are used.

*/

#include "config.h"

#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h> /* for getenv() */

#if HAVE_STDINT_H
#include <stdint.h> /* for uint64_t */
#endif

#if HAVE_UNISTD_H
#include <unistd.h> /* for sysconf() */
#endif

#include <sys/types.h>

#if TIME_WITH_SYS_TIME
# include <sys/time.h>  /* for struct timeval */
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#if HAVE_SYS_TIMES_H
#include <sys/times.h>  /* for times() and struct tms */
#endif

#if HAVE_SYS_RESOURCE_H
#include <sys/resource.h>  /* for struct rusage */
#endif

#if HAVE_SYS_SYSTEMCFG_H
#include <sys/systemcfg.h> /* for RTC_POWER on AIX */
#endif

#include "gmp.h"
#include "gmp-impl.h"

#include "speed.h"



char    speed_time_string[256];
int     speed_precision = 0;
double  speed_unittime;
double  speed_cycletime = 0.0;


/* don't rely on "unsigned" to "double" conversion, it's broken in SunOS 4
   native cc */
#define M_2POWU   (((double) INT_MAX + 1.0) * 2.0)

#define M_2POW32  4294967296.0
#define M_2POW64  (M_2POW32 * M_2POW32)


/* Conditionals for the time functions available are done with normal C
   code, which is a lot easier than wildly nested preprocessor directives.

   The choice of what to use is partly made at run-time, according to
   whether the cycle counter works and the measured accuracy of getrusage
   and gettimeofday.

   A routine that's not available won't be getting called, but is an abort()
   to be sure it isn't called mistakenly.

   It can be assumed that if a function exists then its data type will, but
   if the function doesn't then the data type might or might not exist, so
   the type can't be used unconditionally.  The "struct_rusage" etc macros
   provide dummies when the respective function doesn't exist. */


#if HAVE_SPEED_CYCLECOUNTER
static const int have_cycles = HAVE_SPEED_CYCLECOUNTER;
#else
static const int have_cycles = 0;
#define speed_cyclecounter(p)  abort()
#endif

/* "stck" returns ticks since 1 Jan 1900 00:00 GMT, where each tick is 2^-12
   microseconds.  Same #ifdefs here as in longlong.h.  */
#if defined (__GNUC__) && ! defined (NO_ASM)                            \
  && (defined (__i370__) || defined (__s390__) || defined (__mvs__))
static const int  have_stck = 1;
static const int  use_stck = 1;  /* always use when available */
typedef uint64_t  stck_t; /* gcc for s390 is quite new, always has uint64_t */
#define STCK(timestamp)                 \
  do {                                  \
    asm ("stck %0" : "=m" (timestamp)); \
  } while (0)
#else
static const int  have_stck = 0;
static const int  use_stck = 0;
typedef unsigned long  stck_t;   /* dummy */
#define STCK(timestamp)  abort()
#endif
#define STCK_PERIOD      (1.0 / 4096e6)   /* 2^-12 microseconds */

#if HAVE_READ_REAL_TIME
static const int have_rrt = 1;
#else
static const int have_rrt = 0;
#define read_real_time(t,s)     abort()
#define time_base_to_time(t,s)  abort()
#define RTC_POWER     1
#define RTC_POWER_PC  2
#define timebasestruct_t   struct timebasestruct_dummy
struct timebasestruct_dummy {
  int             flag;
  unsigned int    tb_high;
  unsigned int    tb_low;
};
#endif

#if HAVE_CLOCK_GETTIME
static const int have_cgt = 1;
#define struct_timespec  struct timespec
#else
static const int have_cgt = 0;
#define clock_gettime(id,ts)  (-1)
#define clock_getres(id,ts)   (-1)
#define struct_timespec       struct timespec_dummy
#endif
#ifdef CLOCK_PROCESS_CPUTIME_ID
# define CGT_ID   CLOCK_PROCESS_CPUTIME_ID
#else
# ifdef CLOCK_VIRTUAL
#  define CGT_ID  CLOCK_VIRTUAL
# else
#  define CGT_ID  -1
# endif
#endif

#if HAVE_GETRUSAGE
static const int have_grus = 1;
#define struct_rusage   struct rusage
#else
static const int have_grus = 0;
#define getrusage(n,ru)  abort()
#define struct_rusage    struct rusage_dummy
#endif

#if HAVE_GETTIMEOFDAY
static const int have_gtod = 1;
#define struct_timeval   struct timeval
#else
static const int have_gtod = 0;
#define gettimeofday(tv,tz)  abort()
#define struct_timeval   struct timeval_dummy
#endif

#if HAVE_TIMES
static const int have_times = 1;
#define struct_tms   struct tms
#else
static const int have_times = 0;
#define times(tms)  abort()
#define struct_tms   struct tms_dummy
#endif

struct tms_dummy {
  long  tms_utime;
};
struct timeval_dummy {
  long  tv_sec;
  long  tv_usec;
};
struct rusage_dummy {
  struct_timeval ru_utime;
};
struct timespec_dummy {
  long  tv_sec;
  long  tv_nsec;
};

static int  use_cycles;
static int  use_rrt;
static int  use_gtod;
static int  use_grus;
static int  use_cgt;
static int  use_times;
static int  use_tick_boundary;

static unsigned         start_cycles[2];
static stck_t           start_stck;
static timebasestruct_t start_rrt;
static struct_timespec  start_cgt;
static struct_rusage    start_grus;
static struct_timeval   start_gtod;
static struct_tms       start_times;

static double  cycles_limit = 1e100;
static double  cgt_unittime;
static double  grus_unittime;
static double  gtod_unittime;
static double  times_unittime;

/* for RTC_POWER format, ie. seconds and nanoseconds */
#define TIMEBASESTRUCT_SECS(t)  ((t)->tb_high + (t)->tb_low * 1e-9)


/* Return a string representing a time in seconds, nicely formatted.
   Eg. "10.25ms".  */
char *
unittime_string (double t)
{
  static char  buf[128];
  
  const char  *unit;
  int         prec;

  /* choose units and scale */
  if (t < 1e-9)
    t *= 1e9, unit = "ns";
  else if (t < 1e-6)
    t *= 1e6, unit = "us";
  else if (t < 1.0)
    t *= 1e3, unit = "ms";
  else
    unit = "s";

  /* want 4 significant figures */
  if (t < 1.0)
    prec = 4;
  else if (t < 10.0)
    prec = 3;
  else if (t < 100.0)
    prec = 2;
  else
    prec = 1;

  sprintf (buf, "%.*f%s", prec, t, unit);
  return buf;
}


static jmp_buf  cycles_works_buf;

static RETSIGTYPE
cycles_works_handler (int sig)
{
  longjmp (cycles_works_buf, 1);
}

int
cycles_works_p (void)
{
  static int  result = -1;
  RETSIGTYPE (*old_handler) _PROTO ((int));
  unsigned  cycles[2];

  /* suppress a warning about cycles[] unused */
  cycles[0] = 0;

  if (result != -1)
    goto done;

#ifdef SIGILL
  old_handler = signal (SIGILL, cycles_works_handler);
  if (old_handler == SIG_ERR)
    {
      if (speed_option_verbose)
        printf ("cycles_works_p(): SIGILL not supported, assuming speed_cyclecounter() works\n");
      goto yes;
    }
  if (setjmp (cycles_works_buf))
    {
      if (speed_option_verbose)
        printf ("cycles_works_p(): SIGILL during speed_cyclecounter(), so doesn't work\n");
      result = 0;
      goto done;
    }
  speed_cyclecounter (cycles);
  signal (SIGILL, old_handler);
  if (speed_option_verbose)
    printf ("cycles_works_p(): speed_cyclecounter() works\n");
#else

  if (speed_option_verbose)
    printf ("cycles_works_p(): SIGILL not defined, assuming speed_cyclecounter() works\n");
#endif

 yes:
  result = 1;

 done:
  return result;
}


long
clk_tck (void)
{
  static long  result = -1L;
  if (result != -1L)
    return result;

#if HAVE_SYSCONF
  result = sysconf (_SC_CLK_TCK);
  if (result != -1L)
    {
      if (speed_option_verbose)
        printf ("sysconf(_SC_CLK_TCK) is %ld per second\n", result);
      return result;
    }

  fprintf (stderr,
           "sysconf(_SC_CLK_TCK) not working, using CLK_TCK instead\n");
#endif

#ifdef CLK_TCK
  result = CLK_TCK;
  if (speed_option_verbose)
    printf ("CLK_TCK is %ld per second\n", result);
  return result;
#else
  fprintf (stderr, "CLK_TCK not defined, cannot continue\n");
  abort ();
#endif
}


/* Assume that if a time difference that's non-zero but less than CLK_TCK/2
   is seen then the routine is microsecond accurate.

   FIXME: What if clk_tck() is some big value (like 1000000). */

#define MICROSECONDS_P(name, decl, tv, call)                            \
  do {                                                                  \
    static int  result = -1;                                            \
                                                                        \
    long  clk_usec = 1000000L / clk_tck ();                             \
    long  prev, delta;                                                  \
    int   count = 0;                                                    \
    decl;                                                               \
                                                                        \
    if (result != -1)                                                   \
      return result;                                                    \
                                                                        \
    call;                                                               \
    prev = tv.tv_usec;                                                  \
    for (;;)                                                            \
      {                                                                 \
        call;                                                           \
                                                                        \
        delta = (tv.tv_usec - prev) % 1000000L;                         \
        if (delta != 0)                                                 \
          {                                                             \
            if (speed_option_verbose >= 2)                              \
              printf ("%s saw .%06ld -> .%06ld for delta %ld\n",        \
                      name, prev, tv.tv_usec, delta);                   \
                                                                        \
            if (delta < clk_usec/2)                                     \
              {                                                         \
                result = 1;                                             \
                break;                                                  \
              }                                                         \
                                                                        \
            count++;                                                    \
            if (count > 10)                                             \
              {                                                         \
                result = 0;                                             \
                break;                                                  \
              }                                                         \
          }                                                             \
                                                                        \
        prev = tv.tv_usec;                                              \
      }                                                                 \
                                                                        \
    if (speed_option_verbose)                                           \
      printf ("%s is %s accurate\n",                                    \
              name, result ? "microsecond" : "clock tick");             \
    return result;                                                      \
  } while (0)


int
gettimeofday_microseconds_p (void)
{
  MICROSECONDS_P ("gettimeofday",
                  struct_timeval t, t, gettimeofday (&t, NULL));
}

int
getrusage_microseconds_p (void)
{
  MICROSECONDS_P ("getrusage",
                  struct_rusage r, (r.ru_utime), getrusage (0, &r));
}


int
cgt_works_p (void)
{
  static int  result = -1;
  struct_timespec  unit;

  if (! have_cgt)
    return 0;

  if (CGT_ID == -1)
    {
      if (speed_option_verbose)
        printf ("clock_gettime don't know what ID to use\n");
      result = 0;
      return result;
    }

  if (result != -1)
    return result;

  /* trial run to see if it works */
  if (clock_gettime (CGT_ID, &unit) != 0)
    {
      if (speed_option_verbose)
        printf ("clock_gettime id=%d error: %s\n", CGT_ID, strerror (errno));
      result = 0;
      return result;
    }

  /* get the resolution */
  if (clock_getres (CGT_ID, &unit) != 0)
    {
      if (speed_option_verbose)
        printf ("clock_getres id=%d error: %s\n", CGT_ID, strerror (errno));
      result = 0;
      return result;
    }

  cgt_unittime = unit.tv_sec + unit.tv_nsec * 1e-9;
  printf ("clock_gettime is %s accurate\n",
          unittime_string (cgt_unittime));
  result = 1;
  return result;
}


#define DEFAULT(var,n)  \
  do {                  \
    if (! (var))        \
      (var) = (n);      \
  } while (0)

void
speed_time_init (void)
{
  double supplement_unittime = 0.0;

  static int  speed_time_initialized = 0;
  if (speed_time_initialized)
    return;
  speed_time_initialized = 1;

  speed_cycletime_init ();

  if (have_cycles && cycles_works_p ())
    {
      use_cycles = 1;
      DEFAULT (speed_cycletime, 1.0);
      speed_unittime = speed_cycletime;
      DEFAULT (speed_precision, 10000);
      strcpy (speed_time_string, "CPU cycle counter");

      /* only used if a supplementary method is chosen below */
      cycles_limit = (have_cycles == 1 ? M_2POW32 : M_2POW64) / 2.0
        * speed_cycletime;

      if (have_grus && getrusage_microseconds_p())
        {
          /* this is a good combination */
          use_grus = 1;
          supplement_unittime = grus_unittime = 1.0e-6;
          strcpy (speed_time_string, "CPU cycle counter, supplemented by microsecond getrusage()");
        }
      else if (have_cycles == 1)
        {
          /* When speed_cyclecounter has a limited range, look for something
             to supplement it. */
          if (have_gtod && gettimeofday_microseconds_p())
            {
              use_gtod = 1;
              supplement_unittime = gtod_unittime = 1.0e-6;
              strcpy (speed_time_string, "CPU cycle counter, supplemented by microsecond gettimeofday()");
            }
          else if (have_grus)
            {
              use_grus = 1;
              supplement_unittime = grus_unittime = 1.0 / (double) clk_tck ();
              sprintf (speed_time_string, "CPU cycle counter, supplemented by %s clock tick getrusage()", unittime_string (supplement_unittime));
            }
          else if (have_times)
            {
              use_times = 1;
              supplement_unittime = times_unittime = 1.0 / (double) clk_tck ();
              sprintf (speed_time_string, "CPU cycle counter, supplemented by %s clock tick times()", unittime_string (supplement_unittime));
            }
          else if (have_gtod)
            {
              use_gtod = 1;
              supplement_unittime = gtod_unittime = 1.0 / (double) clk_tck ();
              sprintf (speed_time_string, "CPU cycle counter, supplemented by %s clock tick gettimeofday()", unittime_string (supplement_unittime));
            }
          else
            {
              fprintf (stderr, "WARNING: cycle counter is 32 bits and there's no other functions.\n");
              fprintf (stderr, "    Wraparounds may produce bad results on long measurements.\n");
            }
        }

      if (use_grus || use_times || use_gtod)
        {
          /* must know cycle period to compare cycles to other measuring
             (via cycles_limit) */
          speed_cycletime_need_seconds ();

          if (speed_precision * supplement_unittime > cycles_limit)
            {
              fprintf (stderr, "WARNING: requested precision can't always be achieved due to limited range\n");
              fprintf (stderr, "    cycle counter and limited precision supplemental method\n");
              fprintf (stderr, "    (%s)\n", speed_time_string);
            }
        }
    }
  else if (have_stck)
    {
      strcpy (speed_time_string, "STCK timestamp");
      /* stck is in units of 2^-12 microseconds, which is very likely higher
         resolution than a cpu cycle */
      if (speed_cycletime == 0.0)
        speed_cycletime_fail
          ("Need to know CPU frequency for effective stck unit");
      speed_unittime = MAX (speed_cycletime, STCK_PERIOD);
      DEFAULT (speed_precision, 10000);
    }
  else if (have_rrt)
    {
      timebasestruct_t  t;
      use_rrt = 1;
      DEFAULT (speed_precision, 10000);
      read_real_time (&t, sizeof(t));
      switch (t.flag) {
      case RTC_POWER:
        /* FIXME: What's the actual RTC resolution? */
        speed_unittime = 1e-7;
        strcpy (speed_time_string, "read_real_time() power nanoseconds");
        break;
      case RTC_POWER_PC:
        t.tb_high = 1;
        t.tb_low = 0;
        time_base_to_time (&t, sizeof(t));
        speed_unittime = TIMEBASESTRUCT_SECS(&t) / M_2POW32;
        sprintf (speed_time_string, "%s read_real_time() powerpc ticks",
                 unittime_string (speed_unittime));
        break;
      default:
        fprintf (stderr, "ERROR: Unrecognised timebasestruct_t flag=%d\n",
                 t.flag);
        abort ();
      }
    }
  else if (have_cgt && cgt_works_p() && cgt_unittime < 1.5e-6)
    {
      /* use clock_gettime if microsecond or better resolution */
    choose_cgt:
      use_cgt = 1;
      speed_unittime = cgt_unittime;
      DEFAULT (speed_precision, (cgt_unittime <= 0.1e-6 ? 10000 : 1000));
      strcpy (speed_time_string, "microsecond accurate getrusage()");
    }
  else if (have_grus && getrusage_microseconds_p())
    {
      use_grus = 1;
      speed_unittime = grus_unittime = 1.0e-6;
      DEFAULT (speed_precision, 1000);
      strcpy (speed_time_string, "microsecond accurate getrusage()");
    }
  else if (have_gtod && gettimeofday_microseconds_p())
    {
      use_gtod = 1;
      speed_unittime = gtod_unittime = 1.0e-6;
      DEFAULT (speed_precision, 1000);
      strcpy (speed_time_string, "microsecond accurate gettimeofday()");
    }
  else if (have_cgt && cgt_works_p() && cgt_unittime < 1.5/clk_tck())
    {
      /* use clock_gettime if 1 tick or better resolution */
      goto choose_cgt;
    }
  else if (have_times)
    {
      use_times = 1;
      use_tick_boundary = 1;
      speed_unittime = times_unittime = 1.0 / (double) clk_tck ();
      DEFAULT (speed_precision, 200);
      sprintf (speed_time_string, "%s clock tick times()",
               unittime_string (speed_unittime));
    }
  else if (have_grus)
    {
      use_grus = 1;
      use_tick_boundary = 1;
      speed_unittime = grus_unittime = 1.0 / (double) clk_tck ();
      DEFAULT (speed_precision, 200);
      sprintf (speed_time_string, "%s clock tick getrusage()\n",
               unittime_string (speed_unittime));
    }
  else if (have_gtod)
    {
      use_gtod = 1;
      use_tick_boundary = 1;
      speed_unittime = gtod_unittime = 1.0 / (double) clk_tck ();
      DEFAULT (speed_precision, 200);
      sprintf (speed_time_string, "%s clock tick gettimeofday()",
               unittime_string (speed_unittime));
    }
  else
    {
      fprintf (stderr, "No time measuring method available\n");
      fprintf (stderr, "None of: speed_cyclecounter(), STCK(), getrusage(), gettimeofday(), times()\n");
      abort ();
    }

  if (speed_option_verbose)
    {
      printf ("speed_time_init: %s\n", speed_time_string);
      printf ("    speed_precision     %d\n", speed_precision);
      printf ("    speed_unittime      %.2g\n", speed_unittime);
      if (supplement_unittime)
        printf ("    supplement_unittime %.2g\n", supplement_unittime);
      printf ("    use_tick_boundary   %d\n", use_tick_boundary);
      if (have_cycles)
        printf ("    cycles_limit        %.2g seconds\n", cycles_limit);
    }
}



/* Burn up CPU until a clock tick boundary, for greater accuracy.  Set the
   corresponding "start_foo" appropriately too. */

void
grus_tick_boundary (void)
{
  struct_rusage  prev;
  getrusage (0, &prev);
  do {
    getrusage (0, &start_grus);
  } while (start_grus.ru_utime.tv_usec == prev.ru_utime.tv_usec);
}

void
gtod_tick_boundary (void)
{
  struct_timeval  prev;
  gettimeofday (&prev, NULL);
  do {
    gettimeofday (&start_gtod, NULL);
  } while (start_gtod.tv_usec == prev.tv_usec);
}

void
times_tick_boundary (void)
{
  struct_tms  prev;
  times (&prev);
  do
    times (&start_times);
  while (start_times.tms_utime == prev.tms_utime);
}


/* "have_" values are tested to let unused code go dead.  */

void
speed_starttime (void)
{
  speed_time_init ();

  if (have_grus && use_grus)
    {
      if (use_tick_boundary)
        grus_tick_boundary ();
      else
        getrusage (0, &start_grus);
    }

  if (have_gtod && use_gtod)
    {
      if (use_tick_boundary)
        gtod_tick_boundary ();
      else
        gettimeofday (&start_gtod, NULL);
    }

  if (have_times && use_times)
    {
      if (use_tick_boundary)
        times_tick_boundary ();
      else
        times (&start_times);
    }

  if (have_cgt && use_cgt)
    clock_gettime (CGT_ID, &start_cgt);

  if (have_rrt && use_rrt)
    read_real_time (&start_rrt, sizeof(start_rrt));

  if (have_stck && use_stck)
    STCK (start_stck);

  /* Cycles sampled last for maximum accuracy. */
  if (have_cycles && use_cycles)
    speed_cyclecounter (start_cycles);
}


/* Calculate the difference between two cycle counter samples, as a "double"
   counter of cycles.

   The start and end values are allowed to cancel in integers in case the
   counter values are bigger than the 53 bits that normally fit in a double.

   This works even if speed_cyclecounter() puts a value bigger than 32-bits
   in the low word (the high word always gets a 2**32 multiplier though). */

double
speed_cyclecounter_diff (const unsigned end[2], const unsigned start[2])
{
  unsigned  d;
  double    t;

  if (have_cycles == 1)
    {
      t = (end[0] - start[0]);
    }
  else
    {
      d = end[0] - start[0];
      t = d - (d > end[0] ? M_2POWU : 0.0);
      t += (end[1] - start[1]) * M_2POW32;
    }
  return t;
}


/* Calculate the difference between "start" and "end" using fields "sec" and
   "psec", where each "psec" is a "punit" of a second.

   The high parts are allowed to cancel before being combined with the low
   parts, in case a simple "sec+psec*punit" exceeds the precision of a
   double.

   Total time in units of "psec"s are only calculated in a "double" since an
   integer might overflow.  2^32 microseconds is only a bit over an hour, or
   2^32 nanoseconds only about 4 seconds.

   The casts to "long" are for the beneifit of timebasestruct_t, where the
   fields are only "unsigned", but we want a signed difference.  */

#define DIFF_SECS_ROUTINE(type, sec, psec, punit)               \
  {                                                             \
    long  delta_sec = (long) end->sec - (long) start->sec;      \
    return (double) delta_sec                                   \
      + punit * ((long) end->psec - (long) start->psec);        \
  }                                                             \

double
timebasestruct_diff_secs (const timebasestruct_t *end,
                          const timebasestruct_t *start)
{
  DIFF_SECS_ROUTINE (unsigned, tb_high, tb_low, 1e-9);
}

double
timeval_diff_secs (const struct_timeval *end, const struct_timeval *start)
{
  DIFF_SECS_ROUTINE (long, tv_sec, tv_usec, 1e-6);
}

double
rusage_diff_secs (const struct_rusage *end, const struct_rusage *start)
{
  DIFF_SECS_ROUTINE (long, ru_utime.tv_sec, ru_utime.tv_usec, 1e-6);
}

double
timespec_diff_secs (const struct_timespec *end, const struct_timespec *start)
{
  DIFF_SECS_ROUTINE (long, tv_sec, tv_nsec, 1e-9);
}


double
speed_endtime (void)
{
#define END_USE(name,value)                             \
  do {                                                  \
    if (speed_option_verbose >= 3)                      \
      printf ("speed_endtime(): used %s\n", name);      \
    result = value;                                     \
    goto done;                                          \
  } while (0)

#define END_ENOUGH(name,value)                                          \
  do {                                                                  \
    if (speed_option_verbose >= 3)                                      \
      printf ("speed_endtime(): %s gives enough precision\n", name);    \
    result = value;                                                     \
    goto done;                                                          \
  } while (0)

#define END_EXCEED(name,value)                                            \
  do {                                                                    \
    if (speed_option_verbose >= 3)                                        \
      printf ("speed_endtime(): cycle counter limit exceeded, used %s\n", \
              name);                                                      \
    result = value;                                                       \
    goto done;                                                            \
  } while (0)

  unsigned          end_cycles[2];
  stck_t            end_stck;
  timebasestruct_t  end_rrt;
  struct_timespec   end_cgt;
  struct_timeval    end_gtod;
  struct_rusage     end_grus;
  struct_tms        end_times;
  double            t_gtod, t_grus, t_times, t_cgt, t_rrt, t_stck, t_cycles;
  double            result;

  /* Cycles sampled first for maximum accuracy.
     "have_" values tested to let unused code go dead.  */

  if (have_cycles && use_cycles)  speed_cyclecounter (end_cycles);
  if (have_stck   && use_stck)    STCK (end_stck);
  if (have_rrt    && use_rrt)     read_real_time (&end_rrt, sizeof(end_rrt));
  if (have_cgt    && use_cgt)     clock_gettime (CGT_ID, &end_cgt);
  if (have_gtod   && use_gtod)    gettimeofday (&end_gtod, NULL);
  if (have_grus   && use_grus)    getrusage (0, &end_grus);
  if (have_times  && use_times)   times (&end_times);

  result = -1.0;

  /* suppress warnings about unused variables */
  t_gtod = 0.0;
  t_grus = 0.0;
  t_times = 0.0;

  if (speed_option_verbose >= 4)
    {
      printf ("speed_endtime():\n");
      if (use_cycles)
        printf ("   cycles  0x%X,0x%X -> 0x%X,0x%X\n",
                start_cycles[1], start_cycles[0],
                end_cycles[1], end_cycles[0]);

      if (use_stck)
        printf ("   stck  0x%lX -> 0x%lX\n", start_stck, end_stck);

      if (use_rrt)
        printf ("   read_real_time  (%d)%u,%u -> (%d)%u,%u\n",
                start_rrt.flag, start_rrt.tb_high, start_rrt.tb_low,
                end_rrt.flag, end_rrt.tb_high, end_rrt.tb_low);

      if (use_cgt)
        printf ("   clock_gettime  %ld.%09ld -> %ld.%09ld\n",
                start_cgt.tv_sec, start_cgt.tv_nsec,
                end_cgt.tv_sec, end_cgt.tv_nsec);

      if (use_gtod)
        printf ("   gettimeofday  %ld.%06ld -> %ld.%06ld\n",
                start_gtod.tv_sec, start_gtod.tv_usec,
                end_gtod.tv_sec, end_gtod.tv_usec);

      if (use_grus)
        printf ("   getrusage  %ld.%06ld -> %ld.%06ld\n",
                start_grus.ru_utime.tv_sec, start_grus.ru_utime.tv_usec,
                end_grus.ru_utime.tv_sec, end_grus.ru_utime.tv_usec);

      if (use_times)
        printf ("   times  %ld -> %ld\n",
                start_times.tms_utime, end_times.tms_utime);
    }

  if (use_rrt)
    {
      time_base_to_time (&start_rrt, sizeof(start_rrt));
      time_base_to_time (&end_rrt, sizeof(end_rrt));
      t_rrt = timebasestruct_diff_secs (&end_rrt, &start_rrt);
      END_USE ("read_real_time()", t_rrt);
    }

  if (use_cgt)
    {
      t_cgt = timespec_diff_secs (&end_cgt, &start_cgt);
      END_USE ("clock_gettime()", t_cgt);
    }

  if (use_grus)
    {
      t_grus = rusage_diff_secs (&end_grus, &start_grus);

      /* Use getrusage() if the cycle counter limit would be exceeded, or if
         it provides enough accuracy already. */
      if (use_cycles)  
        {
          if (t_grus >= speed_precision*grus_unittime)
            END_ENOUGH ("getrusage()", t_grus);
          if (t_grus >= cycles_limit)
            END_EXCEED ("getrusage()", t_grus);
        }
    }

  if (use_times)
    {
      t_times = (end_times.tms_utime - start_times.tms_utime) * times_unittime;

      /* Use times() if the cycle counter limit would be exceeded, or if
         it provides enough accuracy already. */
      if (use_cycles)  
        {
          if (t_times >= speed_precision*times_unittime)
            END_ENOUGH ("times()", t_times);
          if (t_times >= cycles_limit)
            END_EXCEED ("times()", t_times);
        }
    }

  if (use_gtod)
    {
      t_gtod = timeval_diff_secs (&end_gtod, &start_gtod);

      /* Use gettimeofday() if it measured a value bigger than the cycle
         counter can handle.  */
      if (use_cycles)  
        {
          if (t_gtod >= cycles_limit)
            END_EXCEED ("gettimeofday()", t_gtod);
        }
    }
  
  if (use_stck)  
    {
      t_stck = (end_stck - start_stck) * STCK_PERIOD;
      END_USE ("stck", t_stck);
    }

  if (use_cycles)  
    {
      t_cycles = speed_cyclecounter_diff (end_cycles, start_cycles)
        * speed_cycletime;
      END_USE ("cycle counter", t_cycles);
    }

  if (use_grus && getrusage_microseconds_p())
    END_USE ("getrusage()", t_grus);

  if (use_gtod && gettimeofday_microseconds_p())
    END_USE ("gettimeofday()", t_gtod);

  if (use_times)  END_USE ("times()",        t_times);
  if (use_grus)   END_USE ("getrusage()",    t_grus);
  if (use_gtod)   END_USE ("gettimeofday()", t_gtod);

  fprintf (stderr, "speed_endtime(): oops, no time method available\n");
  abort ();

 done:
  if (result < 0.0)
    {
      fprintf (stderr,
               "speed_endtime(): fatal error: negative time measured: %.9f\n",
               result);
      abort ();
    }
  return result;
}
