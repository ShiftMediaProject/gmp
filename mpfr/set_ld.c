/* mpfr_set_ld -- convert a machine long double to
                  a multiple precision floating-point number

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

#include <float.h>

#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

/* Various i386 systems have been seen with float.h LDBL constants equal to
   the DBL ones, whereas they ought to be bigger, reflecting the 10-byte
   IEEE extended format on that processor.  gcc 3.2.1 on FreeBSD and Solaris
   has been seen with the problem, and gcc 2.95.4 on FreeBSD 4.7.  */

#if HAVE_LDOUBLE_IEEE_EXT_LITTLE
static const struct {
  char         bytes[10];
  long double  dummy;  /* for memory alignment */
} ldbl_max_struct = {
  { '\377','\377','\377','\377',
    '\377','\377','\377','\377',
    '\376','\177' }
};
#define MPFR_LDBL_MAX   (* (const long double *) ldbl_max_struct.bytes)
#else
#define MPFR_LDBL_MAX   LDBL_MAX
#endif

int
mpfr_set_ld (mpfr_ptr r, long double d, mp_rnd_t rnd_mode)
{
  mpfr_t t, u;
  int inexact, shift_exp = 0, inexact2 = 0;

  LONGDOUBLE_NAN_ACTION (d, goto nan);

  if (d > MPFR_LDBL_MAX)
    {
      mpfr_set_inf (r, 1);
      return 0;
    }

  if (d < -MPFR_LDBL_MAX)
    {
      mpfr_set_inf (r, -1);
      return 0;
    }

  if (d == 0.0)
    return mpfr_set_d (r, (double) d, rnd_mode);

  mpfr_init2 (t, MPFR_LDBL_MANT_DIG);
  mpfr_init2 (u, IEEE_DBL_MANT_DIG);
  mpfr_set_ui (t, 0, GMP_RNDN);
  while (d != 0.0)
    {
      if ((d > (long double) DBL_MAX) || ((-d) > (long double) DBL_MAX))
        { /* d is neither +Inf nor -Inf and u is Inf: this implies that an
             overflow occured, i.e. the exponent of d is too large for the
             double format */
          long double div9, div10, div11, div12, div13;

          div9 = (long double) (double) 1.34078079299425971e154; /* 2^(2^9) */
          div10 = div9 * div9;
          div11 = div10 * div10; /* 2^(2^11) */
          div12 = div11 * div11; /* 2^(2^12) */
          div13 = div12 * div12; /* 2^(2^13) */
          if (ABS(d) >= div13)
            {
              d = d / div13; /* exact */
              shift_exp += 8192;
            }
          if (ABS(d) >= div12)
            {
              d = d / div12; /* exact */
              shift_exp += 4096;
            }
          if (ABS(d) >= div11)
            {
              d = d / div11; /* exact */
              shift_exp += 2048;
            }
          if (ABS(d) >= div10)
            {
              d = d / div10; /* exact */
              shift_exp += 1024;
            }
          /* warning: we may have DBL_MAX=2^1024*(1-2^(-53)) < d < 2^1024,
             therefore we have one extra exponent reduction step */
          if (ABS(d) >= div9)
            {
              d = d / div9; /* exact */
              shift_exp += 512;
            }
          mpfr_set_ui (u, 0, GMP_RNDZ);
        }
      else
        {
          /* since -DBL_MAX <= d <= DBL_MAX, the cast to double should not
             overflow here */
          mpfr_set_d (u, (double) d, GMP_RNDN);
      /* warning: using MPFR_IS_ZERO will cause a READ_UNINIT_MEM if u=Inf */
      if (mpfr_cmp_ui (u, 0) == 0 && (d != (long double) 0.0)) /* underflow */
        {
          long double div10, div11, div12, div13;
          div10 = (long double) (double) 5.5626846462680034577255e-309; /* 2^(-2^10) */
          div11 = div10 * div10; /* 2^(-2^11) */
          div12 = div11 * div11; /* 2^(-2^12) */
          div13 = div12 * div12; /* 2^(-2^13) */
          if (ABS(d) <= div13)
            {
              d = d / div13; /* exact */
              shift_exp -= 8192;
            }
          if (ABS(d) <= div12)
            {
              d = d / div12; /* exact */
              shift_exp -= 4096;
            }
          if (ABS(d) <= div11)
            {
              d = d / div11; /* exact */
              shift_exp -= 2048;
            }
          if (ABS(d) <= div10)
            {
              d = d / div10; /* exact */
              shift_exp -= 1024;
            }
        }
        }
      mpfr_add (t, t, u, GMP_RNDN); /* exact */
      if (!mpfr_number_p (t))
        break;
      d = d - (long double) mpfr_get_d1 (u); /* exact */
    }
  /* now t is exactly the input value d */
  inexact = mpfr_set (r, t, rnd_mode);
  if (shift_exp > 0)
    inexact2 = mpfr_mul_2exp (r, r, shift_exp, rnd_mode);
  else if (shift_exp < 0)
    inexact2 = mpfr_div_2exp (r, r, -shift_exp, rnd_mode);
  if (inexact2) /* overflow */
    inexact = inexact2;
  mpfr_clear (t);
  mpfr_clear (u);

  return inexact;


 nan:
  MPFR_SET_NAN(r);
  MPFR_RET_NAN;
}
