/* auxiliary functions for MPFR tests.

Copyright 1999, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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

#include <stdlib.h>
#include <limits.h>
#include <float.h>
#include <math.h>

#if HAVE_CONFIG_H
#include "config.h"
#endif

/* generates a random long int, a random double,
   and corresponding seed initializing */
#define DBL_RAND() ((double) randlimb() / (double) MP_LIMB_T_MAX)

#define MINNORM 2.2250738585072013831e-308 /* 2^(-1022), smallest normalized */
#define MAXNORM 1.7976931348623157081e308 /* 2^(1023)*(2-2^(-52)) */

/* The MAX, MIN and ABS macros may already be defined if gmp-impl.h has
   been included. They have the same semantics as in gmp-impl.h, but the
   expressions may be slightly different. So, it's better to undefine
   them first, as required by the ISO C standard. */
#undef MAX
#undef MIN
#undef ABS
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define ABS(x) (((x)>0) ? (x) : -(x))

void tests_memory_start _PROTO ((void));
void tests_memory_end _PROTO ((void));

void tests_start_mpfr _PROTO ((void));
void tests_end_mpfr _PROTO ((void));

void tests_machine_prec_double _PROTO ((void));
void tests_machine_prec_long_double _PROTO ((void));

unsigned short x86_fstcw _PROTO ((void));
void x86_fldcw _PROTO ((unsigned short cw));

int mpfr_set_machine_rnd_mode _PROTO ((mp_rnd_t));
void mpfr_test_init _PROTO ((void));
mp_limb_t randlimb _PROTO ((void));
void randseed _PROTO ((unsigned int));
int ulp _PROTO ((double, double));
double dbl _PROTO ((double, int));
double Ulp _PROTO ((double));
int Isnan _PROTO ((double));
