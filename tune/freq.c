/* CPU frequency determination. */

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


#include <stdio.h>
#include <stdlib.h> /* for getenv */
#if HAVE_UNISTD_H
#include <unistd.h> /* for sysconf */
#endif

#include <sys/types.h>
#if HAVE_SYS_SYSCTL_H
#include <sys/sysctl.h>  /* for sysctlbyname() */
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
    printf ("Using GMP_CPU_FREQUENCY %.0f for cycle time %.3g\n",
            atof (e), speed_cycletime);

  return 1;
}


/* sysctlbyname() for BSD flavours.

   On FreeBSD 3.3 the headers have #defines like CPU_WALLCLOCK under
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
  if (sysctlbyname ("machdep.tsc_freq", &val, &valsize, NULL, 0) == 0
      && valsize == sizeof(val))
    {
      if (speed_option_verbose)
        printf ("Using sysctlbyname() machdep.tsc_freq");
      goto success;
    }

  valsize = sizeof(val);
  if (sysctlbyname ("machdep.i586_freq", &val, &valsize, NULL, 0) == 0
      && valsize == sizeof(val))
    {
      if (speed_option_verbose)
        printf ("Using sysctlbyname() machdep.i586_freq");
      goto success;
    }
  return 0;

 done:
  speed_cycletime = 1.0 / (double) val;

  if (speed_option_verbose)
    printf (" %u for cycle time %.3g\n", val, speed_cycletime);

  return 1;
}
#endif


/* /proc/cpuinfo for linux kernel.

   Linux doesn't seem to have any system call to get the CPU frequency, at
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
          if (sscanf (buf, "bogomips : %lf\n", &val) == 1)
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


/* processor_info() for Solaris.  "psrinfo" is the command-line interface to
   this.  "prtconf -vp" gives similar information.  */

#if HAVE_PROCESSOR_INFO
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

  if (speed_option_verbose)
    printf ("Using processor_info() %d mhz for cycle time %.3g\n",
            mhz, speed_cycletime);

  return 1;
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
  fprintf (stderr, "but none of the following available,\n");
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
