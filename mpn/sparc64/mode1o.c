/* UltraSPARC 64 mpn_modexact_1c_odd -- mpn by limb exact style remainder.

   THE FUNCTIONS IN THIS FILE ARE FOR INTERNAL USE ONLY.  THEY'RE ALMOST
   CERTAIN TO BE SUBJECT TO INCOMPATIBLE CHANGES OR DISAPPEAR COMPLETELY IN
   FUTURE GNU MP RELEASES.

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

/*                 64-bit divisor   32-bit divisor
                    cycles/limb      cycles/limb
                     (approx)         (approx)
   Ultrasparc 2i:      108               64
*/


/* This implementation reduces the number of multiplies done, knowing that
   on ultrasparc 1 and 2 the mulx instruction stalls the whole chip.

   The key idea is to use the fact that the low limb of q*d equals l, this
   being the whole aim of the q calculated.  It means there's no need to
   calculate the lowest 32x32->64 part of the q*d, instead it can be
   inferred from l and the other three 32x32->64 parts.

   When d is 32-bits, the same applies, but in this case there's only one
   other 32x32->64 part (ie. HIGH32(q)*d).

   The net effect is that for 64-bit divisor each limb is 4 mulx, or for
   32-bit divisor each is 2 mulx.

   Enhancements:

   No doubt this could be done in assembler, if that helped the scheduling,
   or perhaps guaranteed good code irrespective of the compiler.

   Alternatives:

   It might be possibly to use floating point.  The loop is dominated by
   multiply latency, so not sure if floats would improve that.  One
   possibility would be to take two limbs at a time, with a 128 bit inverse,
   if there's enough registers, which could effectively use float throughput
   to reduce total latency across two limbs.  */



/* umul_ppmm_lowequal sets h to the high limb of q*d, assuming the low limb
   of that product is equal to l.  dh and dl are the 32-bit halves of d.

   |-----high----||----low-----|
   +------+------+
   |             |                 ph = qh * dh
   +------+------+
          +------+------+
          |             |          pm1 = ql * dh
          +------+------+
          +------+------+
          |             |          pm2 = qh * dl
          +------+------+
                 +------+------+
                 |             |   pl = ql * dl (not calculated)
                 +------+------+

   Knowing that the low 64 bits is equal to l means that LOW(pm1) + LOW(pm2)
   + HIGH(pl) == HIGH(l).  The only thing we need from those product parts
   is whether they produce a carry into the high.

   pm_l = LOW(pm1)+LOW(pm2) is done to contribute its carry, then the only
   time there's a further carry from LOW(pm_l)+HIGH(pl) is if LOW(pm_l) >
   HIGH(l).  pl is never actually calculated.  */

#define umul_ppmm_lowequal(h, q, d, dh, dl, l)  \
  do {                                          \
    mp_limb_t  ql, qh, ph, pm1, pm2, pm_l;      \
    ASSERT (dh == HIGH(d));                     \
    ASSERT (dl == LOW(d));                      \
    ASSERT (q*d == l);                          \
                                                \
    ql = LOW (q);                               \
    qh = HIGH (q);                              \
                                                \
    pm1 = ql * dh;                              \
    pm2 = qh * dl;                              \
    ph  = qh * dh;                              \
                                                \
    pm_l = LOW (pm1) + LOW (pm2);               \
                                                \
    (h) = ph + HIGH (pm1) + HIGH (pm2)          \
      + HIGH (pm_l) + ((pm_l << 32) > l);       \
                                                \
    ASSERT_HIGH_PRODUCT (h, q, d);              \
  } while (0)


/* Set h to the high of q*d, assuming the low limb of that product is equal
   to l, and that d fits in 32-bits.

   |-----high----||----low-----|
          +------+------+
          |             |          pm = qh * dl
          +------+------+
                 +------+------+
                 |             |   pl = ql * dl (not calculated)
                 +------+------+

   Knowing that LOW(pm) + HIGH(pl) == HIGH(l) (mod 2^32) means that the only
   time there's a carry from that sum is when LOW(pm) > HIGH(l).  There's no
   need to calculate pl to determine this.  */

#define umul_ppmm_half_lowequal(h, q, d, l)     \
  do {                                          \
    mp_limb_t pm;                               \
    ASSERT (q*d == l);                          \
    ASSERT (HIGH(d) == 0);                      \
                                                \
    pm = HIGH(q) * d;                           \
    (h) = HIGH(pm) + ((pm << 32) > l);          \
    ASSERT_HIGH_PRODUCT (h, q, d);              \
  } while (0)


/* check that h is the high limb of x*y */
#if WANT_ASSERT
#define ASSERT_HIGH_PRODUCT(h, x, y)    \
  do {                                  \
    mp_limb_t  want_h, dummy;           \
    umul_ppmm (want_h, dummy, x, y);    \
    ASSERT (h == want_h);               \
  } while (0)
#else
#define ASSERT_HIGH_PRODUCT(h, q, d)    \
  do { } while (0)
#endif

#define ASSERT_RETVAL(r)                \
  ASSERT (orig_c < d ? r < d : r <= d)


#define LOW(x)   ((x) & 0xFFFFFFFF)
#define HIGH(x)  ((x) >> 32)


mp_limb_t
mpn_modexact_1c_odd (mp_srcptr src, mp_size_t size, mp_limb_t d, mp_limb_t orig_c)
{
  mp_limb_t  c = orig_c;
  mp_limb_t  s, l, q, h, inverse;

  ASSERT (size >= 1);
  ASSERT (d & 1);
  ASSERT_MPN (src, size);
  ASSERT_LIMB (d);
  ASSERT_LIMB (c);

  modlimb_invert (inverse, d);

  if (d <= 0xFFFFFFFF)
    {
      s = *src++;
      size--;
      /* use the loop code when size==1, to cope with c>d */
      if (size == 0)
        goto once_more_32bits;

      do
        {
          SUBC_LIMB (c, l, s, c);
          s = *src++;
          q = l * inverse;
          umul_ppmm_half_lowequal (h, q, d, l);
          c += h;
          size--;
        }
      while (size != 0);

      if (s <= d)
        {
          /* With high s <= d the final step can be a subtract and addback.
             If c==0 then the addback will restore to l>=0.  If c==d then
             will get l==d if s==0, but that's ok per the function
             definition.  */

          l = c - s;
          l += (l > c ? d : 0);

          ASSERT_RETVAL (l);
          return l;
        }
      else
        {
          /* Can't skip a divide, just do the loop code once more. */
        once_more_32bits:
          SUBC_LIMB (c, l, s, c);
          q = l * inverse;
          umul_ppmm_half_lowequal (h, q, d, l);
          c += h;

          ASSERT_RETVAL (c);
          return c;
        }
    }
  else
    {
      mp_limb_t  dl = LOW (d);
      mp_limb_t  dh = HIGH (d);
      long i;

      s = *src++;
      size--;
      /* use the loop code when size==1, to cope with c>d */
      if (size == 0)
        goto once_more_64bits;

      do
        {
          SUBC_LIMB (c, l, s, c);
          s = *src++;
          q = l * inverse;
          umul_ppmm_lowequal (h, q, d, dh, dl, l);
          c += h;
          size--;
        }
      while (size != 0);

      if (s <= d)
        {
          /* With high s <= d the final step can be a subtract and addback.
             If c==0 then the addback will restore to l>=0.  If c==d then
             will get l==d if s==0, but that's ok per the function
             definition.  */

          l = c - s;
          l += (l > c ? d : 0);

          ASSERT_RETVAL (l);
          return l;
        }
      else
        {
          /* Can't skip a divide, just do the loop code once more. */
        once_more_64bits:
          SUBC_LIMB (c, l, s, c);
          q = l * inverse;
          umul_ppmm_lowequal (h, q, d, dh, dl, l);
          c += h;

          ASSERT_RETVAL (c);
          return c;
        }
    }
}
