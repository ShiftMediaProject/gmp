/* mpfr_set_default_prec -- set the default precision

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

#include "gmp.h"
#include "gmp-impl.h"

/* default is 53 bits */
mp_size_t __gmp_default_fp_bit_precision = 53;

void
#if __STDC__
mpfr_set_default_prec (unsigned long int prec_in_bits)
#else
mpfr_set_default_prec (prec_in_bits)
     unsigned long int prec_in_bits;
#endif
{
  __gmp_default_fp_bit_precision = prec_in_bits;
}
