/* Test mpz_gcd, mpz_gcdext, and mpz_gcd_ui.

Copyright 1991, 1993, 1994, 1996, 1997, 2000, 2001, 2002, 2003 Free Software
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
#include "tests.h"

static int one_test __GMP_PROTO ((mpz_t, mpz_t, int));
static void debug_mp __GMP_PROTO ((mpz_t, int));

#define MIN_OPERAND_SIZE 2

/* Fixed values, for regression testing of mpn_hgcd_lehmer. */
struct value { int res; const char *a; const char *b; };
static const struct value hgcd_values[] = {
  { 4,
    "0x1bddff867272a9296ac493c251d7f46f09a5591fe",
    "0xb55930a2a68a916450a7de006031068c5ddb0e5c" },
  { -1, NULL, NULL }
};


int
main (int argc, char **argv)
{
  mpz_t op1, op2, temp1, temp2;
  int i, j, chain_len;
  gmp_randstate_ptr rands;
  mpz_t bs;
  unsigned long bsi, size_range;

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
  struct hgcd hgcd[2];
  struct qstack quotients[2];
  int res[2];
  mp_size_t asize;
  mp_size_t bsize;

  mp_size_t hgcd_init_scratch;
  mp_size_t qstack_scratch;
  mp_size_t hgcd_lehmer_scratch;
  mp_size_t hgcd_scratch;

  mp_ptr hgcd_init_tp[2];
  mp_ptr qstack_tp[2];
  mp_ptr hgcd_lehmer_tp;
  mp_ptr hgcd_tp;
  int k;
  
  asize = a->_mp_size;
  bsize = b->_mp_size;

  hgcd_init_scratch = mpn_hgcd_init_itch (asize);
  qstack_scratch = qstack_itch (asize);
  hgcd_lehmer_scratch = mpn_hgcd_lehmer_itch (asize);
  hgcd_scratch = mpn_hgcd_itch (asize);

  for (k = 0; k < 2; k++)
  {
    hgcd_init_tp[k] = refmpn_malloc_limbs (hgcd_init_scratch);
    mpn_hgcd_init (&hgcd[k], asize, hgcd_init_tp[k]);

    qstack_tp[k] = refmpn_malloc_limbs (qstack_scratch);
    qstack_init (&quotients[k], asize, qstack_tp[k], qstack_scratch);
  }

  hgcd_lehmer_tp = refmpn_malloc_limbs (hgcd_lehmer_scratch);
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
  
  res[0] = mpn_hgcd_lehmer (&hgcd[0], 
			    a->_mp_d, asize, 
			    b->_mp_d, bsize, 
			    &quotients[0],
			    hgcd_lehmer_tp, hgcd_lehmer_scratch);
  res[1] = mpn_hgcd (&hgcd[1], 
		     a->_mp_d, asize, 
		     b->_mp_d, bsize, 
		     &quotients[1],
		     hgcd_tp, hgcd_scratch);

  if (res[0] != res[1])
    {
      fprintf (stderr, "ERROR in test %d\n", i);
      fprintf (stderr, "Different return code from hgcd and hgcd_lehmer\n");
      fprintf (stderr, "op1=");                 debug_mp (a, -16);
      fprintf (stderr, "op2=");                 debug_mp (b, -16);
      fprintf (stderr, "mpn_hgcd_lehmer: %d\n", res[0]);
      fprintf (stderr, "mpn_hgcd:        %d\n", res[1]);
      abort ();
    }
  if (res[0] > 0)
    {
      ASSERT_HGCD (&hgcd[0], a->_mp_d, asize, b->_mp_d, bsize, 0, 4);

      if (!mpn_hgcd_equal (&hgcd[0], &hgcd[1]))
	{
	  fprintf (stderr, "ERROR in test %d\n", i);
	  fprintf (stderr, "hgcd and hgcd_lehmer returned different values\n");
	  fprintf (stderr, "op1=");                 debug_mp (a, -16);
	  fprintf (stderr, "op2=");                 debug_mp (b, -16);
	  abort ();
	}
    }

  refmpn_free_limbs (hgcd_init_tp[0]);
  refmpn_free_limbs (hgcd_init_tp[1]);
  refmpn_free_limbs (qstack_tp[0]);
  refmpn_free_limbs (qstack_tp[1]);
  refmpn_free_limbs (hgcd_lehmer_tp);
  refmpn_free_limbs (hgcd_tp);

  return res[0];
}
