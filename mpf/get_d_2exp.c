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
  mp_size_t size, i, n_limbs_to_use;
  int negative;
  mp_ptr qp;
  int cnt;
  long exp;

  size = SIZ(src);
  if (size == 0)
    {
      *exp2 = 0;
      return 0.0;
    }

  negative = size < 0;
  size = ABS (size);
  qp = PTR(src);

  n_limbs_to_use = MIN (LIMBS_PER_DOUBLE, size);
  qp += size - n_limbs_to_use;
  res = qp[0] / MP_BASE_AS_DOUBLE;
  for (i = 1; i < n_limbs_to_use; i++)
    res = (res + qp[i]) / MP_BASE_AS_DOUBLE;
  count_leading_zeros (cnt, qp[n_limbs_to_use - 1]);
  exp = EXP(src) * GMP_NUMB_BITS - cnt + GMP_NAIL_BITS;
  res = res * ((mp_limb_t) 1 << cnt);

  /* gcc on m68k and x86 holds floats in the coprocessor, which may mean
     "res" has extra precision.  Force it through memory to ensure any
     rounding takes place now and won't become 1.0 in the caller.  */
#if (HAVE_HOST_CPU_FAMILY_m68k || HAVE_HOST_CPU_FAMILY_x86)     \
  && defined (__GNUC__)
  asm ("" : "=m" (res) : "0" (res));
#endif

  /* if hardware floats are in round upwards mode then res may be 1.0 */
  if (UNLIKELY (res >= 1.0))
    {
      res *= 0.5;
      exp++;
    }

  ASSERT (res >= 0.5);
  ASSERT (res < 1.0);

  *exp2 = exp;
  return negative ? -res : res;
}
