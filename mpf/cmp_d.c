/* mpf_cmp_d -- compare mpf and double.

Copyright 2001 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA.
*/

#include "gmp.h"
#include "gmp-impl.h"

int
mpf_cmp_d (mpf_srcptr f, double d)
{
  mp_limb_t  darray[LIMBS_PER_DOUBLE];
  mpf_t      df;

  if (d == 0.0)
    return SIZ(f);

  PTR(df) = darray;
  SIZ(df) = (d >= 0.0 ? LIMBS_PER_DOUBLE : -LIMBS_PER_DOUBLE);
  EXP(df) = __gmp_extract_double (darray, ABS(d));

  return mpf_cmp (f, df);
}
