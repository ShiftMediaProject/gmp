/* mpn_com_n -- mpn bitwise ones-complement. */

/*
Copyright (C) 1999-2000 Free Software Foundation, Inc.

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
the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "gmp.h"
#include "gmp-impl.h"


void
#if __STDC__
mpn_com_n (mp_ptr dst, mp_srcptr src, mp_size_t size)
#else
mpn_com_n (dst, src, size)
     mp_ptr    dst;
     mp_srcptr src;
     mp_size_t size;
#endif
{
  do
    dst[size-1] = ~src[size-1];
  while (--size);
}
