/* double mpf_get_d_2exp (signed long int *exp, mpf_t src).

Copyright 2001, 2002, 2003 Free Software Foundation, Inc.

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
MA 02111-1307, USA. */

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"


double
mpf_get_d_2exp (signed long int *exp2, mpf_srcptr src)
{
  double res;
  mp_size_t size, abs_size;
  mp_srcptr ptr;
  int cnt;
  long exp;

  size = SIZ(src);
  if (UNLIKELY (size == 0))
    {
      *exp2 = 0;
      return 0.0;
    }

  ptr = PTR(src);
  abs_size = ABS (size);
  count_leading_zeros (cnt, ptr[abs_size - 1]);
  cnt -= GMP_NAIL_BITS;

  res = mpn_get_d (ptr, abs_size, (mp_size_t) 0,
                   (long) - (abs_size * GMP_NUMB_BITS - cnt));

  exp = EXP(src) * GMP_NUMB_BITS - cnt;

  /* gcc on m68k and x86 holds floats in the coprocessor, which may mean
     "res" has extra precision.  Force it through memory to ensure any
     rounding takes place now and won't become 1.0 in the caller.  */
#if (HAVE_HOST_CPU_FAMILY_m68k || HAVE_HOST_CPU_FAMILY_x86)     \
  && defined (__GNUC__)
  asm ("" : "=m" (res) : "0" (res));
#endif

  /* if hardware floats are in round upwards mode then res might be 1.0 */
  if (UNLIKELY (res >= 1.0))
    {
      res *= 0.5;
      exp++;
    }

  ASSERT (res >= 0.5);
  ASSERT (res < 1.0);

  *exp2 = exp;
  return (size >= 0 ? res : -res);
}
