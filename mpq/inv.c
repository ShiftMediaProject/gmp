/* mpq_inv(dest,src) -- invert a rational number, i.e. set DEST to SRC
   with the numerator and denominator swapped.

Copyright 1991, 1994, 1995, 2000, 2001 Free Software Foundation, Inc.

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

#include "gmp.h"
#include "gmp-impl.h"

void
mpq_inv (MP_RAT *dest, const MP_RAT *src)
{
  mp_size_t num_size = SIZ(NUM(src));
  mp_size_t den_size = SIZ(DEN(src));

  if (UNLIKELY (num_size == 0))
    DIVIDE_BY_ZERO;

  if (num_size < 0)
    {
      num_size = -num_size;
      den_size = -den_size;
    }
  SIZ(DEN(dest)) = num_size;
  SIZ(NUM(dest)) = den_size;

  /* If dest == src we may just swap the numerator and denominator, but
     we have to ensure the new denominator is positive.  */

  if (dest == src)
    {
      mp_size_t alloc = ALLOC(NUM(dest));
      mp_ptr limb_ptr = PTR(NUM(dest));

      ALLOC(NUM(dest)) = ALLOC(DEN(dest));
      PTR(NUM(dest)) = PTR(DEN(dest));

      ALLOC(DEN(dest)) = alloc;
      PTR(DEN(dest)) = limb_ptr;
    }
  else
    {
      den_size = ABS (den_size);

      MPZ_REALLOC (NUM(dest), den_size);
      MPZ_REALLOC (DEN(dest), num_size);

      MPN_COPY (PTR(NUM(dest)), PTR(DEN(src)), den_size);
      MPN_COPY (PTR(DEN(dest)), PTR(NUM(src)), num_size);
    }
}
