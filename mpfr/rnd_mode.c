/* mpfr_set_machine_rnd_mode -- set the rounding mode for machine floats

Copyright 1999, 2001, 2002 Free Software Foundation, Inc.

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
along with the MPFR Library; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#ifdef MPFR_HAVE_FESETROUND
#include <stdio.h>
#include <stdlib.h>
#include <fenv.h>
#include "gmp.h"
#include "mpfr.h"

/* sets the machine rounding mode to the value rnd_mode */
void 
mpfr_set_machine_rnd_mode (mp_rnd_t rnd_mode)
{
  switch (rnd_mode) {
  case GMP_RNDN: fesetround(FE_TONEAREST); break;
  case GMP_RNDZ: fesetround(FE_TOWARDZERO); break;
  case GMP_RNDU: fesetround(FE_UPWARD); break;
  case GMP_RNDD: fesetround(FE_DOWNWARD); break;
  default: fprintf(stderr, "invalid rounding mode\n"); exit(1);
  }
}
#endif
