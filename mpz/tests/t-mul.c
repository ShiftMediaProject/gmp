/* Test mpz_cmp, mpz_cmp_ui, mpz_tdiv_qr, mpz_mul.

Copyright 1991, 1993, 1994, 1996, 1997, 2000 Free Software Foundation, Inc.

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
#include <sys/time.h>

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#include "urandom.h"

void debug_mp ();
mp_size_t _mpn_mul_classic ();
void mpz_refmul ();

main (argc, argv)
     int argc;
     char **argv;
{
  mpz_t multiplier, multiplicand;
  mpz_t product, ref_product;
  mpz_t quotient, remainder;
  mp_size_t multiplier_size, multiplicand_size;
  int i;
  int reps = 100;
  gmp_randstate_t rands;
  mpz_t bs;
  unsigned long bsi, size_range;
  char *perform_seed;

  gmp_randinit (rands, GMP_RAND_ALG_LC, 64);

  perform_seed = getenv ("GMP_CHECK_RANDOMIZE");
  if (perform_seed != 0)
    {
      struct timeval tv;
      gettimeofday (&tv, NULL);
      gmp_randseed_ui (rands, tv.tv_sec + tv.tv_usec);
      printf ("PLEASE INCLUDE THIS SEED NUMBER IN ALL BUG REPORTS:\n");
      printf ("GMP_CHECK_RANDOMIZE is set--seeding with %ld\n",
	      tv.tv_sec + tv.tv_usec);
    }

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
      size_range = mpz_get_ui (bs) % 16 + 2;

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
      mpz_refmul (ref_product, multiplier, multiplicand);

      if (mpz_cmp (product, ref_product))
	dump_abort ("incorrect plain product",
		    multiplier, multiplicand, product, ref_product);

      if (mpz_cmp_ui (multiplicand, 0) != 0)
	{
	  mpz_tdiv_qr (quotient, remainder, product, multiplicand);
	  if (mpz_cmp_ui (remainder, 0) || mpz_cmp (quotient, multiplier))
	    {
	      debug_mp (quotient, -16);
	      debug_mp (remainder, -16);
	      dump_abort ("incorrect quotient or remainder",
			  multiplier, multiplicand, product, ref_product);
	    }
	}

      /* Test squaring.  */
      mpz_mul (product, multiplier, multiplier);
      mpz_refmul (ref_product, multiplier, multiplier);

      if (mpz_cmp (product, ref_product))
	dump_abort ("incorrect square product",
		    multiplier, multiplier, product, ref_product);
    }

  exit (0);
}

void
mpz_refmul (w, u, v)
     mpz_t w;
     const mpz_t u;
     const mpz_t v;
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

  wsize = _mpn_mul_classic (wp, up, usize, vp, vsize);
  w->_mp_size = sign_product < 0 ? -wsize : wsize;
  if (free_me != NULL)
    (*__gmp_free_func) (free_me, free_me_size * BYTES_PER_MP_LIMB);

  TMP_FREE (marker);
}

mp_size_t
_mpn_mul_classic (prodp, up, usize, vp, vsize)
     mp_ptr prodp;
     mp_srcptr up;
     mp_size_t usize;
     mp_srcptr vp;
     mp_size_t vsize;
{
  mp_size_t i, j;
  mp_limb_t prod_low, prod_high;
  mp_limb_t cy_dig;
  mp_limb_t v_limb, c;

  if (vsize == 0)
    return 0;

  /* Offset UP and PRODP so that the inner loop can be faster.  */
  up += usize;
  prodp += usize;

  /* Multiply by the first limb in V separately, as the result can
     be stored (not added) to PROD.  We also avoid a loop for zeroing.  */
  v_limb = vp[0];
  cy_dig = 0;
  j = -usize;
  do
    {
      umul_ppmm (prod_high, prod_low, up[j], v_limb);
      add_ssaaaa (cy_dig, prodp[j], prod_high, prod_low, 0, cy_dig);
    }
  while (++j < 0);

  prodp[j] = cy_dig;
  prodp++;

  /* For each iteration in the outer loop, multiply one limb from
     U with one limb from V, and add it to PROD.  */
  for (i = 1; i < vsize; i++)
    {
      v_limb = vp[i];
      cy_dig = 0;
      j = -usize;

      /* Inner loops.  Simulate the carry flag by jumping between
	 these loops.  The first is used when there was no carry
	 in the previois iteration; the second when there was carry.  */

      do
	{
	  umul_ppmm (prod_high, prod_low, up[j], v_limb);
	  add_ssaaaa (prod_high, prod_low, prod_high, prod_low, 0, prodp[j]);
	  prod_low += cy_dig;
	  cy_dig = prod_high + (prod_low < cy_dig);
	  prodp[j] = prod_low;
	}
      while (++j < 0);

      prodp[j] = cy_dig;
      prodp++;
    }

  return usize + vsize - (cy_dig == 0);
}

dump_abort (s, multiplier, multiplicand, product, ref_product)
     char *s;
     mpz_t multiplier, multiplicand, product, ref_product;
{
  fprintf (stderr, "ERROR: %s\n", s);
  fprintf (stderr, "multiplier = "); debug_mp (multiplier, -16);
  fprintf (stderr, "multiplicand  = "); debug_mp (multiplicand, -16);
  fprintf (stderr, "product  = "); debug_mp (product, -16);
  fprintf (stderr, "ref_product  = "); debug_mp (ref_product, -16);
  abort();
}

void
debug_mp (x, base)
     mpz_t x;
{
  mpz_out_str (stderr, base, x); fputc ('\n', stderr);
}
