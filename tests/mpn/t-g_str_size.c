/* Test MPN_GET_STR_SIZE.

Copyright 2002 Free Software Foundation, Inc.

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
#include "tests.h"


/* The number of characters required for "limbs" many limbs of all
   0xFF..FFs, in the given base.  */
size_t
str_size_exact (mp_size_t limbs, int base)
{
  mp_ptr  xp;
  size_t  chars;
  TMP_DECL (marker);

  TMP_MARK (marker);
  xp = TMP_ALLOC_LIMBS (limbs);
  refmpn_fill (xp, limbs, MP_LIMB_T_MAX);

  chars = 0;
  do
    {
      chars++;
      refmpn_divrem_1 (xp, 0, xp, limbs, (mp_limb_t) base);
      limbs -= (xp[limbs-1] == 0);
    }
  while (limbs != 0);

  TMP_FREE (marker);
  return chars;
}

void
check_exact (void)
{
  int        base;
  mp_size_t  limbs;
  size_t     got, want;

  for (base = 2; base <= 36; base++)
    {
      for (limbs = 1; limbs < 20; limbs++)
        {
          want = str_size_exact (limbs, base);
          MPN_GET_STR_SIZE (got, base, limbs);
          if (got < want)
            {
              printf ("MPN_GET_STR_SIZE too small\n");
              printf ("  base   %d\n", base);
              printf ("  limbs  %ld\n", limbs);
              printf ("  got    %u\n", got);
              printf ("  want   %u\n", want);
              abort ();
            }
        }
    }
}

int
main (void)
{
  tests_start ();

  check_exact ();

  tests_end ();
  exit (0);
}
