/* mpz_sqrt(root, u) --  Set ROOT to floor(sqrt(U)).

Copyright 1991, 1993, 1994, 1996, 2000, 2001, 2005, 2012 Free Software
Foundation, Inc.

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

#include <stdio.h> /* for NULL */
#include "gmp.h"
#include "gmp-impl.h"

void
mpz_sqrt (mpz_ptr root, mpz_srcptr op)
{
  mp_size_t op_size, root_size;
  mp_ptr root_ptr, op_ptr;

  op_size = SIZ (op);
  if (op_size <= 0)
    {
      if (op_size < 0)
	SQRT_OF_NEGATIVE;
      SIZ(root) = 0;
      return;
    }

  /* The size of the root is accurate after this simple calculation.  */
  root_size = (op_size + 1) / 2;
  SIZ (root) = root_size;

  root_ptr = PTR (root);
  op_ptr = PTR (op);

  if (ALLOC (root) < root_size)
    {
      /* From size relations, we can tell ROOT != OP.  */
      ASSERT (root_ptr != op_ptr);

      root_ptr = __GMP_REALLOCATE_FUNC_LIMBS (root_ptr, ALLOC (root), root_size);
      ALLOC (root) = root_size;
      PTR (root) = root_ptr;
    }
  else
    {
      if (root_ptr == op_ptr)
	{
	  /* Allocate temp space for the root, which we then copy to the
	     shared OP/ROOT variable.  */
	  mp_ptr p;
	  TMP_DECL;
	  TMP_MARK;

	  p = TMP_ALLOC_LIMBS (root_size);
	  mpn_sqrtrem (p, NULL, root_ptr, op_size);

	  MPN_COPY (root_ptr, p, root_size);

	  TMP_FREE;
	  return;
	}
    }

  mpn_sqrtrem (root_ptr, NULL, op_ptr, op_size);
}
