/* mpfr_print_binary -- print the internal binary representation of a
                     floating-point number

Copyright 1999, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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
#include <limits.h>

#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

void
mpfr_print_binary (mpfr_srcptr x)
{
  if (MPFR_IS_NAN (x))
    {
      printf ("@NaN@");
      return;
    }

  if (MPFR_SIGN (x) < 0)
    printf ("-");

  if (MPFR_IS_INF (x))
    printf ("@Inf@");
  else if (MPFR_IS_ZERO (x))
    printf ("0");
  else
    {
      mp_limb_t *mx;
      mp_prec_t px;
      mp_size_t n;

      mx = MPFR_MANT (x);
      px = MPFR_PREC (x);

      printf ("0.");
      for (n = (px - 1) / BITS_PER_MP_LIMB; ; n--)
        {
          mp_limb_t wd, t;

          MPFR_ASSERTN (n >= 0);
          wd = mx[n];
          for (t = MPFR_LIMB_HIGHBIT; t != 0; t >>= 1)
            {
              printf ((wd & t) == 0 ? "0" : "1");
              if (--px == 0)
                {
                  mp_exp_t ex;

                  ex = MPFR_GET_EXP (x);
                  MPFR_ASSERTN (ex >= LONG_MIN && ex <= LONG_MAX);
                  printf ("E%ld", (long) ex);
                  return;
                }
            }
        }
    }
}
