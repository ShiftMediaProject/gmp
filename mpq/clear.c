/* mpq_clear -- free the space occupied by a mpq_t.

Copyright 1991, 1994, 1995, 2000, 2001 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published
by the Free Software Foundation; either version 3 of the License, or (at
your option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library.  If not, see https://www.gnu.org/licenses/.  */

#include "gmp.h"
#include "gmp-impl.h"

void
mpq_clear (mpq_t m)
{
  (*__gmp_free_func) (PTR(NUM(m)),
		    (size_t) ALLOC(NUM(m)) * GMP_LIMB_BYTES);
  (*__gmp_free_func) (PTR(DEN(m)),
		    (size_t) ALLOC(DEN(m)) * GMP_LIMB_BYTES);
}
