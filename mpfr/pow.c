/* mpfr_pow_ui, mpfr_ui_pow_ui -- compute the power of a floating-point
                                  number or machine integer

Copyright (C) 1999 PolKA project, Inria Lorraine and Loria

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

/* sets x to y^n */
void 
#if __STDC__
mpfr_pow_ui (mpfr_ptr x, mpfr_srcptr y, unsigned int n, unsigned char rnd)
#else
mpfr_pow_ui (x, y, n, rnd)
     mpfr_ptr x;
     mpfr_srcptr y; 
     unsigned int n; 
     unsigned char rnd; 
#endif
{
  int i;
  
  if (n==0) { mpfr_set_ui(x, 1, rnd); return; }
  mpfr_set(x, y, rnd);
  for (i=0;(1<<i)<=n;i++);
  /* now 2^(i-1) <= n < 2^i */
  for (i-=2; i>=0; i--) {
    mpfr_mul(x, x, x, rnd);
    if (n & (1<<i)) mpfr_mul(x, x, y, rnd);
  }
  return;
}

/* sets x to y^n */
void mpfr_ui_pow_ui (mpfr_ptr x, unsigned int y, unsigned int n, 
		     unsigned char rnd)
{
  int i;

  if (n==0) { mpfr_set_ui(x, 1, rnd); return; }
  mpfr_set_ui(x, y, rnd);
  for (i=0;(1<<i)<=n;i++);
  /* now 2^(i-1) <= n < 2^i */
  for (i-=2; i>=0; i--) {
    mpfr_mul(x, x, x, rnd);
    if (n & (1<<i)) mpfr_mul_ui(x, x, y, rnd);
  }
  return;
}
