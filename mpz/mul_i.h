/* mpz_mul_ui/si (product, multiplier, small_multiplicand) -- Set PRODUCT to
   MULTIPLICATOR times SMALL_MULTIPLICAND.

Copyright 1991, 1993, 1994, 1996, 2000, 2001, 2002 Free Software Foundation,
Inc.

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


#ifdef OPERATION_mul_si
#define FUNCTION               mpz_mul_si
#define MULTIPLICAND_UNSIGNED
#define MULTIPLICAND_ABS(x)    ((unsigned long) ABS(x))
#endif

#ifdef OPERATION_mul_ui
#define FUNCTION               mpz_mul_ui
#define MULTIPLICAND_UNSIGNED  unsigned
#define MULTIPLICAND_ABS(x)    x
#endif

#ifndef FUNCTION
Error, error, unrecognised OPERATION
#endif


void
FUNCTION (mpz_ptr prod, mpz_srcptr mult,
          MULTIPLICAND_UNSIGNED long int small_mult)
{
  mp_size_t size = SIZ(mult);
  mp_size_t sign_product = size;
  mp_limb_t sml;
  mp_limb_t cy;
  mp_ptr pp;

  if (size == 0 || small_mult == 0)
    {
      SIZ(prod) = 0;
      return;
    }

  size = ABS (size);

  if (small_mult > GMP_NUMB_MAX)
    MPZ_REALLOC (prod, size + 2);
  else
    MPZ_REALLOC (prod, size + 1);

  pp = PTR(prod);

  sml = MULTIPLICAND_ABS (small_mult);

  cy = mpn_mul_1 (pp, PTR(mult), size, sml & GMP_NUMB_MASK);
#if GMP_NAIL_BITS != 0	/* this conditional just shuts of compiler warnings */
  if (small_mult > GMP_NUMB_MAX)
    {
      pp[size] = cy;
      cy = mpn_addmul_1 (pp + 1, PTR(mult), size, sml >> GMP_NUMB_BITS);
      pp[size + 1] = cy;
      size += 2;
      MPN_NORMALIZE_NOT_ZERO (pp, size);
    }
  else
#endif
    {
      pp[size] = cy;
      size += cy != 0;
    }

  SIZ(prod) = ((sign_product < 0) ^ (small_mult < 0)) ? -size : size;
}
