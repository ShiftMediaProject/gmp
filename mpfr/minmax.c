/* mpfr_min -- min and max of x, y  

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

 /* The computation of z=min(x,y)

    z=x if x <= y
    z=y if x > y
 */

int
mpfr_min (mpfr_ptr z, mpfr_srcptr x, mpfr_srcptr y, mp_rnd_t rnd_mode)
{
  if (MPFR_IS_NAN(x) && MPFR_IS_NAN(y) )
    {  
      MPFR_SET_NAN(z); 
      MPFR_RET_NAN;
    }
  MPFR_CLEAR_NAN(z);

  if (MPFR_IS_NAN(x))
    return mpfr_set(z, y, rnd_mode);

  if (MPFR_IS_NAN(y))
    return mpfr_set(z, x, rnd_mode);

  if (MPFR_IS_FP(x) && MPFR_IS_ZERO(x) && MPFR_IS_FP(y) && MPFR_IS_ZERO(y))
    {
      if (MPFR_SIGN(x) < 0)
        return mpfr_set(z, x, rnd_mode);
      else
        return mpfr_set(z, y, rnd_mode);
    }

  if (mpfr_cmp(x,y) <= 0)
    return mpfr_set(z, x, rnd_mode);
  else
    return mpfr_set(z, y, rnd_mode);
}

 /* The computation of z=max(x,y)

    z=x if x >= y
    z=y if x < y
 */

int
mpfr_max (mpfr_ptr z, mpfr_srcptr x, mpfr_srcptr y, mp_rnd_t rnd_mode)
{
  if (MPFR_IS_NAN(x) && MPFR_IS_NAN(y) ) 
    {  
      MPFR_SET_NAN(z);
      MPFR_RET_NAN;
    }
  MPFR_CLEAR_NAN(z);

  if (MPFR_IS_NAN(x))
    return mpfr_set(z, y, rnd_mode);

  if (MPFR_IS_NAN(y))
    return mpfr_set(z, x, rnd_mode);

  if (MPFR_IS_FP(x) && MPFR_IS_ZERO(x) && MPFR_IS_FP(y) && MPFR_IS_ZERO(y))
    {
      if (MPFR_SIGN(x) < 0)
        return mpfr_set(z, y, rnd_mode);
      else
        return mpfr_set(z, x, rnd_mode);
    }

  if (mpfr_cmp(x,y) <= 0)
    return mpfr_set(z, y, rnd_mode);
  else
    return mpfr_set(z, x, rnd_mode);
}
