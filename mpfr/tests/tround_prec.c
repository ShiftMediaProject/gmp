/* Test file for mpfr_round_prec.

Copyright 1999, 2000, 2001, 2002 Free Software Foundation.

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

   mpfr_init2 (x, 3);

   mpfr_set_ui (x, 5, GMP_RNDN);
   mpfr_round_prec (x, GMP_RNDN, 2);
   if (mpfr_cmp_ui(x, 4))
     {
       fprintf (stderr, "Error in tround: got %1.1f instead of 4\n",
		mpfr_get_d1 (x));
       exit (1);
     }

   /* check case when reallocation is needed */
   mpfr_set_prec (x, 3);
   mpfr_set_ui (x, 5, GMP_RNDN); /* exact */
   mpfr_round_prec (x, GMP_RNDN, mp_bits_per_limb + 1);
   if (mpfr_cmp_ui(x, 5))
     {
       fprintf (stderr, "Error in tround: got %1.1f instead of 5\n",
		mpfr_get_d1 (x));
       exit (1);
     }

   mpfr_clear(x);
   mpfr_init2 (x, 3);
   mpfr_set_si (x, -5, GMP_RNDN); /* exact */
   mpfr_round_prec (x, GMP_RNDN, mp_bits_per_limb + 1);
   if (mpfr_cmp_si(x, -5))
     {
       fprintf (stderr, "Error in tround: got %1.1f instead of -5\n",
		mpfr_get_d1 (x));
       exit (1);
     }

   /* check case when new precision needs less limbs */
   mpfr_set_prec (x, mp_bits_per_limb + 1);
   mpfr_set_ui (x, 5, GMP_RNDN); /* exact */
   mpfr_round_prec (x, GMP_RNDN, 3); /* exact */
   if (mpfr_cmp_ui(x, 5))
     {
       fprintf (stderr, "Error in tround: got %1.1f instead of 5\n",
		mpfr_get_d1 (x));
       exit (1);
     }

   mpfr_clear(x);

   return 0;
}
