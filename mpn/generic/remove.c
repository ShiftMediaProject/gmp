/* mpn_remove -- divide out all multiples of odd mpn number from another mpn
   number.

   Contributed to the GNU project by Torbjörn Granlund.

   THE FUNCTION IN THIS FILE IS INTERNAL WITH A MUTABLE INTERFACE.  IT IS ONLY
   SAFE TO REACH IT THROUGH DOCUMENTED INTERFACES.  IN FACT, IT IS ALMOST
   GUARANTEED THAT IT WILL CHANGE OR DISAPPEAR IN A FUTURE GMP RELEASE.

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

#include "gmp.h"
#include "gmp-impl.h"

#if GMP_LIMB_BITS > 50
#define LOG 50
#else
#define LOG GMP_LIMB_BITS
#endif

mp_bitcnt_t
mpn_remove (mp_ptr wp, mp_size_t *wn,
	    mp_ptr up, mp_size_t un, mp_ptr vp, mp_size_t vn)
{
  mp_ptr    pwpsp[LOG];
  mp_size_t pwpsn[LOG];
  mp_size_t npowers;
  mp_ptr tp, qp, np, pp, qp2, scratch_out;
  mp_size_t pn, nn, qn, i;
  mp_bitcnt_t pwr;
  TMP_DECL;

  ASSERT (un > 0);
  ASSERT (vn > 0);
  ASSERT (vp[0] % 2 != 0);	/* 2-adic division wants odd numbers */
  ASSERT (vn > 1 || vp[0] > 1);	/* else we would loop indefinitely */

  TMP_MARK;

  tp = TMP_ALLOC_LIMBS (un);
  qp = TMP_ALLOC_LIMBS (un);
  qp2 = TMP_ALLOC_LIMBS (un);
  np = TMP_ALLOC_LIMBS (un + LOG);
  pp = vp;
  pn = vn;

  scratch_out = TMP_ALLOC_LIMBS (mpn_bdiv_qr_itch (un, un>>1)); /* FIXME */

  MPN_COPY (qp, up, un);
  qn = un;

  npowers = 0;
  while (qn >= pn)
    {
      mpn_bdiv_qr (qp2, tp, qp, qn, pp, pn, scratch_out);
      if (!mpn_zero_p (tp, pn))
	break;			/* could not divide by pp */

      MP_PTR_SWAP (qp, qp2);

      qn = qn - pn;
      qn += qp[qn] != 0;

      pwpsp[npowers] = pp;
      pwpsn[npowers] = pn;
      npowers++;

      nn = 2 * pn - 1;		/* next power will be at least this many limbs */
      if (nn > qn)
	break;			/* next power would be overlarge */

      mpn_sqr_n (np, pp, pn);
      nn += np[nn] != 0;
      pp = np;
      pn = nn;
      np += nn;
    }

  pwr = ((mp_bitcnt_t) 1 << npowers) - 1;

  for (i = npowers - 1; i >= 0; i--)
    {
      pp = pwpsp[i];
      pn = pwpsn[i];
      if (qn >= pn)
	{
	  mpn_bdiv_qr (qp2, tp, qp, qn, pp, pn, scratch_out);
	  if (mpn_zero_p (tp, pn))
	    {
	      pwr += (mp_bitcnt_t) 1 << i;
	      MP_PTR_SWAP (qp, qp2);

	      qn = qn - pn;
	      qn += qp[qn] != 0;
	    }
	}
    }

  MPN_COPY (wp, qp, qn);
  *wn = qn;

  TMP_FREE;
  return pwr;
}
