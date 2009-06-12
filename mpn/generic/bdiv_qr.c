/* mpn_bdiv_qq -- Hensel division with precomputed inverse, returning quotient
   and remainder.

   Contributed to the GNU project by Torbjörn Granlund.

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


/* Computes Q = N / D mod B^n,
	    R = N - QD,
	    destroys N.	 */

mp_limb_t
mpn_bdiv_qr (mp_ptr qp, mp_ptr rp,
	     mp_srcptr np, mp_size_t nn,
	     mp_srcptr dp, mp_size_t dn,
	     mp_ptr tp)
{
  mp_limb_t di;
  mp_limb_t rh;
  mp_size_t qn = nn - dn + 1;

  dn = MIN (qn, dn);
  if (BELOW_THRESHOLD (dn, DC_BDIV_QR_THRESHOLD))
    {
      if (np != rp)
	MPN_COPY (rp, np, qn);
      binvert_limb (di, dp[0]);  di = -di;
      rh = mpn_sb_bdiv_qr (qp, rp, qn, dp, dn, di);
    }
  else if (BELOW_THRESHOLD (dn, MU_BDIV_QR_THRESHOLD))
    {
      if (np != rp)
	MPN_COPY (rp, np, qn);
      binvert_limb (di, dp[0]);  di = -di;
      rh = mpn_dc_bdiv_qr (qp, rp, qn, dp, dn, di);
    }
  else
    {
      rh = mpn_mu_bdiv_qr (qp, rp, np, qn, dp, dn, tp);
    }
  return rh;
}

mp_size_t
mpn_bdiv_qr_itch (mp_size_t nn, mp_size_t dn)
{
  mp_size_t qn;

  return mpn_mu_bdiv_qr_itch (nn, dn);
}
