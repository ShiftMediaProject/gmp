/* mpf_div_ui -- Divide a float with an unsigned integer.

Copyright (C) 1993, 1994, 1996 Free Software Foundation, Inc.

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

void
#if __STDC__
mpf_div_ui (mpf_ptr r, mpf_srcptr u, unsigned long int v)
#else
mpf_div_ui (r, u, v)
     mpf_ptr r;
     mpf_srcptr u;
     unsigned long int v;
#endif
{
  mp_srcptr up;
  mp_ptr qp, rp, rrp;
  mp_size_t usize = u->_mp_size;
  mp_size_t qsize, rsize;
  mp_size_t sign_quotient = usize;
  mp_limb_t q_limb;
  mp_size_t prec;
  mp_exp_t rexp;
  TMP_DECL (marker);

  TMP_MARK (marker);
  prec = r->_mp_prec;
  usize = ABS (usize);

  rsize = prec;

  qp = r->_mp_d;
  up = u->_mp_d;
  rp = (mp_ptr) TMP_ALLOC ((rsize + 1) * BYTES_PER_MP_LIMB);
  if (usize > rsize)
    {
      up += usize - rsize;
      usize = rsize;
      rrp = rp;
    }
  else
    {
      MPN_ZERO (rp, rsize - usize);
      rrp = rp + (rsize - usize);
    }

  /* Move the dividend to the remainder.  */
  MPN_COPY (rrp, up, usize);

  mpn_divmod_1 (qp, rp, rsize, v);
  q_limb = qp[rsize - 1];

  qsize = rsize - (q_limb == 0);
  rexp = u->_mp_exp - (q_limb == 0);
  r->_mp_size = sign_quotient >= 0 ? qsize : -qsize;
  r->_mp_exp = rexp;
  TMP_FREE (marker);
}
