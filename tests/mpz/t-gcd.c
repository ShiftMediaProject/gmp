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

void one_test (mpz_t, mpz_t, mpz_t, int);
void debug_mp _PROTO ((mpz_t, int));
void ref_mpz_gcd (mpz_t, mpz_t, mpz_t);

/* Keep one_test's variables global, so that we don't need
   to reinitialize them for each test.  */
mpz_t gcd, s, t, temp1, temp2;

/* Define this to make all operands be large enough for Schoenhage gcd
   to be used.  */
#define WHACK_SCHOENHAGE 0

#if WHACK_SCHOENHAGE
#define MIN_OPERAND_SIZE (GCD_SCHOENHAGE_THRESHOLD * GMP_NUMB_BITS)
#else
#define MIN_OPERAND_SIZE 1
#endif

int
main (int argc, char **argv)
{
  mpz_t op1, op2, ref;
  int i, j, chain_len;
  gmp_randstate_ptr rands;
  mpz_t bs;
  unsigned long bsi, size_range;

  tests_start ();
  rands = RANDS;

  mpz_init (bs);
  mpz_init (op1);
  mpz_init (op2);
  mpz_init (ref);
  mpz_init (gcd);
  mpz_init (temp1);
  mpz_init (temp2);
  mpz_init (s);
  mpz_init (t);

  for (i = 0; i < 50; i++)
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

      mpz_urandomb (bs, rands, 2);
      bsi = mpz_get_ui (bs);
      if ((bsi & 1) != 0)
	mpz_neg (op1, op1);
      if ((bsi & 2) != 0)
	mpz_neg (op2, op2);

      ref_mpz_gcd (ref, op1, op2);
      one_test (op1, op2, ref, i);


      /* Generate a division chain backwards, allowing otherwise unlikely huge
	 quotients.  */

      mpz_set_ui (op1, 0);
      mpz_urandomb (bs, rands, 32);
      mpz_urandomb (bs, rands, mpz_get_ui (bs) % 16 + 1);
      mpz_rrandomb (op2, rands, mpz_get_ui (bs));
      mpz_add_ui (op2, op2, 1);
      mpz_set (ref, op2);

#if WHACK_SCHOENHAGE
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
      one_test (op1, op2, ref, i);
    }

  mpz_clear (bs);
  mpz_clear (op1);
  mpz_clear (op2);
  mpz_clear (ref);
  mpz_clear (gcd);
  mpz_clear (temp1);
  mpz_clear (temp2);
  mpz_clear (s);
  mpz_clear (t);

  tests_end ();
  exit (0);
}

void
debug_mp (mpz_t x, int base)
{
  mpz_out_str (stderr, base, x); fputc ('\n', stderr);
}

void
one_test (mpz_t op1, mpz_t op2, mpz_t ref, int i)
{
  /*
  printf ("%ld %ld %ld\n", SIZ (op1), SIZ (op2), SIZ (ref));
  fflush (stdout);
  */

  /*
  fprintf (stderr, "op1=");  debug_mp (op1, -16);
  fprintf (stderr, "op2=");  debug_mp (op2, -16);
  */

  mpz_gcd (gcd, op1, op2);
  if (mpz_cmp (ref, gcd) != 0)
    {
      fprintf (stderr, "ERROR in test %d\n", i);
      fprintf (stderr, "mpz_gcd returned incorrect result\n");
      fprintf (stderr, "op1=");                 debug_mp (op1, -16);
      fprintf (stderr, "op2=");                 debug_mp (op2, -16);
      fprintf (stderr, "expected result:\n");   debug_mp (ref, -16);
      fprintf (stderr, "mpz_gcd returns:\n");   debug_mp (gcd, -16);
      abort ();
    }

  /* This should probably move to t-gcd_ui.c */
  if (mpz_fits_ulong_p (op1) || mpz_fits_ulong_p (op2))
    {
      if (mpz_fits_ulong_p (op1))
	mpz_gcd_ui (gcd, op2, mpz_get_ui (op1));
      else
	mpz_gcd_ui (gcd, op1, mpz_get_ui (op2));
      if (mpz_cmp (ref, gcd))
	{
	  fprintf (stderr, "ERROR in test %d\n", i);
	  fprintf (stderr, "mpz_gcd_ui returned incorrect result\n");
	  fprintf (stderr, "op1=");                 debug_mp (op1, -16);
	  fprintf (stderr, "op2=");                 debug_mp (op2, -16);
	  fprintf (stderr, "expected result:\n");   debug_mp (ref, -16);
	  fprintf (stderr, "mpz_gcd returns:\n");   debug_mp (gcd, -16);
	  abort ();
	}
    }

  mpz_gcdext (gcd, s, t, op1, op2);
  if (mpz_cmp (ref, gcd))
    {
      fprintf (stderr, "ERROR in test %d\n", i);
      fprintf (stderr, "mpz_gcdext returned incorrect result\n");
      fprintf (stderr, "op1=");                 debug_mp (op1, -16);
      fprintf (stderr, "op2=");                 debug_mp (op2, -16);
      fprintf (stderr, "expected result:\n");   debug_mp (ref, -16);
      fprintf (stderr, "mpz_gcdext returns:\n");debug_mp (gcd, -16);
      abort ();
    }

  mpz_gcdext (gcd, s, NULL, op1, op2);
  if (mpz_cmp (ref, gcd))
    {
      fprintf (stderr, "ERROR in test %d\n", i);
      fprintf (stderr, "mpz_gcdext returned incorrect result\n");
      fprintf (stderr, "op1=");                 debug_mp (op1, -16);
      fprintf (stderr, "op2=");                 debug_mp (op2, -16);
      fprintf (stderr, "expected result:\n");   debug_mp (ref, -16);
      fprintf (stderr, "mpz_gcdext returns:\n");debug_mp (gcd, -16);
      fprintf (stderr, "                  s:\n");debug_mp (s, -16);
      fprintf (stderr, "                  t:\n");debug_mp (t, -16);
      abort ();
    }

  mpz_mul (temp1, s, op1);
  mpz_mul (temp2, t, op2);
  mpz_add (gcd, temp1, temp2);
  if (mpz_cmp (ref, gcd))
    {
      fprintf (stderr, "ERROR in test %d\n", i);
      fprintf (stderr, "mpz_gcdext returned incorrect result\n");
      fprintf (stderr, "op1=");                 debug_mp (op1, -16);
      fprintf (stderr, "op2=");                 debug_mp (op2, -16);
      fprintf (stderr, "expected result:\n");   debug_mp (ref, -16);
      fprintf (stderr, "mpz_gcdext returns:\n");debug_mp (gcd, -16);
      fprintf (stderr, "                  s:\n");debug_mp (s, -16);
      fprintf (stderr, "                  t:\n");debug_mp (t, -16);
      abort ();
    }
}

void
ref_mpz_gcd (mpz_t r, mpz_t u_in, mpz_t v_in)
{
  mpz_t u, v;
  unsigned long uzeros, vzeros, cnt;
  int cmp;

  if (mpz_cmp_ui (u_in, 0) == 0)
    {
      mpz_abs (r, v_in);
      return;
    }
  if (mpz_cmp_ui (v_in, 0) == 0)
    {
      mpz_abs (r, u_in);
      return;
    }

  mpz_init (u);
  mpz_init (v);

  uzeros = mpz_scan1 (u_in, 0);
  mpz_tdiv_q_2exp (u, u_in, uzeros);
  mpz_abs (u, u);

  vzeros = mpz_scan1 (v_in, 0);
  mpz_tdiv_q_2exp (v, v_in, vzeros);
  mpz_abs (v, v);

  for (;;)
    {
      cmp = mpz_cmp (u, v);

      if (cmp > 0)
	{
	  mpz_sub (u, u, v);
	  cnt = mpz_scan1 (u, 0);
	  mpz_tdiv_q_2exp (u, u, cnt);
	}
      else if (cmp < 0)
	{
	  mpz_sub (v, v, u);
	  cnt = mpz_scan1 (v, 0);
	  mpz_tdiv_q_2exp (v, v, cnt);
	}
      else
	break;
    }

  mpz_mul_2exp (r, u, MIN (uzeros, vzeros));
  mpz_clear (u);
  mpz_clear (v);
}
