/* Utilities for MPFR developers, not exported.

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

#ifndef __MPFR_IMPL_H__
#define __MPFR_IMPL_H__

/* Auto-include limits.h (Before gmp-impl.h) */
#include <limits.h>

/* Auto include local gmp.h if not included */
#ifndef __GMP_H__
#include "gmp.h"
#endif

/* Auto include local gmp-impl.h if not included */
#ifndef __GMP_IMPL_H__
#include "gmp-impl.h"
#endif

/* Auto include local mpfr.h if not included */
#ifndef __MPFR_H
#include "mpfr.h"
#endif

#ifndef HAVE_STRCASECMP
#define strcasecmp mpfr_strcasecmp
#endif

#ifndef HAVE_STRNCASECMP
#define strncasecmp mpfr_strncasecmp
#endif

/* Check GMP */
#if GMP_NAIL_BITS != 0
# error "MPFR doesn't support nonzero values of GMP_NAIL_BITS"
#endif

#if (BITS_PER_MP_LIMB & (BITS_PER_MP_LIMB - 1))
# error "BITS_PER_MP_LIMB must be a power of 2"
#endif


 /* Definition of MPFR_LIMB_HIGHBIT and MPFR_LIMB_ONE */
#ifdef GMP_LIMB_HIGHBIT
# define MPFR_LIMB_HIGHBIT GMP_LIMB_HIGHBIT
#else
# ifdef MP_LIMB_T_HIGHBIT
#  define MPFR_LIMB_HIGHBIT MP_LIMB_T_HIGHBIT
# else
#  error "Neither GMP_LIMB_HIGHBIT nor MP_LIMB_T_HIGHBIT defined in GMP"
# endif
#endif

#define MPFR_LIMB_ONE ((mp_limb_t) 1)
#ifndef MP_LIMB_T_ONE
# define MP_LIMB_T_ONE MPFR_LIMB_ONE
#endif

/* Check if we can represent the number of limbs 
 * associated to the maximum of mpfr_prec_t*/
/* Can't use MPFR_PREC_MAX, so use MPFR_PREC_FORMAT_INT */

#if   MPFR_PREC_FORMAT == 1
# if (MP_SIZE_T_MAX < (USHRT_MAX/BYTES_PER_MP_LIMB))
#  error "Incompatibilty between mp_size_t and mpfr_prec_t."
# endif
#elif MPFR_PREC_FORMAT == 2
# if (MP_SIZE_T_MAX < (UINT_MAX/BYTES_PER_MP_LIMB))
#  error "Incompatibilty between mp_size_t and mpfr_prec_t."
# endif
#elif MPFR_PREC_FORMAT == 3
# if (MP_SIZE_T_MAX < (ULONG_MAX/BYTES_PER_MP_LIMB))
#  error "Incompatibilty between mp_size_t and mpfr_prec_t."
# endif
#else
# error "MPFR Prec format invalid"
#endif

/* Test if X (positive) is a power of 2 */
#define IS_POW2(X) (((X) & ((X) - 1)) == 0)
#define NOT_POW2(X) (((X) & ((X) - 1)) != 0)

/* Defined limits and unsigned type of exponent */
#if __GMP_MP_SIZE_T_INT == 1
typedef unsigned int            mpfr_uexp_t;
# define MPFR_EXP_MAX (INT_MAX)
# define MPFR_EXP_MIN (INT_MIN)
#else
typedef unsigned long int       mpfr_uexp_t;
# define MPFR_EXP_MAX (LONG_MAX)
# define MPFR_EXP_MIN (LONG_MIN)
#endif
#ifndef mp_exp_unsigned_t 
# define mp_exp_unsigned_t mpfr_uexp_t
#endif

#define MPFR_INTPREC_MAX (ULONG_MAX & ~(unsigned long) (BITS_PER_MP_LIMB - 1))

/* Assertions */

/* Compile with -DWANT_ASSERT to check all assert statements */

/* Note: do not use GMP macros ASSERT_ALWAYS and ASSERT as they are not
   expressions, and as a consequence, they cannot be used in a for(),
   with a comma operator and so on. */

/* MPFR_ASSERTN(expr): assertions that should always be checked */
#define MPFR_ASSERTN(expr)  \
   ((void) ((MPFR_UNLIKELY(expr)) || MPFR_UNLIKELY((ASSERT_FAIL (expr), 0))))

/* MPFR_ASSERTD(expr): assertions that should be checked when testing */
#if WANT_ASSERT
# define MPFR_EXP_CHECK 1
# define MPFR_ASSERTD(expr)  MPFR_ASSERTN (expr)
#else
# define MPFR_ASSERTD(expr)  ((void) 0)
#endif

/* Check if the args are correct */
#define MPFR_CHECK1(x,r) \
 MPFR_ASSERTD(mpfr_check(x) && GMP_RNDN <= r && r <= GMP_RNDD)
#define MPFR_CHECK2(x,y,r) \
 MPFR_ASSERTD(mpfr_check(x) && mpfr_check(y) && GMP_RNDN <= r && r <= GMP_RNDD)
#define MPFR_CHECK3(x,y,z,r) \
 MPFR_ASSERTD(mpfr_check(x) && mpfr_check(y) && mpfr_check(z) && \
  GMP_RNDN <= r && r <= GMP_RNDD)

/* Code to deal with impossible
   WARNING: It doesn't use do { } while (0) for Insure++*/
#define MPFR_RET_NEVER_GO_HERE()  {MPFR_ASSERTN(0); return 0;}

/* Theses macros help the compiler to determine if a test is 
 * likely or unlikely. */
#if __GNUC__ >= 3
# define MPFR_LIKELY(x) (__builtin_expect(!!(x),1))
# define MPFR_UNLIKELY(x) (__builtin_expect((x),0))
#else
# define MPFR_LIKELY(x) (x)
# define MPFR_UNLIKELY(x) (x)
#endif

/* Invalid exponent value (to track bugs...) */
#define MPFR_EXP_INVALID ((mp_exp_t) 1 << 30)

/* Use MPFR_GET_EXP and MPFR_SET_EXP instead of MPFR_EXP directly,
   unless when the exponent may be out-of-range, for instance when
   setting the exponent before calling mpfr_check_range.
   MPFR_EXP_CHECK is defined when WANT_ASSERT is defined, but if you
   don't use WANT_ASSERT (for speed reasons), you can still define
   MPFR_EXP_CHECK by setting -DMPFR_EXP_CHECK in $CFLAGS. */

#if MPFR_EXP_CHECK
# define MPFR_GET_EXP(x)          mpfr_get_exp (x)
# define MPFR_SET_EXP(x, exp)     MPFR_ASSERTN (!mpfr_set_exp ((x), (exp)))
# define MPFR_SET_INVALID_EXP(x)  ((void) (MPFR_EXP (x) = MPFR_EXP_INVALID))
#else
# define MPFR_GET_EXP(x)          MPFR_EXP (x)
# define MPFR_SET_EXP(x, exp)     ((void) (MPFR_EXP (x) = (exp)))
# define MPFR_SET_INVALID_EXP(x)  ((void) 0)
#endif

/* Definition of constants */

#define LOG2 0.69314718055994528622 /* log(2) rounded to zero on 53 bits */
#define ALPHA 4.3191365662914471407 /* a+2 = a*log(a), rounded to +infinity */

/* Safe absolute value (to avoid possible integer overflow) */
/* type is the target (unsigned) type */

#define SAFE_ABS(type,x) ((x) >= 0 ? (type)(x) : -(type)(x))

/* macros for doubles, based on gmp union ieee_double_extract */

#ifndef IEEE_DBL_MANT_DIG
#define IEEE_DBL_MANT_DIG 53
#endif

typedef union ieee_double_extract Ieee_double_extract;

/* for x of type ieee_double_extract */
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

/* macros for long doubles */

/* we only require that LDBL_MANT_DIG is a bound on the mantissa length
   of the "long double" type */
#ifndef LDBL_MANT_DIG
# define LDBL_MANT_DIG 113 /* works also if long double == quad */
#endif

/* Various i386 systems have been seen with incorrect LDBL constants in
   float.h (notes in set_ld.c), so force the value we know is right for IEEE
   extended.  */
#if HAVE_LDOUBLE_IEEE_EXT_LITTLE
# define MPFR_LDBL_MANT_DIG   64
#else
# define MPFR_LDBL_MANT_DIG   LDBL_MANT_DIG
#endif

/* LONGDOUBLE_NAN_ACTION executes the code "action" if x is a NaN. */

/* On hppa2.0n-hp-hpux10 with the unbundled HP cc, the test x!=x on a NaN
   has been seen false, meaning NaNs are not detected.  This seemed to
   happen only after other comparisons, not sure what's really going on.  In
   any case we can pick apart the bytes to identify a NaN.  */
#if HAVE_LDOUBLE_IEEE_QUAD_BIG
# define LONGDOUBLE_NAN_ACTION(x, action)                        \
  do {                                                          \
    union {                                                     \
      long double    ld;                                        \
      struct {                                                  \
        unsigned long  sign : 1;                                \
        unsigned long  exp  : 15;                               \
        unsigned long  man3 : 16;                               \
        unsigned long  man2 : 32;                               \
        unsigned long  man1 : 32;                               \
        unsigned long  man0 : 32;                               \
      } s;                                                      \
    } u;                                                        \
    u.ld = (x);                                                 \
    if (u.s.exp == 0x7FFFL                                      \
        && (u.s.man0 | u.s.man1 | u.s.man2 | u.s.man3) != 0)    \
      { action; }                                               \
  } while (0)
#endif

/* Under IEEE rules, NaN is not equal to anything, including itself.
   "volatile" here stops "cc" on mips64-sgi-irix6.5 from optimizing away
   x!=x. */
#ifndef LONGDOUBLE_NAN_ACTION
# define LONGDOUBLE_NAN_ACTION(x, action)                \
  do {                                                  \
    volatile long double __x = LONGDOUBLE_VOLATILE (x); \
    if ((x) != __x)                                     \
      { action; }                                       \
  } while (0)
# define WANT_LONGDOUBLE_VOLATILE 1
#endif

/* If we don't have a proper "volatile" then volatile is #defined to empty,
   in this case call through an external function to stop the compiler
   optimizing anything. */
#if WANT_LONGDOUBLE_VOLATILE
# ifdef volatile
long double __gmpfr_longdouble_volatile _MPFR_PROTO ((long double)) ATTRIBUTE_CONST;
#  define LONGDOUBLE_VOLATILE(x)  (__gmpfr_longdouble_volatile (x))
#  define WANT_GMPFR_LONGDOUBLE_VOLATILE 1
# else
#  define LONGDOUBLE_VOLATILE(x)  (x)
# endif
#endif

/* Debug non IEEE floats */
#ifdef XDEBUG
# undef _GMP_IEEE_FLOATS
#endif
#ifndef _GMP_IEEE_FLOATS
# define _GMP_IEEE_FLOATS 0
#endif

/* We want to test this :
 *  (rnd == GMP_RNDU && test) || (rnd == RNDD && !test)
 * It transforms RNDU or RNDD to Away or Zero according to the sign */
#define MPFR_IS_RNDUTEST_OR_RNDDNOTTEST(rnd, test) \
  (((rnd) + (test)) == GMP_RNDD)

/* Calcul s = (-a) % BITS_PER_MP_LIMB
 * a is unsigned! Check if it works, 
 * otherwise tries another way to calcul it */
#define MPFR_UNSIGNED_MINUS_MODULO(s, a) \
  do { \
  if ((UINT_MAX % BITS_PER_MP_LIMB) == (BITS_PER_MP_LIMB-1) \
      && ((-(unsigned) 1)%BITS_PER_MP_LIMB > 0)) \
    (s) = (-(a)) % BITS_PER_MP_LIMB; \
  else \
    {(s) = (a) % BITS_PER_MP_LIMB;  \
    if (s)  \
      (s) = BITS_PER_MP_LIMB - (s); \
    } \
  MPFR_ASSERTD( (s) >= 0 && (s) < BITS_PER_MP_LIMB); \
  } while (0) 


/* Definition of the special values of the exponent */
/* 
 * Clear flags macros are still defined and should be still used
 * since the functions must not assume the internal format.
 * How to deal with special values ?
 *  1. Check if is a special value (Zero, Nan, Inf) wiht MPFR_IS_SINGULAR
 *  2. Deal with the special value with MPFR_IS_NAN, MPFR_IS_INF, etc
 *  3. Else clear the flags of the dest (it must be done after since src 
 *     may be also the dest!)
 * MPFR_SET_INF, MPFR_SET_NAN, MPFR_SET_ZERO must clear by 
 * themselves the other flags.
 */

#define MPFR_PREC(x) ((x)->_mpfr_prec)
#define MPFR_EXP(x) ((x)->_mpfr_exp)
#define MPFR_MANT(x) ((x)->_mpfr_d)

/* Old ESIZE */
#define MPFR_LIMB_SIZE(x) ((MPFR_PREC((x)) - 1) / BITS_PER_MP_LIMB + 1)

/* Enum special value of exponent.*/
# define MPFR_EXP_ZERO (MPFR_EXP_MIN+1)
# define MPFR_EXP_NAN  (MPFR_EXP_MIN+2)
# define MPFR_EXP_INF  (MPFR_EXP_MIN+3)

#define MPFR_CLEAR_FLAGS(x)

#define MPFR_IS_NAN(x)   (MPFR_EXP(x) == MPFR_EXP_NAN)
#define MPFR_SET_NAN(x)  (MPFR_EXP(x) =  MPFR_EXP_NAN)
#define MPFR_IS_INF(x)   (MPFR_EXP(x) == MPFR_EXP_INF)
#define MPFR_SET_INF(x)  (MPFR_EXP(x) =  MPFR_EXP_INF)
#define MPFR_IS_ZERO(x)  (MPFR_EXP(x) == MPFR_EXP_ZERO)
#define MPFR_SET_ZERO(x) (MPFR_EXP(x) =  MPFR_EXP_ZERO)
#define MPFR_NOTZERO(x)  (MPFR_EXP(x) != MPFR_EXP_ZERO)

#define MPFR_IS_FP(x)       (!MPFR_IS_NAN(x) && !MPFR_IS_INF(x))
#define MPFR_IS_SINGULAR(x) (MPFR_EXP(x) <= MPFR_EXP_INF)
#define MPFR_IS_PURE_FP(x)  (!MPFR_IS_SINGULAR(x))

#define MPFR_ARE_SINGULAR(x,y) \
  (MPFR_UNLIKELY(MPFR_IS_SINGULAR(x)) || MPFR_UNLIKELY(MPFR_IS_SINGULAR(y)))

#define MPFR_SIGN_POS (1)
#define MPFR_SIGN_NEG (-1)

#define MPFR_IS_STRICTPOS(x)  (MPFR_NOTZERO((x)) && MPFR_SIGN(x) > 0)
#define MPFR_IS_STRICTNEG(x)  (MPFR_NOTZERO((x)) && MPFR_SIGN(x) < 0)

#define MPFR_IS_NEG(x) (MPFR_SIGN(x) < 0)
#define MPFR_IS_POS(x) (MPFR_SIGN(x) > 0)

#define MPFR_SET_POS(x) (MPFR_SIGN(x) = MPFR_SIGN_POS)
#define MPFR_SET_NEG(x) (MPFR_SIGN(x) = MPFR_SIGN_NEG)

#define MPFR_CHANGE_SIGN(x) (MPFR_SIGN(x) = -MPFR_SIGN(x))
#define MPFR_SET_SAME_SIGN(x, y) (MPFR_SIGN(x) = MPFR_SIGN(y))
#define MPFR_SET_OPPOSITE_SIGN(x, y) (MPFR_SIGN(x) = -MPFR_SIGN(y))
#define MPFR_ASSERT_SIGN(s) \
 (MPFR_ASSERTD((s) == MPFR_SIGN_POS || (s) == MPFR_SIGN_NEG))
#define MPFR_SET_SIGN(x, s) \
  (MPFR_ASSERT_SIGN(s), MPFR_SIGN(x) = s)
#define MPFR_IS_POS_SIGN(s1) (s1 > 0)
#define MPFR_IS_NEG_SIGN(s1) (s1 < 0)
#define MPFR_MULT_SIGN(s1, s2) ((s1) * (s2))
/* Transform a sign to 1 or -1 */
#define MPFR_FROM_SIGN_TO_INT(s) (s)
#define MPFR_INT_SIGN(x) MPFR_FROM_SIGN_TO_INT(MPFR_SIGN(x))

/* Special inexact value */
#define MPFR_EVEN_INEX 2

/* When returning the ternary inexact value, ALWAYS use one of the
   following two macros, unless the flag comes from another function
   returning the ternary inexact value */
#define MPFR_RET(I) return \
  (I) ? ((__gmpfr_flags |= MPFR_FLAGS_INEXACT), (I)) : 0
#define MPFR_RET_NAN return (__gmpfr_flags |= MPFR_FLAGS_NAN), 0

/* Heap Memory gestion */
#define MPFR_GET_ALLOC_SIZE(x) \
 ( ((mp_size_t*) MPFR_MANT(x))[-1] + 0)
#define MPFR_SET_ALLOC_SIZE(x, n) \
 ( ((mp_size_t*) MPFR_MANT(x))[-1] = n)
#define MPFR_MALLOC_SIZE(s) \
  ((size_t) (sizeof(mp_size_t)*1 + BYTES_PER_MP_LIMB*(s)))
#define MPFR_SET_MANT_PTR(x,p) \
   (MPFR_MANT(x) = (mp_limb_t*) ((mp_size_t*) p + 1))
#define MPFR_GET_REAL_PTR(x) \
   ((mp_limb_t*) ((mp_size_t*) MPFR_MANT(x) - 1))

/* Temporary memory gestion */
/* temporary allocate 1 limb at xp, and initialize mpfr variable x */
/* The temporary var doesn't have any size field, but it doesn't matter
 * since only functions dealing with the Heap care about it */
#define MPFR_TMP_INIT1(xp, x, p) \
 ( MPFR_PREC(x) = (p), \
   MPFR_MANT(x) = (xp), \
   MPFR_SET_POS(x), \
   MPFR_SET_INVALID_EXP(x)) 

#define MPFR_TMP_INIT(xp, x, p, s) \
  (xp = (mp_ptr) TMP_ALLOC(BYTES_PER_MP_LIMB*(s)), \
   MPFR_TMP_INIT1(xp, x, p))

#if defined (__cplusplus)
extern "C" {
#endif

extern mpfr_t __mpfr_const_log2;
extern mpfr_prec_t __gmpfr_const_log2_prec;

extern mpfr_t __mpfr_const_pi;
extern mpfr_prec_t __gmpfr_const_pi_prec;

#ifdef HAVE_STRCASECMP
int strcasecmp _MPFR_PROTO ((const char *, const char *));
#else
int mpfr_strcasecmp _MPFR_PROTO ((const char *, const char *));
#endif

#ifdef HAVE_STRNCASECMP
int strncasecmp _MPFR_PROTO ((const char *, const char *, size_t));
#else
int mpfr_strncasecmp _MPFR_PROTO ((const char *, const char *, size_t));
#endif

void mpfr_inits2 _MPFR_PROTO ((mp_prec_t, mpfr_ptr, ...));
void mpfr_inits _MPFR_PROTO ((mpfr_ptr, ...));
void mpfr_clears _MPFR_PROTO ((mpfr_ptr, ...));

int mpfr_set_underflow _MPFR_PROTO ((mpfr_ptr, mp_rnd_t, int));
int mpfr_set_overflow _MPFR_PROTO ((mpfr_ptr, mp_rnd_t, int));
void mpfr_save_emin_emax _MPFR_PROTO ((void));
void mpfr_restore_emin_emax _MPFR_PROTO ((void));

int mpfr_add1 _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_srcptr, mp_rnd_t));
int mpfr_sub1 _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_srcptr, mp_rnd_t));
int mpfr_sub1sp _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_srcptr, mp_rnd_t));
int mpfr_can_round_raw _MPFR_PROTO ((mp_limb_t *, mp_size_t, int, 
				     mp_exp_t, mp_rnd_t, mp_rnd_t, mp_prec_t));

double mpfr_get_d3 _MPFR_PROTO ((mpfr_srcptr, mp_exp_t, mp_rnd_t));
int mpfr_cmp2 _MPFR_PROTO ((mpfr_srcptr, mpfr_srcptr, mp_prec_t *));

long          __gmpfr_ceil_log2 _MPFR_PROTO ((double));
long          __gmpfr_floor_log2 _MPFR_PROTO ((double));
double        __gmpfr_ceil_exp2 _MPFR_PROTO ((double));
unsigned long __gmpfr_isqrt _MPFR_PROTO ((unsigned long));
unsigned long __gmpfr_cuberoot _MPFR_PROTO ((unsigned long));

int mpfr_exp_2 _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mp_rnd_t));
int mpfr_exp3 _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mp_rnd_t));
int mpfr_powerof2_raw _MPFR_PROTO ((mpfr_srcptr));

void mpfr_setmax _MPFR_PROTO ((mpfr_ptr, mp_exp_t));
void mpfr_setmin _MPFR_PROTO ((mpfr_ptr, mp_exp_t));

long mpn_exp _MPFR_PROTO ((mp_limb_t *, mp_exp_t *, int, 
			   mp_exp_t, size_t));

void mpfr_print_binary _MPFR_PROTO ((mpfr_srcptr));
void mpfr_print_mant_binary _MPFR_PROTO ((__gmp_const char *, 
					  __gmp_const mp_limb_t *, mp_prec_t));
void mpfr_set_str_binary _MPFR_PROTO ((mpfr_ptr, __gmp_const char *));

int mpfr_round_raw _MPFR_PROTO ((mp_limb_t *, mp_limb_t *,
				 mp_prec_t, int, mp_prec_t, mp_rnd_t, int *));
int mpfr_round_raw_2 _MPFR_PROTO ((mp_limb_t *, mp_limb_t *,
				   mp_prec_t, int, mp_prec_t, mp_rnd_t));
int mpfr_round_raw_3 _MPFR_PROTO ((mp_limb_t *, mp_limb_t *,
				  mp_prec_t, int, mp_prec_t, mp_rnd_t, int *));
int mpfr_round_raw_4 _MPFR_PROTO ((mp_limb_t *, mp_limb_t *,
				   mp_prec_t, int, mp_prec_t, mp_rnd_t));

#define mpfr_round_raw2(xp, xn, neg, r, prec) \
  mpfr_round_raw_2((mp_limb_t*)0,(xp),(xn)*BITS_PER_MP_LIMB,(neg),(prec),(r))

int mpfr_check _MPFR_PROTO ((mpfr_srcptr));
int mpfr_sub1sp _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_srcptr, mp_rnd_t));

#if defined (__cplusplus)
}
#endif

#endif
