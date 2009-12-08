/* Test bdiv functions

Copyright 2009 Free Software Foundation, Inc.

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

static int
bdiv_q_valid_p (mp_srcptr np, mp_size_t nn, mp_srcptr dp, mp_size_t dn,
		mp_srcptr qp)
{
  mp_ptr tp;
  int c;
  TMP_DECL;

  TMP_MARK;
  tp = TMP_ALLOC_LIMBS (dn + nn);
  refmpn_mul_basecase (tp, qp, nn, dp, dn);
  MPN_CMP (c, tp, np, nn);
  TMP_FREE;

  return c == 0;
}

static int
bdiv_qr_valid_p (mp_srcptr np, mp_size_t nn, mp_srcptr dp, mp_size_t dn,
		 mp_srcptr qp, mp_srcptr rp, mp_limb_t rh)
{
  mp_size_t qn;
  mp_ptr tp;
  mp_size_t cy;
  int c;
  TMP_DECL;

  TMP_MARK;
  qn = nn - dn;
  tp = TMP_ALLOC_LIMBS (nn);

  if (qn >= dn)
    refmpn_mul_basecase (tp, qp, qn, dp, dn);
  else
    refmpn_mul_basecase (tp, dp, dn, qp, qn);

  MPN_CMP (c, tp, np, qn);
  if (c)
    {
      TMP_FREE;
      return 0;
    }

  cy = refmpn_sub_nc (tp + qn, np + qn, tp + qn, dn, 0);
  MPN_CMP (c, tp + qn, rp, dn);
  TMP_FREE;

  return cy == rh && c == 0;
}

#define SIZE_LOG 10
#define MAX_DN (1L << SIZE_LOG)
#define MAX_NN (1L << (SIZE_LOG + 1))

#define COUNT 1000
int
main (int argc, char **argv)
{
  mp_ptr np, dp, qp, rp, scratch;
  mp_size_t itch;
  int count = COUNT;
  int test;
  gmp_randstate_ptr rands;
  TMP_DECL;
  TMP_MARK;

  if (argc > 1)
    {
      char *end;
      count = strtol (argv[1], &end, 0);
      if (*end || count <= 0)
	{
	  fprintf (stderr, "Invalid test count: %s.\n", argv[1]);
	  return 1;
	}
    }

  tests_start ();
  rands = RANDS;

  dp = TMP_ALLOC_LIMBS (MAX_DN + 1);
  np = TMP_ALLOC_LIMBS (MAX_NN + 1);
  qp = TMP_ALLOC_LIMBS (MAX_NN + 1);
  rp = TMP_ALLOC_LIMBS (MAX_DN + 1);

  /* Claim larger nn that we'll use, to allow for a large quotient.
     FIXME: This probably isn't the right way. */
  itch = mpn_bdiv_qr_itch (MAX_NN + MAX_DN, MAX_DN);
  scratch = TMP_ALLOC_LIMBS (itch);
			     
  for (test = 0; test < count; test++)
    {
      unsigned size_range;
      mp_size_t nn, dn;
      mp_limb_t rh;

      /* We generate dn in the range 1 <= dn <= (1 << size_range). */
      size_range = 1
	+ gmp_urandomm_ui (rands, SIZE_LOG);

      dn = 1
	+ gmp_urandomm_ui (rands, (1L << size_range));
      nn = dn + 1 + gmp_urandomm_ui (rands, MAX_NN - dn);

      mpn_random2 (dp, dn);
      mpn_random2 (np, nn);
      dp[0] |= 1;

      ASSERT_ALWAYS (mpn_bdiv_q_itch (nn, dn) <= itch);

      mpn_bdiv_q (qp, np, nn, dp, dn, scratch);
      if (!bdiv_q_valid_p (np, nn, dp, dn, qp))
	{
	  gmp_printf ("bdiv_q test %d failed, nn = %d, dn = %d:\n"
		      "n = %Nx\n"
		      "d = %Nx\n"
		      "q = %Nx\n",
		      test, nn, dn,
		      np, nn, dp, dn, qp, nn);
	  abort();
	}

      ASSERT_ALWAYS (mpn_bdiv_qr_itch (nn, dn) <= itch);

      rh = mpn_bdiv_qr (qp, rp, np, nn, dp, dn, scratch);
      if (!bdiv_qr_valid_p (np,nn, dp, dn, qp, rp, rh))
	{
	  gmp_printf ("bdiv_qr test %d failed, nn = %d, dn = %d:\n"
		      "n = %Nx\n"
		      "d = %Nx\n"
		      "q = %Nx\n"
		      "r = %Mx %Nx\n",
		      test, nn, dn,
		      np, nn, dp, dn, qp, nn - dn,
		      rh, rp, dn);
	  abort();
	}
    }
  TMP_FREE;
  return 0;
}
