/* mpfr_get_ld -- convert a multiple precision floating-point number
                  to a machine long double

Copyright 2002, 2003 Free Software Foundation, Inc.

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
#include <float.h>

#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

#ifndef LDBL_MANT_DIG
#define LDBL_MANT_DIG 113 /* works also if long double == quad */
#endif

#ifndef DBL_MANT_DIG
#define DBL_MANT_DIG 53
#endif


/* Various i386 systems have been seen with incorrect LDBL constants in
   float.h (notes in set_ld.c), so force the value we know is right for IEEE
   extended.  */

#if HAVE_LDOUBLE_IEEE_EXT_LITTLE
#define MPFR_LDBL_MANT_DIG   64
#else
#define MPFR_LDBL_MANT_DIG   LDBL_MANT_DIG
#endif


long double
mpfr_get_ld (mpfr_srcptr x, mp_rnd_t rnd_mode)
{

  if (!mpfr_number_p (x)) /* NaN or Inf: check before 0.0 */
    {
      return (long double) mpfr_get_d (x, rnd_mode);
    }
  else if (MPFR_IS_ZERO(x))
    {
      return MPFR_SIGN(x) < 0 ? -0.0 : 0.0;
    }
  else /* now x is a normal non-zero number */
    {
      long double r; /* result */
      long double m;
      double s; /* part of result */
      mp_exp_t e; /* exponent of x */
      mp_exp_t sh; /* exponent shift, so that x/2^sh is in the double range */
      int negative;
      mpfr_t y, z;

      /* first round x to the target long double precision, so that
         all subsequent operations are exact (this avoids double rounding
         problems) */
      mpfr_init2 (y, MPFR_LDBL_MANT_DIG);
      mpfr_set (y, x, rnd_mode);
      negative = MPFR_SIGN(y) < 0;
      e = MPFR_EXP(y);
      if (e > 1023)
        {
          sh = e - 1023;
          MPFR_EXP(y) = 1023;
        }
      else if (e < -1021)
        {
          sh = e + 1021;
          MPFR_EXP(y) = -1021;
        }
      else
        {
          sh = 0;
        }
      /* now -1021 <= e - sh = EXP(y) <= 1023 */
      r = 0.0;
      mpfr_init2 (z, DBL_MANT_DIG);

      do
        {
          s = mpfr_get_d (y, GMP_RNDN); /* high part of y */
          r += (long double) s;
          mpfr_set_d (z, s, rnd_mode);  /* exact */
          mpfr_sub (y, y, z, rnd_mode); /* exact */
        }
      while (MPFR_NOTZERO(y));

      /* we now have to multiply back by 2^sh */
      if (sh != 0)
        {
          if (sh > 0)
            m = 2.0;
          else
            {
              m = 0.5;
              sh = -sh;
            }
          e = 1; /* invariant: m = 2^e */
          for (;;)
            {
              if (sh % 2)
                r = r * m;
              sh >>= 1;
              if (sh == 0)
                break;
              m = m * m;
              e = e + e;
            }
        }

      mpfr_clear (z);
      mpfr_clear (y);

      return r;
    }

}
