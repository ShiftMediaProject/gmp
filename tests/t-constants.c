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


#ifdef _LONG_LONG_LIMB
#define LL(l,ll)   ll
#else
#define LL(l,ll)   l
#endif


#define CHECK_LIMB_S(x, xname, y, yname)                \
  do {                                                  \
    if ((x) != (y))                                     \
      {                                                 \
        printf (LL("%s == %lx, but %s == %lx\n",        \
                   "%s == %llx, but %s == %llx\n"),     \
                xname, x, yname, y);                    \
        error = 1;                                      \
      }                                                 \
  } while (0)

#define CHECK_INT_S(x, xname, y, yname)                                 \
  do {                                                                  \
    if ((x) != (y))                                                     \
      {                                                                 \
        printf ("%s == %d, but %s == %d\n", xname, x, yname, y);        \
        error = 1;                                                      \
      }                                                                 \
  } while (0)



#define CHECK_CONDITION_S(x, xname)             \
  do {                                          \
    if (!(x))                                   \
      {                                         \
        printf ("%s is false\n", xname);        \
        error = 1;                              \
      }                                         \
  } while (0)


/* How many bits seem to work in the given type. */
#define CALC_BITS(result, type) \
  do {                          \
    type  n = 1;                \
    result = 0;                 \
    while (n != 0)              \
      {                         \
        n <<= 1;                \
        result++;               \
      }                         \
  } while (0)

#define CHECK_BITS_S(constant, constant_name, type)     \
  do {                                                  \
    int   calculated;                                   \
    CALC_BITS (calculated, type);                       \
    if (calculated != constant)                         \
      {                                                 \
        printf ("%s == %d, but calculated %d\n",        \
                constant_name, constant, calculated);   \
        error = 1;                                      \
      }                                                 \
  } while (0)


#define CHECK_HIGHBIT(value, type)              \
  do {                                          \
    type  n = value;                            \
    if (n == 0)                                 \
      {                                         \
        printf ("%s == 0\n", #value);           \
        error = 1;                              \
      }                                         \
    n <<= 1;                                    \
    if (n != 0)                                 \
      {                                         \
        printf ("%s << 1 != 0\n", #value);      \
        error = 1;                              \
      }                                         \
  } while (0)


#define CHECK_MAX_S(max_val, max_name, min_val, min_name, type) \
  do {                                                          \
    type  n = max_val;                                          \
    n++;                                                        \
    if (n != min_val)                                           \
      {                                                         \
        printf ("%s + 1 != %s\n", max_name, min_name);          \
        error = 1;                                              \
      }                                                         \
    if (max_val <= min_val)                                     \
      {                                                         \
        printf ("%s <= %s\n", max_name, min_name);              \
        error = 1;                                              \
      }                                                         \
  } while (0)



/* FIXME: printf formats not right for a long long limb. */

#if HAVE_STRINGIZE
#define CHECK_LIMB(x,y)     CHECK_LIMB_S (x, #x, y, #y)
#define CHECK_INT(x,y)      CHECK_INT_S (x, #x, y, #y)
#define CHECK_CONDITION(x)  CHECK_CONDITION_S (x, #x)
#define CHECK_BITS(c,t)     CHECK_BITS_S (c, #c, t)
#define CHECK_MAX(m,n,t)    CHECK_MAX_S (m, #m, n, #n, t)
#else
#define CHECK_LIMB(x,y)     CHECK_LIMB_S (x, "x", y, "y")
#define CHECK_INT(x,y)      CHECK_INT_S (x, "x", y, "y")
#define CHECK_CONDITION(x)  CHECK_CONDITION_S (x, "x")
#define CHECK_BITS(c,t)     CHECK_BITS_S (c, "c", t)
#define CHECK_MAX(m,n,t)    CHECK_MAX_S (m, "m", n, "n", t)
#endif


int
main (void)
{
  int  error = 0;

  CHECK_INT (BYTES_PER_MP_LIMB, sizeof(mp_limb_t));
  CHECK_INT (mp_bits_per_limb, BITS_PER_MP_LIMB);

  CHECK_BITS (BITS_PER_MP_LIMB,  mp_limb_t);
  CHECK_BITS (BITS_PER_LONGINT,  long);
  CHECK_BITS (BITS_PER_INT,      int);
  CHECK_BITS (BITS_PER_SHORTINT, short);
  CHECK_BITS (BITS_PER_CHAR,     char);

  CHECK_HIGHBIT (MP_LIMB_T_HIGHBIT, mp_limb_t);
  CHECK_HIGHBIT (ULONG_HIGHBIT,     unsigned long);
  CHECK_HIGHBIT (UINT_HIGHBIT,      unsigned int);
  CHECK_HIGHBIT (USHRT_HIGHBIT,     unsigned short);
  CHECK_HIGHBIT (LONG_HIGHBIT,      long);
  CHECK_HIGHBIT (INT_HIGHBIT,       int);
  CHECK_HIGHBIT (SHRT_HIGHBIT,      short);

  CHECK_MAX (LONG_MAX,      LONG_MIN,      long);
  CHECK_MAX (INT_MAX,       INT_MIN,       int);
  CHECK_MAX (SHRT_MAX,      SHRT_MIN,      short);
  CHECK_MAX (ULONG_MAX,     0,             unsigned long);
  CHECK_MAX (UINT_MAX,      0,             unsigned int);
  CHECK_MAX (USHRT_MAX,     0,             unsigned short);
  CHECK_MAX (MP_SIZE_T_MAX, MP_SIZE_T_MIN, mp_size_t);

  /* UHWtype should have at least enough bits for half a UWtype */
  {
    int  bits_per_UWtype, bits_per_UHWtype;
    CALC_BITS (bits_per_UWtype,  UWtype);
    CALC_BITS (bits_per_UHWtype, UHWtype);
    CHECK_CONDITION (2*bits_per_UHWtype >= bits_per_UWtype);
  }

  {
    mp_limb_t  modlimb_inverse_3_calc;
    modlimb_invert (modlimb_inverse_3_calc, CNST_LIMB(3));
    CHECK_LIMB (MODLIMB_INVERSE_3, modlimb_inverse_3_calc);
  }

  {
    mp_limb_t  MODLIMB_INVERSE_3_times_3 = MODLIMB_INVERSE_3 * CNST_LIMB(3);
    CHECK_LIMB (MODLIMB_INVERSE_3_times_3, CNST_LIMB(1));
  }

#ifdef PP_INVERTED
  {
    mp_limb_t  pp_inverted_calc;
    invert_limb (pp_inverted_calc, PP);
    CHECK_LIMB (PP_INVERTED, pp_inverted_calc);
  }
#endif

  if (error)
    abort ();

  exit (0);
}
