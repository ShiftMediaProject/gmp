/* Utilities for MPFR developers, not exported.

Copyright (C) 1999-2000 Free Software Foundation.

This file is part of the MPFR Library.

The MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */


/* Fudges */
#define _mp_allocate_func      __gmp_allocate_func
#define _mp_reallocate_func    __gmp_reallocate_func
#define _mp_free_func          __gmp_free_func
#define _mp_default_allocate   __gmp_default_allocate
#define _mp_default_reallocate __gmp_default_reallocate
#define _mp_default_free       __gmp_default_free





/* Definition of constants */

#define LOG2 0.69314718055994528622 /* log(2) rounded to zero on 53 bits */

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
#define MPFR_CLEAR_FLAGS(x) (((x) -> _mpfr_size &= ~(3 << 29)))
#define MPFR_IS_NAN(x) (((x)->_mpfr_size >> 30)&1)
#define MPFR_SET_NAN(x) ((x)->_mpfr_size |= (1<<30))
#define MPFR_CLEAR_NAN(x) (((x) -> _mpfr_size &= ~(1 << 30)))
#define MPFR_IS_INF(x) (((x)->_mpfr_size >> 29)&1)
#define MPFR_SET_INF(x) ((x)->_mpfr_size |= (1<<29))
#define MPFR_CLEAR_INF(x) ((x)->_mpfr_size &= ~(1 << 29))
#define MPFR_IS_FP(x) ((((x) -> _mpfr_size >> 29) & 3) == 0)
#define MPFR_ABSSIZE(x) ((x)->_mpfr_size & ((1<<29)-1))
#define MPFR_SIZE(x) ((x)->_mpfr_size)
#define MPFR_EXP(x) ((x)->_mpfr_exp)
#define MPFR_MANT(x) ((x)->_mpfr_d)
#define MPFR_ISNONNEG(x) (MPFR_NOTZERO((x)) && MPFR_SIGN(x)>=0)
#define MPFR_ISNEG(x) (MPFR_NOTZERO((x)) && MPFR_SIGN(x)==-1)
#define MPFR_CHANGE_SIGN(x) (MPFR_SIZE(x) ^= (((mp_size_t)1)<<31))
#define MPFR_SET_SAME_SIGN(x, y) if (MPFR_SIGN((x)) != MPFR_SIGN((y))) { MPFR_CHANGE_SIGN((x)); }
#define MPFR_PREC(x) ((x)->_mpfr_prec)
#define MPFR_NOTZERO(x) (MPFR_MANT(x)[(MPFR_PREC(x)-1)/BITS_PER_MP_LIMB])
#define MPFR_IS_ZERO(x) ((MPFR_NOTZERO(x))==0)
#define MPFR_SET_ZERO(x) (MPFR_MANT(x)[(MPFR_PREC(x)-1)/BITS_PER_MP_LIMB] = 0)
#define mpfr_sgn(x) ((MPFR_NOTZERO(x)) ? MPFR_SIGN(x) : 0)

/* Memory gestion */

/* temporary allocate s limbs at xp, and initialize mpfr variable x */
#define MPFR_INIT(xp, x, p, s) xp = (mp_ptr) TMP_ALLOC(s*BYTES_PER_MP_LIMB); \
   MPFR_PREC(x) = p; MPFR_MANT(x) = xp; MPFR_SIZE(x) = s; MPFR_EXP(x) = 0; 
/* same when xp is already allocated */
#define MPFR_INIT1(xp, x, p, s) \
   MPFR_PREC(x)=p; MPFR_MANT(x)=xp; MPFR_SIZE(x)=s;

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

int mpfr_round_raw _PROTO ((mp_limb_t *, mp_limb_t *, mp_prec_t, int,
			     mp_prec_t, mp_rnd_t));
int mpfr_round_raw2 _PROTO((mp_limb_t *, mp_prec_t, int, mp_rnd_t, mp_prec_t));
int mpfr_can_round_raw _PROTO ((mp_limb_t *, mp_prec_t, int, mp_prec_t, 
				mp_rnd_t, mp_rnd_t, mp_prec_t));
double mpfr_get_d2 _PROTO ((mpfr_srcptr, long)); 
mp_size_t mpn_sqrtrem_new _PROTO ((mp_limb_t *, mp_limb_t *, mp_limb_t *, mp_size_t));
int mpfr_cmp2 _PROTO ((mpfr_srcptr, mpfr_srcptr));
long _mpfr_ceil_log2 _PROTO ((double));
long _mpfr_floor_log2 _PROTO ((double));
double _mpfr_ceil_exp2 _PROTO ((double));
unsigned long _mpfr_isqrt _PROTO ((unsigned long));
unsigned long _mpfr_cuberoot _PROTO ((unsigned long));

#if defined (__cplusplus)
}
#endif  

