/* Utilities for MPFR developers, not exported.

Copyright 1999, 2000, 2001, 2002 Free Software Foundation, Inc.

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

/* Test if X (positive) is a power of 2 */

#define IS_POW2(X) (((X) & ((X) - 1)) == 0)
#define NOT_POW2(X) (((X) & ((X) - 1)) != 0)

/* This unsigned type must correspond to the signed one defined in gmp.h */
#if defined (_CRAY) && ! defined (_CRAYMPP)
typedef unsigned int            mp_exp_unsigned_t;
typedef unsigned int            mp_size_unsigned_t;
#else
typedef unsigned long int       mp_exp_unsigned_t;
typedef unsigned long int       mp_size_unsigned_t;
#endif

#define MP_EXP_T_MAX ((mp_exp_t) ((~ (mp_exp_unsigned_t) 0) >> 1))
#define MP_EXP_T_MIN (-MP_EXP_T_MAX-1)

#define MP_LIMB_T_ONE ((mp_limb_t) 1)

#if (BITS_PER_MP_LIMB & (BITS_PER_MP_LIMB - 1))
#error "BITS_PER_MP_LIMB must be a power of 2"
#endif

#define MPFR_INTPREC_MAX (ULONG_MAX & ~(unsigned long) (BITS_PER_MP_LIMB - 1))

/* Assertions */

/* Compile with -DWANT_ASSERT to check all assert statements */

/* MPFR_ASSERTN(expr): assertions that should always be checked */
/* #define MPFR_ASSERTN(expr) ASSERT_ALWAYS(expr) */
#define MPFR_ASSERTN(expr)  ((expr) ? (void) 0 : (void) ASSERT_FAIL (expr))

/* MPFR_ASSERTD(expr): assertions that should be checked when testing */
/* #define MPFR_ASSERTD(expr) ASSERT(expr) */
#if WANT_ASSERT
#define MPFR_ASSERTD(expr)  ASSERT_ALWAYS (expr)
#else
#define MPFR_ASSERTD(expr)  ((void) 0)
#endif

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

#define DBL_POS_INF (1.0/0.0)
#define DBL_NEG_INF (-1.0/0.0)
#define DBL_NAN (0.0/0.0)

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
#define MPFR_SET_ABSSIZE(x, n) \
  ((x)->_mpfr_size = ((x)->_mpfr_size & ((mp_size_unsigned_t) 7 << 29)) \
                     | (mp_size_unsigned_t) (n))
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

/* The following macro restores the exponent range and the flags,
   checks that the result is in the exponent range and returns the
   ternary inexact value. */
#define MPFR_RESTORE_RET(inex, x, rnd_mode) \
  do \
    { \
      int inex_cr; \
      mpfr_restore_emin_emax(); \
      inex_cr = mpfr_check_range(x, rnd_mode); \
      if (inex_cr) \
        return inex_cr; /* underflow or overflow */ \
      MPFR_RET(inex); \
    } \
  while(0)

/* Memory gestion */

/* temporary allocate s limbs at xp, and initialize mpfr variable x */
#define MPFR_INIT(xp, x, p, s) \
  (xp = (mp_ptr) TMP_ALLOC((size_t) (s) * BYTES_PER_MP_LIMB), \
   MPFR_PREC(x) = (p), \
   MPFR_MANT(x) = (xp), \
   MPFR_SIZE(x) = (s), \
   MPFR_EXP(x) = 0)
/* same when xp is already allocated */
#define MPFR_INIT1(xp, x, p, s) \
  (MPFR_PREC(x) = (p), MPFR_MANT(x) = (xp), MPFR_SIZE(x) = (s))

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

extern mp_prec_t __mpfr_const_log2_prec;
int mpfr_set_underflow _PROTO ((mpfr_ptr, mp_rnd_t, int));
int mpfr_set_overflow _PROTO ((mpfr_ptr, mp_rnd_t, int));
void mpfr_save_emin_emax _PROTO ((void));
void mpfr_restore_emin_emax _PROTO ((void));
int mpfr_add1 _PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_srcptr,
                       mp_rnd_t, mp_exp_unsigned_t));
int mpfr_sub1 _PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_srcptr,
                       mp_rnd_t, int));
int mpfr_round_raw_generic _PROTO ((mp_limb_t *, mp_limb_t *, mp_prec_t, int,
				    mp_prec_t, mp_rnd_t, int *, int));
int mpfr_can_round_raw _PROTO ((mp_limb_t *, mp_size_t, int, mp_exp_t,
				mp_rnd_t, mp_rnd_t, mp_prec_t));
double mpfr_get_d3 _PROTO ((mpfr_srcptr, mp_exp_t, mp_rnd_t));
int mpfr_cmp_abs _PROTO ((mpfr_srcptr, mpfr_srcptr));
int mpfr_cmp2 _PROTO ((mpfr_srcptr, mpfr_srcptr, mp_prec_t *));
long _mpfr_ceil_log2 _PROTO ((double));
long _mpfr_floor_log2 _PROTO ((double));
double _mpfr_ceil_exp2 _PROTO ((double));
unsigned long _mpfr_isqrt _PROTO ((unsigned long));
unsigned long _mpfr_cuberoot _PROTO ((unsigned long));
int mpfr_exp_2 _PROTO ((mpfr_ptr, mpfr_srcptr, mp_rnd_t));
int mpfr_exp3 _PROTO ((mpfr_ptr, mpfr_srcptr, mp_rnd_t));

#define mpfr_round_raw(yp, xp, xprec, neg, yprec, r, inexp) \
  mpfr_round_raw_generic((yp), (xp), (xprec), (neg), (yprec), (r), (inexp), 0)

#define mpfr_round_raw2(xp, xn, neg, r, prec) \
  mpfr_round_raw_generic(0, (xp), (xn) * BITS_PER_MP_LIMB, (neg), \
			 (prec), (r), 0, 1); 

#if defined (__cplusplus)
}
#endif  
