/* Check the values of some constants. */

/*
Copyright (C) 2000 Free Software Foundation, Inc.

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

#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"


#if HAVE_STRINGIZE
#define CHECK_CONSTANT(x,y)                                     \
  if ((x) != (y))                                               \
    {                                                           \
      printf ("%s == %d, but %s == %d\n", #x, x, #y, y);        \
      abort ();                                                 \
    }
#define CHECK_CONDITION(x)              \
  if (!(x))                             \
    {                                   \
      printf ("%s is false\n", #x);     \
      abort ();                         \
    }

#else
#define CHECK_CONSTANT(x,y)                     \
  if ((x) != (y))                               \
    {                                           \
      printf ("x == %d, but y == %d\n", x, y);  \
      abort ();                                 \
    }
#define CHECK_CONDITION(x)      \
  if (!(x))                     \
    {                           \
      printf ("x is false\n");  \
      abort ();                 \
    }
#endif


int
main (void)
{
  CHECK_CONSTANT (BYTES_PER_MP_LIMB, sizeof(mp_limb_t));
  CHECK_CONSTANT (BITS_PER_MP_LIMB,  8*sizeof(mp_limb_t));
  CHECK_CONSTANT (mp_bits_per_limb,  8*sizeof(mp_limb_t));
  CHECK_CONSTANT (BITS_PER_LONGINT,  8*sizeof(long));
  CHECK_CONSTANT (BITS_PER_INT,      8*sizeof(int));
  CHECK_CONSTANT (BITS_PER_SHORTINT, 8*sizeof(short));
  CHECK_CONSTANT (BITS_PER_CHAR,     8*sizeof(char));

  CHECK_CONDITION (2*sizeof(UHWtype) >= sizeof(UWtype));

  {
    mp_limb_t  modlimb_inverse_3_calc;
    modlimb_invert (modlimb_inverse_3_calc, CNST_LIMB(3));
    CHECK_CONSTANT (MODLIMB_INVERSE_3, modlimb_inverse_3_calc);
    CHECK_CONDITION (MODLIMB_INVERSE_3 * CNST_LIMB(3) == CNST_LIMB(1));
  }

  {
    mp_limb_t  pp_inverted_calc;
    invert_limb (pp_inverted_calc, PP);
    CHECK_CONSTANT (PP_INVERTED, pp_inverted_calc);
  }

  exit (0);
}
