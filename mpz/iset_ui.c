/* mpz_init_set_ui(dest,val) -- Make a new multiple precision in DEST and
   assign VAL to the new number.

Copyright 1991, 1993, 1994, 1995, 2000, 2001, 2002 Free Software Foundation,
Inc.

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

void
mpz_init_set_ui (mpz_ptr dest, unsigned long int val)
{
  mp_size_t size;

  dest->_mp_alloc = 1;
  dest->_mp_d = (mp_ptr) (*__gmp_allocate_func) (BYTES_PER_MP_LIMB);

  dest->_mp_d[0] = val & GMP_NUMB_MASK;
  size = val != 0;

#if GMP_NAIL_BITS != 0
  if (val > GMP_NUMB_MAX)
    {
      MPZ_REALLOC (dest, 2);
      dest->_mp_d[1] = val >> GMP_NUMB_BITS;
      size = 2;
    }
#endif

  dest->_mp_size = size;
}
