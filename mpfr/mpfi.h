/* mpfi.h -- Include file for mpfi.
 
Copyright 1999 PolKA project, Inria Lorraine and Loria

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
along with the MPFR Library; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#ifndef _mpfi_h
#define _mpfi_h

#define MPFI_RNDD GMP_RNDD
#define MPFI_RNDU GMP_RNDU

#include <math.h>

/* Machine dependant implementations */

#include "gmp.h"
#include "mpfr.h"

typedef struct {
  __mpfr_struct left;
  __mpfr_struct right;
}__mpfi_struct;

typedef __mpfi_struct mpfi_t[1];
typedef __mpfi_struct *mpfi_ptr;
typedef __gmp_const __mpfi_struct *mpfi_srcptr;

/* Gestion de la precision */

void mpfi_set_prec(const unsigned int);

unsigned int mpfi_get_prec();

/* Gestion de memoire */

void mpfi_clear(mpfi_ptr);

/* initializations */

void   mpfi_init    (mpfi_ptr);
void   mpfi_set     (mpfi_ptr, mpfi_srcptr);
void   mpfi_set_si  (mpfi_ptr, const long);
void   mpfi_set_ui  (mpfi_ptr, const unsigned long);
void   mpfi_set_d   (mpfi_ptr, const double);


/* void   mpfi_set_str (mpfi_ptr, const char *); */

/* arithmetic operations */

void   mpfi_add     (mpfi_ptr, mpfi_srcptr, mpfi_srcptr);
void   mpfi_sub     (mpfi_ptr, mpfi_srcptr, mpfi_srcptr);
void   mpfi_mul     (mpfi_ptr, mpfi_srcptr, mpfi_srcptr);
void   mpfi_div     (mpfi_ptr, mpfi_srcptr, mpfi_srcptr);

void   mpfi_add_d   (mpfi_ptr, mpfi_srcptr, const double);
void   mpfi_sub_d   (mpfi_ptr, mpfi_srcptr, const double);
void   mpfi_mul_d   (mpfi_ptr, mpfi_srcptr, const double);
void   mpfi_div_d   (mpfi_ptr, mpfi_srcptr, const double);

#define mpfi_add_si(a,b,c) mpfi_add_d(a,b,(double) c);
#define mpfi_sub_si(a,b,c) mpfi_sub_d(a,b,(double) c);
#define mpfi_mul_si(a,b,c) mpfi_mul_d(a,b,(double) c);
#define mpfi_div_si(a,b,c) mpfi_div_d(a,b,(double) c);

/* destructive arithmetic operations */

void   mpfi_dadd    (mpfi_ptr, mpfi_srcptr);
void   mpfi_dsub    (mpfi_ptr, mpfi_srcptr);
void   mpfi_dmul    (mpfi_ptr, mpfi_srcptr);
void   mpfi_ddiv    (mpfi_ptr, mpfi_srcptr);

void   mpfi_dadd_d  (mpfi_ptr, const double);
void   mpfi_dsub_d  (mpfi_ptr, const double);
void   mpfi_dmul_d  (mpfi_ptr, const double);
void   mpfi_ddiv_d  (mpfi_ptr, const double);

#define mpfi_dadd_si(a,c) mpfi_dadd_d(a,(double) c);
#define mpfi_dsub_si(a,c) mpfi_dsub_d(a,(double) c);
#define mpfi_dmul_si(a,c) mpfi_dmul_d(a,(double) c);
#define mpfi_ddiv_si(a,c) mpfi_ddiv_d(a,(double) c);

/* destructive composite arithmetic operations */


/* various operations */

void mpfi_neg(mpfi_ptr, mpfi_srcptr);
void mpfi_inv(mpfi_ptr, mpfi_srcptr);
void mpfi_mul_2exp(mpfi_ptr, mpfi_srcptr,unsigned long);

/* interval manipulation */

void   mpfi_set_left  (mpfi_ptr, mpfr_srcptr);
void   mpfi_set_right (mpfi_ptr, mpfr_srcptr);
void   mpfi_get_left  (mpfi_srcptr, mpfr_ptr);
void   mpfi_get_right (mpfi_srcptr, mpfr_ptr);


/* customizable comparison functions */

int    (*mpfi_cmp)  (mpfi_srcptr,mpfi_srcptr);
int    (*mpfi_cmp_d)   (mpfi_srcptr,const double);
int    (*mpfi_cmp_ui)  (mpfi_srcptr,const unsigned long);
int    (*mpfi_cmp_si)  (mpfi_srcptr,const long);

int    (*mpfi_is_pos)  (mpfi_srcptr);
int    (*mpfi_is_neg)  (mpfi_srcptr);
int    (*mpfi_is_zero) (mpfi_srcptr);


/* Customizable error handling */

#ifdef MPFI_USE_CHECK_ERROR
int    reset_error();
int    get_error();
char * get_error_str();
#endif

#endif
