/* mpz_set_fr -- set a multiple-precision integer from a floating-point number

Copyright (C) 2000 Free Software Foundation.

This file is part of the MPFR Library.

The MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include "gmp.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#include "gmp-impl.h"
#include "longlong.h"

/* puts the mantissa of f into z, and returns 'exp' such that f = z * 2^exp
 */
mp_exp_t
#if __STDC__
mpz_set_fr (mpz_ptr z, mpfr_srcptr f)
#else
mpz_set_fr (z, f) 
     mpz_ptr z;
     mpfr_srcptr f;
#endif
{
  int fn, sh;

  /* TODO: semantique de INF, NAN dans ce contexte ; lever une FPE ? */

  fn = 1 + (MPFR_PREC(f)-1)/BITS_PER_MP_LIMB;

  /* check whether allocated space for z is enough */
  if (ALLOC(z) < fn) MPZ_REALLOC(z,fn);

  sh = fn*BITS_PER_MP_LIMB - MPFR_PREC(f);
  if (sh) mpn_rshift(PTR(z), MPFR_MANT(f), fn, sh);
  else MPN_COPY(PTR(z), MPFR_MANT(f), fn);

  SIZ(z) = fn;

  return MPFR_EXP(f)-MPFR_PREC(f);
}
