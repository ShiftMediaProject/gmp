/* Check mp_bases values.

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
#include "longlong.h"
#include "tests.h"


int
main (int argc, char *argv[])
{
  mp_limb_t  bb, h, l, bb_inv;
  int        i, j;

  for (i = 2; i < numberof (mp_bases); i++)
    {
      if (POW2_P (i))
        {
          count_trailing_zeros (j, i);
          if (mp_bases[i].big_base != (mp_limb_t) j)
            {
              printf ("mp_bases[%d].big_base (trailing zeros) wrong\n", i);
              abort ();
            }
        }
      else
        {
          bb = 1;
          for (j = 0; j < mp_bases[i].chars_per_limb; j++)
            {
              umul_ppmm (h, bb, bb, i);
              if (h != 0 || (bb & GMP_NAIL_MASK) != 0)
                {
                  printf ("mp_bases[%d].chars_per_limb overflow\n", i);
                  abort ();
                }
            }
          umul_ppmm (h, l, bb, i);
          if (h == 0 && (l & GMP_NAIL_MASK) == 0)
            {
              printf ("mp_bases[%d].chars_per_limb too small\n", i);
              abort ();
            }

          if (mp_bases[i].big_base != bb)
            {
              printf ("mp_bases[%d].big_base wrong\n", i);
              abort ();
            }

          invert_limb (bb_inv, bb << refmpn_count_leading_zeros (bb));
          if (mp_bases[i].big_base_inverted != bb_inv)
            {
              printf ("mp_bases[%d].big_base_inverted wrong\n", i);
              abort ();
            }
        }
    }

  if (MP_BASES_CHARS_PER_LIMB_10 != mp_bases[10].chars_per_limb)
    {
      printf ("MP_BASES_CHARS_PER_LIMB_10 not the same as mp_bases[10].chars_per_limb\n");
      abort ();
    }

  if (MP_BASES_BIG_BASE_10 != mp_bases[10].big_base)
    {
      printf ("MP_BASES_BIG_BASE_10 not the same as mp_bases[10].big_base\n");
      abort ();
    }

  if (MP_BASES_BIG_BASE_INVERTED_10 != mp_bases[10].big_base_inverted)
    {
      printf ("MP_BASES_BIG_BASE_INVERTED_10 not the same as mp_bases[10].big_base_inverted\n");
      abort ();
    }

  if (MP_BASES_NORMALIZATION_STEPS_10
      != refmpn_count_leading_zeros (MP_BASES_BIG_BASE_10))
    {
      printf ("MP_BASES_NORMALIZATION_STEPS_10 wrong\n");
      abort ();
    }

  exit (0);
}
