/* mpz_init_set_si(val) -- Make a new multiple precision number with
   value val.

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
mpz_init_set_si (mpz_ptr x, signed long int val)
{
  x->_mp_alloc = 1;
  x->_mp_d = (mp_ptr) (*__gmp_allocate_func) (BYTES_PER_MP_LIMB);
  x->_mp_d[0] = (unsigned long) (val >= 0 ? val : -val);
  x->_mp_size = (val > 0 ? 1 : val == 0 ? 0 : -1);
}
