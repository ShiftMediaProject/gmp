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


/* GMP_CPU_FREQUENCY environment variable.  Should be in Hertz and can be
   floating point, for example "450e6". */
int
speed_cpu_frequency_environment (void)
{
  char  *e;
 
  e = getenv ("GMP_CPU_FREQUENCY");
  if (e == NULL)
    return 0;

  speed_cycletime = 1.0 / atof (e);

  if (speed_option_verbose)
    printf ("Using GMP_CPU_FREQUENCY %.2f for cycle time %.3g\n",
            atof (e), speed_cycletime);

  return 1;
}


/* sysctl() and sysctlbyname() for BSD flavours.  The "sysctl -a" command
   prints everything available.

   FreeBSD 2.2.8 i386 - machdep.i586_freq is in Hertz.

   FreeBSD 3.3 i368 - machdep.tsc_freq is in Hertz.  There's no obvious
       define for tsc_freq in sysctl.h, but sysctlbyname works fine.

   FreeBSD 4.1 and NetBSD 1.4 alpha - hw.model string gives "Digital AlphaPC
       164LX 599 MHz".  NetBSD 1.4 doesn't have sysctlbyname, so sysctl() is
       used.

   Darwin 1.3 powerpc - hw.cpufrequency is in hertz, but for some reason
       only seems to be available from sysctl(), not sysctlbyname().

   GNU/Linux with glibc 2.2 has a sysctl call, but it doesn't give a cpu
       frequency as such, not in kernel 2.2 at least.  */

#if HAVE_SYSCTLBYNAME
#define HAVE_SPEED_CPU_FREQUENCY_SYSCTLBYNAME 1
int
speed_cpu_frequency_sysctlbyname (void)
{
  unsigned  val;
  size_t    size;

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

  return 0;
}
#endif

#if HAVE_SYSCTL
#define HAVE_SPEED_CPU_FREQUENCY_SYSCTL 1
int
speed_cpu_frequency_sysctl (void)
{
#if defined (CTL_HW) && defined (HW_CPU_FREQ)
  {
    int       mib[2];
    unsigned  val;
    size_t    size;

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
  }
#endif

#if defined (CTL_HW) && defined (HW_MODEL)
  {
    int       mib[2];
    char      str[128];
    unsigned  val;
    size_t    size;

    mib[0] = CTL_HW;
    mib[1] = HW_MODEL;
    size = sizeof(str);
    if (sysctl (mib, 2, str, &size, NULL, 0) == 0)
      {
        char  *p = &str[size-1];
        int   i;

        /* find the second last space */
        for (i = 0; i < 2; i++)
          {
            for (;;)
              {
                if (p <= str)
                  goto hw_model_fail;
                p--;
                if (*p == ' ')
                  break;
              }
          }

        if (sscanf (p, "%u MHz", &val) != 1)
          goto hw_model_fail;

        if (speed_option_verbose)
          printf ("Using sysctl() hw.model %u for cycle time %.3g\n",
                  val, speed_cycletime);
        speed_cycletime = 1e-6 / (double) val;
        return 1;
      }
  }
 hw_model_fail:
#endif

  return 0;
}
#endif


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
   It prints a line like: cpu0 is a "75 MHz TI,TMS390Z55" CPU */

#if HAVE_POPEN
#define HAVE_SPEED_CPU_FREQUENCY_SUNOS_SYSINFO 1
int
speed_cpu_frequency_sunos_sysinfo (void)
{
  FILE    *fp;
  char    buf[128];
  double  val;
  int     ret = 0;

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
  return ret;
}
#endif


/* "/etc/hw -r cpu" for SCO OpenUnix 8, printing a line like
	The speed of the CPU is approximately 450Mhz
*/

#if HAVE_POPEN
#define HAVE_SPEED_CPU_FREQUENCY_SCO_ETCHW 1
int
speed_cpu_frequency_sco_etchw (void)
{
  FILE    *fp;
  char    buf[128];
  double  val;
  int     ret = 0;

  /* Error messages are sent to /dev/null in case /etc/hw doesn't exist.
     The brackets are necessary for some shells. */
  if ((fp = popen ("(/etc/hw -r cpu) 2>/dev/null", "r")) != NULL)
    {
      while (fgets (buf, sizeof (buf), fp) != NULL)
        {
          if (sscanf (buf, " The speed of the CPU is approximately %lfMhz", &val) == 1)
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
  return ret;
}
#endif


/* FreeBSD on i386 gives a line like the following at bootup, and which can
   be read back from /var/run/dmesg.boot.

       CPU: AMD Athlon(tm) Processor (755.29-MHz 686-class CPU)
       CPU: Pentium 4 (1707.56-MHz 686-class CPU)

   This is useful on FreeBSD 4.x, where there's no sysctl machdep.tsc_freq
   or machdep.i586_freq.

   It's better to use /var/run/dmesg.boot than to run /sbin/dmesg, since the
   latter prints the current system message buffer, which is a limited size
   and can wrap around if the system is up for a long time.  */

int
speed_cpu_frequency_bsd_dmesg (void)
{
  FILE    *fp;
  char    buf[256], *p;
  double  val;
  int     ret = 0;

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

   Darwin has a processor_info, but in a different style.  It doesn't have
   <sys/processor.h> so we can differentiate it on that basis.  */

#if HAVE_PROCESSOR_INFO && HAVE_SYS_PROCESSOR_H
#define HAVE_SPEED_CPU_FREQUENCY_PROCESSOR_INFO 1
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

  if (speed_option_verbose)
    printf ("Using processor_info() %d mhz for cycle time %.3g\n",
            mhz, speed_cycletime);

  return 1;
}
#endif


#if HAVE_SPEED_CYCLECOUNTER && HAVE_GETTIMEOFDAY

/* The cycle counter is sampled on the same side of gettimeofday for greater
   accuracy.  The return value is a cycle time period in seconds.  */

#define HAVE_SPEED_CPU_FREQUENCY_MEASURE 1
double
speed_cpu_frequency_measure_one (void)
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
    printf ("speed_cpu_frequency_measure_one() dc=%.1f dt=%ld\n", dc, dt);
  return dt * 1e-6 / dc;
}

/* MEASURE_MATCH is how many readings within MEASURE_TOLERANCE of each other
   are required.  This must be at least 2.  */
int
speed_cpu_frequency_measure (void)
{
#define MEASURE_MAX_ATTEMPTS   20
#define MEASURE_TOLERANCE      1.005  /* 0.5% */
#define MEASURE_MATCH          3

  double  t[MEASURE_MAX_ATTEMPTS];
  int     i, j;

  if (! gettimeofday_microseconds_p ())
    return 0;
  if (! cycles_works_p ())
    return 0;

  for (i = 0; i < numberof (t); i++)
    {
      t[i] = speed_cpu_frequency_measure_one ();
      if (speed_option_verbose >= 2)
        printf ("speed_cpu_frequency_measure() t[%d] is %.6g\n", i, t[i]);

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
  return 0;
}
#endif


/* Each function returns 1 if it succeeds in setting speed_cycletime, or 0
   if not.  */

const struct {
  int         (*fun) _PROTO ((void));
  const char  *description;

} speed_cpu_frequency_table[] = {

  /* This should be first, so an environment variable can override anything
     the system gives. */
  { speed_cpu_frequency_environment,
    "environment variable GMP_CPU_FREQUENCY (in Hertz)" },

#if HAVE_SPEED_CPU_FREQUENCY_SYSCTL
  { speed_cpu_frequency_sysctl,
    "sysctl() hw.model" },
#endif

#if HAVE_SPEED_CPU_FREQUENCY_SYSCTLBYNAME
  { speed_cpu_frequency_sysctlbyname,
    "sysctlbyname() machdep.tsc_freq or machdep.i586_freq" },
#endif

  /* SCO openunix 8 puts a dummy pi_clock==16 in processor_info, so be sure
     to check /etc/hw before that function. */
#if HAVE_SPEED_CPU_FREQUENCY_SCO_ETCHW
  { speed_cpu_frequency_sco_etchw,
    "SCO /etc/hw program output" },
#endif

#if HAVE_SPEED_CPU_FREQUENCY_PROCESSOR_INFO
  { speed_cpu_frequency_processor_info,
    "processor_info() pi_clock" },
#endif

  { speed_cpu_frequency_proc_cpuinfo,
    "linux kernel /proc/cpuinfo file, cpu MHz or bogomips" },

  { speed_cpu_frequency_bsd_dmesg,
    "BSD /var/run/dmesg.boot file" },

#if HAVE_SPEED_CPU_FREQUENCY_SUNOS_SYSINFO
  { speed_cpu_frequency_sunos_sysinfo,
    "SunOS /bin/sysinfo program output, cpu0" },
#endif

#if HAVE_SPEED_CPU_FREQUENCY_MEASURE
  { speed_cpu_frequency_measure,
    "cycle counter measured with microsecond gettimeofday()" },
#endif
};


void
speed_cycletime_init (void)
{
  static int  attempted = 0;
  int  i;

  if (attempted)
    return;
  attempted = 1;

  for (i = 0; i < numberof (speed_cpu_frequency_table); i++)
    if ((*speed_cpu_frequency_table[i].fun)())
      return;

  if (speed_option_verbose)
    printf ("CPU frequency couldn't be determined\n");
}


void
speed_cycletime_fail (const char *str)
{
  int  i;
  fprintf (stderr, "Measuring with: %s\n", speed_time_string);
  fprintf (stderr, "%s,\n", str);
  fprintf (stderr, "but none of the following are available,\n");
  for (i = 0; i < numberof (speed_cpu_frequency_table); i++)
    fprintf (stderr, "    - %s\n", speed_cpu_frequency_table[i].description);
  abort ();
}

void
speed_cycletime_need_cycles (void)
{
  speed_time_init ();
  if (speed_cycletime == 0.0)
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
