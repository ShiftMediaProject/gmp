/*  mpn_divrem_n -- Karatsuba division in 2*K(n) limb operations

Copyright (C) 1999-2000 PolKA project, Inria Lorraine and Loria

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

/*
[1] Fast Recursive Division, by Christoph Burnikel and Joachim Ziegler,
    Technical report MPI-I-98-1-022, october 1998,
    cf http://www.mpi-sb.mpg.de/~ziegler/TechRep.ps.gz.
    Implemented by Paul Zimmermann, 1999.
*/

#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"

extern void mpn_divrem_n2 (mp_limb_t *, mp_limb_t *, mp_limb_t *, mp_size_t, mp_limb_t *);
extern void mpn_divrem_3by2 (mp_limb_t *, mp_limb_t *, mp_limb_t *, mp_size_t, mp_limb_t *);

/* mpn_divrem_n(n) calls 2*mul(n/2)+2*div(n/2), thus to be faster then
   div(n)=4*div(n/2), we need mul(n/2) to be faster than the classic way,
   i.e. n/2 >= KARATSUBA_MUL_THRESHOLD */

#define DIV_LIMIT (7*KARATSUBA_MUL_THRESHOLD)

static void mpn_decr(mp_limb_t *Q)
{
  while ((*Q++)-- == 0);
}

/* implements algorithm of page 8 in [1]: divides (A,2n) by (B,n) and puts the
   quotient in (Q,n), the remainder in (A,n).
   Returns most significant limb of the quotient, which is 0 or 1.
*/
mp_limb_t
mpn_divrem_n(mp_limb_t *Q, mp_limb_t *A, mp_limb_t *B, mp_size_t n)
{
  if (n<DIV_LIMIT) return mpn_divrem(Q, 0, A, 2*n, B, n);
  else {
    mp_limb_t cc=0;
    if (mpn_cmp(A+n, B, n)>=0) {
      cc=1;
      mpn_sub_n(A+n, A+n, B, n);
    }
    if (n%2) {
       /* divide (2n-2) most significant limbs from A by those (n-1) from B */
       mpn_divrem_n(Q+1, A+2, B+1, n-1);
       /* now (Q+1, n-1) contains the quotient of (A+2,2n-2) by (B+1,n-1)
	  and (A+2, n-1) contains the remainder */
       if (mpn_sub_1(A+n, A+n, 1, mpn_submul_1(A+1, Q+1, n-1, B[0]))) { 
	 /* quotient two large */
	 mpn_decr(Q+1);
	 if (mpn_add_n(A+1, A+1, B, n)==0) {
	   mpn_decr(Q+1); mpn_add_n(A+1, A+1, B, n);
	 }
       }
       /* now divide (A,n+1) by (B,n) */
       mpn_divrem(Q, 0, A, n+1, B, n);
    }
    else { 
       mp_limb_t *tmp; int n2=n/2;
       TMP_DECL (marker); 

       TMP_MARK (marker);
       tmp = (mp_limb_t*) TMP_ALLOC(n*sizeof(mp_limb_t));
       mpn_divrem_3by2(Q+n2, A+n2, B, n2, tmp);
       mpn_divrem_3by2(Q, A, B, n2, tmp);
       TMP_FREE (marker);
    }
    return cc;
  }
}

/* inner procedure, with no memory allocation 
   assumes mpn_cmp(A+n, B, n) < 0
*/
void mpn_divrem_n2(mp_limb_t *Q, mp_limb_t *A, mp_limb_t *B, mp_size_t n,
			mp_limb_t *tmp)
{
  if (n%2) {
    /* divide (2n-2) most significant limbs from A by those (n-1) from B */
    mpn_divrem_n2(Q+1, A+2, B+1, n-1, tmp);
    /* now (Q+1, n-1) contains the quotient of (A+2,2n-2) by (B+1,n-1)
       and (A+2, n-1) contains the remainder */
    if (mpn_sub_1(A+n, A+n, 1, mpn_submul_1(A+1, Q+1, n-1, B[0]))) { 
      /* quotient two large */
      mpn_decr(Q+1);
      if (mpn_add_n(A+1, A+1, B, n)==0) { /* still too large */
	mpn_decr(Q+1); mpn_add_n(A+1, A+1, B, n);
      }
    }
    /* now divide (A,n+1) by (B,n) */
    mpn_divrem(Q, 0, A, n+1, B, n);
  }
  else {
    int n2=n/2;
    mpn_divrem_3by2(Q+n2, A+n2, B, n2, tmp);
    mpn_divrem_3by2(Q, A, B, n2, tmp);
  }
}

/* divides (A,3n) by (B,2n) and puts the quotient in (Q,n), 
   the remainder in (A,2n) */
void
mpn_divrem_3by2(mp_limb_t *Q, mp_limb_t *A, mp_limb_t *B, 
			     mp_size_t n, mp_limb_t *tmp)
{
  int twon = n+n;

  if (n<DIV_LIMIT) mpn_divrem(Q, 0, A+n, twon, B+n, n);
  else mpn_divrem_n2(Q, A+n, B+n, n, tmp);
  /* q=(Q,n), c=(A+n,n) with the notations of [1] */
  mpn_mul_n(tmp, Q, B, n);
  if (mpn_sub_n(A, A, tmp, twon)) /* R=(A,2n) */ {
    mpn_decr(Q);
    if (mpn_add_n(A, A, B, twon)==0) { /* Q still too large */
      mpn_decr(Q); mpn_add_n(A, A, B, twon);
    }
  }
  return;
}
