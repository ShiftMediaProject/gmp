/* mpfr_atan -- arc-tangent of a floating-point number

Copyright 2001 Free Software Foundation.

This file is part of the MPFR Library, and was contributed by Mathieu Dutour.

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

#include <stdio.h>
#include <stdlib.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

#define CST   2.27  /* CST=1+ln(2.4)/ln(2) */
#define CST2  1.45  /* CST2=1/ln(2) */

static int mpfr_atan_aux _PROTO((mpfr_ptr, mpz_srcptr, int, int));

#undef B
#define A
#define A1 1
#define A2 2
#define C
#define C1  3
#define C2  2
#define NO_FACTORIAL
#define GENERIC mpfr_atan_aux 
#include "generic.c"
#undef C
#undef C1
#undef C2
#undef A
#undef A1
#undef A2
#undef NO_FACTORIAL
#undef GENERIC

int
mpfr_atan (mpfr_ptr arctangent, mpfr_srcptr x, mp_rnd_t rnd_mode)
{
  mpfr_t Pisur2;
  mpfr_t xp;
  mpfr_t arctgt;

  int comparaison, signe, suplement;

  mpfr_t t_arctan;
  int i;
  mpz_t ukz; 
  mpfr_t ukf;
  mpfr_t sk,Ak;
  mpz_t square;
  mpfr_t tmp_arctan;
  mpfr_t tmp, tmp2;
#ifdef DEBUG
  mpfr_t tst;
#endif
  int twopoweri;
  int Prec;
  int prec_x;
  int prec_arctan;
  int good = 0;
  int realprec;
  int estimated_delta;
  /* calculation of the floor */
  mp_exp_t exptol;

  int N0;
  int logn;

  /* Trivial cases */
  if (MPFR_IS_NAN(x))
    {
      MPFR_SET_NAN(arctangent);
      MPFR_RET_NAN;
    }

  if (MPFR_IS_INF(x))
    {
      MPFR_CLEAR_FLAGS(arctangent);
      if (MPFR_SIGN(x) > 0) /* arctan(+inf) = Pi/2 */
	mpfr_const_pi (arctangent, rnd_mode);
      else /* arctan(-inf) = -Pi/2 */
	{
	  if (rnd_mode == GMP_RNDU)
	    rnd_mode = GMP_RNDD;
	  else if (rnd_mode == GMP_RNDD)
	    rnd_mode = GMP_RNDU;
	  mpfr_const_pi (arctangent, rnd_mode);
	}
      MPFR_EXP(arctangent)--;
      return 1; /* inexact */
    }

  MPFR_CLEAR_FLAGS(arctangent);
  if (MPFR_IS_ZERO(x))
    {
      mpfr_set_ui (arctangent, 0, GMP_RNDN);
      return 0; /* exact result */
    }

  signe = MPFR_SIGN(x);
  prec_arctan = MPFR_PREC(arctangent);

  /* Set x_p=|x| */
  mpfr_init2(xp, MPFR_PREC(x));
  mpfr_set(xp, x, rnd_mode);
  if (signe == -1)
      MPFR_CHANGE_SIGN(xp);

  /* Other simple case arctang(-+1)=-+pi/4 */
  comparaison=mpfr_cmp_ui(xp, 1);
  if (comparaison == 0) {
    mpfr_init2(Pisur2, prec_arctan);
    mpfr_const_pi(Pisur2, rnd_mode);
    mpfr_div_2ui(arctangent, Pisur2, 2, rnd_mode);
    if (signe == -1)
      MPFR_CHANGE_SIGN(arctangent);
    mpfr_clear(Pisur2);
    mpfr_clear(xp);
    return 0; /* Result correct */
  }
  if (comparaison > 0)
      suplement = 2;
  else
      suplement = 2-MPFR_EXP(xp);

  prec_x = _mpfr_ceil_log2 ((double) MPFR_PREC(x) / BITS_PER_MP_LIMB);
  logn = _mpfr_ceil_log2 ((double) prec_x);
  if (logn < 2) logn = 2;
  realprec = prec_arctan + _mpfr_ceil_log2((double) prec_arctan) + 4;
  mpz_init(ukz);
  mpz_init(square);


  while (!good){
    N0 = _mpfr_ceil_log2((double) realprec + suplement + CST);
    estimated_delta = 1 + suplement + _mpfr_ceil_log2((double) (3*N0-2));
    Prec = realprec+estimated_delta;

    /* Initialisation    */
    mpfr_init2(sk,Prec);
    mpfr_init2(ukf, Prec);
    mpfr_init2(t_arctan, Prec);
    mpfr_init2(tmp_arctan, Prec);
    mpfr_init2(tmp, Prec);
    mpfr_init2(tmp2, Prec);
    mpfr_init2(Ak, Prec);
    mpfr_init2(arctgt, Prec);

#ifdef DEBUG
    /* Tests  */
    mpfr_init2(tst, realprec);
#endif

    if (comparaison > 0)
      {
	mpfr_init2(Pisur2, Prec);
	mpfr_const_pi(Pisur2, GMP_RNDN);
	mpfr_div_2ui(Pisur2, Pisur2, 1, GMP_RNDN);
	mpfr_ui_div(sk, 1, xp, GMP_RNDN);
      }
    else
	mpfr_set(sk, xp, GMP_RNDN);

    /* Assignation  */
    mpfr_set_ui (tmp_arctan, 0, GMP_RNDN);
    twopoweri = 1;
    for(i = 0; i <= N0; i++){
      mpfr_mul_2ui(tmp, sk, twopoweri, GMP_RNDN);
      /* Calculation of  trunc(tmp) --> mpz */
      mpfr_trunc (ukf, tmp);
      exptol = mpfr_get_z_exp (ukz, ukf);
      if (exptol>0)
	  mpz_mul_2exp (ukz, ukz, exptol);
      else
	  mpz_tdiv_q_2exp (ukz, ukz, (unsigned long int) (-exptol));

      /* Calculation of arctan(Ak) */
      mpz_mul(square, ukz, ukz);
      mpz_neg(square, square);
      mpfr_atan_aux(t_arctan, square, 2*twopoweri, N0 - i);
      mpfr_set_z(Ak, ukz, GMP_RNDN);
      mpfr_div_2ui(Ak, Ak, twopoweri, GMP_RNDN);
      mpfr_mul(t_arctan, t_arctan, Ak, GMP_RNDN);

      /* Addition and iteration */
      mpfr_add(tmp_arctan, tmp_arctan, t_arctan, GMP_RNDN);
      if (i<N0)
	{
	  mpfr_sub(tmp, sk, Ak, GMP_RNDN);
	  mpfr_mul(tmp2, sk, Ak, GMP_RNDN);
	  mpfr_add_ui(tmp2, tmp2, 1, GMP_RNDN);
	  mpfr_div(sk, tmp, tmp2, GMP_RNDN);
	  twopoweri <<= 1;
	}
    }

    if (comparaison > 0)
      {
	mpfr_sub(arctgt, Pisur2, tmp_arctan, GMP_RNDN);
	if (signe == -1)
	    MPFR_CHANGE_SIGN(arctgt);
      }
    else
      {
	mpfr_set(arctgt, tmp_arctan, GMP_RNDN);
	if (signe == -1)
	    MPFR_CHANGE_SIGN(arctgt);
      }

#ifdef DEBUG
    mpfr_set(tst, arctgt, rnd_mode);
#endif

    if (mpfr_can_round(arctgt, realprec, GMP_RNDN, rnd_mode, MPFR_PREC(arctangent)))
      {
	mpfr_set(arctangent, arctgt, rnd_mode);
	good = 1;
	realprec += 1;
      }
    else
      {
	realprec += _mpfr_ceil_log2 ((double) realprec);
      }

    mpfr_clear(sk);
    mpfr_clear(ukf);
    mpfr_clear(t_arctan);
    mpfr_clear(tmp_arctan);
    mpfr_clear(tmp);
    mpfr_clear(tmp2);
    mpfr_clear(Ak);
    mpfr_clear(arctgt);

#ifdef DEBUG  
    mpfr_clear(tst);
#endif
    if (comparaison > 0)
      mpfr_clear(Pisur2);
  }

  mpfr_clear(xp);
  mpz_clear(ukz);
  mpz_clear(square);

  return 1; /* inexact result */
}
