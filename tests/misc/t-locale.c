/* Test locale support, or attempt to do so.

Copyright 2001 Free Software Foundation, Inc.

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
#include <stdlib.h>
#include <string.h>

#if HAVE_LOCALE_H
#include <locale.h>    /* for lconv */
#endif

#include "gmp.h"
#include "gmp-impl.h"
#include "tests.h"


#if HAVE_LOCALECONV

char *decimal_point;

/* Replace the libc localeconv with one we can manipulate. */
struct lconv *
localeconv (void)
{
  static struct lconv  l;
  l.decimal_point = decimal_point;
  return &l;
}


void
check_input (void)
{
  static char *point[] = {
    ".", ",", "xy", "xyz", "xyz***"
  };

  static const struct {
    const char  *str;
    double      d;
  } data[] = {

    { "1%s",   1.0 },
    { "1%s0",  1.0 },
    { "1%s00", 1.0 },

    { "%s5",    0.5 },
    { "0%s5",   0.5 },
    { "00%s5",  0.5 },
    { "00%s50", 0.5 },

    { "1%s5",    1.5 },
    { "1%s5e1", 15.0 },
  };

  int     i, j, neg;
  char    str[128];
  mpf_t   f;
  double  d;

  mpf_init (f);

  for (i = 0; i < numberof (point); i++)
    {
      decimal_point = point[i];

      for (neg = 0; neg <= 1; neg++)
        {
          for (j = 0; j < numberof (data); j++)
            {
              strcpy (str, neg ? "-" : "");
              sprintf (str+strlen(str), data[j].str, decimal_point);

              d = data[j].d;
              if (neg)
                d = -d;

              mpf_set_d (f, 123.0);
              if (mpf_set_str (f, str, 10) != 0)
                {
                  printf ("mpf_set_str error\n");
                  printf ("  point  %s\n", decimal_point);
                  printf ("  str    %s\n", str);
                  abort ();
                }
              if (mpf_cmp_d (f, d) != 0)
                {
                  printf    ("mpf_set_str wrong result\n");
                  printf    ("  point  %s\n", decimal_point);
                  printf    ("  str    %s\n", str);
                  mpf_trace ("  f", f);
                  printf    ("  d=%g\n", d);
                  abort ();
                }

              mpf_set_d (f, 123.0);
              if (gmp_sscanf (str, "%Ff", f) != 1)
                {
                  printf ("gmp_sscanf wrong return value\n");
                  printf ("  point  %s\n", decimal_point);
                  printf ("  str    %s\n", str);
                  abort ();
                }
              if (mpf_cmp_d (f, d) != 0)
                {
                  printf    ("gmp_sscanf wrong result\n");
                  printf    ("  point  %s\n", decimal_point);
                  printf    ("  str    %s\n", str);
                  mpf_trace ("  f", f);
                  printf    ("  d=%g\n", d);
                  abort ();
                }
            }
        }
    }
  mpf_clear (f);
}

int
main (void)
{
  /* The localeconv replacement breaks printf "%lu" on SunOS 4, so we can't
     print the seed in tests_rand_start().  Nothing random is used in this
     program though, so just use the memory tests alone.  */
  tests_memory_start ();

  {
    mpf_t  f;
    char   buf[128];
    mpf_init (f);
    decimal_point = ",";
    mpf_set_d (f, 1.5);
    gmp_snprintf (buf, sizeof(buf), "%.1Ff", f);
    mpf_clear (f);
    if (strcmp (buf, "1,5") != 0)
      {
        printf ("Test skipped, replacing localeconv doesn't work\n");
        goto done;
      }
  }

  check_input ();

 done:
  tests_memory_end ();
  exit (0);
}


#else

int
main (void)
{
  printf ("Test skipped, no locale support\n");
  exit (0);
}

#endif
