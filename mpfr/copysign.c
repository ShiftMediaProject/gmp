/* mpfr_copysign -- Produce a value with the magnitude of x and sign of y

Copyright (C) 2001 Free Software Foundation.

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
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

 /* The computation of z with magnitude of x and sign of y 

    z = sign(y) * abs(x)
 */

int
mpfr_copysign (mpfr_ptr z, mpfr_srcptr x ,mpfr_srcptr y , mp_rnd_t rnd_mode) 
{
    
    if (MPFR_IS_NAN(y)) 
    {  
      MPFR_SET_NAN(z);
      return 1; 
    }

    if (MPFR_IS_NAN(x)) 
    {  
      MPFR_SET_NAN(z);
      MPFR_SET_SAME_SIGN(z,y);
      return 1; 
    }

    MPFR_CLEAR_NAN(z);

    if (MPFR_IS_INF(x)) {  
    
      MPFR_SET_INF(z);
      MPFR_SET_SAME_SIGN(z,y);
      return 0; 
    }

    MPFR_CLEAR_INF(z);
    
    if (!MPFR_NOTZERO(x)) 
    {  
      MPFR_SET_ZERO(z);
      MPFR_SET_SAME_SIGN(z,y);      
      return 0; 
    }
    
    /* GENERAL CASE*/
    {
      int inexact=0;
      inexact =mpfr_set(z,x,rnd_mode);
      MPFR_SET_SAME_SIGN(z,y);
      return inexact; 
    }

}
