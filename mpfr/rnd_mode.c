/* mpfr_set_machine_rnd_mode -- set the rounding mode for machine floats

Copyright (C) 1999, 2001 Free Software Foundation, Inc.

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

#ifdef TEST
#include <stdio.h>
#include "gmp.h"
#include "mpfr.h"

/* functions to set/get the machine rounding mode */
#ifdef _ISOC9X_SOURCE
#include <fenv.h>
#define TONEAREST fesetround(FE_TONEAREST)
#define TOZERO    fesetround(FE_TOWARDZERO)
#define TOINFP    fesetround(FE_UPWARD)
#define TOINFM    fesetround(FE_DOWNWARD)
#elif __mips
#include <sys/fpu.h>
extern int swapRM();
#define TOZERO swapRM(ROUND_TO_ZERO)
#define TOINFP swapRM(ROUND_TO_PLUS_INFINITY)
#define TONEAREST swapRM(ROUND_TO_NEAREST)
#define TOINFM swapRM(ROUND_TO_MINUS_INFINITY)
#elif (defined (__sparc__) || defined(hpux) || defined(freebsd))
#ifdef hpux
#include <math.h>
#else
#ifdef freebsd
#include <floatingpoint.h>
#else
#include <ieeefp.h>
#endif
#endif
#define TOZERO fpsetround(FP_RZ)
#define TOINFP fpsetround(FP_RP)
#define TONEAREST fpsetround(FP_RN)
#define TOINFM fpsetround(FP_RM)
#elif defined (__alpha)
#ifdef __GNUC__
/* GCC patched include files forget to define those... */
#define FP_RND_RZ       0
#define FP_RND_RN       1
#define FP_RND_RP       2
#define FP_RND_RM       3
#endif
#include <float.h>
#define TOZERO write_rnd(FP_RND_RZ)
#define TOINFP write_rnd(FP_RND_RP)
#define TONEAREST write_rnd(FP_RND_RN)
#define TOINFM write_rnd(FP_RND_RM)
#elif AIX
/* those values should be defined in float.h but strangely are not */
#define FP_RND_RZ       0
#define FP_RND_RN       1
#define FP_RND_RP       2
#define FP_RND_RM       3
#include <float.h>
#define TOZERO fp_swap_rnd(FP_RND_RZ)
#define TOINFP fp_swap_rnd(FP_RND_RP)
#define TONEAREST fp_swap_rnd(FP_RND_RN)
#define TOINFM fp_swap_rnd(FP_RND_RM)
#elif sunos
#include <floatingpoint.h>
char *out;
#define TOZERO ieee_flags("set","direction","tozero",&out)
#define TOINFP ieee_flags("set","direction","positive",&out)
#define TONEAREST ieee_flags("set","direction","nearest",&out)
#define TOINFM ieee_flags("set","direction","negative",&out)
#elif (defined (__powerpc__) && defined(linux))
#include <fpu_control.h>
#define TOZERO _FPU_SETCW(_FPU_RC_ZERO)
#define TOINFP _FPU_SETCW(_FPU_RC_UP)
#define TOINFM _FPU_SETCW(_FPU_RC_DOWN)
#define TONEAREST _FPU_SETCW(_FPU_RC_NEAREST)
#elif (defined (__i386__) || defined (__i486__) || defined (linux))
#ifdef __CYGWIN32__ /* no fpu_control.h under Cygnus */
#define _FPU_EXTENDED 0x300
#define _FPU_DOUBLE   0x200
#define _FPU_DEFAULT  0x137f
#define _FPU_RC_NEAREST 0x0
#define _FPU_RC_DOWN    0x400
#define _FPU_RC_UP      0x800
#define _FPU_RC_ZERO    0xC00
#else
#include <fpu_control.h>
#endif
#if defined(LIBC211) || defined(__CYGWIN32__)
#define __setfpucw(cw) __asm__ ("fldcw %0" : : "m" (cw))
#endif
/* be careful to put Precision control bits
   to round to double precision instead of the
   default (round to extended precision) */
#define _fpu_ieee ((_FPU_DEFAULT & (~_FPU_EXTENDED)) | _FPU_DOUBLE)
#define TOZERO __setfpucw(_fpu_ieee | _FPU_RC_ZERO)
#define TOINFP __setfpucw(_fpu_ieee | _FPU_RC_UP)
#define TOINFM __setfpucw(_fpu_ieee | _FPU_RC_DOWN)
#define TONEAREST __setfpucw(_fpu_ieee)
#endif

/* sets the machine rounding mode to the value rnd_mode */
void 
mpfr_set_machine_rnd_mode (mp_rnd_t rnd_mode)
{
  switch (rnd_mode) {
  case GMP_RNDN: TONEAREST; break;
  case GMP_RNDZ: TOZERO; break;
  case GMP_RNDU: TOINFP; break;
  case GMP_RNDD: TOINFM; break;
  default: fprintf(stderr, "invalid rounding mode\n"); exit(1);
  }
}
#endif
