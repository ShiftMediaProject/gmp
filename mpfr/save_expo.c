/* Save/restore the minimum and maximum exponents.

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

#include <stdio.h>
#include <stdlib.h>
#include "gmp.h"
#include "mpfr.h"
#include "mpfr-impl.h"

static unsigned int saved_flags;
static mp_exp_t saved_emin;
static mp_exp_t saved_emax;
static unsigned int save_ctr = 0;

void
mpfr_save_emin_emax (void)
{
  if (save_ctr++ == 0)
    {
      saved_flags = __mpfr_flags;
      saved_emin = __mpfr_emin;
      saved_emax = __mpfr_emax;
      __mpfr_emin = MPFR_EMIN_MIN;
      __mpfr_emax = MPFR_EMAX_MAX;
    }
  else if (save_ctr == 0)
    {
      fprintf(stderr,
              "Error: Too many consecutive calls to mpfr_save_emin_emax!\n"
              "Probably a bug.\n");
      exit(1);
    }
}

void
mpfr_restore_emin_emax (void)
{
  if (--save_ctr == 0)
    {
      __mpfr_flags = saved_flags;
      __mpfr_emin = saved_emin;
      __mpfr_emax = saved_emax;
    }
}
