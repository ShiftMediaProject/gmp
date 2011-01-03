/* mpz_sqrtrem(root,rem,x) -- Set ROOT to floor(sqrt(X)) and REM
   to the remainder, i.e. X - ROOT**2.

Copyright 1991, 1993, 1994, 1996, 2000, 2001, 2005, 2011 Free Software
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
mpz_sqrtrem (mpz_ptr root, mpz_ptr rem, mpz_srcptr op)
{
  mp_size_t op_size, root_size, rem_size;
  mp_ptr root_ptr, op_ptr;
  mp_ptr free_me;
  mp_size_t free_me_size;
  TMP_DECL;

  TMP_MARK;
  free_me = NULL;
  op_size = SIZ (op);
  if (op_size <= 0)
    {
      if (op_size < 0)
	SQRT_OF_NEGATIVE;
      SIZ(root) = 0;
      SIZ(rem) = 0;
      return;
    }

  MPZ_REALLOC (rem, op_size);

  /* The size of the root is accurate after this simple calculation.  */
  root_size = (op_size + 1) / 2;

  root_ptr = PTR (root);
  op_ptr = PTR (op);

  if (ALLOC (root) < root_size)
    {
      if (root_ptr == op_ptr)
	{
	  free_me = root_ptr;
	  free_me_size = ALLOC (root);
	}
      else
	(*__gmp_free_func) (root_ptr, ALLOC (root) * BYTES_PER_MP_LIMB);

      ALLOC (root) = root_size;
      root_ptr = (mp_ptr) (*__gmp_allocate_func) (root_size * BYTES_PER_MP_LIMB);
      PTR (root) = root_ptr;
    }
  else
    {
      /* Make OP not overlap with ROOT.  */
      if (root_ptr == op_ptr)
	{
	  /* ROOT and OP are identical.  Allocate temporary space for OP.  */
	  op_ptr = TMP_ALLOC_LIMBS (op_size);
	  /* Copy to the temporary space.  Hack: Avoid temporary variable
	     by using ROOT_PTR.  */
	  MPN_COPY (op_ptr, root_ptr, op_size);
	}
    }

  rem_size = mpn_sqrtrem (root_ptr, PTR (rem), op_ptr, op_size);

  SIZ (root) = root_size;

  /* Write remainder size last, to enable us to define this function to
     give only the square root remainder, if the user calls if with
     ROOT == REM.  */
  SIZ (rem) = rem_size;

  if (free_me != NULL)
    (*__gmp_free_func) (free_me, free_me_size * BYTES_PER_MP_LIMB);
  TMP_FREE;
}
