/* Test file for mpfr_frac.

Copyright 2002, 2003 Free Software Foundation.

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

#include <stdio.h>
#include <stdlib.h>
#include "gmp.h"
#include "mpfr.h"
#include "mpfr-test.h"

#define PIP 70
#define PFP 70
#define PMAX (PIP+2*PFP)

static void
check0 (mpfr_ptr ip, mpfr_ptr fp, mp_prec_t prec, mp_rnd_t rnd)
{
  mpfr_t sum, tmp, dst, fp2;
  int inex1, inex2;

  mpfr_init2 (sum, PMAX);
  mpfr_init2 (tmp, PMAX);
  mpfr_init2 (dst, prec);
  mpfr_init2 (fp2, prec);

  if (MPFR_SIGN (ip) != MPFR_SIGN (fp))
    {
      printf ("Internal error (1)\n");
      exit (1);
    }
  if (mpfr_add (sum, ip, fp, GMP_RNDZ))
    {
      printf ("Wrong inexact flag in mpfr_add\n");
      exit (1);
    }
  if (MPFR_SIGN (sum) != MPFR_SIGN (fp))
    {
      printf ("Internal error (2)\n");
      exit (1);
    }

  inex1 = mpfr_frac (dst, sum, rnd);
  inex2 = mpfr_set (fp2, fp, rnd);
  if (inex1 != inex2)
    {
      printf ("Wrong inexact flag in mpfr_frac for\n");
      mpfr_out_str (stdout, 2, 0, sum, GMP_RNDN);
      printf ("\nGot %d instead of %d\n", inex1, inex2);
      exit (1);
    }
  if (!mpfr_number_p (dst) ||
      MPFR_SIGN (dst) != MPFR_SIGN (fp2) ||
      mpfr_cmp (dst, fp2))
    {
      printf ("Error in mpfr_frac (y, x, %s) with\nx = ",
              mpfr_print_rnd_mode (rnd));
      mpfr_out_str (stdout, 2, 0, sum, GMP_RNDN);
      printf ("\nGot        ");
      mpfr_out_str (stdout, 2, 0, dst, GMP_RNDN);
      printf ("\ninstead of ");
      mpfr_out_str (stdout, 2, 0, fp2, GMP_RNDN);
      printf ("\n");
      exit (1);
    }

  if (prec == PMAX)
    {
      inex1 = mpfr_frac (sum, sum, rnd);
      if (inex1)
        {
          printf ("Wrong inexact flag in mpfr_frac\n");
          exit (1);
        }
      if (!mpfr_number_p (sum) ||
          MPFR_SIGN (sum) != MPFR_SIGN (fp) ||
          mpfr_cmp (sum, fp))
        {
          printf ("Error in mpfr_frac (x, x, %s) with\nx = ",
                  mpfr_print_rnd_mode (rnd));
          mpfr_add (tmp, ip, fp, GMP_RNDZ);
          mpfr_out_str (stdout, 2, 0, tmp, GMP_RNDN);
          printf ("\nGot        ");
          mpfr_out_str (stdout, 2, 0, sum, GMP_RNDN);
          printf ("\ninstead of ");
          mpfr_out_str (stdout, 2, 0, fp, GMP_RNDN);
          printf ("\n");
          exit (1);
        }
    }

  mpfr_clear (fp2);
  mpfr_clear (dst);
  mpfr_clear (tmp);
  mpfr_clear (sum);
}

static void
check1 (mpfr_ptr ip, mpfr_ptr fp)
{
  mp_rnd_t rnd;

  for (rnd = 0; rnd < 4; rnd++)
    {
      check0 (ip, fp, PMAX, rnd);
      check0 (ip, fp, 70, rnd);
      mpfr_neg (fp, fp, GMP_RNDN);
      mpfr_neg (ip, ip, GMP_RNDN);
      check0 (ip, fp, PMAX, rnd);
      check0 (ip, fp, 70, rnd);
      mpfr_neg (fp, fp, GMP_RNDN);
      mpfr_neg (ip, ip, GMP_RNDN);
    }
}

int
main (void)
{
  mpfr_t ip, fp;
  int ni, nf1, nf2;

  tests_start_mpfr ();

  mpfr_init2 (ip, PIP);
  mpfr_init2 (fp, PFP);

  for (ni = -1; ni < PIP; ni++)
    {
      if (ni <= 0)
        { /* ni + 1 */
          mpfr_set_si (ip, ni, GMP_RNDN);
          mpfr_add_ui (ip, ip, 1, GMP_RNDN);
        }
      else
        { /* 2^ni + 1 */
          mpfr_set_ui (ip, 1, GMP_RNDN);
          mpfr_mul_2ui (ip, ip, ni, GMP_RNDN);
          mpfr_add_ui (ip, ip, 1, GMP_RNDN);
        }

      mpfr_set_ui (fp, 0, GMP_RNDN);
      check1 (ip, fp);

      for (nf1 = 1; nf1 < PFP; nf1++)
        {
          mpfr_set_ui (fp, 1, GMP_RNDN);
          mpfr_div_2ui (fp, fp, nf1, GMP_RNDN);
          check1 (ip, fp);
          nf2 = 1 + (randlimb () % (PFP - 1));
          mpfr_set_ui (fp, 1, GMP_RNDN);
          mpfr_div_2ui (fp, fp, nf2, GMP_RNDN);
          mpfr_add_ui (fp, fp, 1, GMP_RNDN);
          mpfr_div_2ui (fp, fp, nf1, GMP_RNDN);
          check1 (ip, fp);
        }
    }

  mpfr_set_ui (ip, 1, GMP_RNDN);
  mpfr_div_ui (ip, ip, 0, GMP_RNDN);
  mpfr_set_ui (fp, 0, GMP_RNDN);
  check1 (ip, fp);  /* test infinities */

  mpfr_clear (ip);
  mpfr_clear (fp);
  tests_end_mpfr ();
  return 0;
}
