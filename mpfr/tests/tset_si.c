/* Test file for mpfr_set_si and mpfr_set_ui.

Copyright (C) 1999 Free Software Foundation.

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
#include "mpfr-impl.h"
#include "time.h"

int
main(int argc, char **argv)
{
  mpfr_t x; long k, z, d; unsigned long zl, dl, N; 
  
  mpfr_init2(x, 100);

  srandom(time(NULL)); 

  N = (argc==1) ? 1000000 : atoi(argv[1]);

  for (k = 1; k <= N; k++)
    {
      z = random() - (1 << 30);      
      mpfr_set_si(x, z, GMP_RNDZ); 
      d = (int)mpfr_get_d(x);
      if (d != z) {
	fprintf(stderr, "Error in mpfr_set_si: expected %ld got %ld\n", z, d); exit(1);
      }
    }

  for (k = 1; k <= N; k++)
    {
      zl = random();
      mpfr_set_ui(x, zl, GMP_RNDZ); 
      dl = (unsigned int) mpfr_get_d(x);
      if (dl != zl) {
	fprintf(stderr, "Error in mpfr_set_ui: expected %lu got %lu\n", zl, dl); exit(1);
      }
    }

  mpfr_set_prec(x, 3);
  mpfr_set_si(x, 77617, GMP_RNDD); /* should be 65536 */
  if (MPFR_MANT(x)[0] != ((mp_limb_t)1 << (mp_bits_per_limb-1))) {
    fprintf(stderr, "Error in mpfr_set_si(x:3, 77617, GMP_RNDD)\n");
    mpfr_print_raw(x); putchar('\n');
    exit(1);
  }
  mpfr_set_ui(x, 77617, GMP_RNDD); /* should be 65536 */
  if (MPFR_MANT(x)[0] != ((mp_limb_t)1 << (mp_bits_per_limb-1))) {
    fprintf(stderr, "Error in mpfr_set_ui(x:3, 77617, GMP_RNDD)\n");
    mpfr_print_raw(x); putchar('\n');
    exit(1);
  }

  mpfr_set_prec(x, 1);
  mpfr_set_si(x, 33096, GMP_RNDU);
  if (mpfr_get_d(x) != 65536.0) {
    fprintf(stderr, "Error in mpfr_set_si, expected 65536, got %lu\n",
	    (unsigned long) mpfr_get_d(x));
    exit(1);
  }
  mpfr_set_ui(x, 33096, GMP_RNDU);
  if (mpfr_get_d(x) != 65536.0) {
    fprintf(stderr, "Error in mpfr_set_ui, expected 65536, got %lu\n",
	    (unsigned long) mpfr_get_d(x));
    exit(1);
  }

  mpfr_clear(x); 
  return(0);
}
