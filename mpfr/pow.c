/* mpfr_pow_ui, mpfr_ui_pow_ui -- compute the power of a floating-point
                                  number or machine integer

Copyright (C) 1999 Free Software Foundation.

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

#include <stdio.h>
#include "gmp.h"
#include "mpfr.h"
#include "mpfr-impl.h"

/* sets x to y^n, and returns ceil(log2(max ulp error)) */
int
#if __STDC__
mpfr_pow_ui (mpfr_ptr x, mpfr_srcptr y, unsigned long int n, mp_rnd_t rnd)
#else
mpfr_pow_ui (x, y, n, rnd)
     mpfr_ptr x;
     mpfr_srcptr y;
     unsigned long int n;
     mp_rnd_t rnd;
#endif
{
  long int i;
  unsigned long m;
  double err;
  mpfr_t ycopy;

  if (MPFR_IS_NAN(y)) { MPFR_SET_NAN(x); return 0; }

  MPFR_CLEAR_NAN(x);

  if (MPFR_IS_INF(y)) 
    {
      if (n == 0) { MPFR_SET_NAN(x); return 0; }
      /* Inf^n = Inf, (-Inf)^n = Inf for n even, -Inf for n odd */
      else if ((MPFR_SIGN(y) < 0) && (n % 2 == 1)) {
	if (MPFR_SIGN(x) > 0) MPFR_CHANGE_SIGN(x);
      }
      else if (MPFR_SIGN(x) < 0)
	MPFR_CHANGE_SIGN(x);
      MPFR_SET_INF(x); return 0; 
    }
  
  MPFR_CLEAR_INF(x);

  if (n==0) { mpfr_set_ui(x, 1, rnd); return 0; }

  if (x == y) {
    mpfr_init2 (ycopy, MPFR_PREC(y));
    mpfr_set (ycopy, y, GMP_RNDN); /* exact */
  }

  mpfr_set(x, y, rnd);
  err = 1.0;
  for (m=n, i=0; m; i++, m>>=1);
  /* now 2^(i-1) <= n < 2^i */
  for (i-=2; i>=0; i--) {
    mpfr_mul(x, x, x, rnd); err = 2.0*err+1.0;
    if (n & (1<<i)) {
      mpfr_mul(x, x, (x == y) ? ycopy : y, rnd);
      err += 1.0;
    }
  }
  
  if (x == y) mpfr_clear (ycopy);
  return _mpfr_ceil_log2 (err);
}

/* sets x to y^n, and returns ceil(log2(max ulp error)) */

/* TODO: gerer l'infini en cas de debordement ? Fait mecaniquement si fait 
   dans mul ? */

int
#if __STDC__
mpfr_ui_pow_ui (mpfr_ptr x, unsigned long int y, unsigned long int n,
		     mp_rnd_t rnd)
#else
mpfr_ui_pow_ui (x, y, n, rnd)
     mpfr_ptr x;
     unsigned long int y;
     unsigned long int n;
     mp_rnd_t rnd;
#endif
{
  long int i; double err;

  MPFR_CLEAR_FLAGS(x); 
  if (n==0) { mpfr_set_ui(x, 1, rnd); return 0; }

  mpfr_set_ui(x, y, rnd);
  err = 1.0;

  for (i=0;(1<<i)<=n;i++);
  /* now 2^(i-1) <= n < 2^i */
  for (i-=2; i>=0; i--) {
    mpfr_mul(x, x, x, rnd); err = 2.0 * err + 1.0;
    if (n & (1<<i)) {
      mpfr_mul_ui(x, x, y, rnd);
      err = err + 1.0;
    }
  }
  return _mpfr_ceil_log2 (err);
}
