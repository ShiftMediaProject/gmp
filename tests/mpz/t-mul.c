/* Test mpz_cmp, mpz_cmp_ui, mpz_tdiv_qr, mpz_mul.

Copyright 1991, 1993, 1994, 1996, 1997, 2000, 2001, 2002 Free Software
Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <stdio.h>
#include <stdlib.h>

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#include "tests.h"

void debug_mp _PROTO ((mpz_t, int));
static void ref_mpn_mul _PROTO ((mp_ptr,mp_srcptr,mp_size_t,mp_srcptr,mp_size_t));
static void ref_mpz_mul _PROTO ((mpz_t, const mpz_t, const mpz_t));
void dump_abort _PROTO ((int, char *, mpz_t, mpz_t, mpz_t, mpz_t));

void
one (int i, mpz_t multiplicand, mpz_t multiplier)
{
  mpz_t product, ref_product;
  mpz_t quotient, remainder;

  mpz_init (product);
  mpz_init (ref_product);
  mpz_init (quotient);
  mpz_init (remainder);

  mpz_mul (product, multiplier, multiplicand);

  ref_mpz_mul (ref_product, multiplier, multiplicand);
  if (mpz_cmp (product, ref_product))
    dump_abort (i, "incorrect plain product",
		multiplier, multiplicand, product, ref_product);

  if (mpz_cmp_ui (multiplicand, 0) != 0)
    {
      mpz_tdiv_qr (quotient, remainder, product, multiplicand);
      if (mpz_cmp_ui (remainder, 0) || mpz_cmp (quotient, multiplier))
	{
	  debug_mp (quotient, -16);
	  debug_mp (remainder, -16);
	  dump_abort (i, "incorrect quotient or remainder",
		      multiplier, multiplicand, product, ref_product);
	}
    }

  /* Test squaring.  */
  mpz_mul (product, multiplier, multiplier);
  ref_mpz_mul (ref_product, multiplier, multiplier);

  if (mpz_cmp (product, ref_product))
    dump_abort (i, "incorrect square product",
		multiplier, multiplier, product, ref_product);

  mpz_clear (product);
  mpz_clear (ref_product);
  mpz_clear (quotient);
  mpz_clear (remainder);
}

int
main (int argc, char **argv)
{
  mpz_t multiplier, multiplicand;
  mp_size_t multiplier_size, multiplicand_size, size;
  int i;
  int reps = 500;
  gmp_randstate_ptr rands;
  mpz_t bs;
  unsigned long bsi, size_range;

  tests_start ();
  rands = RANDS;

  if (argc == 2)
     reps = atoi (argv[1]);

  mpz_init (bs);
  mpz_init (multiplier);
  mpz_init (multiplicand);

  for (i = 0; i < reps; i++)
    {
      mpz_urandomb (bs, rands, 32);
      size_range = mpz_get_ui (bs) % 14 + 2;

      mpz_urandomb (bs, rands, size_range);
      multiplier_size = mpz_get_ui (bs);
      mpz_rrandomb (multiplier, rands, multiplier_size);

      mpz_urandomb (bs, rands, size_range);
      multiplicand_size = mpz_get_ui (bs);
      mpz_rrandomb (multiplicand, rands, multiplicand_size);

      mpz_urandomb (bs, rands, 2);
      bsi = mpz_get_ui (bs);
      if ((bsi & 1) != 0)
	mpz_neg (multiplier, multiplier);
      if ((bsi & 2) != 0)
	mpz_neg (multiplicand, multiplicand);

      /* printf ("%d %d\n", SIZ (multiplier), SIZ (multiplicand)); */

      one (i, multiplicand, multiplier);
    }

  size = 20;
  for (i = 0; size / GMP_NUMB_BITS < SQR_FFT_THRESHOLD; i++)
    {
      size = size * 5 / 4;
      mpz_rrandomb (multiplier, rands, size);
      mpz_rrandomb (multiplicand, rands, size);

      /* printf ("%d %d\n", SIZ (multiplier), SIZ (multiplicand)); */

      one (-1, multiplicand, multiplier);
    }

  mpz_clear (bs);
  mpz_clear (multiplier);
  mpz_clear (multiplicand);

  tests_end ();
  exit (0);
}

static void
ref_mpz_mul (mpz_t w, const mpz_t u, const mpz_t v)
{
  mp_size_t usize = u->_mp_size;
  mp_size_t vsize = v->_mp_size;
  mp_size_t wsize;
  mp_size_t sign_product;
  mp_ptr up, vp;
  mp_ptr wp;
  mp_size_t talloc;

  sign_product = usize ^ vsize;
  usize = ABS (usize);
  vsize = ABS (vsize);

  if (usize == 0 || vsize == 0)
    {
      SIZ (w) = 0;
      return;
    }

  talloc = usize + vsize;

  up = u->_mp_d;
  vp = v->_mp_d;

  wp = __GMP_ALLOCATE_FUNC_LIMBS (talloc);

  if (usize > vsize)
    ref_mpn_mul (wp, up, usize, vp, vsize);
  else
    ref_mpn_mul (wp, vp, vsize, up, usize);
  wsize = usize + vsize;
  wsize -= wp[wsize - 1] == 0;
  MPZ_REALLOC (w, wsize);
  MPN_COPY (PTR(w), wp, wsize);

  SIZ(w) = sign_product < 0 ? -wsize : wsize;
  __GMP_FREE_FUNC_LIMBS (wp, talloc);
}

static void mul_kara (mp_ptr, mp_srcptr, mp_srcptr, mp_size_t, mp_ptr);
static void mul_basecase (mp_ptr, mp_srcptr, mp_size_t, mp_srcptr, mp_size_t);

#define KARA_THRES 12

static void
ref_mpn_mul (mp_ptr wp, mp_srcptr up, mp_size_t un, mp_srcptr vp, mp_size_t vn)
{
  mp_ptr tp;
  mp_limb_t cy;

  if (vn < KARA_THRES)
    {
      if (vn != 0)
	mul_basecase (wp, up, un, vp, vn);
      else
	MPN_ZERO (wp, un);
      return;
    }

  tp = __GMP_ALLOCATE_FUNC_LIMBS (4 * vn);

  mul_kara (tp + 2 * vn, up, vp, vn, tp);

  if (un - vn < vn)
    ref_mpn_mul (wp + vn, vp, vn, up + vn, un - vn);
  else
    ref_mpn_mul (wp + vn, up + vn, un - vn, vp, vn);

  MPN_COPY (wp, tp + 2 * vn, vn);
  cy = mpn_add_n (wp + vn, wp + vn, tp + 3 * vn, vn);
  mpn_incr_u (wp + 2 * vn, cy);

  __GMP_FREE_FUNC_LIMBS (tp, 4 * vn);
}

static void
mul_kara (mp_ptr wp, mp_srcptr up, mp_srcptr vp, mp_size_t n, mp_ptr tp)
{
  mp_size_t nh;
  mp_limb_t cy;
  int add_flag;

  nh = n / 2;
  add_flag = 0;
  if (mpn_cmp (up, up + nh, nh) >= 0)
    mpn_sub_n (wp, up, up + nh, nh);
  else
    {
      add_flag = ~0;
      mpn_sub_n (wp, up + nh, up, nh);
    }
  if (mpn_cmp (vp, vp + nh, nh) >= 0)
    mpn_sub_n (wp + nh, vp, vp + nh, nh);
  else
    {
      add_flag = ~add_flag;
      mpn_sub_n (wp + nh, vp + nh, vp, nh);
    }
  if (nh < KARA_THRES)
    {
      mul_basecase (tp,          wp,      nh, wp + nh, nh);
      mul_basecase (wp,          up,      nh, vp,      nh);
      mul_basecase (wp + 2 * nh, up + nh, nh, vp + nh, nh);
    }
  else
    {
      mul_kara (tp, wp,      wp + nh,          nh, tp + 2 * nh);
      mul_kara (wp, up,      vp,               nh, tp + 2 * nh);
      mul_kara (wp + 2 * nh, up + nh, vp + nh, nh, tp + 2 * nh);
    }
  if (add_flag)
    cy = mpn_add_n (tp, wp, tp, 2 * nh);
  else
    cy = -mpn_sub_n (tp, wp, tp, 2 * nh);
  cy += mpn_add_n (tp, wp + 2 * nh, tp, 2 * nh);
  cy += mpn_add_n (wp + nh, wp + nh, tp, 2 * nh);
  mpn_incr_u (wp + 3 * nh, cy);

  if ((n & 1) != 0)
    {
      wp[2 * n - 2] = mpn_addmul_1 (wp + 2 * nh, vp, n - 1, up[2 * nh]);
      wp[2 * n - 1] = mpn_addmul_1 (wp + 2 * nh, up, n,     vp[2 * nh]);
    }
}

static void
mul_basecase (mp_ptr wp, mp_srcptr up, mp_size_t un, mp_srcptr vp, mp_size_t vn)
{
  mp_size_t i, j;
  mp_limb_t prod_low, prod_high;
  mp_limb_t cy_dig;
  mp_limb_t v_limb;

  /* Multiply by the first limb in V separately, as the result can
     be stored (not added) to PROD.  We also avoid a loop for zeroing.  */
  v_limb = vp[0];
  cy_dig = 0;
  for (j = un; j > 0; j--)
    {
      mp_limb_t u_limb, w_limb;
      u_limb = *up++;
      umul_ppmm (prod_high, prod_low, u_limb, v_limb << GMP_NAIL_BITS);
      add_ssaaaa (cy_dig, w_limb, prod_high, prod_low, 0, cy_dig << GMP_NAIL_BITS);
      *wp++ = w_limb >> GMP_NAIL_BITS;
    }

  *wp++ = cy_dig;
  wp -= un;
  up -= un;

  /* For each iteration in the outer loop, multiply one limb from
     U with one limb from V, and add it to PROD.  */
  for (i = 1; i < vn; i++)
    {
      v_limb = vp[i];
      cy_dig = 0;

      for (j = un; j > 0; j--)
	{
	  mp_limb_t u_limb, w_limb;
	  u_limb = *up++;
	  umul_ppmm (prod_high, prod_low, u_limb, v_limb << GMP_NAIL_BITS);
	  w_limb = *wp;
	  add_ssaaaa (prod_high, prod_low, prod_high, prod_low, 0, w_limb << GMP_NAIL_BITS);
	  prod_low >>= GMP_NAIL_BITS;
	  prod_low += cy_dig;
#if GMP_NAIL_BITS == 0
	  cy_dig = prod_high + (prod_low < cy_dig);
#else
	  cy_dig = prod_high;
	  cy_dig += prod_low >> GMP_NUMB_BITS;
#endif
	  *wp++ = prod_low & GMP_NUMB_MASK;
	}

      *wp++ = cy_dig;
      wp -= un;
      up -= un;
    }
}

void
dump_abort (int i, char *s,
	    mpz_t multiplier, mpz_t multiplicand, mpz_t product, mpz_t ref_product)
{
  mpz_t diff;
  fprintf (stderr, "ERROR: %s in test %d\n", s, i);
  fprintf (stderr, "multiplier   = "); debug_mp (multiplier, -16);
  fprintf (stderr, "multiplicand = "); debug_mp (multiplicand, -16);
  fprintf (stderr, "    product  = "); debug_mp (product, -16);
  fprintf (stderr, "ref_product  = "); debug_mp (ref_product, -16);
  mpz_init (diff);
  mpz_sub (diff, ref_product, product);
  fprintf (stderr, "diff:         "); debug_mp (diff, -16);
  abort();
}

void
debug_mp (mpz_t x, int base)
{
  mpz_out_str (stderr, base, x); fputc ('\n', stderr);
}
