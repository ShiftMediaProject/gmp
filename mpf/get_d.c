/* double mpf_get_d (mpf_t src) -- Return the double approximation to SRC.

Copyright 1996, 2001, 2002 Free Software Foundation, Inc.

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

/* HPPA 8000, 8200, 8500, and 8600 traps FCNV,UDW,DBL for values >= 2^63.  This
   makes it slow.  Worse, the Linux kernel apparently uses untested code in its
   trap handling routines, and gets the sign wrong.  Their compiler port
   doesn't define __hppa as it should.  Here is a workaround:  */
#if (defined (__hppa) || defined (__hppa__)) && GMP_LIMB_BITS == 64
#define limb2dbl(limb) \
    ((limb) >> (GMP_LIMB_BITS - 1) != 0					\
     ? 2.0 * (double) (mp_limb_signed_t) (((limb) >> 1) | ((limb) & 1))	\
     : (double) (mp_limb_signed_t) (limb))
#else
#define limb2dbl(limb) \
    (double) (limb)
#endif

double
mpf_get_d (mpf_srcptr src)
{
  double res;
  mp_size_t size, i, n_limbs_to_use;
  int negative;
  mp_ptr qp;

  size = SIZ(src);
  if (size == 0)
    return 0.0;

  negative = size < 0;
  size = ABS (size);
  qp = PTR(src);

  res = limb2dbl (qp[size - 1]);
  n_limbs_to_use = MIN (LIMBS_PER_DOUBLE, size);
  for (i = 2; i <= n_limbs_to_use; i++)
    res = res * MP_BASE_AS_DOUBLE + limb2dbl (qp[size - i]);

  res = __gmp_scale2 (res, (EXP(src) - n_limbs_to_use) * GMP_NUMB_BITS);

  return negative ? -res : res;
}
