/* Test mpz_cmp, mpz_cmp_ui, mpz_tdiv_qr, mpz_mul.

Copyright 1991, 1993, 1994, 1996, 1997, 2000, 2001 Free Software Foundation,
Inc.

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
static void base_mul _PROTO ((mp_ptr,mp_srcptr,mp_size_t,mp_srcptr,mp_size_t));
static void ref_mpz_mul _PROTO ((mpz_t, const mpz_t, const mpz_t));
void dump_abort _PROTO ((int, char *, mpz_t, mpz_t, mpz_t, mpz_t));

int
main (int argc, char **argv)
{
  mpz_t multiplier, multiplicand;
  mpz_t product, ref_product;
  mpz_t quotient, remainder;
  mp_size_t multiplier_size, multiplicand_size;
  int i;
  int reps = 100;
  gmp_randstate_ptr rands;
  mpz_t bs;
  unsigned long bsi, size_range;

  tests_start ();
  rands = RANDS;

  mpz_init (bs);

  if (argc == 2)
     reps = atoi (argv[1]);

  mpz_init (multiplier);
  mpz_init (multiplicand);
  mpz_init (product);
  mpz_init (ref_product);
  mpz_init (quotient);
  mpz_init (remainder);

  for (i = 0; i < reps; i++)
    {
      mpz_urandomb (bs, rands, 32);
      size_range = mpz_get_ui (bs) % 18 + 2;

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

      /* printf ("%ld %ld\n", SIZ (multiplier), SIZ (multiplicand)); */

      mpz_mul (product, multiplier, multiplicand);

      if (size_range <= 16)  /* avoid calling ref_mpz_mul for huge operands */
	{
	  ref_mpz_mul (ref_product, multiplier, multiplicand);
	  if (mpz_cmp (product, ref_product))
	    dump_abort (i, "incorrect plain product",
			multiplier, multiplicand, product, ref_product);
	}

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
      if (size_range <= 16)  /* avoid calling ref_mpz_mul for huge operands */
	{
	  mpz_mul (product, multiplier, multiplier);
	  ref_mpz_mul (ref_product, multiplier, multiplier);
	}
      else
	{
	  mpz_mul (product, multiplier, multiplier);
	  mpz_set (multiplicand, multiplier);
	  mpz_mul (ref_product, multiplier, multiplicand);
	}

      if (mpz_cmp (product, ref_product))
	dump_abort (i, "incorrect square product",
		    multiplier, multiplier, product, ref_product);
    }

  mpz_clear (bs);
  mpz_clear (multiplier);
  mpz_clear (multiplicand);
  mpz_clear (product);
  mpz_clear (ref_product);
  mpz_clear (quotient);
  mpz_clear (remainder);

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
  mp_ptr free_me = NULL;
  size_t free_me_size;
  TMP_DECL (marker);

  TMP_MARK (marker);
  sign_product = usize ^ vsize;
  usize = ABS (usize);
  vsize = ABS (vsize);

  if (usize < vsize)
    {
      /* Swap U and V.  */
      {const __mpz_struct *t = u; u = v; v = t;}
      {mp_size_t t = usize; usize = vsize; vsize = t;}
    }

  if (vsize == 0)
    {
      SIZ (w) = 0;
      return;
    }

  up = u->_mp_d;
  vp = v->_mp_d;
  wp = w->_mp_d;

  /* Ensure W has space enough to store the result.  */
  wsize = usize + vsize;
  if (w->_mp_alloc < wsize)
    {
      if (wp == up || wp == vp)
	{
	  free_me = wp;
	  free_me_size = w->_mp_alloc;
	}
      else
	(*__gmp_free_func) (wp, w->_mp_alloc * BYTES_PER_MP_LIMB);

      w->_mp_alloc = wsize;
      wp = (mp_ptr) (*__gmp_allocate_func) (wsize * BYTES_PER_MP_LIMB);
      w->_mp_d = wp;
    }
  else
    {
      /* Make U and V not overlap with W.  */
      if (wp == up)
	{
	  /* W and U are identical.  Allocate temporary space for U.  */
	  up = (mp_ptr) TMP_ALLOC (usize * BYTES_PER_MP_LIMB);
	  /* Is V identical too?  Keep it identical with U.  */
	  if (wp == vp)
	    vp = up;
	  /* Copy to the temporary space.  */
	  MPN_COPY (up, wp, usize);
	}
      else if (wp == vp)
	{
	  /* W and V are identical.  Allocate temporary space for V.  */
	  vp = (mp_ptr) TMP_ALLOC (vsize * BYTES_PER_MP_LIMB);
	  /* Copy to the temporary space.  */
	  MPN_COPY (vp, wp, vsize);
	}
    }

  base_mul (wp, up, usize, vp, vsize);
  wsize = usize + vsize;
  wsize -= wp[wsize - 1] == 0;
  w->_mp_size = sign_product < 0 ? -wsize : wsize;
  if (free_me != NULL)
    (*__gmp_free_func) (free_me, free_me_size * BYTES_PER_MP_LIMB);

  TMP_FREE (marker);
}

static void
base_mul (mp_ptr wp, mp_srcptr up, mp_size_t un, mp_srcptr vp, mp_size_t vn)
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
