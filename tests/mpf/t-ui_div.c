/* Test mpf_ui_div.

Copyright 2004 Free Software Foundation, Inc.

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


void
check_one (mpf_ptr got, unsigned long u, mpf_srcptr v)
{
  mpf_t      uf;
  mp_limb_t  ulimbs[2];
  mp_size_t  usize;

  mpf_ui_div (got, u, v);

  ulimbs[0] = u & GMP_NUMB_MASK;
  usize = (u != 0);
#if BITS_PER_ULONG > GMP_NUMB_BITS
  u >>= GMP_NUMB_BITS;
  ulimbs[1] = u;
  usize += (u != 0);
#endif
  PTR(uf) = ulimbs;
  SIZ(uf) = usize;
  EXP(uf) = usize;

  if (! refmpf_validate_division ("mpf_ui_div", got, uf, v))
    {
      mp_trace_base = -16;
      printf    ("  u 0x%lX  (%lu)\n", u, u);
      mpf_trace ("  v", v);
      abort ();
    }
}

void
check_rand (void)
{
  unsigned long  min_prec = __GMPF_BITS_TO_PREC (1);
  gmp_randstate_ptr  rands = RANDS;
  unsigned long  prec, u;
  mpf_t  got, v;
  int    i;

  mpf_init (got);
  mpf_init (v);

  for (i = 0; i < 200; i++)
    {
      /* got precision */
      prec = min_prec + gmp_urandomm_ui (rands, 15L);
      refmpf_set_prec_limbs (got, prec);

      /* u */
      prec = gmp_urandomm_ui (rands, BITS_PER_ULONG+1);
      u = gmp_urandomb_ui (rands, prec);

      /* v precision */
      prec = min_prec + gmp_urandomm_ui (rands, 15L);
      refmpf_set_prec_limbs (v, prec);

      /* v, non-zero */
      do {
        mpf_random2 (v, PREC(v), (mp_exp_t) 20);
      } while (SIZ(v) == 0);

      /* v possibly negative */
      if (gmp_urandomb_ui (rands, 1L))
        mpf_neg (v, v);

      check_one (got, u, v);
    }

  mpf_clear (got);
  mpf_clear (v);
}

void
check_various (void)
{
  mpf_t got, v;

  mpf_init (got);
  mpf_init (v);

  /* 99/4 == 24 */
  mpf_set_prec (got, 20L);
  mpf_set_ui (v, 4L);
  mpf_ui_div (got, 99L, v);
  MPF_CHECK_FORMAT (got);
  ASSERT_ALWAYS (mpf_cmp_ui (got, 24L));

  /* 1/(2^n+1), a case where truncating the divisor would be wrong */
  mpf_set_prec (got, 500L);
  mpf_set_prec (v, 900L);
  mpf_set_ui (v, 1L);
  mpf_mul_2exp (v, v, 800L);
  mpf_add_ui (v, v, 1L);
  check_one (got, 1L, v);

  mpf_clear (got);
  mpf_clear (v);
}

int
main (void)
{
  tests_start ();

  check_rand ();
  check_various ();

  tests_end ();
  exit (0);
}
