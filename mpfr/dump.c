/* mpfr_dump -- Dump a float to stdout.

   THIS IS AN INTERNAL FUNCTION WITH A MUTABLE INTERFACE.  IT IS NOT SAFE TO
   CALL THIS FUNCTION DIRECTLY.  IN FACT, IT IS ALMOST GUARANTEED THAT THIS
   FUNCTION WILL CHANGE OR DISAPPEAR IN A FUTURE GNU MP RELEASE.


Copyright (C) 1993, 1994, 1995, 2000 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <stdio.h>
#include <strings.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

void
#if __STDC__
mpfr_dump (mpfr_srcptr u, mp_rnd_t rnd_mode)
#else
mpfr_dump (u, rnd_mode)
     mpfr_srcptr u;
     mp_rnd_t rnd_mode; 
#endif
{
  mp_exp_t exp;
  char *str;

  if (MPFR_IS_INF(u)) 
    {
      if (MPFR_SIGN(u) == 1) printf("Inf\n"); else printf("-Inf\n"); 
      return; 
    }
  if (MPFR_IS_NAN(u)) 
    {
      printf("NaN\n"); 
      return; 
    }

  str = mpfr_get_str (NULL, &exp, 10, 0, u, rnd_mode);
  if (str[0] == '-')
    printf ("-0.%se%ld\n", str + 1, exp);
  else
    printf ("0.%se%ld\n", str, exp);
  (*__gmp_free_func) (str, strlen(str)+1);
}
