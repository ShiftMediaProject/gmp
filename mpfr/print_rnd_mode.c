/* mpfr_print_rnd_mode -- convert a given rounding mode to a string

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
#include "gmp-impl.h"
#include "mpfr.h"

char *
#if __STDC__
mpfr_print_rnd_mode(unsigned char rnd_mode)
#else
mpfr_print_rnd_mode(rnd_mode)
     unsigned char rnd_mode; 
#endif
{
  switch (rnd_mode)
    {
    case GMP_RNDD: return("GMP_RNDD"); 
    case GMP_RNDU: return("GMP_RNDU"); 
    case GMP_RNDN: return("GMP_RNDN"); 
    case GMP_RNDZ: return("GMP_RNDZ"); 
    default: return("unknown rounding mode"); 
    }
}
