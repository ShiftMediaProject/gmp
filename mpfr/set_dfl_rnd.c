/* mpfr_set_default_rounding_mode -- set the default rounding mode

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

#include "gmp.h"
#include "gmp-impl.h"

char __gmp_default_rounding_mode = 0;

void
#if __STDC__
mpfr_set_default_rounding_mode (char rnd_mode)
#else
mpfr_set_default_rounding_mode (rnd_mode)
     char rnd_mode;
#endif
{
  __gmp_default_rounding_mode = rnd_mode;
}

