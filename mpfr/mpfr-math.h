/* Macros MPFR_DBL_NAN, MPFR_DBL_INFP and MPFR_DBL_INFM

Copyright 2002 Free Software Foundation, Inc.

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

#if HAVE_DOUBLE_IEEE_LITTLE_ENDIAN || HAVE_DOUBLE_IEEE_LITTLE_SWAPPED
#define _MPFR_NAN_BYTES  { 0, 0, 0xc0, 0x7f }
#define _MPFR_INFP_BYTES { 0, 0, 0x80, 0x7f }
#define _MPFR_INFM_BYTES { 0, 0, 0x80, 0xff }
#endif

#if HAVE_DOUBLE_IEEE_BIG_ENDIAN
#define _MPFR_NAN_BYTES  { 0x7f, 0xc0, 0, 0 }
#define _MPFR_INFP_BYTES { 0x7f, 0x80, 0, 0 }
#define _MPFR_INFM_BYTES { 0xff, 0x80, 0, 0 }
#endif

#ifdef NAN
#define MPFR_DBL_NAN ((double) NAN)
#else
#ifdef _MPFR_NAN_BYTES
/* A struct is used here rather than a union of a float and a char array
   since union initializers aren't available in K&R, in particular not in
   the HP bundled cc.  Alignment will hopefully be based on the structure
   size, rather than it's contents, so we should be ok.  */
#ifdef __alpha
/* gcc 3.0.2 on an alphaev56-unknown-freebsd4.3 doesn't seem to correctly
   convert from a float NAN to a double NAN, use an 8-byte form instead as a
   workaround.  */
static struct { unsigned char c[8]; } __mpfr_nan
= { { 0, 0, 0, 0, 0, 0, 0xf8, 0x7f } };
#define MPFR_DBL_NAN (*((double *) __mpfr_nan.c))
#else
static struct { unsigned char c[4]; } __mpfr_nan = { _MPFR_NAN_BYTES };
#define MPFR_DBL_NAN ((double) *((float *) __mpfr_nan.c))
#endif
#else
#define MPFR_DBL_NAN (0./0.)
#endif
#endif

#ifdef HAVE_HUGE_VAL
#define MPFR_DBL_INFP HUGE_VAL
#define MPFR_DBL_INFM (-HUGE_VAL)
#else
#ifdef _MPFR_INFP_BYTES
static struct { unsigned char c[4]; } __mpfr_infp = { _MPFR_INFP_BYTES };
static struct { unsigned char c[4]; } __mpfr_infm = { _MPFR_INFM_BYTES };
#define MPFR_DBL_INFP ((double) *((float *) __mpfr_infp.c))
#define MPFR_DBL_INFM ((double) *((float *) __mpfr_infm.c))
#else
#define MPFR_DBL_INFP (1/0.)
#define MPFR_DBL_INFM (-1/0.)
#endif
#endif
