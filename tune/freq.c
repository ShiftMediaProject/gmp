/* CPU frequency determination.

Copyright 1999, 2000, 2001 Free Software Foundation, Inc.

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

#include "config.h"

#include <stdio.h>
#include <stdlib.h> /* for getenv, qsort */
#if HAVE_UNISTD_H
#include <unistd.h> /* for sysconf */
#endif

#include <sys/types.h>
#if HAVE_SYS_PARAM_H
#include <sys/param.h>   /* for constants needed by NetBSD <sys/sysctl.h> */
#endif
#if HAVE_SYS_SYSCTL_H
#include <sys/sysctl.h>  /* for sysctlbyname() */
#endif

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

#if HAVE_SYS_PROCESSOR_H
#include <sys/processor.h>  /* for solaris processor_info_t */
#endif

/* Remove definitions from NetBSD <sys/param.h>, to avoid conflicts with
   gmp-impl.h. */
#ifdef MIN
#undef MIN
#endif
#ifdef MAX
#undef MAX
#endif

#include "gmp.h"
#include "gmp-impl.h"

#include "speed.h"


#define HELP(str)                       \
  if (help)                             \
    {                                   \
      printf ("    - %s\n", str);       \
      return 0;                         \
    }


/* GMP_CPU_FREQUENCY environment variable.  Should be in Hertz and can be
   floating point, for example "450e6". */
static int
freq_environment (int help)
{
  char  *e;

  HELP ("environment variable GMP_CPU_FREQUENCY (in Hertz)");

  e = getenv ("GMP_CPU_FREQUENCY");
  if (e == NULL)
    return 0;

  speed_cycletime = 1.0 / atof (e);

  if (speed_option_verbose)
    printf ("Using GMP_CPU_FREQUENCY %.2f for cycle time %.3g\n",
            atof (e), speed_cycletime);

  return 1;
}


/* i386 FreeBSD 2.2.8 sysctlbyname machdep.i586_freq is in Hertz.
   There's no obvious defines available to get this from plain sysctl.  */
static int
freq_sysctlbyname_i586_freq (int help)
{
#if HAVE_SYSCTLBYNAME
  unsigned  val;
  size_t    size;

  HELP ("sysctlbyname() machdep.i586_freq");

  size = sizeof(val);
  if (sysctlbyname ("machdep.i586_freq", &val, &size, NULL, 0) == 0
      && size == sizeof(val))
    {
      if (speed_option_verbose)
        printf ("Using sysctlbyname() machdep.i586_freq %u for cycle time %.3g\n",
                val, speed_cycletime);
      speed_cycletime = 1.0 / (double) val;
      return 1;
    }
#endif
  return 0;
}


/* i368 FreeBSD 3.3 sysctlbyname machdep.tsc_freq is in Hertz.
   There's no obvious defines to get this from plain sysctl.  */

static int
freq_sysctlbyname_tsc_freq (int help)
{
#if HAVE_SYSCTLBYNAME
  unsigned  val;
  size_t    size;

  HELP ("sysctlbyname() machdep.tsc_freq");

  size = sizeof(val);
  if (sysctlbyname ("machdep.tsc_freq", &val, &size, NULL, 0) == 0
      && size == sizeof(val))
    {
      if (speed_option_verbose)
        printf ("Using sysctlbyname() machdep.tsc_freq %u for cycle time %.3g\n",
                val, speed_cycletime);
      speed_cycletime = 1.0 / (double) val;
      return 1;
    }
#endif
  return 0;
}


/* Apple powerpc Darwin 1.3 sysctl hw.cpufrequency is in hertz.  For some
   reason only seems to be available from sysctl(), not sysctlbyname().  */

static int
freq_sysctl_hw_cpufrequency (int help)
{
#if HAVE_SYSCTL && defined (CTL_HW) && defined (HW_CPU_FREQ)
  int       mib[2];
  unsigned  val;
  size_t    size;

  HELP ("sysctl() hw.cpufrequency");

  mib[0] = CTL_HW;
  mib[1] = HW_CPU_FREQ;
  size = sizeof(val);
  if (sysctl (mib, 2, &val, &size, NULL, 0) == 0)
    {
      if (speed_option_verbose)
        printf ("Using sysctl() hw.cpufrequency %u for cycle time %.3g\n",
                val, speed_cycletime);
      speed_cycletime = 1.0 / (double) val;
      return 1;
    }
#endif
  return 0;
}


/* Alpha FreeBSD 4.1 and NetBSD 1.4 sysctl- hw.model string gives "Digital
   AlphaPC 164LX 599 MHz".  NetBSD 1.4 doesn't seem to have sysctlbyname, so
   sysctl() is used.  */

static int
freq_sysctl_hw_model (int help)
{
#if HAVE_SYSCTL && defined (CTL_HW) && defined (HW_MODEL)
  int       mib[2];
  char      str[128];
  unsigned  val;
  size_t    size;
  char  *p;
  int   i;

  HELP ("sysctl() hw.model");

  mib[0] = CTL_HW;
  mib[1] = HW_MODEL;
  size = sizeof(str);
  if (sysctl (mib, 2, str, &size, NULL, 0) == 0)
    {
      /* find the second last space */
      p = &str[size-1];
      for (i = 0; i < 2; i++)
        {
          for (;;)
            {
              if (p <= str)
                return 0;
              p--;
              if (*p == ' ')
                break;
            }
        }

      if (sscanf (p, "%u MHz", &val) != 1)
        return 0;

      if (speed_option_verbose)
        printf ("Using sysctl() hw.model %u for cycle time %.3g\n",
                val, speed_cycletime);
      speed_cycletime = 1e-6 / (double) val;
      return 1;
    }
#endif
  return 0;
}


/* /proc/cpuinfo for linux kernel.

   Linux doesn't seem to have any system call to get the CPU frequency, at
   least not in 2.0.x or 2.2.x, so it's necessary to read /proc/cpuinfo.

   i386 2.0.36 - "bogomips" is the CPU frequency.

   i386 2.2.13 - has both "cpu MHz" and "bogomips", and it's "cpu MHz" which
                 is the frequency.

   alpha 2.2.5 - "cycle frequency [Hz]" seems to be right, "BogoMIPS" is
                 very slightly different.

   alpha 2.2.18pre21 - "cycle frequency [Hz]" is 0 on at least one system,
                 "BogoMIPS" seems near enough.
  */

static int
freq_proc_cpuinfo (int help)
{
  FILE    *fp;
  char    buf[128];
  double  val;
  int     ret = 0;

  HELP ("linux kernel /proc/cpuinfo file, cpu MHz or bogomips");

  if ((fp = fopen ("/proc/cpuinfo", "r")) != NULL)
    {
      while (fgets (buf, sizeof (buf), fp) != NULL)
        {
          if (sscanf (buf, "cycle frequency [Hz]    : %lf", &val) == 1
              && val != 0.0)
            {
              speed_cycletime = 1.0 / val;
              if (speed_option_verbose)
                printf ("Using /proc/cpuinfo \"cycle frequency\" %.2f for cycle time %.3g\n", val, speed_cycletime);
              ret = 1;
              break;
            }
          if (sscanf (buf, "cpu MHz  : %lf\n", &val) == 1)
            {
              speed_cycletime = 1e-6 / val;
              if (speed_option_verbose)
                printf ("Using /proc/cpuinfo \"cpu MHz\" %.2f for cycle time %.3g\n", val, speed_cycletime);
              ret = 1;
              break;
            }
          if (sscanf (buf, "bogomips : %lf\n", &val) == 1
              || sscanf (buf, "BogoMIPS : %lf\n", &val) == 1)
            {
              speed_cycletime = 1e-6 / val;
              if (speed_option_verbose)
                printf ("Using /proc/cpuinfo \"bogomips\" %.2f for cycle time %.3g\n", val, speed_cycletime);
              ret = 1;
              break;
            }
        }
      fclose (fp);
    }
  return ret;
}


/* /bin/sysinfo for SunOS 4.
   Prints a line like: cpu0 is a "75 MHz TI,TMS390Z55" CPU */
static int
freq_sunos_sysinfo (int help)
{
  int     ret = 0;
#if HAVE_POPEN
  FILE    *fp;
  char    buf[128];
  double  val;

  HELP ("SunOS /bin/sysinfo program output, cpu0");

  /* Error messages are sent to /dev/null in case /bin/sysinfo doesn't
     exist.  The brackets are necessary for some shells. */
  if ((fp = popen ("(/bin/sysinfo) 2>/dev/null", "r")) != NULL)
    {
      while (fgets (buf, sizeof (buf), fp) != NULL)
        {
          if (sscanf (buf, " cpu0 is a \"%lf MHz", &val) == 1)
            {
              speed_cycletime = 1e-6 / val;
              if (speed_option_verbose)
                printf ("Using /bin/sysinfo \"cpu0 MHz\" %.2f for cycle time %.3g\n", val, speed_cycletime);
              ret = 1;
              break;
            }
        }
      pclose (fp);
    }
#endif
  return ret;
}


/* "/etc/hw -r cpu" for SCO OpenUnix 8, printing a line like
	The speed of the CPU is approximately 450Mhz  */
static int
freq_sco_etchw (int help)
{
  int     ret = 0;
#if HAVE_POPEN
  FILE    *fp;
  char    buf[128];
  double  val;

  HELP ("SCO /etc/hw program output");

  /* Error messages are sent to /dev/null in case /etc/hw doesn't exist.
     The brackets are necessary for some shells. */
  if ((fp = popen ("(/etc/hw -r cpu) 2>/dev/null", "r")) != NULL)
    {
      while (fgets (buf, sizeof (buf), fp) != NULL)
        {
          if (sscanf (buf, " The speed of the CPU is approximately %lfMhz",
                      &val) == 1)
            {
              speed_cycletime = 1e-6 / val;
              if (speed_option_verbose)
                printf ("Using /etc/hw %.2f MHz, for cycle time %.3g\n",
                        val, speed_cycletime);
              ret = 1;
              break;
            }
        }
      pclose (fp);
    }
#endif
  return ret;
}


/* "hinv -c processor" for IRIX.
   The first line printed is for instance "2 195 MHZ IP27 Processors".  */
static int
freq_irix_hinv (int help)
{
  int     ret = 0;
#if HAVE_POPEN
  FILE    *fp;
  char    buf[128];
  double  val;
  int     nproc;

  HELP ("IRIX \"hinv -c processor\" output");

  /* Error messages are sent to /dev/null in case hinv doesn't exist.  The
     brackets are necessary for some shells. */
  if ((fp = popen ("(hinv -c processor) 2>/dev/null", "r")) != NULL)
    {
      while (fgets (buf, sizeof (buf), fp) != NULL)
        {
          if (sscanf (buf, "%d %lf MHZ", &nproc, &val) == 2)
            {
              speed_cycletime = 1e-6 / val;
              if (speed_option_verbose)
                printf ("Using hinv -c processor \"%.2f MHZ\" for cycle time %.3g\n", val, speed_cycletime);
              ret = 1;
              break;
            }
        }
      pclose (fp);
    }
#endif
  return ret;
}


/* FreeBSD on i386 gives a line like the following at bootup, and which can
   be read back from /var/run/dmesg.boot.

       CPU: AMD Athlon(tm) Processor (755.29-MHz 686-class CPU)
       CPU: Pentium 4 (1707.56-MHz 686-class CPU)

   This is useful on FreeBSD 4.x, where there's no sysctl machdep.tsc_freq
   or machdep.i586_freq.

   It's better to use /var/run/dmesg.boot than to run /sbin/dmesg, since the
   latter prints the current system message buffer, which is a limited size
   and can wrap around if the system is up for a long time.  */

static int
freq_bsd_dmesg (int help)
{
  FILE    *fp;
  char    buf[256], *p;
  double  val;
  int     ret = 0;

  HELP ("BSD /var/run/dmesg.boot file");

  if ((fp = fopen ("/var/run/dmesg.boot", "r")) != NULL)
    {
      while (fgets (buf, sizeof (buf), fp) != NULL)
        {
          if (memcmp (buf, "CPU:", 4) == 0)
            {
              for (p = buf; *p != '\0'; p++)
                {
                  if (sscanf (p, "(%lf-MHz", &val) == 1)
                    {
                      speed_cycletime = 1e-6 / val;
                      if (speed_option_verbose)
                        printf ("Using /var/run/dmesg.boot CPU: %.2f MHz for cycle time %.3g\n", val, speed_cycletime);
                      ret = 1;
                      break;
                    }
                }
            }
        }
      fclose (fp);
    }
  return ret;
}


/* processor_info() for Solaris.  "psrinfo" is the command-line interface to
   this.  "prtconf -vp" gives similar information.

   Apple Darwin has a processor_info, but in an incompatible style.  It
   doesn't have <sys/processor.h>, so test for that.  */

static int
freq_processor_info (int help)
{
#if HAVE_PROCESSOR_INFO && HAVE_SYS_PROCESSOR_H
  processor_info_t  p;
  int  i, n, mhz = 0;

  HELP ("processor_info() pi_clock");

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
                   "freq_processor_info(): There's more than one CPU and they have different clock speeds\n");
          return 0;
        }

      mhz = p.pi_clock;
    }

  speed_cycletime = 1.0e-6 / (double) mhz;

  if (speed_option_verbose)
    printf ("Using processor_info() %d mhz for cycle time %.3g\n",
            mhz, speed_cycletime);
  return 1;

#else
  return 0;
#endif
}


/* The cycle counter is sampled on the same side of gettimeofday for greater
   accuracy.  The return value is a cycle time period in seconds.  */
#if HAVE_SPEED_CYCLECOUNTER && HAVE_GETTIMEOFDAY
static double
freq_measure_one (void)
{
  struct timeval  st, et;
  unsigned        sc[2], ec[2];
  long            dt;
  double          dc;

  gettimeofday (&st, NULL);
  speed_cyclecounter (sc);

  for (;;)
    {
      gettimeofday (&et, NULL);
      speed_cyclecounter (ec);

      dt = (et.tv_sec - st.tv_sec) * 1000000L + (et.tv_usec - st.tv_usec);
      if (dt >= 100000)
        break;
    }

  dc = speed_cyclecounter_diff (ec, sc);
  if (speed_option_verbose >= 2)
    printf ("freq_measure_one() dc=%.1f dt=%ld\n", dc, dt);
  return dt * 1e-6 / dc;
}
#endif

static int
freq_measure (int help)
{
#if HAVE_SPEED_CYCLECOUNTER && HAVE_GETTIMEOFDAY

/* MEASURE_MATCH is how many readings within MEASURE_TOLERANCE of each other
   are required.  This must be at least 2.  */
#define MEASURE_MAX_ATTEMPTS   20
#define MEASURE_TOLERANCE      1.005  /* 0.5% */
#define MEASURE_MATCH          3

  double  t[MEASURE_MAX_ATTEMPTS];
  int     i, j;

  if (! gettimeofday_microseconds_p ())
    return 0;
  if (! cycles_works_p ())
    return 0;

  HELP ("cycle counter measured with microsecond gettimeofday()");

  for (i = 0; i < numberof (t); i++)
    {
      t[i] = freq_measure_one ();
      if (speed_option_verbose >= 2)
        printf ("freq_measure() t[%d] is %.6g\n", i, t[i]);

      qsort (t, i+1, sizeof(t[0]), (qsort_function_t) double_cmp_ptr);
      if (speed_option_verbose >= 3)
        for (j = 0; j <= i; j++)
          printf ("   t[%d] is %.6g\n", j, t[j]);

      for (j = 0; j+MEASURE_MATCH-1 <= i; j++)
        {
          if (t[j+MEASURE_MATCH-1] <= t[j] * MEASURE_TOLERANCE)
            {
              /* use the average of the range found */
              speed_cycletime = (t[j+MEASURE_MATCH-1] + t[j]) / 2.0;
              if (speed_option_verbose)
                printf ("Using gettimeofday() measured cycle counter %.4g (%.2f MHz)\n",
                        speed_cycletime, 1e-6/speed_cycletime);
              return 1;
            }
        }
    }
#endif
  return 0;
}


/* Each function returns 1 if it succeeds in setting speed_cycletime, or 0
   if not.

   In general system call tests are first since they're fast, then file
   tests, then tests running programs.  Necessary exceptions to this rule
   are noted.  The measuring is last since it's time consuming, and rather
   wasteful of cpu.  */

static int
freq_all (int help)
{
  return
    /* This should be first, so an environment variable can override
       anything the system gives. */
    freq_environment (help)

    || freq_sysctl_hw_model (help)
    || freq_sysctl_hw_cpufrequency (help)
    || freq_sysctlbyname_i586_freq (help)
    || freq_sysctlbyname_tsc_freq (help)

    /* SCO openunix 8 puts a dummy pi_clock==16 in processor_info, so be
       sure to check /etc/hw before that function. */
    || freq_sco_etchw (help)

    || freq_processor_info (help)
    || freq_proc_cpuinfo (help)
    || freq_bsd_dmesg (help)
    || freq_irix_hinv (help)
    || freq_sunos_sysinfo (help)
    || freq_measure (help);
};


void
speed_cycletime_init (void)
{
  static int  attempted = 0;

  if (attempted)
    return;
  attempted = 1;

  if (freq_all (0))
    return;

  if (speed_option_verbose)
    printf ("CPU frequency couldn't be determined\n");
}


void
speed_cycletime_fail (const char *str)
{
  fprintf (stderr, "Measuring with: %s\n", speed_time_string);
  fprintf (stderr, "%s,\n", str);
  fprintf (stderr, "but none of the following are available,\n");
  freq_all (1);
  abort ();
}

void
speed_cycletime_need_cycles (void)
{
  speed_time_init ();
  if (speed_cycletime == 1.0)
    speed_cycletime_fail
      ("Need to know CPU frequency to give times in cycles");
}

void
speed_cycletime_need_seconds (void)
{
  speed_time_init ();
  if (speed_cycletime == 1.0)
    speed_cycletime_fail
      ("Need to know CPU frequency to convert cycles to seconds");
}
