/* mpfr_set_d -- convert a machine double precision float to
                 a multiple precision floating-point number

Copyright 1999, 2000, 2001, 2002 Free Software Foundation, Inc.

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

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#include "mpfr.h"
#include "mpfr-impl.h"

#if (BITS_PER_MP_LIMB==32)
#define MPFR_LIMBS_PER_DOUBLE 2
#elif (BITS_PER_MP_LIMB >= 64)
#define MPFR_LIMBS_PER_DOUBLE 1
#elif (BITS_PER_MP_LIMB == 16)
#define MPFR_LIMBS_PER_DOUBLE 4
#endif

static int __mpfr_extract_double _PROTO ((mp_ptr, double));

/* Included from gmp-2.0.2, patched to support denorms */

#ifdef XDEBUG
#undef _GMP_IEEE_FLOATS
#endif

#ifndef _GMP_IEEE_FLOATS
#define _GMP_IEEE_FLOATS 0
#endif

static int
__mpfr_extract_double (mp_ptr rp, double d)
     /* e=0 iff BITS_PER_MP_LIMB=32 and rp has only one limb */
{
  long exp;
  mp_limb_t manl;
#if BITS_PER_MP_LIMB == 32
  mp_limb_t manh;
#endif

  /* BUGS

     1. Should handle Inf and NaN in IEEE specific code.
     2. Handle Inf and NaN also in default code, to avoid hangs.
     3. Generalize to handle all BITS_PER_MP_LIMB >= 32.
     4. This lits is incomplete and misspelled.
   */

  if (d == 0.0)
    {
      rp[0] = 0;
      return 0;
    }

#if _GMP_IEEE_FLOATS
  {
    union ieee_double_extract x;
    x.d = d;

    exp = x.s.exp;
    if (exp)
      {
#if BITS_PER_MP_LIMB == 64
	manl = ((MP_LIMB_T_ONE << 63)
		| ((mp_limb_t) x.s.manh << 43) | ((mp_limb_t) x.s.manl << 11));
#else
	manh = (MP_LIMB_T_ONE << 31) | (x.s.manh << 11) | (x.s.manl >> 21);
	manl = x.s.manl << 11;
#endif
      }
    else /* denormalized number */
      {
#if BITS_PER_MP_LIMB == 64
	manl = ((mp_limb_t) x.s.manh << 43) | ((mp_limb_t) x.s.manl << 11);
#else
        manh = (x.s.manh << 11) /* high 21 bits */
          | (x.s.manl >> 21); /* middle 11 bits */
	manl = x.s.manl << 11; /* low 21 bits */
#endif
      }
  }
#else
  {
    /* Unknown (or known to be non-IEEE) double format.  */
    exp = 0;
    if (d >= 1.0)
      {
        if (d * 0.5 == d)
          abort ();

        while (d >= 32768.0)
          {
            d *= (1.0 / 65536.0);
            exp += 16;
          }
        while (d >= 1.0)
          {
            d *= 0.5;
            exp += 1;
          }
      }
    else if (d < 0.5)
      {
        while (d < (1.0 / 65536.0))
          {
            d *=  65536.0;
            exp -= 16;
          }
        while (d < 0.5)
          {
            d *= 2.0;
            exp -= 1;
          }
      }

    d *= MP_BASE_AS_DOUBLE;
#if BITS_PER_MP_LIMB == 64
    manl = d;
#else
    manh = d;
    manl = (d - manh) * MP_BASE_AS_DOUBLE;
#endif

    exp += 1022;
  }
#endif

  if (exp) exp = (unsigned) exp - 1022; else exp = -1021;

#if BITS_PER_MP_LIMB == 64
  rp[0] = manl;
#else
  rp[1] = manh;
  rp[0] = manl;
#endif

  return exp;
}

/* End of part included from gmp-2.0.2 */

int
mpfr_set_d (mpfr_ptr r, double d, mp_rnd_t rnd_mode)
{
  int signd, sizetmp, inexact;
  unsigned int cnt, k;
  mpfr_ptr tmp;
  TMP_DECL(marker);

  TMP_MARK(marker);
  MPFR_CLEAR_FLAGS(r);

  if (d == 0)
    {
      union ieee_double_extract x;

      MPFR_SET_ZERO(r);
      /* set correct sign */
      x.d = d;
      if (((x.s.sig == 1) && (MPFR_SIGN(r) > 0))
	  || ((x.s.sig == 0) && (MPFR_SIGN(r) < 0)))
	MPFR_CHANGE_SIGN(r);
      return 0; /* 0 is exact */
    }
  else if (DOUBLE_ISNAN(d))
    {
      MPFR_SET_NAN(r);
      MPFR_RET_NAN;
    }
  else if (DOUBLE_ISINF(d))
    {
      MPFR_SET_INF(r);
      if ((d > 0 && (MPFR_SIGN(r) == -1)) || (d < 0 && (MPFR_SIGN(r) == 1)))
	MPFR_CHANGE_SIGN(r);
      return 0; /* infinity is exact */
    }

  /* warning: don't use tmp=r here, even if SIZE(r) >= MPFR_LIMBS_PER_DOUBLE,
     since PREC(r) may be different from PREC(tmp), and then both variables
     would have same precision in the mpfr_set4 call below. */
  tmp = (mpfr_ptr) TMP_ALLOC(sizeof(mpfr_t));
  MPFR_MANT(tmp) = TMP_ALLOC(MPFR_LIMBS_PER_DOUBLE * BYTES_PER_MP_LIMB);
  MPFR_PREC(tmp) = 53;
  MPFR_SIZE(tmp) = MPFR_LIMBS_PER_DOUBLE;
  sizetmp = MPFR_LIMBS_PER_DOUBLE;

  signd = (d < 0) ? -1 : 1;
  d = ABS (d);

  MPFR_EXP(tmp) = __mpfr_extract_double (MPFR_MANT(tmp), d);

  /* determine number k of zero high limbs */
  for (k = 0; k < sizetmp && MPFR_MANT(tmp)[sizetmp - 1 - k] == 0; k++);

  count_leading_zeros (cnt, MPFR_MANT(tmp)[sizetmp - 1 - k]);

  if (cnt)
    mpn_lshift (MPFR_MANT(tmp) + k, MPFR_MANT(tmp), sizetmp - k, cnt);
  else if (k)
    MPN_COPY (MPFR_MANT(tmp) + k, MPFR_MANT(tmp), sizetmp - k);
  if (k)
    MPN_ZERO (MPFR_MANT(tmp), k);

  MPFR_EXP(tmp) -= cnt + k * BITS_PER_MP_LIMB;

  /* tmp is exact since PREC(tmp)=53 */
  inexact = mpfr_set4 (r, tmp, rnd_mode, signd);

  TMP_FREE(marker);
  return inexact;
}
