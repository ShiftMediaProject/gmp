/* Utilities for MPFR developers, not exported.

Copyright (C) 1999, 2000, 2001 Free Software Foundation, Inc.

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

/* This unsigned type must correspond to the signed one defined in gmp.h */
#if defined (_CRAY) && ! defined (_CRAYMPP)
typedef unsigned int            mp_exp_unsigned_t;
typedef unsigned int            mp_size_unsigned_t;
#else
typedef unsigned long int       mp_exp_unsigned_t;
typedef unsigned long int       mp_size_unsigned_t;
#endif

#define MP_LIMB_T_ONE ((mp_limb_t) 1)

/* Assertions */

/* Compile with -DWANT_ASSERT to check all assert statements */

/* MPFR_ASSERTN(expr): assertion checked in the normal debug level */
#define MPFR_ASSERTN(expr) ASSERT_ALWAYS(expr)

/* MPFR_ASSERTD(expr): assertion checked in debug level 33 or higher */
#define MPFR_ASSERTD(expr) ASSERT(expr)

/* Definition of constants */

#define LOG2 0.69314718055994528622 /* log(2) rounded to zero on 53 bits */
#define ALPHA 4.3191365662914471407 /* a+2 = a*log(a), rounded to +infinity */

/* Safe absolute value (to avoid possible integer overflow) */

#define SAFE_ABS(type,x) ((x) >= 0 ? (unsigned type)(x) : -(unsigned type)(x))

/* macros for doubles, based on gmp union ieee_double_extract */

typedef union ieee_double_extract Ieee_double_extract;

#define DOUBLE_ISNANorINF(x) (((Ieee_double_extract *)&(x))->s.exp == 0x7ff)
#define DOUBLE_ISINF(x) (DOUBLE_ISNANorINF(x) && \
			 (((Ieee_double_extract *)&(x))->s.manl == 0) && \
                         (((Ieee_double_extract *)&(x))->s.manh == 0))
#define DOUBLE_ISNAN(x) (DOUBLE_ISNANorINF(x) && \
			 ((((Ieee_double_extract *)&(x))->s.manl != 0) || \
                         (((Ieee_double_extract *)&(x))->s.manh != 0)))

/* bit 31 of _mpfr_size is used for sign,
   bit 30 of _mpfr_size is used for Nan flag,
   bit 29 of _mpfr_size is used for Inf flag,
   remaining bits are used to store the number of allocated limbs */
#define MPFR_CLEAR_FLAGS(x) \
  (((x) -> _mpfr_size &= ~((mp_size_unsigned_t) 3 << 29)))
#define MPFR_IS_NAN(x) (((x)->_mpfr_size >> 30) & 1)
#define MPFR_SET_NAN(x) ((x)->_mpfr_size |= ((mp_size_unsigned_t) 1 << 30))
#define MPFR_CLEAR_NAN(x) \
  (((x) -> _mpfr_size &= ~((mp_size_unsigned_t) 1 << 30)))
#define MPFR_IS_INF(x) (((x)->_mpfr_size >> 29) & 1)
#define MPFR_SET_INF(x) ((x)->_mpfr_size |= ((mp_size_unsigned_t) 1 << 29))
#define MPFR_CLEAR_INF(x) ((x)->_mpfr_size &= ~((mp_size_unsigned_t) 1 << 29))
#define MPFR_IS_FP(x) ((((x) -> _mpfr_size >> 29) & 3) == 0)
#define MPFR_ABSSIZE(x) \
  ((x)->_mpfr_size & (((mp_size_unsigned_t) 1 << 29) - 1))
#define MPFR_SIZE(x) ((x)->_mpfr_size)
#define MPFR_EXP(x) ((x)->_mpfr_exp)
#define MPFR_MANT(x) ((x)->_mpfr_d)
#define MPFR_ISNONNEG(x) (MPFR_NOTZERO((x)) && MPFR_SIGN(x) >= 0)
#define MPFR_ISNEG(x) (MPFR_NOTZERO((x)) && MPFR_SIGN(x) == -1)
#define MPFR_SET_POS(x) (MPFR_SIZE(x) &= ~(((mp_size_unsigned_t) 1) << 31))
#define MPFR_SET_NEG(x) (MPFR_SIZE(x) |= (((mp_size_unsigned_t) 1) << 31))
#define MPFR_CHANGE_SIGN(x) (MPFR_SIZE(x) ^= (((mp_size_unsigned_t) 1) << 31))
#define MPFR_SET_SAME_SIGN(x, y) \
  (MPFR_SIGN((x)) != MPFR_SIGN((y)) && (MPFR_CHANGE_SIGN((x)), 0))
#define MPFR_PREC(x) ((x)->_mpfr_prec)
#define MPFR_NOTZERO(x) \
  (MPFR_MANT(x)[(MPFR_PREC(x)-1)/BITS_PER_MP_LIMB] != (mp_limb_t) 0)
#define MPFR_IS_ZERO(x) \
  (MPFR_MANT(x)[(MPFR_PREC(x)-1)/BITS_PER_MP_LIMB] == (mp_limb_t) 0)
#define MPFR_SET_ZERO(x) \
  (MPFR_MANT(x)[(MPFR_PREC(x)-1)/BITS_PER_MP_LIMB] = (mp_limb_t) 0)
#define MPFR_ESIZE(x) \
  ((MPFR_PREC((x)) - 1) / BITS_PER_MP_LIMB + 1); 
#define MPFR_EVEN_INEX 2

/* When returning the ternary inexact value, ALWAYS use one of the
   following two macros, unless the flag comes from another function
   returning the ternary inexact value */
#define MPFR_RET(I) return \
  (I) ? ((__mpfr_flags |= MPFR_FLAGS_INEXACT), (I)) : 0
#define MPFR_RET_NAN return (__mpfr_flags |= MPFR_FLAGS_NAN), 0

/* Memory gestion */

/* temporary allocate s limbs at xp, and initialize mpfr variable x */
#define MPFR_INIT(xp, x, p, s) \
  (xp = (mp_ptr) TMP_ALLOC((s)*BYTES_PER_MP_LIMB), \
   MPFR_PREC(x) = p, MPFR_MANT(x) = xp, MPFR_SIZE(x) = s, MPFR_EXP(x) = 0)
/* same when xp is already allocated */
#define MPFR_INIT1(xp, x, p, s) \
  (MPFR_PREC(x) = p, MPFR_MANT(x) = xp, MPFR_SIZE(x) = s)

#ifndef _PROTO
#if defined (__STDC__) || defined (__cplusplus)
#define _PROTO(x) x
#else
#define _PROTO(x) ()
#endif
#endif

#if defined (__cplusplus)
extern "C" {
#endif  

int mpfr_set_underflow _PROTO ((mpfr_ptr, mp_rnd_t, int));
int mpfr_set_overflow _PROTO ((mpfr_ptr, mp_rnd_t, int));
void mpfr_save_emin_emax _PROTO ((void));
void mpfr_restore_emin_emax _PROTO ((void));
int mpfr_add1 _PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_srcptr,
                       mp_rnd_t, mp_exp_unsigned_t));
int mpfr_sub1 _PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_srcptr,
                       mp_rnd_t, mp_exp_unsigned_t));
int mpfr_round_raw_generic _PROTO ((mp_limb_t *, mp_limb_t *, mp_prec_t, int,
				    mp_prec_t, mp_rnd_t, int *, int));
int mpfr_can_round_raw _PROTO ((mp_limb_t *, mp_prec_t, int, mp_prec_t, 
				mp_rnd_t, mp_rnd_t, mp_prec_t));
double mpfr_get_d2 _PROTO ((mpfr_srcptr, long)); 
mp_size_t mpn_sqrtrem_new _PROTO ((mp_limb_t *, mp_limb_t *, mp_limb_t *, mp_size_t));
int mpfr_cmp_abs _PROTO ((mpfr_srcptr, mpfr_srcptr));
mp_prec_t mpfr_cmp2 _PROTO ((mpfr_srcptr, mpfr_srcptr));
long _mpfr_ceil_log2 _PROTO ((double));
long _mpfr_floor_log2 _PROTO ((double));
double _mpfr_ceil_exp2 _PROTO ((double));
unsigned long _mpfr_isqrt _PROTO ((unsigned long));
unsigned long _mpfr_cuberoot _PROTO ((unsigned long));

#define mpfr_round_raw(yp, xp, xprec, neg, yprec, r, inexp) \
  mpfr_round_raw_generic((yp), (xp), (xprec), (neg), (yprec), (r), (inexp), 0)

#define mpfr_round_raw2(xp, xn, neg, r, prec) \
  mpfr_round_raw_generic(NULL, (xp), (xn) * BITS_PER_MP_LIMB, (neg), \
			 (prec), (r), 0, 1); 

#if defined (__cplusplus)
}
#endif  

