/* __gmpn_divrem_1n

   THIS FILE CONTAINS INTERNAL FUNCTIONS WITH MUTABLE INTERFACES.  IT IS
   ONLY SAFE TO REACH THEM THROUGH DOCUMENTED INTERFACES.  IN FACT, IT IS
   ALMOST GUARANTEED THAT THEY'LL CHANGE OR DISAPPEAR IN A FUTURE GNU MP
   RELEASE.


Copyright (C) 1999, 2000 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"


/* Botch.  Called from mpn_divrem in gmp.h.  */
mp_limb_t
#if __STDC__
__gmpn_divrem_1n (mp_ptr qp, mp_size_t qxn,
	      mp_ptr np, mp_size_t nn,
	      mp_limb_t d)
#else
__gmpn_divrem_1n (qp, qxn, np, nn, d)
     mp_ptr qp;
     mp_size_t qxn;
     mp_ptr np;
     mp_size_t nn;
     mp_limb_t d;
#endif
{
  mp_limb_t ret;
  mp_ptr qp2;
  TMP_DECL (mark);

  TMP_MARK (mark);
  qp2 = (mp_ptr) TMP_ALLOC ((nn + qxn) * sizeof (mp_limb_t));

  np[0] = mpn_divrem_1 (qp2, qxn, np, nn, d);
  MPN_COPY (qp, qp2, (nn + qxn) - 1);
  ret =  qp2[nn + qxn - 1];

  TMP_FREE (mark);
  return ret;
}
