/* mpn_add_n, mpn_sub_n -- add or subtract equal length limb vectors.

Copyright (C) 1992, 1993, 1994, 1996, 2000 Free Software Foundation, Inc.

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


#ifdef OPERATION_add_n
#define FUNCTION   mpn_add_n
#define VARIATION  y = x + y; cy += (y < x);
#endif

#ifdef OPERATION_sub_n
#define FUNCTION   mpn_sub_n
#define VARIATION  y = x - y; cy += (y > x);
#endif

#ifndef FUNCTION
Error, error, need OPERATION_add_n or OPERATION_sub_n
#endif


mp_limb_t
FUNCTION (mp_ptr res_ptr, mp_srcptr s1_ptr, mp_srcptr s2_ptr, mp_size_t size)
{
  register mp_limb_t x, y, cy;
  register mp_size_t j;

  /* The loop counter and index J goes from -SIZE to -1.  This way
     the loop becomes faster.  */
  j = -size;

  /* Offset the base pointers to compensate for the negative indices.  */
  s1_ptr -= j;
  s2_ptr -= j;
  res_ptr -= j;

  cy = 0;
  do
    {
      y = s2_ptr[j];
      x = s1_ptr[j];
      y += cy;	     	/* previous carry/borrow into second operand */
      cy = (y < cy);	/* new carry from that                       */
      VARIATION;        /* this add or sub, and its carry            */
      res_ptr[j] = y;
    }
  while (++j != 0);

  return cy;
}
