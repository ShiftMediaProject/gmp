/* mpf_set_d -- Assign a float from a IEEE double.

Copyright (C) 1993, 1994, 1995 Free Software Foundation, Inc.

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

void
#if __STDC__
mpf_set_d (mpf_ptr r, double d)
#else
mpf_set_d (r, d)
     mpf_ptr r;
     double d;
#endif
{
  mp_ptr rp;
  mp_size_t size;
  mp_exp_t exp;
  mp_limb_t manh, manl;
  mp_limb_t man2, man1, man0;
  union ieee_double_extract x;
  unsigned sc;

  /* ??? This needs more work since it assumes that limbs are 32 bits.
     See mpz/set_d for ideas on how to handle 64-bit machines.  */

  if (d == 0)
    {
      r->_mp_exp = 0;
      r->_mp_size = 0;
      return;
    }

  rp = r->_mp_d;
  x.d = d;

  exp = x.s.exp;
  sc = (unsigned) (exp + 2) % BITS_PER_MP_LIMB;

  manh = 0x80000000 | (x.s.manh << 11) | (x.s.manl >> 21);
  manl = x.s.manl << 11;
  if (sc != 0)
    {
      man2 = manh >> (BITS_PER_MP_LIMB - sc);
      man1 = (manl >> (BITS_PER_MP_LIMB - sc)) | (manh << sc);
      man0 = manl << sc;
    }
  else
    {
      man2 = manh;
      man1 = manl;
      man0 = 0;
    }

  if (man0 == 0)
    {
      if (man1 == 0)
	{
	  size = 1;
	  rp[0] = man2;
	}
      else
	{
	  size = 2;
	  rp[1] = man2;
	  rp[0] = man1;
	}
    }
  else
    {
      size = 3;
      rp[2] = man2;
      rp[1] = man1;
      rp[0] = man0;
    }

  r->_mp_exp = (exp + 1) / BITS_PER_MP_LIMB - 1024 / BITS_PER_MP_LIMB + 1;
  r->_mp_size = x.s.sig == 0 ? size : -size;
}
