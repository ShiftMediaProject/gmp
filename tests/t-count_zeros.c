/* Test count_leading_zeros and count_trailing_zeros.

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
MA 02111-1307, USA. */

#include <stdio.h>
#include <stdlib.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#include "tests.h"

void
check_clz (int want, mp_limb_t n)
{
  int  got;
  count_leading_zeros (got, n);
  if (got != want)
    {
      printf ("count_leading_zeros wrong\n");
      printf    ("  n    %lX\n", n);
      printf    ("  want %d\n", want);
      printf    ("  got  %d\n", got);
      abort ();
    }
}

void
check_ctz (int want, mp_limb_t n)
{
  int  got;
  count_trailing_zeros (got, n);
  if (got != want)
    {
      printf ("count_trailing_zeros wrong\n");
      mpn_trace ("  n    ", &n, (mp_size_t) 1);
      printf    ("  want %d\n", want);
      printf    ("  got  %d\n", got);
      abort ();
    }
}

void
check_various (void)
{
  mp_limb_t  n;
  int        i;

#ifdef COUNT_LEADING_ZEROS_0
  check_clz (COUNT_LEADING_ZEROS_0, CNST_LIMB(0));
#endif

  for (n=1, i=0; i < BITS_PER_MP_LIMB; n<<=1, i++)
    {
      check_clz (i, CNST_LIMB(1) << (BITS_PER_MP_LIMB-1-i));
      check_ctz (i, CNST_LIMB(1) << i);

      check_ctz (i, MP_LIMB_T_MAX << i);
      check_clz (i, MP_LIMB_T_MAX >> i);
    }
}


int
main (int argc, char *argv[])
{
  tests_start ();
  mp_trace_base = 16;

  check_various ();

  tests_end ();
  exit (0);
}
