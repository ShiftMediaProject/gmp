/* mpz_si_kronecker -- long+mpz Kronecker/Jacobi symbol.

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
mpz_si_kronecker (long a, mpz_srcptr b)
{
  mp_srcptr  b_ptr = PTR(b);
  mp_limb_t  b_low = b_ptr[0];
  mp_size_t  b_size = SIZ (b);
  mp_size_t  b_abs_size = ABS (b_size);
  mp_limb_t  a_limb, b_rem;
  unsigned   twos;
  int        result_bit1;

#if GMP_NUMB_BITS < BITS_PER_ULONG
  if (a > GMP_NUMB_MAX || a < -GMP_NUMB_MAX)
    {
      mp_limb_t  alimbs[2];
      mpz_t      az;
      ALLOC(az) = numberof (alimbs);
      PTR(az) = alimbs;
      mpz_set_si (az, a);
      return mpz_kronecker (az, b);
    }
#endif

  if (b_abs_size == 0)
    return JACOBI_S0 (a);  /* (a/0) */

  /* account for the effect of the sign of b, then ignore it */
  result_bit1 = JACOBI_BSGN_SS_BIT1 (a, b_size);

  if ((b_low & 1) != 0)
    {
      /* b odd */

      result_bit1 ^= JACOBI_ASGN_SU_BIT1 (a, b_low);
      a_limb = (unsigned long) ABS(a);

      if ((a_limb & 1) == 0)
        {
          /* (0/b)=1 for b=+/-1, 0 otherwise */
          if (a_limb == 0)
            return (b_abs_size == 1 && b_low == 1);

          /* a even, b odd */
          count_trailing_zeros (twos, a_limb);
          a_limb >>= twos;
          /* (a*2^n/b) = (a/b) * twos(n,a) */
          result_bit1 ^= JACOBI_TWOS_U_BIT1 (twos, b_low);
        }
    }
  else
    {
      /* (even/even)=0, and (0/b)=0 for b!=+/-1 */
      if ((a & 1) == 0)
        return 0;

      /* a odd, b even */

      /* establish shifted b_low with valid bit1 for use with ASGN and RECIP */
      MPN_STRIP_LOW_ZEROS_NOT_ZERO (b_ptr, b_abs_size, b_low);
      if ((b_low & 1) == 0)
        {
          if (b_low == GMP_NUMB_HIGHBIT)
            {
              if (b_abs_size == 1)
                {
                  /* (a/0x80000000) = (a/2)^(BPML-1) */
                  result_bit1 ^= JACOBI_TWOS_U_BIT1 (GMP_NUMB_BITS - 1, a);
                  return JACOBI_BIT1_TO_PN (result_bit1);
                }

              /* b_abs_size > 1 */
              b_low = b_ptr[1] << 1;
            }
          else
            {
              count_trailing_zeros (twos, b_low);
              b_low >>= twos;
            }
        }

      result_bit1 ^= JACOBI_ASGN_SU_BIT1 (a, b_low);
      a_limb = (unsigned long) ABS(a);
    }

  if (a_limb == 1)
    return JACOBI_BIT1_TO_PN (result_bit1);  /* (1/b)=1 */

  /* (a/b*2^n) = (b*2^n mod a / a) * recip(a,b) */
  JACOBI_MOD_OR_MODEXACT_1_ODD (result_bit1, b_rem, b_ptr, b_abs_size, a_limb);
  result_bit1 ^= JACOBI_RECIP_UU_BIT1 (a_limb, b_low);
  return mpn_jacobi_base (b_rem, a_limb, result_bit1);
}
