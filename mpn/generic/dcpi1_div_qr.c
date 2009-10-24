/* mpn_dcpi1_div_qr_n -- recursive divide-and-conquer division for arbitrary
   size operands.

   Contributed to the GNU project by Torbjorn Granlund.

   THE FUNCTIONS IN THIS FILE ARE INTERNAL WITH A MUTABLE INTERFACE.  IT IS
   ONLY SAFE TO REACH THEM THROUGH DOCUMENTED INTERFACES.  IN FACT, IT IS
   ALMOST GUARANTEED THAT THEY WILL CHANGE OR DISAPPEAR IN A FUTURE GMP
   RELEASE.

Copyright 2006, 2007, 2009 Free Software Foundation, Inc.

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

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"


mp_limb_t
mpn_dcpi1_div_qr_n (mp_ptr qp, mp_ptr np, mp_srcptr dp, mp_size_t n,
		    gmp_pi1_t *dinv, mp_ptr tp)
{
  mp_size_t lo, hi;
  mp_limb_t cy, qh, ql;

  lo = n >> 1;			/* floor(n/2) */
  hi = n - lo;			/* ceil(n/2) */

  if (BELOW_THRESHOLD (hi, DC_DIV_QR_THRESHOLD))
    qh = mpn_sbpi1_div_qr (qp + lo, np + 2 * lo, 2 * hi, dp + lo, hi, dinv->inv32);
  else
    qh = mpn_dcpi1_div_qr_n (qp + lo, np + 2 * lo, dp + lo, hi, dinv, tp);

  mpn_mul (tp, qp + lo, hi, dp, lo);

  cy = mpn_sub_n (np + lo, np + lo, tp, n);
  if (qh != 0)
    cy += mpn_sub_n (np + n, np + n, dp, lo);

  while (cy != 0)
    {
      qh -= mpn_sub_1 (qp + lo, qp + lo, hi, 1);
      cy -= mpn_add_n (np + lo, np + lo, dp, n);
    }

  if (BELOW_THRESHOLD (lo, DC_DIV_QR_THRESHOLD))
    ql = mpn_sbpi1_div_qr (qp, np + hi, 2 * lo, dp + hi, lo, dinv->inv32);
  else
    ql = mpn_dcpi1_div_qr_n (qp, np + hi, dp + hi, lo, dinv, tp);

  mpn_mul (tp, dp, hi, qp, lo);

  cy = mpn_sub_n (np, np, tp, n);
  if (ql != 0)
    cy += mpn_sub_n (np + lo, np + lo, dp, hi);

  while (cy != 0)
    {
      mpn_sub_1 (qp, qp, lo, 1);
      cy -= mpn_add_n (np, np, dp, n);
    }

  return qh;
}

mp_limb_t
mpn_dcpi1_div_qr (mp_ptr qp,
		  mp_ptr np, mp_size_t nn,
		  mp_srcptr dp, mp_size_t dn,
		  gmp_pi1_t *dinv)
{
  mp_size_t qn;
  mp_limb_t qh, cy;
  mp_ptr tp;
  TMP_DECL;

  TMP_MARK;

  tp = TMP_SALLOC_LIMBS (dn);

  qn = nn - dn;
  qp += qn;
  np += nn;
  dp += dn;

  if (qn > dn)
    {
      /* Reduce qn mod dn without division, optimizing small operations.  */
      do
	qn -= dn;
      while (qn > dn);

      qp -= qn;			/* point at low limb of next quotient block */
      np -= qn;			/* point in the middle of partial remainder */

      /* Perform the typically smaller block first.  */
      if (qn == 1)
	{
	  mp_limb_t q, r, n1, n0, d;
	  d = (dp - qn)[0];
	  n1 = (np - qn)[1];
	  n0 = (np - qn)[0];
	  qh = (n1 >= d);
	  n1 -= d & -(mp_limb_t) qh;
	  udiv_qrnnd_preinv (q, r, n1, n0, d, dinv->inv21);
	  qp[0] = q;
	  (np - qn)[0] = r;
	}
      else if (qn == 2)
	qh = mpn_divrem_2 (qp, 0L, np - qn, 4, dp - qn); /* FIXME: obsolete function */
      else if (BELOW_THRESHOLD (qn, DC_DIV_QR_THRESHOLD))
	qh = mpn_sbpi1_div_qr (qp, np - qn, 2 * qn, dp - qn, qn, dinv->inv32);
      else
	qh = mpn_dcpi1_div_qr_n (qp, np - qn, dp - qn, qn, dinv, tp);

      if (qn != dn)
	{
	  if (qn > dn - qn)
	    mpn_mul (tp, qp, qn, dp - dn, dn - qn);
	  else
	    mpn_mul (tp, dp - dn, dn - qn, qp, qn);

	  cy = mpn_sub_n (np - dn, np - dn, tp, dn);
	  if (qh != 0)
	    cy += mpn_sub_n (np - dn + qn, np - dn + qn, dp - dn, dn - qn);

	  while (cy != 0)
	    {
	      qh -= mpn_sub_1 (qp, qp, qn, 1);
	      cy -= mpn_add_n (np - dn, np - dn, dp - dn, dn);
	    }
	}

      qn = nn - dn - qn;
      do
	{
	  qp -= dn;
	  np -= dn;
	  mpn_dcpi1_div_qr_n (qp, np - dn, dp - dn, dn, dinv, tp);
	  qn -= dn;
	}
      while (qn > 0);
    }
  else
    {
      if (qn == 0)
	{
	  qh = mpn_cmp (np - dn, dp - dn, dn) >= 0;
	  if (qh)
	    mpn_sub_n (np - dn, np - dn, dp - dn, dn);
	  TMP_FREE;
	  return qh;
	}

      qp -= qn;			/* point at low limb of next quotient block */
      np -= qn;			/* point in the middle of partial remainder */

      if (BELOW_THRESHOLD (qn, DC_DIV_QR_THRESHOLD))
	qh = mpn_sbpi1_div_qr (qp, np - qn, 2 * qn, dp - qn, qn, dinv->inv32);
      else
	qh = mpn_dcpi1_div_qr_n (qp, np - qn, dp - qn, qn, dinv, tp);

      if (qn != dn)
	{
	  if (qn > dn - qn)
	    mpn_mul (tp, qp, qn, dp - dn, dn - qn);
	  else
	    mpn_mul (tp, dp - dn, dn - qn, qp, qn);

	  cy = mpn_sub_n (np - dn, np - dn, tp, dn);
	  if (qh != 0)
	    cy += mpn_sub_n (np - dn + qn, np - dn + qn, dp - dn, dn - qn);

	  while (cy != 0)
	    {
	      qh -= mpn_sub_1 (qp, qp, qn, 1);
	      cy -= mpn_add_n (np - dn, np - dn, dp - dn, dn);
	    }
	}
    }

  TMP_FREE;
  return qh;
}
