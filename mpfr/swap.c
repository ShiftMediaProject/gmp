/* mpfr_swap (U, V) -- Swap U and V.

Copyright 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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

#include <string.h>
#include "gmp.h"
#include "mpfr.h"

/*
 * We now use memcpy instead of copying the structure field by field.
 * Thus we avoid copying values that may have trap representations,
 * and if we change the structure in the future, this will still be
 * valid. Also, there should not be any noticeable speed change in
 * practice.
 */

void
mpfr_swap (mpfr_ptr u, mpfr_ptr v)
{
  mpfr_t tmp;

  memcpy (tmp, u, sizeof (mpfr_t));
  memcpy (u, v, sizeof (mpfr_t));
  memcpy (v, tmp, sizeof (mpfr_t));
}
