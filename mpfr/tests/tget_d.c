/* Test file for mpfr_get_d

Copyright (C) 1999-2001 Free Software Foundation.

This file is part of the MPFR Library.

The MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <stdio.h>
#include <stdlib.h>
#include "gmp.h"
#include "mpfr.h"

int
main (void)
{
   mpfr_t x;

   mpfr_init2 (x, 2);

   /* checks that rounds to nearest sets the last
     bit to zero in case of equal distance */
   mpfr_set_d (x, 5.0, GMP_RNDN);
   if (mpfr_get_d (x) != 4.0)
     {
       fprintf (stderr, "Error in tget_d: got %1.1f instead of 4.0\n",
	       mpfr_get_d (x));
       exit (1);
     }

   mpfr_set_d (x, 9.84891017624509146344e-01, GMP_RNDU); 
   if (mpfr_get_d (x) != 1.0)
     {
       fprintf (stderr, "Error in tround: got %f instead of 1.0\n",
		mpfr_get_d (x));
       exit (1);
     }
   
   mpfr_clear(x);

   return 0;
}
