/* mpfr_zeta -- compute the Riemann Zeta function

Copyright 2003 Free Software Foundation.
Contributed by Jean-Luc Re'my and the Spaces project, INRIA Lorraine.

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

/* #define DEBUG */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

void mpfr_zeta_part_b _PROTO ((mpfr_t, mpfr_srcptr, int, int, mpfr_t *));
void mpfr_zeta_c      _PROTO ((int, mpfr_t *));
void mpfr_zeta_part_a _PROTO ((mpfr_t, mpfr_srcptr, int));
void mpfr_zeta_pos    _PROTO ((mpfr_t, mpfr_srcptr, mp_rnd_t));

/* 
   Parameters:
   s - the input floating-point number
   n, p - parameters from the algorithm
   tc - an array of p floating-point numbers tc[1]..tc[p] 
   Output:
   b is the result
*/
void
mpfr_zeta_part_b (mpfr_t b, mpfr_srcptr s, int n, int p, mpfr_t *tc)
{
  int n2, l, t, precb;
  mpfr_t s1, d, u;

  if (p == 0)
    {
      mpfr_set_ui (b, 0, GMP_RNDN);
      return;
    }

  n2 = n * n;
  precb = mpfr_get_prec (b);
  mpfr_init2 (s1, precb);
  mpfr_init2 (d, precb);
  mpfr_init2 (u, precb);
  /* t equals 2p-2, 2p-3, ... ; s1 equals s+t */
  t = 2 * p - 2;
  mpfr_set (d, tc[p], GMP_RNDN);
  for (l=1; l<p; l++)
    {
      mpfr_add_ui (s1, s, t, GMP_RNDN);
      mpfr_mul (d, d, s1, GMP_RNDN);
      t = t - 1;
      mpfr_add_ui (s1, s, t, GMP_RNDN);
      mpfr_mul (d, d, s1, GMP_RNDN);
      t = t - 1;
      mpfr_div_ui (d, d, n2, GMP_RNDN);
      mpfr_add (d, d, tc[p-l], GMP_RNDN);
      if (mpfr_cmpabs (d, tc[p-l]) > 0)
	mpfr_set (d, tc[p-l], GMP_RNDN); 
    }
  mpfr_mul (d, d, s, GMP_RNDN);
  mpfr_add_ui (s1, s, 1, GMP_RNDN);
  mpfr_neg (s1, s1, GMP_RNDN);
  mpfr_ui_pow (u, n, s1, GMP_RNDN);
  mpfr_mul (d, d, u, GMP_RNDN);
  mpfr_set (b, d, GMP_RNDN);
#ifdef DEBUG
  printf ("\npart b=");
  mpfr_out_str (stdout, 10, 0, b, GMP_RNDN);
  printf ("\n");
#endif
  mpfr_clear (s1);
  mpfr_clear (d);
  mpfr_clear (u);
}

/* Input: p - an integer
   Output: fills tc[1..p]
*/
void
mpfr_zeta_c (int p, mpfr_t *tc)
{
  mpfr_t d;
  int k, l;

  if (p > 0)
    {
      mpfr_init2 (d, mpfr_get_prec (tc[1]));
      mpfr_set_ui (tc[1], 1, GMP_RNDN);
      mpfr_div_ui (tc[1], tc[1], 12, GMP_RNDN);
      for (k=2; k<=p; k++)
	{
	  mpfr_set_ui (d, k-1, GMP_RNDN);
	  mpfr_div_ui (d, d, 12*k+6, GMP_RNDN);
	  for (l=2; l<=k-1; l++)
	    {
	      mpfr_div_ui (d, d, 4*(2*k-2*l+3)*(2*k-2*l+2), GMP_RNDN);
	      mpfr_add (d, d, tc[l], GMP_RNDN);
	    }
	  mpfr_div_ui (tc[k], d, 24, GMP_RNDN);
	  mpfr_neg (tc[k], tc[k], GMP_RNDN);
	}
      mpfr_clear(d);
    }
}

/* Input: s - a floating-point number
          n - an integer
   Output: sum - a floating-point number */
void
mpfr_zeta_part_a (mpfr_t sum, mpfr_srcptr s, int n)
{
  int i, preca;
  mpfr_t u, s1;

  preca = mpfr_get_prec (sum);
  mpfr_init2 (u, preca);
  mpfr_init2 (s1, preca);
  mpfr_neg (s1, s, GMP_RNDN);
  mpfr_ui_pow (u, n, s1, GMP_RNDN);
  mpfr_div_2exp (u, u, 1, GMP_RNDN);
  mpfr_set (sum, u, GMP_RNDN);
  for (i=n-1; i>1; i--)
    {
      mpfr_ui_pow (u, i, s1, GMP_RNDN);
      mpfr_add (sum, sum, u, GMP_RNDN);
    }
  mpfr_add_ui (sum, sum, 1, GMP_RNDN);
#ifdef DEBUG
  printf ("\npart a = ");
  mpfr_out_str (stdout, 10, 0, sum, GMP_RNDN);
  printf ("\n");
#endif
  mpfr_clear(s1);
  mpfr_clear(u);
}

/* Input: s - a floating-point number >= 1/2.
          rnd_mode - a rounding mode.
          Assumes s is neither NaN nor Infinite.
   Output: z - Zeta(s) rounded to the precision of z with direction rnd_mode
*/
void
mpfr_zeta_pos (mpfr_t z, mpfr_srcptr s, mp_rnd_t rnd_mode)
{
  int p, n, l, dint, add, d, can_round;
  double beta, sd, dnep;
  mpfr_t a,b,c,z_pre,f,g,s1;
  mpfr_t *tc1;
  mp_prec_t precz, precs;

  precz = mpfr_get_prec (z);
  precs = mpfr_get_prec (s);

  mpfr_init2 (a, MPFR_PREC_MIN);
  mpfr_init2 (b, MPFR_PREC_MIN);
  mpfr_init2 (c, MPFR_PREC_MIN);
  mpfr_init2 (z_pre, MPFR_PREC_MIN);
  mpfr_init2 (f, MPFR_PREC_MIN);
  mpfr_init2 (g, MPFR_PREC_MIN);
#ifdef DEBUG
  printf ("Warning: mpfr_zeta assumes that s and Zeta(s) are not both representable in mpfr\n");
  printf ("s=");
  mpfr_print_binary (s);
  printf ("\n");
#endif
  d = precz + 11;
  mpfr_init2 (s1, precs);
  do
    {
      /* Principal loop: we compute, in z_pre,
	 an approximation of Zeta(s), that we send to mpfr_can_round */
      mpfr_sub_ui (s1, s, 1, GMP_RNDN);
      MPFR_ASSERTN (MPFR_IS_FP (s1));
      if (MPFR_IS_ZERO (s1))
        {
          mpfr_set_inf (z, 1);
          return;
        }
      else if (MPFR_GET_EXP (s1) <= -(d-3)/2)
	/* Branch 1: when s-1 is very small, one
	  uses the approximation Zeta(s)=1/(s-1)+gamma,
	  where gamma is Euler's constant */
	{
	  dint = MAX(d + 3, precs);
#ifdef DEBUG
	  printf ("branch 1\n");
	  printf ("internal precision=%d\n", dint);
#endif
	  mpfr_set_prec (z_pre, dint);
	  mpfr_set_prec (g, dint);
	  mpfr_ui_div (z_pre, 1, s1, GMP_RNDN);
	  mpfr_const_euler (g, GMP_RNDN);
	  mpfr_add (z_pre, z_pre, g, GMP_RNDN);
	}
      else /* Branch 2 */
        {
#ifdef DEBUG	
          printf ("branch 2\n");
#endif
          /* Computation of parameters n, p and working precision */
          dnep = (double) d * log (2.0);
          sd = mpfr_get_d (s, GMP_RNDN);
          beta = dnep + 0.61 + sd * log (6.2832 / sd);
          if (beta <= 0.0)
            {
              p = 0;
              n = 1 + (int) (exp ((dnep - log(2.0)) / sd));
            }
          else
            {
              p = 1 + (int) beta / 2;
              n = 1 + (int) floor((sd + 2.0 * (double) p - 1.0) / 6.2832);
            }
#ifdef DEBUG
          printf ("\nn=%d\np=%d\n",n,p);
#endif
          add = 4 + (int) floor (1.5 * log ((double) d) / log(2.0));
          if (add < 10)
            add = 10;
          dint = d + add;
          if (dint < precs)
            dint = precs;
#ifdef DEBUG
          printf("internal precision=%d\n",dint);
#endif
          tc1 = (mpfr_t*) malloc ((p + 1) * sizeof(mpfr_t));
          for (l=1; l<=p; l++)
            mpfr_init2 (tc1[l], dint);
          mpfr_set_prec (a, dint);
          mpfr_set_prec (b, dint);
          mpfr_set_prec (c, dint);
          mpfr_set_prec (z_pre, dint);
          mpfr_set_prec (f, dint);
#ifdef DEBUG
          printf ("precision of z =%d\n", precz);
#endif
          /* Computation of the coefficients c_k */
          mpfr_zeta_c (p, tc1);
          /* Computation of the 3 parts of the fonction Zeta. */
          mpfr_zeta_part_a (a, s, n);
          mpfr_zeta_part_b (b, s, n, p, tc1);
          mpfr_sub_ui (s1, s, 1, GMP_RNDN);
          mpfr_ui_div (c, 1, s1, GMP_RNDN);
          mpfr_ui_pow (f, n, s1, GMP_RNDN);
          mpfr_div (c, c, f, GMP_RNDN);
#ifdef DEBUG
          printf ("\npart c=");
          mpfr_out_str (stdout, 10, 0, c, GMP_RNDN);
          printf ("\n");
#endif
          mpfr_add (z_pre, a, c, GMP_RNDN);
          mpfr_add (z_pre, z_pre, b, GMP_RNDN);
          for (l=1; l<=p; l++)
            mpfr_clear (tc1[l]);
          free(tc1);
          /* End branch 2 */
        }
#ifdef DEBUG
      printf ("\nZeta(s) before rounding=");
      mpfr_print_binary (z_pre);
#endif
      can_round = mpfr_can_round (z_pre, d - 3, GMP_RNDN, rnd_mode, precz);
      if (can_round)
	{
#ifdef DEBUG
	  printf ("\nwe can round");
#endif
	}
      else
	{
#ifdef DEBUG	  
	  printf ("\nwe cannot round");
#endif
	  d = d + 5;
	}
    }
  while (can_round == 0);

  mpfr_set (z, z_pre, rnd_mode);
#ifdef DEBUG
  printf ("\nZeta(s) after rounding=");
  mpfr_print_binary (z);
  printf ("\n");
#endif
  mpfr_clear (a);
  mpfr_clear (b);
  mpfr_clear (c);
  mpfr_clear (z_pre);
  mpfr_clear (f);
  mpfr_clear (g);
  mpfr_clear (s1);
}

void
mpfr_zeta (mpfr_t z, mpfr_srcptr s, mp_rnd_t rnd_mode)
{
  double sd, eps, m1, c;
  int can_round;
  long add;
  mpfr_t z_pre, s1, s2, y, sfrac, sint, p;
  mp_prec_t precz, prec1, precs, precs1;

  if (mpfr_nan_p (s))
    {
      mpfr_set_nan (z); /* Zeta(NaN) = NaN */
      return;
    }
  
  if (mpfr_inf_p (s))
    {
      if (MPFR_SIGN(s) > 0)
	{
	  mpfr_set_ui (z, 1, GMP_RNDN); /* Zeta(+Inf) = 1 */
	  return;
	}
      else
	{
	  mpfr_set_nan (z); /* Zeta(-Inf) = NaN */
	  return;
	}
    }

  /* now s is neither NaN nor Infinite */

  if (mpfr_cmp_ui(s,0) == 0) /* Case s = 0 */
    {
      mpfr_set_ui(z, 1, rnd_mode);
      mpfr_div_2ui(z, z, 1, rnd_mode);
      mpfr_neg(z,z,rnd_mode);
      return;
    }
  
  mpfr_init2(s2, mpfr_get_prec(s));
  mpfr_div_2ui(s2, s, 1, rnd_mode);
  if ((MPFR_SIGN(s) == -1) && (mpfr_floor(s2, s2) == 0)) /* Case s = -2n */
    {
      mpfr_set_ui(z, 0, rnd_mode);
      mpfr_clear(s2);
      return;
    };
  mpfr_clear(s2);
  precz = mpfr_get_prec (z);
  precs = mpfr_get_prec (s);
  /* Precision precs1 needed to represent 1 - s, and s + 2,
     without any truncation */
  precs1 = precs + 2 + MAX (0, - abs (MPFR_GET_EXP (s)));
  sd = mpfr_get_d (s, GMP_RNDN);
  /* Precision prec1 is the precision on elementary computations; it ensures
     a final precision prec1 - add for zeta(s) */
  eps = pow (2.0, - (double) precz - 14.0);
  m1 = 1.0 + MAX(1.0 / eps,  2.0 * fabs (sd - 1.0)) * (1.0 + eps);
  c = (1.0 + eps) * (1.0 + eps * MAX(8.0, m1));
  /* add = 1 + floor(log(c*c*c*(13 + m1))/log(2)); */
  add = __gmpfr_ceil_log2 (c * c * c * (13.0 + m1));
  prec1 = precz + add; /* Note that prec1 is still incremented by  10 at the first  entry of loop below */
  prec1 = MAX(prec1, precs1);
  if (MPFR_SIGN (s) != -1 && MPFR_GET_EXP (s) >= 0) /* Case s >= 1/2 */
    mpfr_zeta_pos (z, s, rnd_mode);
  else
    {
      mpfr_init2(z_pre, MPFR_PREC_MIN);
      mpfr_init2(s1, MPFR_PREC_MIN);
      mpfr_init2(y, MPFR_PREC_MIN);
      mpfr_init2(p, MPFR_PREC_MIN);
      mpfr_init2(sint, MPFR_PREC_MIN);
      mpfr_init2(sfrac, MPFR_PREC_MIN);
      do
	{
	  prec1 = prec1 + 10;
      mpfr_set_prec(z_pre, prec1);
      mpfr_set_prec(s1, prec1);
      mpfr_set_prec(y, prec1);
      mpfr_set_prec(p, prec1);
      mpfr_set_prec(sint, prec1);
      mpfr_set_prec(sfrac, prec1);
      mpfr_ui_sub(s1, 1, s, GMP_RNDN);
      mpfr_add_ui(sfrac, s, 2, GMP_RNDN);
      mpfr_div_2ui(sfrac, sfrac, 2, GMP_RNDN);
      mpfr_floor(sint, sfrac);
      mpfr_sub(sfrac, sfrac, sint, GMP_RNDN);
      mpfr_mul_2ui(sfrac, sfrac, 2, GMP_RNDN);
      mpfr_sub_ui(sfrac, sfrac, 2, GMP_RNDN);
      if (mpfr_cmp_ui(sfrac, 1) > 0)
        mpfr_ui_sub (sfrac, 2, sfrac, GMP_RNDN);
      else if (mpfr_cmp_si(sfrac, -1) < 0)
        {
          mpfr_neg(sfrac, sfrac, GMP_RNDN);
          mpfr_sub_ui(sfrac, sfrac, 2, GMP_RNDN);
        }
      mpfr_div_2ui(sfrac, sfrac, 1, GMP_RNDN);
      mpfr_zeta_pos(z_pre, s1, GMP_RNDN);
      mpfr_gamma(y, s1, GMP_RNDN);
      mpfr_mul(z_pre, z_pre, y, GMP_RNDN);
      mpfr_const_pi(p, GMP_RNDD);
      mpfr_mul(y, sfrac, p, GMP_RNDN);
      mpfr_sin(y, y, GMP_RNDN);
      mpfr_mul(z_pre, z_pre, y, GMP_RNDN);
      mpfr_mul_2ui(y, p, 1, GMP_RNDN);
      mpfr_neg(s1, s1, GMP_RNDN);
      mpfr_pow(y, y, s1, GMP_RNDN);
      mpfr_mul(z_pre, z_pre, y, GMP_RNDN);
      mpfr_mul_2ui(z_pre, z_pre, 1, GMP_RNDN);
      can_round = mpfr_can_round (z_pre, prec1 - add, GMP_RNDN, rnd_mode, precz);}
      while (can_round == 0);
      mpfr_set (z, z_pre, rnd_mode);
      mpfr_clear(z_pre);
      mpfr_clear(s1);
      mpfr_clear(y);
      mpfr_clear(p);
      mpfr_clear(sint);
      mpfr_clear(sfrac);
	}
}





