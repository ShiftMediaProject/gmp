/* Test mpz_gcd, mpz_gcdext, and mpz_gcd_ui.

Copyright 1991, 1993, 1994, 1996, 1997, 2000, 2001, 2002, 2003, 2004 Free
Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.  */

#include <stdio.h>
#include <stdlib.h>

#include "gmp.h"
#include "gmp-impl.h"
#include "tests.h"

static int one_test __GMP_PROTO ((mpz_t, mpz_t, int));
static void debug_mp __GMP_PROTO ((mpz_t, int));

#define MIN_OPERAND_SIZE 2

/* Fixed values, for regression testing of mpn_hgcd. */
struct value { int res; const char *a; const char *b; };
static const struct value hgcd_values[] = {
#if GMP_NUMB_BITS == 32
  { 4,
    "0x1bddff867272a9296ac493c251d7f46f09a5591fe",
    "0xb55930a2a68a916450a7de006031068c5ddb0e5c" },
  { 4,
    "0x2f0ece5b1ee9c15e132a01d55768dc13",
    "0x1c6f4fd9873cdb24466e6d03e1cc66e7" },
  { 4, "0x7FFFFC003FFFFFFFFFC5", "0x3FFFFE001FFFFFFFFFE3"},
#endif
  { -1, NULL, NULL }
};

struct hgcd_ref
{
  /* Sign here, u and v are stored as absolute values */
  int sign;

  mpz_t r[4];
  mpz_t u[4];
  mpz_t v[4];
};

static void hgcd_ref_init __GMP_PROTO ((struct hgcd_ref *hgcd));
static void hgcd_ref_clear __GMP_PROTO ((struct hgcd_ref *hgcd));
static int hgcd_ref __GMP_PROTO ((struct hgcd_ref *hgcd, const mpz_t a, const mpz_t b));
static int hgcd_ref_equal __GMP_PROTO ((const struct hgcd *hgcd, const struct hgcd_ref *ref));

int
main (int argc, char **argv)
{
  mpz_t op1, op2, temp1, temp2;
  int i, j, chain_len;
  gmp_randstate_ptr rands;
  mpz_t bs;
  unsigned long size_range;

  tests_start ();
  rands = RANDS;

  mpz_init (bs);
  mpz_init (op1);
  mpz_init (op2);
  mpz_init (temp1);
  mpz_init (temp2);

  for (i = 0; hgcd_values[i].res >= 0; i++)
    {
      int res;

      mpz_set_str (op1, hgcd_values[i].a, 0);
      mpz_set_str (op2, hgcd_values[i].b, 0);

      res = one_test (op1, op2, -1-i);
      if (res != hgcd_values[i].res)
	{
	  fprintf (stderr, "ERROR in test %d\n", -1-i);
	  fprintf (stderr, "Bad return code from hgcd\n");
	  fprintf (stderr, "op1=");                 debug_mp (op1, -16);
	  fprintf (stderr, "op2=");                 debug_mp (op2, -16);
	  fprintf (stderr, "expected: %d\n", hgcd_values[i].res);
	  fprintf (stderr, "hgcd:     %d\n", res);
	  abort ();
	}
    }

  for (i = 0; i < 15; i++)
    {
      /* Generate plain operands with unknown gcd.  These types of operands
	 have proven to trigger certain bugs in development versions of the
	 gcd code.  The "hgcd->row[3].rsize > M" ASSERT is not triggered by
	 the division chain code below, but that is most likely just a result
	 of that other ASSERTs are triggered before it.  */

      mpz_urandomb (bs, rands, 32);
      size_range = mpz_get_ui (bs) % 13 + 2;

      mpz_urandomb (bs, rands, size_range);
      mpz_urandomb (op1, rands, mpz_get_ui (bs) + MIN_OPERAND_SIZE);
      mpz_urandomb (bs, rands, size_range);
      mpz_urandomb (op2, rands, mpz_get_ui (bs) + MIN_OPERAND_SIZE);

      if (mpz_cmp (op1, op2) < 0)
	mpz_swap (op1, op2);

      if (mpz_size(op1) > 0)
	one_test (op1, op2, i);

      /* Generate a division chain backwards, allowing otherwise
	 unlikely huge quotients.  */

      mpz_set_ui (op1, 0);
      mpz_urandomb (bs, rands, 32);
      mpz_urandomb (bs, rands, mpz_get_ui (bs) % 16 + 1);
      mpz_rrandomb (op2, rands, mpz_get_ui (bs));
      mpz_add_ui (op2, op2, 1);

#if 0
      chain_len = 1000000;
#else
      mpz_urandomb (bs, rands, 32);
      chain_len = mpz_get_ui (bs) % (GMP_NUMB_BITS * GCD_SCHOENHAGE_THRESHOLD / 256);
#endif

      for (j = 0; j < chain_len; j++)
	{
	  mpz_urandomb (bs, rands, 32);
	  mpz_urandomb (bs, rands, mpz_get_ui (bs) % 12 + 1);
	  mpz_rrandomb (temp2, rands, mpz_get_ui (bs) + 1);
	  mpz_add_ui (temp2, temp2, 1);
	  mpz_mul (temp1, op2, temp2);
	  mpz_add (op1, op1, temp1);

	  /* Don't generate overly huge operands.  */
	  if (SIZ (op1) > 3 * GCD_SCHOENHAGE_THRESHOLD)
	    break;

	  mpz_urandomb (bs, rands, 32);
	  mpz_urandomb (bs, rands, mpz_get_ui (bs) % 12 + 1);
	  mpz_rrandomb (temp2, rands, mpz_get_ui (bs) + 1);
	  mpz_add_ui (temp2, temp2, 1);
	  mpz_mul (temp1, op1, temp2);
	  mpz_add (op2, op2, temp1);

	  /* Don't generate overly huge operands.  */
	  if (SIZ (op2) > 3 * GCD_SCHOENHAGE_THRESHOLD)
	    break;
	}
      if (mpz_cmp (op1, op2) < 0)
	mpz_swap (op1, op2);

      if (mpz_size(op1) > 0)
	one_test (op1, op2, i);
    }

  exit (0);
}

static void
debug_mp (mpz_t x, int base)
{
  mpz_out_str (stderr, base, x); fputc ('\n', stderr);
}

static int
one_test (mpz_t a, mpz_t b, int i)
{
  struct hgcd hgcd;
  struct hgcd_ref ref;
  struct qstack quotients;
  int res[2];
  mp_size_t asize;
  mp_size_t bsize;

  mp_size_t hgcd_init_scratch;
  mp_size_t qstack_scratch;
  mp_size_t hgcd_scratch;

  mp_ptr hgcd_init_tp;
  mp_ptr qstack_tp;
  mp_ptr hgcd_tp;

  asize = a->_mp_size;
  bsize = b->_mp_size;

  hgcd_init_scratch = mpn_hgcd_init_itch (asize);
  hgcd_init_tp = refmpn_malloc_limbs (hgcd_init_scratch);
  mpn_hgcd_init (&hgcd, asize, hgcd_init_tp);

  qstack_scratch = qstack_itch (asize);
  qstack_tp = refmpn_malloc_limbs (qstack_scratch);
  qstack_init (&quotients, asize, qstack_tp, qstack_scratch);

  hgcd_scratch = mpn_hgcd_itch (asize);
  hgcd_tp = refmpn_malloc_limbs (hgcd_scratch);

#if 0
  fprintf (stderr,
	   "one_test: i = %d asize = %d, bsize = %d\n",
	   i, a->_mp_size, b->_mp_size);

  gmp_fprintf (stderr,
	       "one_test: i = %d\n"
	       "  a = %Zx\n"
	       "  b = %Zx\n",
	       i, a, b);
#endif
  hgcd_ref_init (&ref);

  res[0] = hgcd_ref (&ref, a, b);
  res[1] = mpn_hgcd (&hgcd,
		     a->_mp_d, asize,
		     b->_mp_d, bsize,
		     &quotients,
		     hgcd_tp, hgcd_scratch);

  if (res[0] != res[1])
    {
      fprintf (stderr, "ERROR in test %d\n", i);
      fprintf (stderr, "Different return code from hgcd and hgcd_ref\n");
      fprintf (stderr, "op1=");                 debug_mp (a, -16);
      fprintf (stderr, "op2=");                 debug_mp (b, -16);
      fprintf (stderr, "hgcd_ref: %d\n", res[0]);
      fprintf (stderr, "mpn_hgcd: %d\n", res[1]);
      abort ();
    }
  if (res[0] > 0)
    {
      ASSERT_HGCD (&hgcd, a->_mp_d, asize, b->_mp_d, bsize, 0, 4);

      if (!hgcd_ref_equal (&hgcd, &ref))
	{
	  fprintf (stderr, "ERROR in test %d\n", i);
	  fprintf (stderr, "mpn_hgcd and hgcd_ref returned different values\n");
	  fprintf (stderr, "op1=");                 debug_mp (a, -16);
	  fprintf (stderr, "op2=");                 debug_mp (b, -16);
	  abort ();
	}
    }

  refmpn_free_limbs (hgcd_init_tp);
  refmpn_free_limbs (qstack_tp);
  refmpn_free_limbs (hgcd_tp);
  hgcd_ref_clear (&ref);

  return res[0];
}

static void
hgcd_ref_init (struct hgcd_ref *hgcd)
{
  unsigned i;
  for (i = 0; i<4; i++)
    {
      mpz_init (hgcd->r[i]);
      mpz_init (hgcd->u[i]);
      mpz_init (hgcd->v[i]);
    }
}

static void
hgcd_ref_clear (struct hgcd_ref *hgcd)
{
  unsigned i;
  for (i = 0; i<4; i++)
    {
      mpz_clear (hgcd->r[i]);
      mpz_clear (hgcd->u[i]);
      mpz_clear (hgcd->v[i]);
    }
}

static int
hgcd_ref (struct hgcd_ref *hgcd, const mpz_t a, const mpz_t b)
{
  mp_size_t M = (a->_mp_size + 1) / 2;
  mpz_t t;
  mpz_t q;
  int res;

  if (mpz_size(b) <= M)
    return 0;

  mpz_init (q);
  mpz_fdiv_qr(q, hgcd->r[2], a, b);

  if (mpz_size (hgcd->r[2]) <= M)
    {
      mpz_clear (q);
      return 0;
    }

  mpz_set (hgcd->r[0], a); mpz_set (hgcd->r[1], b);

  mpz_set_ui (hgcd->u[0], 1); mpz_set_ui (hgcd->v[0], 0);
  mpz_set_ui (hgcd->u[1], 0); mpz_set_ui (hgcd->v[1], 1);
  mpz_set_ui (hgcd->u[2], 1); mpz_set    (hgcd->v[2], q);

  hgcd->sign = 0;

  mpz_init (t);

  for (;;)
    {
      mpz_fdiv_qr(q, hgcd->r[3], hgcd->r[1], hgcd->r[2]);

      mpz_mul (hgcd->u[3], q, hgcd->u[2]);
      mpz_add (hgcd->u[3], hgcd->u[3], hgcd->u[1]);

      mpz_mul (hgcd->v[3], q, hgcd->v[2]);
      mpz_add (hgcd->v[3], hgcd->v[3], hgcd->v[1]);

      if (mpz_size (hgcd->r[3]) <= M)
	{
#if 0
	  unsigned i;
	  printf("hgcd_ref: sign = %d\n", hgcd->sign);
	  for (i = 0; i < 4; i++)
	    gmp_printf("r = %Zd, u = %Zd, v = %Zd\n",
		       hgcd->r[i], hgcd->u[i], hgcd->v[i]);
#endif
	  /* Check Jebelean's criterion */

	  if (hgcd->sign >= 0)
	    {
	      /* Check if r1 - r2 >= u2 - u1 */
	      mpz_add (t, hgcd->u[2], hgcd->u[1]);
	    }
	  else
	    {
	      /* Check if r1 - r2 >= v2 - v1 */
	      mpz_add (t, hgcd->v[2], hgcd->v[1]);
	    }

	  /* Check r1 >= t + r2 */
	  mpz_add (t, t, hgcd->r[2]);
	  if (mpz_cmp (hgcd->r[1], t) < 0)
	    {
	      res = 2; break;
	    }

	  /* Now r2 is correct */
	  if (hgcd->sign >= 0)
	    {
	      /* Check r3 >= max (-u3, -v3) = u3 */
	      if (mpz_cmp (hgcd->r[3], hgcd->u[3]) < 0)
		{
		  res = 3; break;
		}

	      /* Check r3 - r2 >= v3 - v2 */
	      mpz_add (t, hgcd->v[3], hgcd->v[2]);
	    }
	  else
	    {
	      /* Check r3 >= max (-u3, -v3) = v3 */
	      if (mpz_cmp (hgcd->r[3], hgcd->v[3]) < 0)
		{
		  res = 3; break;
		}

	      /* Check r3 - r2 >= u3 - u2 */
	      mpz_add (t, hgcd->u[3], hgcd->u[2]);
	    }

	  /* Check r2 >= t + r3 */
	  mpz_add (t, t, hgcd->r[3]);
	  if (mpz_cmp (hgcd->r[2], t) < 0)
	    {
	      res = 3; break;
	    }

	  /* Now r3 is correct */
	  res = 4; break;
	}

      /* Shift rows */
      hgcd->sign = ~hgcd->sign;
      mpz_swap (hgcd->r[0], hgcd->r[1]);
      mpz_swap (hgcd->r[1], hgcd->r[2]);
      mpz_swap (hgcd->r[2], hgcd->r[3]);

      mpz_swap (hgcd->u[0], hgcd->u[1]);
      mpz_swap (hgcd->u[1], hgcd->u[2]);
      mpz_swap (hgcd->u[2], hgcd->u[3]);

      mpz_swap (hgcd->v[0], hgcd->v[1]);
      mpz_swap (hgcd->v[1], hgcd->v[2]);
      mpz_swap (hgcd->v[2], hgcd->v[3]);
    }

  mpz_clear (t);
  mpz_clear (q);

  return res;
}

static int
mpz_mpn_equal (const mpz_t a, mp_srcptr bp, mp_size_t bsize)
{
  mp_srcptr ap = a->_mp_d;
  mp_size_t asize = a->_mp_size;

  MPN_NORMALIZE (bp, bsize);
  return asize == bsize && mpn_cmp(ap, bp, asize) == 0;
}

static int
hgcd_ref_equal (const struct hgcd *hgcd, const struct hgcd_ref *ref)
{
  unsigned i;

  if (ref->sign != hgcd->sign)
    return 0;

  for (i = 0; i<4; i++)
    {
      if (!mpz_mpn_equal (ref->r[i], hgcd->row[i].rp, hgcd->row[i].rsize))
	return 0;
      if (!mpz_mpn_equal (ref->u[i], hgcd->row[i].uvp[0], hgcd->size))
	return 0;
      if (!mpz_mpn_equal (ref->v[i], hgcd->row[i].uvp[1], hgcd->size))
	return 0;
    }
  return 1;
}
