/* mpfr_dump -- Dump a float to stdout.

Copyright 1999, 2001 Free Software Foundation, Inc.
(Copied from the GNU MP Library.)

This file is part of the MPFR Library.

The MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <stdio.h>
#include <string.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

void
mpfr_dump (mpfr_srcptr u, mp_rnd_t rnd_mode)
{
  mp_exp_t exp;
  char *str;

  if (MPFR_IS_NAN(u)) 
    {
      printf ("NaN\n"); 
      return; 
    }

  if (MPFR_IS_INF(u)) 
    {
      if (MPFR_SIGN(u) == 1)
        printf ("Inf\n");
      else
        printf ("-Inf\n"); 
      return; 
    }

  str = mpfr_get_str (NULL, &exp, 10, 0, u, rnd_mode);
  if (str[0] == '-')
    printf ("-0.%se%ld\n", str + 1, exp);
  else
    printf ("0.%se%ld\n", str, exp);
  (*__gmp_free_func) (str, strlen(str)+1);
}
