/* mpn_zero_p -- test for zero.

   THIS FUNCTION IS FOR INTERNAL USE AND MAY DISAPPEAR OR BE SUBJECT TO
   INCOMPATIBLE CHANGES IN FUTURE GMP RELEASES.
   
Copyright (C) 1994, 1996, 2000 Free Software Foundation, Inc.

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


/* Callers expecting non-zero values should do an inline test of a limb
   before calling here.  On that basis speed here isn't too important.

   n==0 is allowed and is considered a zero value.  */

int
mpn_zero_p (mp_ptr p, mp_size_t n)
{
  mp_size_t i;

  for (i = 0; i < n; i++)
    if (p[i] != 0)
      return 0;

  return 1;
}
