/* Test file for mpfr_mul_2exp.

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
#include <time.h>
#include "gmp.h"
#include "mpfr.h"

/* checks that x*y gives the same results in double
   and with mpfr with 53 bits of precision */

int
main(argc,argv) int argc; char *argv[];
{
  double x, z; mpfr_t w;

  mpfr_init2(w, 53); 

  srand48(time(NULL)); 
  x = drand48(); 
  mpfr_set_d(w, x, 0);
  mpfr_mul_2exp(w, w, 10, GMP_RNDZ); 
  if (x != (z = mpfr_get_d(w)/1024))
    {
      fprintf(stderr, "%f != %f\n", x, z); 
      exit (1); 
    }
  exit (0); 
}

