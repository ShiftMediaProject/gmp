/* mpn/generic/div_qr_1n_pi1.c method 2.

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

#undef DIV_QR_1N_METHOD
#define DIV_QR_1N_METHOD 2
#undef mpn_div_qr_1n_pi1
#define mpn_div_qr_1n_pi1 mpn_div_qr_1n_pi1_2

#include "mpn/generic/div_qr_1n_pi1.c"
