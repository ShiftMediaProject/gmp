/* Exercise mpz_get_si.

Copyright 2000, 2001 Free Software Foundation, Inc.

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

#include <stdio.h>
#include <stdlib.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "tests.h"


void
check_data (void)
{
  static const struct {
    int         base;
    const char  *f;
    long        want;
  } data[] = {
    { 10, "0",      0L },
    { 10, "1",      1L },
    { 10, "-1",     -1L },
    { 10, "2",      2L },
    { 10, "-2",     -2L },
    { 10, "12345",  12345L },
    { 10, "-12345", -12345L },

    /* fraction bits ignored */
    { 10, "0.5",    0L },
    { 10, "-0.5",   0L },
    { 10, "1.1",    1L },
    { 10, "-1.1",   -1L },
    { 10, "1.9",    1L },
    { 10, "-1.9",   -1L },
    { 16, "1.000000000000000000000000000000000000000000000000001", 1L },
    { 16, "-1.000000000000000000000000000000000000000000000000001", -1L },

    /* low bits extracted (this is undocumented) */
    { 16, "1000000000000000000000000000000000000000000000000001", 1L },
    { 16, "-1000000000000000000000000000000000000000000000000001", -1L },
  };

  int    i;
  mpf_t  f;
  long   got;

  mpf_init2 (f, 2000L);
  for (i = 0; i < numberof (data); i++)
    {
      mpf_set_str_or_abort (f, data[i].f, data[i].base);

      got = mpf_get_si (f);
      if (got != data[i].want)
        {
          printf ("mpf_get_si wrong at data[%d]\n", i); 
          printf ("   f     \"%s\"\n", data[i].f);
          printf ("     dec "); mpf_out_str (stdout, 10, 0, f); printf ("\n");
          printf ("     hex "); mpf_out_str (stdout, 16, 0, f); printf ("\n");
          printf ("   got   %ld (0x%lX)\n", got, got);
          printf ("   want  %ld (0x%lX)\n", data[i].want, data[i].want);
          abort();                                    
        }
    }
  mpf_clear (f);
}


void
check_max (void)
{
  mpf_t  f;
  long   want;
  long   got;

  mpf_init2 (f, 200L);

#define CHECK_MAX(name)                                         \
  if (got != want)                                              \
    {                                                           \
      printf ("mpf_get_si wrong on %s\n", name);                \
      printf ("   f    ");                                      \
      mpf_out_str (stdout, 10, 0, f); printf (", hex ");        \
      mpf_out_str (stdout, 16, 0, f); printf ("\n");            \
      printf ("   got  %ld, hex %lX\n", got, got);              \
      printf ("   want %ld, hex %lX\n", want, want);            \
      abort();                                                  \
    }

  want = LONG_MAX;
  mpf_set_si (f, want);
  got = mpf_get_si (f);
  CHECK_MAX ("LONG_MAX");

  want = LONG_MIN;
  mpf_set_si (f, want);
  got = mpf_get_si (f);
  CHECK_MAX ("LONG_MIN");

  mpf_clear (f);
}


int
main (void)
{
  tests_start ();
  check_data ();
  check_max ();
  tests_end ();
  exit (0);
}
