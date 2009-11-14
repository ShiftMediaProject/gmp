/* mpn_bdiv_q -- Hensel division with precomputed inverse, returning quotient.

   Contributed to the GNU project by Torbjorn Granlund.

   THE FUNCTIONS IN THIS FILE ARE INTERNAL WITH MUTABLE INTERFACES.  IT IS ONLY
   SAFE TO REACH THEM THROUGH DOCUMENTED INTERFACES.  IN FACT, IT IS ALMOST
   GUARANTEED THAT THEY WILL CHANGE OR DISAPPEAR IN A FUTURE GMP RELEASE.

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
	    destroys N.  */

void
mpn_bdiv_q (mp_ptr qp,
	    mp_srcptr np, mp_size_t nn,
	    mp_srcptr dp, mp_size_t dn,
	    mp_ptr tp)
{
  mp_limb_t di;
  mp_size_t qn = nn - dn + 1;

  dn = MIN (qn, dn);
  if (BELOW_THRESHOLD (dn, DC_BDIV_Q_THRESHOLD))
    {
      MPN_COPY (tp, np, qn);
      binvert_limb (di, dp[0]);  di = -di;
      mpn_sbpi1_bdiv_q (qp, tp, qn, dp, dn, di);
    }
  else if (BELOW_THRESHOLD (dn, MU_BDIV_Q_THRESHOLD))
    {
      MPN_COPY (tp, np, qn);
      binvert_limb (di, dp[0]);  di = -di;
      mpn_dcpi1_bdiv_q (qp, tp, qn, dp, dn, di);
    }
  else
    {
      mpn_mu_bdiv_q (qp, np, qn, dp, dn, tp);
    }
  return;
}

mp_size_t
mpn_bdiv_q_itch (mp_size_t nn, mp_size_t dn)
{
  return mpn_mu_bdiv_q_itch (nn, dn);
}
