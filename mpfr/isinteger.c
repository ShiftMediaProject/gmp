/* mpfr_isinteger -- test if a mpfr variable is integer

Copyright 2001 Free Software Foundation, Inc.

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
#include "mpfr.h"
#include "mpfr-impl.h"

int 
mpfr_isinteger (mpfr_srcptr x)
{

  mpfr_t u;
  int expo;
  mp_prec_t prec;
  int result;

  expo=(int)MPFR_EXP(x);
  prec=MPFR_PREC(x);

  if (expo<=0) 
    return 0;

  if (expo>=prec) 
    return 1;

  mpfr_init2(u,prec);
  mpfr_trunc(u,x);

  result = (mpfr_cmp (x,u) == 0);

  mpfr_clear (u);
  return result;
}
