/* mpfr.h -- Include file for mpfr.

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

#ifndef __MPFR_H
#define __MPFR_H

/* Define MPFR version number */
#define MPFR_VERSION_MAJOR 2
#define MPFR_VERSION_MINOR 1
#define MPFR_VERSION_PATCHLEVEL 0

/* Macros dealing with MPFR VERSION */
#define MPFR_VERSION_NUM(a,b,c) (((a) << 16L) | ((b) << 8) | (c))
#define MPFR_VERSION MPFR_VERSION_NUM(MPFR_VERSION_MAJOR,MPFR_VERSION_MINOR,MPFR_VERSION_PATCHLEVEL)

/* Check if GMP is included, and try to include it (Works with local GMP) */
#ifndef __GMP_H__
# include <gmp.h>
#endif

/* Check if stdio.h is included */
#if defined (EOF)
# define _MPFR_H_HAVE_FILE 1
#endif

/* Definition of rounding modes */
typedef enum {
  GMP_RNDN=0,
  GMP_RNDZ=1,
  GMP_RNDU=2,
  GMP_RNDD=3
} mpfr_rnd_t;

/* Flags of __gmpfr_flags */
#define MPFR_FLAGS_UNDERFLOW 1
#define MPFR_FLAGS_OVERFLOW 2
#define MPFR_FLAGS_NAN 4
#define MPFR_FLAGS_INEXACT 8
#define MPFR_FLAGS_ALL 15

/* Define precision : 1 (short), 2 (int) or 3 (long) */
#ifndef MPFR_PREC_FORMAT
# if __GMP_MP_SIZE_T_INT == 1
#  define MPFR_PREC_FORMAT 2
# else
#  define MPFR_PREC_FORMAT 3
# endif
#endif

#if   MPFR_PREC_FORMAT == 1
typedef unsigned short mpfr_prec_t;
#elif MPFR_PREC_FORMAT == 2
typedef unsigned int   mpfr_prec_t;
#elif MPFR_PREC_FORMAT == 3
typedef unsigned long  mpfr_prec_t;
#else
# error "Invalid MPFR Prec format"
#endif

/* Definition of precision limits */
#define MPFR_PREC_MIN 2
#define MPFR_PREC_MAX ((mpfr_prec_t)(~(mpfr_prec_t)0) >>1)

/* Definition of sign */
typedef int          mpfr_sign_t;

/* Definition of the standard exponent limits */
#define MPFR_EMAX_DEFAULT ((mp_exp_t) (((unsigned long) 1 << 30) - 1))
#define MPFR_EMIN_DEFAULT (-(MPFR_EMAX_DEFAULT))

/* Definition of the intervals of the exponent limits */
#define MPFR_EMIN_MIN MPFR_EMIN_DEFAULT
#define MPFR_EMIN_MAX MPFR_EMAX_DEFAULT
#define MPFR_EMAX_MIN MPFR_EMIN_DEFAULT
#define MPFR_EMAX_MAX MPFR_EMAX_DEFAULT

/* Definition of the main structure */
typedef struct {
  mpfr_prec_t  _mpfr_prec; 
  mpfr_sign_t  _mpfr_sign;
  mp_exp_t     _mpfr_exp;
  mp_limb_t   *_mpfr_d;
} __mpfr_struct;

/* Compatibility with previous types of MPFR */
#define mp_rnd_t  mpfr_rnd_t
#define mp_prec_t mpfr_prec_t
#define __gmp_default_rounding_mode __gmpfr_default_rounding_mode
#define __mpfr_emin __gmpfr_emin
#define __mpfr_emax __gmpfr_emax
#define __mpfr_flags __gmpfr_flags
#define  __mpfr_default_fp_bit_precision __gmpfr_default_fp_bit_precision

/*
   The represented number is

    _mpfr_sign*(_mpfr_d[k-1]/B+_mpfr_d[k-2]/B^2+...+_mpfr_d[0]/B^k)*2^_mpfr_exp

   where k=ceil(_mp_prec/BITS_PER_MP_LIMB) and B=2^BITS_PER_MP_LIMB.

   For the msb (most significant bit) normalized representation, we must have
   _mpfr_d[k-1]>=B/2, unless the number is zero (in that case its sign is still
   given by sign(_mpfr_size)).

   We must also have the last k*BITS_PER_MP_LIMB-_mp_prec bits set to zero.
*/

typedef __mpfr_struct mpfr_t[1];
typedef __mpfr_struct *mpfr_ptr;
typedef __gmp_const __mpfr_struct *mpfr_srcptr;

#define MPFR_SIGN(x) (((x)->_mpfr_sign))

/* size_t is defined by GMP */

/* Prototypes */
#if defined (__STDC__) || defined (__cplusplus)
# define _MPFR_PROTO(x) x
#else
# define _MPFR_PROTO(x) ()
#endif

#if defined (__cplusplus)
extern "C" {
#endif
  
extern unsigned int __gmpfr_flags;
extern mp_exp_t __gmpfr_emin;
extern mp_exp_t __gmpfr_emax;

mp_exp_t mpfr_get_emin _MPFR_PROTO ((void));
int mpfr_set_emin _MPFR_PROTO ((mp_exp_t));
mp_exp_t mpfr_get_emax _MPFR_PROTO ((void));
int mpfr_set_emax _MPFR_PROTO ((mp_exp_t));
void mpfr_clear_flags _MPFR_PROTO ((void));
void mpfr_clear_underflow _MPFR_PROTO ((void));
void mpfr_clear_overflow _MPFR_PROTO ((void));
void mpfr_clear_nanflag _MPFR_PROTO ((void));
void mpfr_clear_inexflag _MPFR_PROTO ((void));
int mpfr_check_range _MPFR_PROTO ((mpfr_ptr, int, mpfr_rnd_t));
int mpfr_underflow_p _MPFR_PROTO ((void));
int mpfr_overflow_p _MPFR_PROTO ((void));
int mpfr_nanflag_p _MPFR_PROTO ((void));
int mpfr_inexflag_p _MPFR_PROTO ((void));

void mpfr_init2 _MPFR_PROTO ((mpfr_ptr, mp_prec_t));
void mpfr_init _MPFR_PROTO ((mpfr_ptr));
int mpfr_prec_round _MPFR_PROTO ((mpfr_ptr, mp_prec_t, mpfr_rnd_t));
int mpfr_can_round _MPFR_PROTO ((mpfr_ptr, mp_exp_t, mpfr_rnd_t, mpfr_rnd_t,
			    mp_prec_t));
mp_exp_t mpfr_get_exp _MPFR_PROTO ((mpfr_srcptr));
int mpfr_set_exp _MPFR_PROTO ((mpfr_ptr, mp_exp_t));
int mpfr_set_d _MPFR_PROTO ((mpfr_ptr, double, mpfr_rnd_t));
int mpfr_set_ld _MPFR_PROTO ((mpfr_ptr, long double, mpfr_rnd_t));
int mpfr_set_z _MPFR_PROTO ((mpfr_ptr, mpz_srcptr, mpfr_rnd_t));
void mpfr_set_nan _MPFR_PROTO ((mpfr_ptr));
void mpfr_set_inf _MPFR_PROTO ((mpfr_ptr, int));
mp_exp_t mpfr_get_z_exp _MPFR_PROTO ((mpz_ptr, mpfr_srcptr));
int mpfr_set_q _MPFR_PROTO ((mpfr_ptr, mpq_srcptr, mpfr_rnd_t));
double mpfr_get_d _MPFR_PROTO ((mpfr_srcptr, mpfr_rnd_t));
long double mpfr_get_ld _MPFR_PROTO ((mpfr_srcptr, mpfr_rnd_t));
double mpfr_get_d1 _MPFR_PROTO ((mpfr_srcptr));
double mpfr_get_d_2exp _MPFR_PROTO ((long *, mpfr_srcptr, mpfr_rnd_t));
long mpfr_get_si _MPFR_PROTO ((mpfr_srcptr, mpfr_rnd_t));
unsigned long mpfr_get_ui _MPFR_PROTO ((mpfr_srcptr, mpfr_rnd_t));
int mpfr_set_f _MPFR_PROTO ((mpfr_ptr, mpf_srcptr, mpfr_rnd_t));
int mpfr_set_si _MPFR_PROTO ((mpfr_ptr, long, mpfr_rnd_t));
int mpfr_set_ui _MPFR_PROTO ((mpfr_ptr, unsigned long, mpfr_rnd_t));
void mpfr_random _MPFR_PROTO ((mpfr_ptr));
void mpfr_random2 _MPFR_PROTO ((mpfr_ptr, mp_size_t, mp_exp_t));
int mpfr_urandomb _MPFR_PROTO ((mpfr_ptr, gmp_randstate_t));
void mpfr_clear _MPFR_PROTO ((mpfr_ptr));
void mpfr_nextabove _MPFR_PROTO ((mpfr_ptr));
void mpfr_nextbelow _MPFR_PROTO ((mpfr_ptr));
void mpfr_nexttoward _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr));
int mpfr_set_str _MPFR_PROTO ((mpfr_ptr, __gmp_const char *, int, mpfr_rnd_t));
int mpfr_init_set_str _MPFR_PROTO ((mpfr_ptr, __gmp_const char *, int, 
				    mpfr_rnd_t));
char* mpfr_get_str _MPFR_PROTO ((char *, mp_exp_t *, int, size_t, mpfr_srcptr,
				 mpfr_rnd_t));
#ifdef _MPFR_H_HAVE_FILE
#define mpfr_inp_str mpfr_inp_str_internal
#define mpfr_out_str mpfr_out_str_internal
size_t mpfr_inp_str _MPFR_PROTO ((mpfr_ptr, FILE *, int, mpfr_rnd_t));
size_t mpfr_out_str _MPFR_PROTO ((FILE *, int, size_t, mpfr_srcptr, mpfr_rnd_t));
#endif
int mpfr_mul _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_pow_ui _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, unsigned long int, mpfr_rnd_t));
int mpfr_ui_pow_ui _MPFR_PROTO ((mpfr_ptr, unsigned long int, 
				 unsigned long int, mpfr_rnd_t));
int mpfr_div _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_agm _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_sqrt _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_sqrt_ui _MPFR_PROTO ((mpfr_ptr, unsigned long, mpfr_rnd_t));
int mpfr_add _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_add_ui _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, unsigned long, mpfr_rnd_t));
int mpfr_sub_ui _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, unsigned long, mpfr_rnd_t));
int mpfr_add_one_ulp _MPFR_PROTO ((mpfr_ptr, mpfr_rnd_t));
int mpfr_sub _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_ui_sub _MPFR_PROTO ((mpfr_ptr, unsigned long, mpfr_srcptr, mpfr_rnd_t));
void mpfr_reldiff _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_const_pi _MPFR_PROTO ((mpfr_ptr, mpfr_rnd_t));
int mpfr_const_log2 _MPFR_PROTO ((mpfr_ptr, mpfr_rnd_t));
int mpfr_const_euler _MPFR_PROTO ((mpfr_ptr, mpfr_rnd_t));
int mpfr_log _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_exp _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_exp2 _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_exp10 _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_sin _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_sin_cos _MPFR_PROTO ((mpfr_ptr, mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_cos _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_tan _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_mul_ui _MPFR_PROTO((mpfr_ptr, mpfr_srcptr, unsigned long int, mpfr_rnd_t));
int mpfr_cmp_ui_2exp _MPFR_PROTO ((mpfr_srcptr, unsigned long int, mp_exp_t));
int mpfr_cmp_si_2exp _MPFR_PROTO ((mpfr_srcptr, long int, mp_exp_t));
int mpfr_mul_2exp _MPFR_PROTO((mpfr_ptr, mpfr_srcptr, unsigned long int, mpfr_rnd_t));
int mpfr_div_2exp _MPFR_PROTO((mpfr_ptr, mpfr_srcptr, unsigned long int, mpfr_rnd_t));
int mpfr_mul_2ui _MPFR_PROTO((mpfr_ptr, mpfr_srcptr, unsigned long int, mpfr_rnd_t));
int mpfr_div_2ui _MPFR_PROTO((mpfr_ptr, mpfr_srcptr, unsigned long int, mpfr_rnd_t));
int mpfr_mul_2si _MPFR_PROTO((mpfr_ptr, mpfr_srcptr, long int, mpfr_rnd_t));
int mpfr_div_2si _MPFR_PROTO((mpfr_ptr, mpfr_srcptr, long int, mpfr_rnd_t));
void mpfr_set_prec _MPFR_PROTO((mpfr_ptr, mp_prec_t));
void mpfr_set_prec_raw _MPFR_PROTO((mpfr_ptr, mp_prec_t));
void mpfr_set_default_prec _MPFR_PROTO((mp_prec_t));
mp_prec_t mpfr_get_default_prec _MPFR_PROTO((void));
extern mp_prec_t __gmpfr_default_fp_bit_precision;
extern mpfr_rnd_t __gmpfr_default_rounding_mode;
__gmp_const char * mpfr_print_rnd_mode _MPFR_PROTO((mpfr_rnd_t));
int mpfr_neg _MPFR_PROTO((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_sub_one_ulp _MPFR_PROTO((mpfr_ptr, mpfr_rnd_t));
int mpfr_div_ui _MPFR_PROTO((mpfr_ptr, mpfr_srcptr, unsigned long int, mpfr_rnd_t));
int mpfr_ui_div _MPFR_PROTO((mpfr_ptr, unsigned long int, mpfr_srcptr, mpfr_rnd_t));
mp_prec_t mpfr_get_prec _MPFR_PROTO((mpfr_srcptr));
void mpfr_set_default_rounding_mode _MPFR_PROTO((mpfr_rnd_t));
int mpfr_eq _MPFR_PROTO((mpfr_srcptr, mpfr_srcptr, unsigned long));
int mpfr_rint _MPFR_PROTO((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_round _MPFR_PROTO((mpfr_ptr, mpfr_srcptr));
int mpfr_trunc _MPFR_PROTO((mpfr_ptr, mpfr_srcptr));
int mpfr_ceil _MPFR_PROTO((mpfr_ptr, mpfr_srcptr));
int mpfr_floor _MPFR_PROTO((mpfr_ptr, mpfr_srcptr));
int mpfr_frac _MPFR_PROTO((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_fits_ulong_p _MPFR_PROTO((mpfr_srcptr, mpfr_rnd_t));
int mpfr_fits_slong_p _MPFR_PROTO((mpfr_srcptr, mpfr_rnd_t));
int mpfr_fits_uint_p _MPFR_PROTO((mpfr_srcptr, mpfr_rnd_t));
int mpfr_fits_sint_p _MPFR_PROTO((mpfr_srcptr, mpfr_rnd_t));
int mpfr_fits_ushort_p _MPFR_PROTO((mpfr_srcptr, mpfr_rnd_t));
int mpfr_fits_sshort_p _MPFR_PROTO((mpfr_srcptr, mpfr_rnd_t));
void mpfr_extract _MPFR_PROTO((mpz_ptr, mpfr_srcptr, unsigned int));
void mpfr_swap _MPFR_PROTO((mpfr_ptr, mpfr_ptr));
void mpfr_dump _MPFR_PROTO((mpfr_srcptr, mpfr_rnd_t));
int mpfr_set4 _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t, int));
int mpfr_cmp3 _MPFR_PROTO ((mpfr_srcptr, mpfr_srcptr, int));
int mpfr_cmp_d _MPFR_PROTO ((mpfr_srcptr, double));
int mpfr_cmpabs _MPFR_PROTO ((mpfr_srcptr, mpfr_srcptr));
int mpfr_nan_p _MPFR_PROTO((mpfr_srcptr));
int mpfr_inf_p _MPFR_PROTO((mpfr_srcptr));
int mpfr_number_p _MPFR_PROTO((mpfr_srcptr));
int mpfr_acos _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_asin _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_atan _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_erf _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));

int mpfr_sinh _MPFR_PROTO((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_tanh _MPFR_PROTO((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_factorial _MPFR_PROTO ((mpfr_ptr, unsigned long int, mpfr_rnd_t));
int mpfr_ui_pow _MPFR_PROTO ((mpfr_ptr, unsigned long int, mpfr_srcptr, mpfr_rnd_t));

int mpfr_atanh _MPFR_PROTO((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_acosh _MPFR_PROTO((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_asinh _MPFR_PROTO((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));

int mpfr_cosh _MPFR_PROTO((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_sinh _MPFR_PROTO((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_tanh _MPFR_PROTO((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_asin _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_atan _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_fac_ui _MPFR_PROTO ((mpfr_ptr, unsigned long int, mpfr_rnd_t));
int mpfr_fma _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_srcptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_hypot _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_pow _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,mpfr_srcptr, mpfr_rnd_t));
int mpfr_pow_si _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, long int, mpfr_rnd_t));
int mpfr_integer_p _MPFR_PROTO ((mpfr_srcptr));
int mpfr_log2 _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_log10 _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_log1p _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_expm1 _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_cbrt _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_gamma _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_zeta _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));

int mpfr_min _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_max _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_dim _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_srcptr, mpfr_rnd_t));
int mpfr_copysign _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_srcptr, mpfr_rnd_t));

int mpfr_mul_z _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpz_srcptr, mpfr_rnd_t));
int mpfr_div_z _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpz_srcptr, mpfr_rnd_t));
int mpfr_add_z _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpz_srcptr, mpfr_rnd_t));
int mpfr_sub_z _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpz_srcptr, mpfr_rnd_t));

int mpfr_mul_q _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpq_srcptr, mpfr_rnd_t));
int mpfr_div_q _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpq_srcptr, mpfr_rnd_t));
int mpfr_add_q _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpq_srcptr, mpfr_rnd_t));
int mpfr_sub_q _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpq_srcptr, mpfr_rnd_t));

int mpfr_greater_p _MPFR_PROTO ((mpfr_srcptr, mpfr_srcptr));
int mpfr_greaterequal_p _MPFR_PROTO ((mpfr_srcptr, mpfr_srcptr));
int mpfr_less_p _MPFR_PROTO ((mpfr_srcptr, mpfr_srcptr));
int mpfr_lessequal_p _MPFR_PROTO ((mpfr_srcptr, mpfr_srcptr));
int mpfr_lessgreater_p _MPFR_PROTO ((mpfr_srcptr, mpfr_srcptr));
int mpfr_equal_p _MPFR_PROTO ((mpfr_srcptr, mpfr_srcptr));
int mpfr_unordered_p _MPFR_PROTO ((mpfr_srcptr, mpfr_srcptr));

int mpfr_sgn _MPFR_PROTO ((mpfr_srcptr));

#if defined (__cplusplus)
}
#endif

/* Compatibility with 2.0.1 */
#define mpfr_cmp_abs mpfr_cmpabs
#define mpfr_round_prec(x,r,p) mpfr_prec_round(x,p,r)

/* Prevent from using mpfr_get_e{min,max} as lvalues */
#define mpfr_get_emin() (__gmpfr_emin + 0)
#define mpfr_get_emax() (__gmpfr_emax + 0)

#define mpfr_clear_flags() \
  ((void) (__gmpfr_flags = 0))
#define mpfr_clear_underflow() \
  ((void) (__gmpfr_flags &= MPFR_FLAGS_ALL ^ MPFR_FLAGS_UNDERFLOW))
#define mpfr_clear_overflow() \
  ((void) (__gmpfr_flags &= MPFR_FLAGS_ALL ^ MPFR_FLAGS_OVERFLOW))
#define mpfr_clear_nanflag() \
  ((void) (__gmpfr_flags &= MPFR_FLAGS_ALL ^ MPFR_FLAGS_NAN))
#define mpfr_clear_inexflag() \
  ((void) (__gmpfr_flags &= MPFR_FLAGS_ALL ^ MPFR_FLAGS_INEXACT))
#define mpfr_underflow_p() \
  ((int) (__gmpfr_flags & MPFR_FLAGS_UNDERFLOW))
#define mpfr_overflow_p() \
  ((int) (__gmpfr_flags & MPFR_FLAGS_OVERFLOW))
#define mpfr_nanflag_p() \
  ((int) (__gmpfr_flags & MPFR_FLAGS_NAN))
#define mpfr_inexflag_p() \
  ((int) (__gmpfr_flags & MPFR_FLAGS_INEXACT))

#define mpfr_round(a,b) mpfr_rint((a), (b), -1)
#define mpfr_trunc(a,b) mpfr_rint((a), (b), GMP_RNDZ)
#define mpfr_ceil(a,b)  mpfr_rint((a), (b), GMP_RNDU)
#define mpfr_floor(a,b) mpfr_rint((a), (b), GMP_RNDD)

#define mpfr_cmp_ui(b,i) mpfr_cmp_ui_2exp((b),(i),0)
#define mpfr_cmp_si(b,i) mpfr_cmp_si_2exp((b),(i),0)
#define mpfr_set(a,b,r) mpfr_set4(a,b,r,MPFR_SIGN(b))
#define mpfr_abs(a,b,r) mpfr_set4(a,b,r,1)
#define mpfr_cmp(b, c) mpfr_cmp3(b, c, 1)
#define mpfr_mul_2exp(y,x,n,r) mpfr_mul_2ui((y),(x),(n),(r))
#define mpfr_div_2exp(y,x,n,r) mpfr_div_2ui((y),(x),(n),(r))

#define mpfr_init_set_si(x, i, rnd) \
 ( mpfr_init(x), mpfr_set_si((x), (i), (rnd)) )
#define mpfr_init_set_ui(x, i, rnd) \
 ( mpfr_init(x), mpfr_set_ui((x), (i), (rnd)) )
#define mpfr_init_set_d(x, d, rnd) \
 ( mpfr_init(x), mpfr_set_d((x), (d), (rnd)) )
#define mpfr_init_set_z(x, i, rnd) \
 ( mpfr_init(x), mpfr_set_z((x), (i), (rnd)) )
#define mpfr_init_set_q(x, i, rnd) \
 ( mpfr_init(x), mpfr_set_q((x), (i), (rnd)) )
#define mpfr_init_set(x, y, rnd) \
 ( mpfr_init(x), mpfr_set((x), (y), (rnd)) )
#define mpfr_init_set_f(x, y, rnd) \
 ( mpfr_init(x), mpfr_set_f((x), (y), (rnd)) )

#endif
