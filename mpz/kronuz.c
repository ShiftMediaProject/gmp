/* mpz_ui_kronecker -- ulong+mpz Kronecker/Jacobi symbol.

Copyright 1999, 2000, 2001, 2002 Free Software Foundation, Inc.

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


/* This implementation depends on BITS_PER_MP_LIMB being even, so that
   (a/2)^BITS_PER_MP_LIMB = 1 and so there's no need to pay attention to how
   many low zero limbs are stripped.  */
#if BITS_PER_MP_LIMB % 2 != 0
Error, error, unsupported BITS_PER_MP_LIMB
#endif


int
mpz_ui_kronecker (unsigned long a, mpz_srcptr b)
{
  mp_srcptr  b_ptr = PTR(b);
  mp_limb_t  b_low = b_ptr[0];
  mp_size_t  b_abs_size = ABSIZ (b);
  mp_limb_t  b_rem;
  int        twos;
  int        result_bit1 = 0;

  /* (a/-1)=1 when a>=0, so the sign of b is ignored */

  if (b_abs_size == 0)
    return JACOBI_U0 (a);  /* (a/0) */

  if (a > GMP_NUMB_MAX)
    {
      mp_limb_t  alimbs[2];
      mpz_t      az;
      ALLOC(az) = numberof (alimbs);
      PTR(az) = alimbs;
      mpz_set_ui (az, a);
      return mpz_kronecker (az, b);
    }

  if (! (b_low & 1))
    {
      /* (0/b)=0 for b!=+/-1; (even/even)=0 */
      if (! (a & 1))
        return 0;

      /* a odd, b even */

      /* establish shifted b_low for use with reciprocity below */
      MPN_STRIP_LOW_ZEROS_NOT_ZERO (b_ptr, b_abs_size, b_low);
      if (! (b_low & 1))
        {
          if (b_low == GMP_NUMB_HIGHBIT)
            {
              if (b_abs_size == 1)   /* (a/0x80000000) == (a/2)^(BPML-1) */
                return JACOBI_TWOS_U (GMP_NUMB_BITS-1, a);

              /* b_abs_size > 1 */
              b_low = b_ptr[1] << 1;
            }
          else
            {
              count_trailing_zeros (twos, b_low);
              b_low >>= twos;
            }
        }
    }
  else
    {
      if (a == 0)        /* (0/b)=1 for b=+/-1, 0 otherwise */
        return (b_abs_size == 1 && b_low == 1);

      if (! (a & 1))
        {
          /* a even, b odd */
          count_trailing_zeros (twos, a);
          a >>= twos;
          /* (a*2^n/b) = (a/b) * (2/a)^n */
          result_bit1 = JACOBI_TWOS_U_BIT1 (twos, b_low);
        }
    }

  if (a == 1)
    return JACOBI_BIT1_TO_PN (result_bit1);  /* (1/b)=1 */

  /* (a/b*2^n) = (b*2^n mod a / a) * RECIP(a,b) */
  JACOBI_MOD_OR_MODEXACT_1_ODD (result_bit1, b_rem, b_ptr, b_abs_size, a);
  result_bit1 ^= JACOBI_RECIP_UU_BIT1 (a, b_low);
  return mpn_jacobi_base (b_rem, (mp_limb_t) a, result_bit1);
}
