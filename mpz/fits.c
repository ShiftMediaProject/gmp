/* int mpz_fits_X_p (mpz_t z) -- test whether z fits type X.

Copyright (C) 1997, 2000 Free Software Foundation, Inc.

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
MA 02111-1307, USA. */

#include "gmp.h"
#include "gmp-impl.h"


#ifdef OPERATION_fits_sshort
#define FUNCTION    mpz_fits_sshort_p
#define TEST        MPZ_FITS_STYPE_SDT (size, data, short)
#endif

#ifdef OPERATION_fits_sint
#define FUNCTION    mpz_fits_sint_p
#define TEST        MPZ_FITS_STYPE_SDT (size, data, int)
#endif

#ifdef OPERATION_fits_slong
#define FUNCTION    mpz_fits_slong_p
#define TEST        MPZ_FITS_STYPE_SDT (size, data, long)
#endif

#ifdef OPERATION_fits_ushort
#define FUNCTION    mpz_fits_ushort_p
#define TEST        MPZ_FITS_UTYPE_SDT (size, data, unsigned short)
#endif

#ifdef OPERATION_fits_uint
#define FUNCTION    mpz_fits_uint_p
#define TEST        MPZ_FITS_UTYPE_SDT (size, data, unsigned int)
#endif

#ifdef OPERATION_fits_ulong
#define FUNCTION    mpz_fits_ulong_p
#define TEST        MPZ_FITS_UTYPE_SDT (size, data, unsigned long)
#endif

#ifndef FUNCTION
Error, unrecognised OPERATION symbol.
#endif


int
FUNCTION (mpz_srcptr z)
{
  mp_size_t size = SIZ(z);
  mp_limbs_t data = PTR(z)[0];

  return TEST;
}
