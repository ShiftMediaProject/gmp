/* Check the values of some constants. */

/*
Copyright 2000, 2001 Free Software Foundation, Inc.

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


/* How many bits seem to work in the given type. */
#define CALC_BITS_PER_TYPE(result, type)        \
  do {                                          \
    type  n = 1;                                \
    result = 0;                                 \
    while (n != 0)                              \
      {                                         \
        n <<= 1;                                \
        result++;                               \
      }                                         \
  } while (0)


/* FIXME: printf formats not right for a long long limb. */

#if HAVE_STRINGIZE
#define CHECK_CONSTANT(x,y)                                               \
  if ((x) != (y))                                                         \
    {                                                                     \
      printf ("%s == %lx, but %s == %lx\n",                               \
	      #x, (unsigned long int) (x), #y, (unsigned long int) (y));  \
      abort ();                                                           \
    }
#define CHECK_CONDITION(x)              \
  if (!(x))                             \
    {                                   \
      printf ("%s is false\n", #x);     \
      abort ();                         \
    }
#define CHECK_BITS_PER_TYPE(constant, type)             \
  do {                                                  \
    int   calculated;                                   \
    CALC_BITS_PER_TYPE (calculated, type);              \
    if (calculated != constant)                         \
      {                                                 \
        printf ("%s == %d, but calculated %d\n",        \
                #constant, constant, calculated);       \
        abort ();                                       \
      }                                                 \
  } while (0)

#else
#define CHECK_CONSTANT(x,y)                                               \
  if ((x) != (y))                                                         \
    {                                                                     \
      printf ("x == %lx, but y == %lx\n",                                 \
              (unsigned long int) (x), (unsigned long int) (y));          \
      abort ();                                                           \
    }
#define CHECK_CONDITION(x)      \
  if (!(x))                     \
    {                           \
      printf ("x is false\n");  \
      abort ();                 \
    }
#define CHECK_BITS_PER_TYPE(constant, type)                                   \
  do {                                                                        \
    int   calculated;                                                         \
    CALC_BITS_PER_TYPE (calculated, type);                                    \
    if (calculated != constant)                                               \
      {                                                                       \
        printf ("constant == %d, but calculated %d\n", constant, calculated); \
        abort ();                                                             \
      }                                                                       \
  } while (0)

#endif


int
main (void)
{
  CHECK_CONSTANT (BYTES_PER_MP_LIMB, sizeof(mp_limb_t));
  CHECK_CONSTANT (mp_bits_per_limb, BITS_PER_MP_LIMB);

  CHECK_BITS_PER_TYPE (BITS_PER_MP_LIMB,  mp_limb_t);
  CHECK_BITS_PER_TYPE (BITS_PER_LONGINT,  long);
  CHECK_BITS_PER_TYPE (BITS_PER_INT,      int);
  CHECK_BITS_PER_TYPE (BITS_PER_SHORTINT, short);
  CHECK_BITS_PER_TYPE (BITS_PER_CHAR,     char);

  /* UHWtype should have at least enough bits for half a UWtype */
  {
    int  bits_per_UWtype, bits_per_UHWtype;
    CALC_BITS_PER_TYPE (bits_per_UWtype,  UWtype);
    CALC_BITS_PER_TYPE (bits_per_UHWtype, UHWtype);
    CHECK_CONDITION (2*bits_per_UHWtype >= bits_per_UWtype);
  }

  {
    mp_limb_t  modlimb_inverse_3_calc;
    modlimb_invert (modlimb_inverse_3_calc, CNST_LIMB(3));
    CHECK_CONSTANT (MODLIMB_INVERSE_3, modlimb_inverse_3_calc);
    CHECK_CONDITION (MODLIMB_INVERSE_3 * CNST_LIMB(3) == CNST_LIMB(1));
  }

#ifdef PP_INVERTED
  {
    mp_limb_t  pp_inverted_calc;
    invert_limb (pp_inverted_calc, PP);
    CHECK_CONSTANT (PP_INVERTED, pp_inverted_calc);
  }
#endif

  exit (0);
}
