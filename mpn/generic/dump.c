/* THIS IS AN INTERNAL FUNCTION WITH A MUTABLE INTERFACE.  IT IS NOT SAFE TO
   CALL THIS FUNCTION DIRECTLY.  IN FACT, IT IS ALMOST GUARANTEED THAT THIS
   FUNCTION WILL CHANGE OR DISAPPEAR IN A FUTURE GNU MP RELEASE.


Copyright (C) 1996, 2000 Free Software Foundation, Inc.

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
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA.
*/

#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"

void
mpn_dump (ptr, size)
     mp_srcptr ptr;
     mp_size_t size;
{
  MPN_NORMALIZE (ptr, size);

  if (size == 0)
    printf ("0\n");
  else
    {
      size--;
      printf ("%lX", ptr[size]);

      while (size)
	{
	  size--;
	  printf ("%0*lX", (int) (2 * BYTES_PER_MP_LIMB), ptr[size]);
	}
      printf ("\n");
    }
}
