/* Test file for mpfr_can_round.

Copyright (C) 1999 PolKA project, Inria Lorraine and Loria

This file is part of the MPFR Library.

The MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <stdio.h>
#include <stdlib.h>
#include "gmp.h"
#include "mpfr.h"

int main()
{
   mpfr_t x;

   /* checks that rounds to nearest sets the last
     bit to zero in case of equal distance */
   mpfr_init2(x, 59);
   mpfr_set_str_raw(x, "-0.10010001010111000011110010111010111110000000111101100111111E663"); 
   if (mpfr_can_round(x, 54, GMP_RNDZ, GMP_RNDZ, 53) != 0) {
     fprintf(stderr, "Error in mpfr_can_round\n"); exit(1);
   }
   mpfr_clear(x);
   exit (0);
}
