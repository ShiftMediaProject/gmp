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

void one_test __GMP_PROTO ((mpz_t, mpz_t, mpz_t, int));
void debug_mp __GMP_PROTO ((mpz_t, int));
void ref_mpz_gcd __GMP_PROTO ((mpz_t, mpz_t, mpz_t));

/* Keep one_test's variables global, so that we don't need
   to reinitialize them for each test.  */
mpz_t gcd, s, t, temp1, temp2;

int
main (int argc, char **argv)
{
  struct hgcd hgcd;
  struct qstack quotients;
  int res;
  mp_size_t ssize, tsize;

  mp_size_t hgcd_init_scratch, qstack_scratch, hgcd_scratch;
  mp_ptr tp;
 
  mpz_init (s);
  mpz_init (t);

  mpz_set_str (s, "0x1bddff867272a9296ac493c251d7f46f09a5591fe", 0);
  mpz_set_str (t, "0xb55930a2a68a916450a7de006031068c5ddb0e5c", 0);

  ssize = s[0]._mp_size;
  tsize = t[0]._mp_size;

  hgcd_init_scratch = mpn_hgcd_init_itch (ssize);
  qstack_scratch = qstack_itch (ssize);
  hgcd_scratch = mpn_hgcd_itch (ssize);
  
  tp = refmpn_malloc_limbs (hgcd_init_scratch + qstack_scratch + hgcd_scratch);
  
  mpn_hgcd_init (&hgcd, ssize, tp);
  tp += hgcd_init_scratch;

  qstack_init (&quotients, ssize, tp, qstack_scratch);
  tp += qstack_scratch;

  res = mpn_hgcd_lehmer (&hgcd, 
			 s[0]._mp_d, ssize, 
			 t[0]._mp_d, tsize, 
			 &quotients,
			 tp, hgcd_scratch);

  exit (0);
}
