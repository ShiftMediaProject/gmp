/* Test MPN_INCR_U and MPN_DECR_U.

Copyright 2001 Free Software Foundation, Inc.

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
#include <stdlib.h>

#include "gmp.h"
#include "gmp-impl.h"
#include "tests.h"


#define M     MP_LIMB_T_MAX
#define SIZE  ((mp_size_t) 10)


void
check_one (const char *name, int i,
           mp_srcptr src, mp_limb_t n,
           mp_srcptr got, mp_srcptr want, mp_size_t size)
{
  if (refmpn_cmp (got, want, size) != 0)
    {
      printf ("Wrong at %s i=%d\n", name, i);
      mpn_trace ("  src", src,  size);
      mpn_trace ("    n", &n,   (mp_size_t) 1);
      mpn_trace ("  got", got,  size);
      mpn_trace (" want", want, size);
      abort ();
    }
}


/* The i386 MPN_INCR_U and MPN_DECR_U have special cases for "n" being a
   compile-time constant 1, so exercise that explicitly.  */

void
check_incr_const_1 (void)
{
  static const struct {
    const mp_limb_t  src[SIZE];
    const mp_limb_t  want[SIZE];
  } data[] = {
    { { 0 },   { 1 } },
    { { 123 }, { 124 } },
    { { M, 0 },   { 0, 1 } },
    { { M, 123 }, { 0, 124 } },
    { { M, M, 0 },   { 0, 0, 1 } },
    { { M, M, 123 }, { 0, 0, 124 } },
    { { M, M, M, 0 },   { 0, 0, 0, 1 } },
    { { M, M, M, 123 }, { 0, 0, 0, 124 } },
    { { M, M, M, M, 0 },   { 0, 0, 0, 0, 1 } },
    { { M, M, M, M, 123 }, { 0, 0, 0, 0, 124 } },
  };

  mp_limb_t  got[SIZE];
  int   i;

  for (i = 0; i < numberof (data); i++)
    {
      refmpn_copyi (got, data[i].src, SIZE);
      MPN_INCR_U (got, SIZE, CNST_LIMB(1));
      check_one ("check_incr_const_1", i,
                 data[i].src, CNST_LIMB(1),
                 got, data[i].want, SIZE);
    }
}

void
check_decr_const_1 (void)
{
  static const struct {
    const mp_limb_t  src[SIZE];
    const mp_limb_t  want[SIZE];
  } data[] = {
    { { 1 },   { 0 } },
    { { 123 }, { 122 } },
    { { M },   { M-1 } },
    { { 0, 1 },   { M, 0 } },
    { { 0, 123 }, { M, 122 } },
    { { 0, M },   { M, M-1 } },
    { { 0, 0, 1 },   { M, M, 0 } },
    { { 0, 0, 123 }, { M, M, 122 } },
    { { 0, 0, M },   { M, M, M-1 } },
    { { 0, 0, 0, 1 },   { M, M, M, 0 } },
    { { 0, 0, 0, 123 }, { M, M, M, 122 } },
    { { 0, 0, 0, M },   { M, M, M, M-1 } },
    { { 0, 0, 0, 0, 1 },   { M, M, M, M, 0 } },
    { { 0, 0, 0, 0, 123 }, { M, M, M, M, 122 } },
    { { 0, 0, 0, 0, M },   { M, M, M, M, M-1 } },
  };

  mp_limb_t  got[SIZE];
  int   i;

  for (i = 0; i < numberof (data); i++)
    {
      refmpn_copyi (got, data[i].src, SIZE);
      MPN_DECR_U (got, SIZE, CNST_LIMB(1));
      check_one ("check_incr_const_1", i,
                 data[i].src, CNST_LIMB(1),
                 got, data[i].want, SIZE);
    }
}

void
check_incr_data (void)
{
  static const struct {
    mp_limb_t        n;
    const mp_limb_t  src[SIZE];
    const mp_limb_t  want[SIZE];
  } data[] = {
    { 1, { 0 },   { 1 } },
    { 1, { 123 }, { 124 } },
    { 2, { 0 },   { 2 } },
    { 2, { 123 }, { 125 } },
    { M, { 0 },   { M } },

    { 1, { M, 0 },   { 0,   1 } },
    { 1, { M, 123 }, { 0,   124 } },
    { 2, { M, 0 },   { 1,   1 } },
    { 2, { M, 123 }, { 1,   124 } },
    { M, { M, 0 },   { M-1, 1 } },
    { M, { M, 123 }, { M-1, 124 } },

    { 1, { M, M, 0 },   { 0,   0, 1 } },
    { 1, { M, M, 123 }, { 0,   0, 124 } },
    { 2, { M, M, 0 },   { 1,   0, 1 } },
    { 2, { M, M, 123 }, { 1,   0, 124 } },
    { M, { M, M, 0 },   { M-1, 0, 1 } },
    { M, { M, M, 123 }, { M-1, 0, 124 } },

    { 1, { M, M, M, 0 },   { 0,   0, 0, 1 } },
    { 1, { M, M, M, 123 }, { 0,   0, 0, 124 } },
    { 2, { M, M, M, 0 },   { 1,   0, 0, 1 } },
    { 2, { M, M, M, 123 }, { 1,   0, 0, 124 } },
    { M, { M, M, M, 0 },   { M-1, 0, 0, 1 } },
    { M, { M, M, M, 123 }, { M-1, 0, 0, 124 } },

    { 1, { M, M, M, M, 0 },   { 0,   0, 0, 0, 1 } },
    { 1, { M, M, M, M, 123 }, { 0,   0, 0, 0, 124 } },
    { 2, { M, M, M, M, 0 },   { 1,   0, 0, 0, 1 } },
    { 2, { M, M, M, M, 123 }, { 1,   0, 0, 0, 124 } },
    { M, { M, M, M, M, 0 },   { M-1, 0, 0, 0, 1 } },
    { M, { M, M, M, M, 123 }, { M-1, 0, 0, 0, 124 } },
  };

  mp_limb_t  got[SIZE];
  int   i;

  for (i = 0; i < numberof (data); i++)
    {
      refmpn_copyi (got, data[i].src, SIZE);
      MPN_INCR_U (got, SIZE, data[i].n);
      check_one ("check_incr_data", i,
                 data[i].src, data[i].n,
                 got, data[i].want, SIZE);
    }
}

void
check_decr_data (void)
{
  static const struct {
    mp_limb_t        n;
    const mp_limb_t  src[SIZE];
    const mp_limb_t  want[SIZE];
  } data[] = {
    { 1,   { 1 },   { 0 } },
    { 1,   { 123 }, { 122 } },
    { 2,   { 2 },   { 0 } },
    { 2,   { 123 }, { 121 } },
    { M,   { M },   { 0 } },
    { M-1, { M },   { 1 } },

    { 1,   { 0,   1 },   { M,   0   } },
    { 1,   { 0,   123 }, { M,   122 } },
    { 2,   { 0,   123 }, { M-1, 122 } },
    { 2,   { 1,   123 }, { M,   122 } },
    { M,   { 0,   123 }, { 1,   122 } },
    { M,   { M-1, M },   { M,   M-1 } },

    { 1,   { 0,   0, 1   }, { M,   M, 0   } },
    { 1,   { 0,   0, 123 }, { M,   M, 122 } },
    { 2,   { 0,   0, 123 }, { M-1, M, 122 } },
    { 2,   { 1,   0, 123 }, { M,   M, 122 } },
    { M,   { 0,   0, 123 }, { 1,   M, 122 } },
    { M,   { M-1, 0, M   }, { M,   M, M-1 } },

    { 1,   { 0,   0, 0, 1   }, { M,   M, M, 0   } },
    { 1,   { 0,   0, 0, 123 }, { M,   M, M, 122 } },
    { 2,   { 0,   0, 0, 123 }, { M-1, M, M, 122 } },
    { 2,   { 1,   0, 0, 123 }, { M,   M, M, 122 } },
    { M,   { 0,   0, 0, 123 }, { 1,   M, M, 122 } },
    { M,   { M-1, 0, 0, M   }, { M,   M, M, M-1 } },

    { 1,   { 0,   0, 0, 0, 1   }, { M,   M, M, M, 0   } },
    { 1,   { 0,   0, 0, 0, 123 }, { M,   M, M, M, 122 } },
    { 2,   { 0,   0, 0, 0, 123 }, { M-1, M, M, M, 122 } },
    { 2,   { 1,   0, 0, 0, 123 }, { M,   M, M, M, 122 } },
    { M,   { 0,   0, 0, 0, 123 }, { 1,   M, M, M, 122 } },
    { M,   { M-1, 0, 0, 0, M   }, { M,   M, M, M, M-1 } },

    { 1,   { 0,   0, 0, 0, 0, 1   }, { M,   M, M, M, M, 0   } },
    { 1,   { 0,   0, 0, 0, 0, 123 }, { M,   M, M, M, M, 122 } },
    { 2,   { 0,   0, 0, 0, 0, 123 }, { M-1, M, M, M, M, 122 } },
    { 2,   { 1,   0, 0, 0, 0, 123 }, { M,   M, M, M, M, 122 } },
    { M,   { 0,   0, 0, 0, 0, 123 }, { 1,   M, M, M, M, 122 } },
    { M,   { M-1, 0, 0, 0, 0, M   }, { M,   M, M, M, M, M-1 } },
  };

  mp_limb_t  got[SIZE];
  int   i;

  for (i = 0; i < numberof (data); i++)
    {
      refmpn_copyi (got, data[i].src, SIZE);
      MPN_DECR_U (got, SIZE, data[i].n);
      check_one ("check_decr_data", i,
                 data[i].src, data[i].n,
                 got, data[i].want, SIZE);
    }
}


int
main (void)
{
  tests_start ();
  mp_trace_base = -16;

  check_incr_data ();
  check_decr_data ();

  check_incr_const_1 ();
  check_decr_const_1 ();

  tests_end ();
  exit (0);
}
