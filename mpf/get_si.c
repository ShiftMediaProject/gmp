/* mpf_get_si -- mpf to long conversion */

/*
Copyright 2001, 2002 Free Software Foundation, Inc.

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


/* Any fraction bits are truncated, meaning simply discarded.

   For values bigger than a long, the low bits are returned, like
   mpz_get_si, but this isn't documented.

   Notice this is equivalent to mpz_set_f + mpz_get_si.  */

long
mpf_get_si (mpf_srcptr f)
{
  mp_exp_t exp;
  mp_size_t size, abs_size;
  mp_ptr fp;
  mp_limb_t fl;

  size = SIZ (f);
  if (size == 0)
    return 0L;

  /* fraction alone truncates to zero */
  exp = EXP (f);
  if (exp <= 0)
    return 0L;

  /* exponent bigger than available data means low limb zero */
  abs_size = ABS (size);
  if (exp > abs_size)
    return 0L;

  fp = PTR(f);
  fl = fp[abs_size - exp];

#if GMP_NAIL_BITS != 0
  if (ULONG_MAX > GMP_NUMB_MAX != 0 && exp > 1)
    fl |= fp[abs_size - exp + 1] << GMP_NUMB_BITS;
#endif

  if (size > 0)
    return fl & LONG_MAX;
  else
    /* this form necessary to correctly handle -0x80..00 */
    return ~ ((fl - 1) & LONG_MAX);
}
