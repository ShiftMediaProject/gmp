/* Old function entrypoints retained for binary compatibility. */

/*
Copyright (C) 2000 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA.
*/

#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"


/* mpn_divexact_by3 was a function in gmp 3.0, but in gmp 3.1 it became a
   macro calling mpn_divexact_by3c.  */

#undef mpn_divexact_by3
int
__MPN (mpn_divexact_by3) (mp_ptr dst, mp_srcptr src, mp_size_t size)
{
  mpn_divexact_by3c (dst, src, size, 0);
}
