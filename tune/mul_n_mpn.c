/* mpn/generic/mul_n.c forced to USE_MORE_MPN. */

/*
Copyright 2000 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA.
*/

#define USE_MORE_MPN 1

#define __gmpn_kara_mul_n  mpn_kara_mul_n_mpn
#define __gmpn_kara_sqr_n  mpn_kara_sqr_n_mpn
#define __gmpn_toom3_mul_n mpn_toom3_mul_n_mpn
#define __gmpn_toom3_sqr_n mpn_toom3_sqr_n_mpn
#define __gmpn_mul_n       mpn_mul_n_mpn

#include "../mpn/generic/mul_n.c"
