/* Test file for mpfr_can_round.

Copyright 1999, 2001 Free Software Foundation, Inc.

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

   /* checks that rounds to nearest sets the last
     bit to zero in case of equal distance */
   mpfr_init2 (x, 59);
   mpfr_set_str_raw (x, "-0.10010001010111000011110010111010111110000000111101100111111E663"); 
   if (mpfr_can_round (x, 54, GMP_RNDZ, GMP_RNDZ, 53) != 0)
     {
       fprintf (stderr, "Error (1) in mpfr_can_round\n");
       exit (1);
     }

   mpfr_set_str_raw (x, "-Inf"); 
   if (mpfr_can_round (x, 2000, GMP_RNDZ, GMP_RNDZ, 2000) != 0)
     {
       fprintf (stderr, "Error (2) in mpfr_can_round\n");
       exit (1);
     }

   mpfr_set_prec (x, 64);
   mpfr_set_str_raw (x, "0.1011001000011110000110000110001111101011000010001110011000000000");
   if (mpfr_can_round (x, 65, GMP_RNDN, GMP_RNDN, 54))
     {
       fprintf (stderr, "Error (3) in mpfr_can_round\n");
       exit (1);
     }

   mpfr_clear (x);

   return 0;
}
