/* mpn_sec_sqr.

   Contributed to the GNU project by Torbjörn Granlund.

Copyright 2013 Free Software Foundation, Inc.

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
mpn_sec_sqr (mp_ptr rp,
	     mp_srcptr ap, mp_size_t an,
	     mp_ptr tp)
{
  mpn_sqr_basecase (rp, ap, an);
}

mp_size_t
mpn_sec_sqr_itch (mp_size_t an)
{
  return 0;
}
