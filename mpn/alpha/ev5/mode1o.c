/* Alpha EV5 mpn_modexact_1c_odd -- mpn by limb exact style remainder.

Copyright 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"


/*
        cycles/limb
   EV5:    30.0
   EV6:    15.0
*/


/* modlimb_invert is already faster than invert_limb or a "%", so the
   modexact style can be used even at size==1.

   On ev5, the dependent chain is as follows, and is what the code runs as.

        3  sub    y = x - h
       13  mulq   q = y * inverse
       14  umulh  h = high (q * d)
       --
       30

   For reference, ev6 runs this code at 15 cycles, which is 1 faster than
   the generic loop at 16.  But maybe something better is possible.  */

mp_limb_t
mpn_modexact_1c_odd (mp_srcptr src, mp_size_t size, mp_limb_t d,
                     mp_limb_t orig_c)
{
  mp_limb_t  s, x, y, inverse, dummy, ret;
  mp_limb_t  c = 0;
  mp_limb_t  h = orig_c;
  mp_size_t  i;

  ASSERT (size >= 1);
  ASSERT (d & 1);

  modlimb_invert (inverse, d);

  i = 0;
  if (size == 1)
    {
      s = src[0];
      goto last_step;
    }

  do
    {
      s = src[i];
      x = s - c;
      c = (x > s);

      y = x - h;
      c += (y > x);
      y *= inverse;
      umul_ppmm (h, dummy, y, d);
    }
  while (++i < size-1);


  s = src[i];
  if (s <= d)
    {
      /* With high s<=d the final step can be a subtract and addback.  If
	 c+h==0 then the addback will restore to l>=0.  If c+h==d then will
	 get x==d if s==0, but that's ok per the function definition.  */

      c += h;

      x = c - s;
      if (x > c)
	x += d;
      ret = x;
    }
  else
    {
      /* Can't skip a divide, just do the loop code once more. */
    last_step:
      x = s - c;
      c = (x > s);

      y = x - h;
      c += (y > x);

      y *= inverse;
      umul_ppmm (h, dummy, y, d);

      ret = c + h;
    }

  ASSERT (orig_c < d ? ret < d : ret <= d);
  return ret;
}
