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

#if defined (_LITTLE_ENDIAN) || defined (__LITTLE_ENDIAN__)		\
 || defined (__alpha)							\
 || (defined (__arm__) && (defined (__ARMWEL__) || defined (__linux__)))\
 || defined (__clipper__)						\
 || defined (__cris)							\
 || defined (__i386__)							\
 || defined (__i860__)							\
 || defined (__i960__)							\
 || defined (__ia64)							\
 || defined (MIPSEL) || defined (_MIPSEL)				\
 || defined (__ns32000__)						\
 || defined (__WINNT) || defined (_WIN32)
#define _MPFR_NAN_BYTES  { 0, 0, 0xc0, 0x7f }
#define _MPFR_INFP_BYTES { 0, 0, 0xf0, 0x7f }
#define _MPFR_INFM_BYTES { 0, 0, 0xf0, 0xff }
#else
#if defined (_BIG_ENDIAN) || defined (__BIG_ENDIAN__)			\
 || defined (__mc68000__) || defined (__mc68020__) || defined (__m68k__)\
 || defined (mc68020)                                                   \
 || defined (__a29k__) || defined (_AM29K)				\
 || defined (__arm__)							\
 || (defined (__convex__) && defined (_IEEE_FLOAT_))			\
 || defined (_CRAYMPP) || defined (_CRAYIEEE)				\
 || defined (__i370__) || defined (__mvs__)				\
 || defined (__m88000__)						\
 || defined (MIPSEB) || defined (_MIPSEB)				\
 || defined (__hppa) || defined (__hppa__)				\
 || defined (__pyr__)							\
 || defined (__ibm032__)						\
 || defined (_IBMR2) || defined (_ARCH_PPC)				\
 || defined (__sh__)							\
 || defined (__sparc) || defined (sparc)				\
 || defined (__we32k__)
#define _MPFR_NAN_BYTES  { 0x7f, 0xc0, 0, 0 }
#define _MPFR_INFP_BYTES { 0x7f, 0xf0, 0, 0 }
#define _MPFR_INFM_BYTES { 0xff, 0xf0, 0, 0 }
#endif
#endif

#ifdef NAN
#define MPFR_DBL_NAN ((double) NAN)
#else
#ifdef _MPFR_NAN_BYTES
static union { unsigned char c[4]; float d; } __mpfr_nan
= { _MPFR_NAN_BYTES };
#define MPFR_DBL_NAN ((double) __mpfr_nan.d)
#else
#define MPFR_DBL_NAN (0./0.)
#endif
#endif

#ifdef HUGE_VAL
#define MPFR_DBL_INFP HUGE_VAL
#define MPFR_DBL_INFM (-HUGE_VAL)
#else
#ifdef _MPFR_INFP_BYTES
static union { unsigned char c[4]; float d; } __mpfr_infp
= { _MPFR_INFP_BYTES };
static union { unsigned char c[4]; float d; } __mpfr_infm
= { _MPFR_INFM_BYTES };
#define MPFR_DBL_INFP ((double) __mpfr_infp.d)
#define MPFR_DBL_INFM ((double) __mpfr_infm.d)
#else
#define MPFR_DBL_INFP (1/0.)
#define MPFR_DBL_INFM (-1/0.)
#endif
#endif
