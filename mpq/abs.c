/* mpq_abs -- absolute value of a rational.

Copyright 2000, 2001 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.  */

#define __GMP_FORCE_mpq_abs 1

#include "gmp.h"
#include "gmp-impl.h"


void
mpq_abs (mpq_ptr dst, mpq_srcptr src)
{
  mp_size_t  num_abs_size = ABSIZ(NUM(src));

  if (dst != src)
    {
      mp_size_t  den_size = SIZ(DEN(src));

      MPZ_REALLOC (mpq_numref(dst), num_abs_size);
      MPZ_REALLOC (mpq_denref(dst), den_size);

      MPN_COPY (PTR(NUM(dst)), PTR(NUM(src)), num_abs_size);
      MPN_COPY (PTR(DEN(dst)), PTR(DEN(src)), den_size);

      SIZ(DEN(dst)) = den_size;
    }

  SIZ(NUM(dst)) = num_abs_size;
}
