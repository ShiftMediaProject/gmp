/* mpf_clears() -- Clear multiple mpf_t variables.

Copyright 2009, 2014 Free Software Foundation, Inc.

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

#include <stdarg.h>
#include <stdio.h>		/* for NULL */
#include "gmp.h"
#include "gmp-impl.h"

void
mpf_clears (mpf_ptr x, ...)
{
  va_list  ap;

  va_start (ap, x);

  while (x != NULL)
    {
      (*__gmp_free_func) (x->_mp_d, (size_t) (x->_mp_prec + 1) * GMP_LIMB_BYTES);
      x = va_arg (ap, mpf_ptr);
    }
  va_end (ap);
}
