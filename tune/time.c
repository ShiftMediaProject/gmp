/* Time routines for speed measurments. */

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

/* speed_time_init() - initialize timing things.  speed_starttime() calls
   this if it hasn't been done yet, so you only need to call this explicitly
   if you want to use the global variables before the first measurement.
  
   speed_starttime() - start a time measurment.

   speed_endtime() - end a time measurment, return time taken, in seconds.

   speed_unittime - global variable with the unit of time measurement
   accuracy, in seconds.

   speed_precision - global variable which is the intended accuracy of time
   measurements.  speed_measure() for instance runs target routines with
   enough repetitions so it takes at least speed_unittime*speed_precision
   seconds.  A program can provide an option so the user can set this.

   speed_cycletime - the time in seconds for each CPU cycle, for example on
   a 100 MHz CPU this would be 1.0e-8.  If the CPU frequency is unknown,
   speed_cycletime is 1.0.  See speed_cycletime_init().

   speed_time_string - a string describing the time method in use.


   Enhancements:

   Add support for accurate timing on more CPUs, machines and systems.

   Extend automatic CPU frequency determination to more kernels and systems.

 */


#include <stdio.h>
#include <stdlib.h> /* for getenv */
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <sys/types.h>
#if HAVE_SYS_SYSCTL_H
#include <sys/sysctl.h>
#endif

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"

#include "speed.h"


#if HAVE_SPEED_CYCLECOUNTER
#define SPEED_USE_CYCLECOUNTER               1
#else
#define SPEED_USE_MICROSECOND_GETRUSAGE      0
#define SPEED_USE_MICROSECOND_GETTIMEOFDAY   1
#define SPEED_USE_TMS_UTIME                  0
#endif


#define TIMEVAL_SECS(tp) \
  ((double) (tp)->tv_sec + (double) (tp)->tv_usec * 1.0e-6)


/* Look for an environment variable for CPU clock frequency.
   GMP_CPU_FREQUENCY should be in Hertz, in floating point form,
   eg. "450e6". */
int
speed_cpu_frequency_environment (void)
{
  char  *e;

  e = getenv ("GMP_CPU_FREQUENCY");
  if (e == NULL)
    return 0;

  speed_cycletime = 1.0 / atof (e);
  return 1;
}


/* On FreeBSD 3.3 the headers have #defines like CPU_WALLCLOCK under
   CTL_MACHDEP but don't seem to have anything for machdep.tsc_freq or
   machdep.i586_freq.  Using the string forms with sysctlbyname() works
   though, and lets libc worry about the defines and headers.

   FreeBSD 3.3 has tsc_freq, FreeBSD 2.2.8 has i586_freq instead.
   The "sysctl -a" command prints everything available. */

#if HAVE_SYSCTLBYNAME
int
speed_cpu_frequency_sysctlbyname (void)
{
  unsigned  val;
  size_t    valsize;

  valsize = sizeof(val);
  if (sysctlbyname ("machdep.tsc_freq", &val, &valsize, NULL, 0) != 0
      || valsize != sizeof(val))
    {
      valsize = sizeof(val);
      if (sysctlbyname ("machdep.i586_freq", &val, &valsize, NULL, 0) != 0
          || valsize != sizeof(val))
        return 0;
    }

  speed_cycletime = 1.0 / (double) val;
  return 1;
}
#endif


/* Linux doesn't seem to have any system call to get the CPU frequency, at
   least not in 2.0.x or 2.2.x, so it's necessary to read /proc/cpuinfo.

   i386 2.0.36 - "bogomips" is the CPU frequency.

   i386 2.2.13 - has both "cpu MHz" and "bogomips", and it's "cpu MHz" which
                 is the frequency.

   alpha 2.2.5 - "cycle frequency [Hz]" seems to be right, "BogoMIPS" is
                 very slightly different.  */

int
speed_cpu_frequency_proc_cpuinfo (void)
{
  FILE    *fp;
  char    buf[128];
  double  val;
  int     ret = 0;

  if ((fp = fopen ("/proc/cpuinfo", "r")) != NULL)
    {
      while (fgets (buf, sizeof (buf), fp) != NULL)
        {
          if (sscanf (buf, "cycle frequency [Hz]    : %lf est.\n", &val) == 1)
            {
              speed_cycletime = 1.0 / val;
              ret = 1;
              break;
            }
          if (sscanf (buf, "cpu MHz  : %lf\n", &val) == 1)
            {
              speed_cycletime = 1e-6 / val;
              ret = 1;
              break;
            }
          if (sscanf (buf, "bogomips : %lf\n", &val) == 1)
            {
              speed_cycletime = 1e-6 / val;
              ret = 1;
              break;
            }
        }
      fclose (fp);
    }
  return ret;
}


/* SunOS /bin/sysinfo prints a line like:
       cpu0 is a "75 MHz TI,TMS390Z55" CPU */

#if HAVE_POPEN
int
speed_cpu_frequency_sunos_sysinfo (void)
{
  FILE    *fp;
  char    buf[128];
  double  val;
  int     ret = 0;

  /* Error messages are sent to /dev/null in case /bin/sysinfo doesn't
     exist.  The brackets are necessary for some shells (eg. ash). */
  if ((fp = popen ("(/bin/sysinfo) 2>/dev/null", "r")) != NULL)
    {
      while (fgets (buf, sizeof (buf), fp) != NULL)
        {
          if (sscanf (buf, " cpu0 is a \"%lf MHz", &val) == 1)
            {
              speed_cycletime = 1e-6 / val;
              ret = 1;
              break;
            }
        }
      pclose (fp);
    }
  return ret;
}
#endif


/* This is for Solaris.  "psrinfo" is the command-line interface to
   processor_info().  "prtconf -vp" gives similar information.  */

#if HAVE_PROCESSOR_INFO
#include <sys/unistd.h>     /* for _SC_NPROCESSORS_CONF */
#include <sys/processor.h>  /* for processor_info_t */
int
speed_cpu_frequency_processor_info (void)
{
  processor_info_t  p;
  int  i, n, mhz = 0;

  n = sysconf (_SC_NPROCESSORS_CONF);
  for (i = 0; i < n; i++)
    {
      if (processor_info (i, &p) != 0)
        continue;
      if (p.pi_state != P_ONLINE)
        continue;

      if (mhz != 0 && p.pi_clock != mhz)
        {
          fprintf (stderr,
                   "speed_cpu_frequency_processor_info(): There's more than one CPU and they have different clock speeds\n");
          return 0;
        }

      mhz = p.pi_clock;
    }

  speed_cycletime = 1.0e-6 / (double) mhz;
  return 1;
}
#endif


/* Each function returns 1 if it succeeds in setting speed_cycletime, or 0
   if not.  */

static const struct {
  int         (*fun) _PROTO ((void));
  const char  *description;

} speed_cpu_frequency_table[] = {

  /* This should be first, so an environment variable can override anything
     the system gives. */
  { speed_cpu_frequency_environment,
    "environment variable GMP_CPU_FREQUENCY (in Hertz)" },

#if HAVE_SYSCTLBYNAME
  { speed_cpu_frequency_sysctlbyname,
    "sysctlbyname() machdep.tsc_freq or machdep.i586_freq" },
#endif

#if HAVE_PROCESSOR_INFO
  { speed_cpu_frequency_processor_info,
    "processor_info() pi_clock" },
#endif

  { speed_cpu_frequency_proc_cpuinfo,
    "linux kernel /proc/cpuinfo file, cpu MHz or bogomips" },

#if HAVE_POPEN
  { speed_cpu_frequency_sunos_sysinfo,
    "SunOS /bin/sysinfo program cpu0 output" },
#endif
};


int
speed_cycletime_init (void)
{
  int  i;

  for (i = 0; i < numberof (speed_cpu_frequency_table); i++)
    if ((*speed_cpu_frequency_table[i].fun)())
      return 1;

  fprintf (stderr,
           "Cannot determine CPU frequency, need one of the following\n");
  for (i = 0; i < numberof (speed_cpu_frequency_table); i++)
    fprintf (stderr, "\t- %s\n", speed_cpu_frequency_table[i].description);

  return 0;
}


/* ---------------------------------------------------------------------- */
#if SPEED_USE_CYCLECOUNTER

const char *speed_time_string 
  = "Time measurements using CPU cycle counter.\n";

/* bigish value because we have a fast timer */
int speed_precision = 10000;

double speed_unittime;
double speed_cycletime;

static int  speed_time_initialized = 0;
static unsigned  speed_starttime_save[2];

/* Knowing the CPU frequency is mandatory, so cycles can be converted to
   seconds.  */
void
speed_time_init (void)
{
  if (speed_time_initialized)
    return;
  speed_time_initialized = 1;

  if (!speed_cycletime_init ())
    exit (1);

  speed_unittime = speed_cycletime;
}

void
speed_starttime (void)
{
  if (!speed_time_initialized)
    speed_time_init ();
  speed_cyclecounter (speed_starttime_save);
}

#define M_2POWU   ((double) (1L << (BITS_PER_INT-2)) * 4.0)
#define M_2POW32  4294967296.0

double
speed_endtime (void)
{
  unsigned  endtime[2], e0;
  double    t;

  speed_cyclecounter (endtime);

  /* This still works even if speed_cyclecounter() puts a value bigger than
     32-bits in the low word.  The start and end values are allowed to
     cancel in uints in case a uint is more than the 53 bits that will
     normally fit in a double. */
  e0 = endtime[0] - speed_starttime_save[0];
  t = e0 - (e0 > endtime[0] ? M_2POWU : 0);
  t += (endtime[1] - speed_starttime_save[1]) * M_2POW32;

  return t * speed_unittime;
}

#endif


/* ---------------------------------------------------------------------- */
#if SPEED_USE_MICROSECOND_GETRUSAGE
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

const char *speed_time_string
  = "Time measurements using microsecond accurate getrusage.\n";

int speed_precision = 1000;

double speed_unittime = 1.0e-6;
double speed_cycletime = 1.0;

static struct rusage  speed_starttime_save;
static int  speed_time_initialized = 0;

void
speed_time_init (void)
{
  if (speed_time_initialized)
    return;
  speed_time_initialized = 1;

  speed_cycletime_init ();
}

void
speed_starttime (void)
{
  if (!speed_time_initialized)
    speed_time_init ();

  getrusage (0, &speed_starttime_save);
}

double
speed_endtime (void)
{
  struct rusage r;

  getrusage (0, &r);
  return TIMEVAL_SECS (&r.ru_utime)
    - TIMEVAL_SECS (&speed_starttime_save.ru_utime);
}
#endif


/* ---------------------------------------------------------------------- */
#if SPEED_USE_MICROSECOND_GETTIMEOFDAY
/* This method is for systems with a microsecond accurate gettimeofday().

   A dummy timezone parameter is always given to gettimeofday(), in case it
   doesn't allow NULL.  */

#include <sys/time.h>

const char *speed_time_string 
  = "Time measurements using microsecond accurate gettimeofday.\n";

/* highish value because we have an accurate timer */
int speed_precision = 1000;

double speed_unittime = 1.0e-6;
double speed_cycletime = 1.0;

static struct timeval  speed_starttime_save;
static int  speed_time_initialized = 0;

void
speed_time_init (void)
{
  if (speed_time_initialized)
    return;
  speed_time_initialized = 1;

  speed_cycletime_init ();
}

void
speed_starttime (void)
{
  struct timezone  tz;
  if (!speed_time_initialized)
    speed_time_init ();

  gettimeofday (&speed_starttime_save, &tz);
}

double
speed_endtime (void)
{
  struct timeval   t;
  struct timezone  tz;

  gettimeofday (&t, &tz);
  return TIMEVAL_SECS (&t) - TIMEVAL_SECS (&speed_starttime_save);
}

#endif


/* ---------------------------------------------------------------------- */
#if SPEED_USE_TMS_UTIME
/* You're in trouble if you have to use this method.  Speed measurments and
   threshold tuning are going to take a long time. */

#if STDC_HEADERS
#include <errno.h>      /* for errno */
#include <string.h>     /* for strerror */
#endif
#if HAVE_UNISTD_H
#include <unistd.h>     /* for sysconf */
#endif
#include <sys/times.h>  /* for times */

const char *speed_time_string 
  = "Time measurements using tms_utime.\n";


/* lowish default value so we don't take days and days to do tuning */
int  speed_precision = 200;

double  speed_unittime;
double  speed_cycletime = 1.0;

static struct tms  speed_starttime_save;
static int  speed_time_initialized = 0;

void
speed_time_init (void)
{
  long  clk_tck;

  if (speed_time_initialized)
    return;
  speed_time_initialized = 1;

  speed_cycletime_init ();

#if HAVE_SYSCONF
  clk_tck = sysconf (_SC_CLK_TCK);
  if (clk_tck == -1L)
    {
      fprintf (stderr, "sysconf(_SC_CLK_TCK) not available: %s\n",
	       strerror(errno));
      fprintf (stderr, "\tusing CLK_TCK instead\n");
      clk_tck = CLK_TCK;
    }
#else
  clk_tck = CLK_TCK;
#endif

  speed_unittime = 1.0 / (double) clk_tck;
}

/* Burn up CPU until a times() tms_utime tick boundary.
   Doing so lets you know a measurement has started on a tick boundary,
   effectively halving the uncertainty in the measurement.
   *t1 gets the start times() values the caller should use. */
void
times_utime_boundary (struct tms *t1)
{
  struct tms  t2;
  times (&t2);
  do
    times (t1);
  while (t1->tms_utime == t2.tms_utime);
}

void
speed_starttime (void)
{
  if (!speed_time_initialized)
    speed_time_init ();
  times_utime_boundary (&speed_starttime_save);
}

double
speed_endtime (void)
{
  struct tms  t;
  times (&t);
  return (t.tms_utime - speed_starttime_save.tms_utime) * speed_unittime;
}

#endif
