/* UltraSPARC 64 mpn_modexact_1c_odd -- mpn by limb exact style remainder.

   THE FUNCTIONS IN THIS FILE ARE FOR INTERNAL USE ONLY.  THEY'RE ALMOST
   CERTAIN TO BE SUBJECT TO INCOMPATIBLE CHANGES OR DISAPPEAR COMPLETELY IN
   FUTURE GNU MP RELEASES.

Copyright 2003 Free Software Foundation, Inc.

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


#define LOW32(x)   ((x) & 0xFFFFFFFF)
#define HIGH32(x)  ((x) >> 32)


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
    ASSERT (dh == HIGH32(d));                   \
    ASSERT (dl == LOW32(d));                    \
    ASSERT (q*d == l);                          \
                                                \
    ql = LOW32 (q);                             \
    qh = HIGH32 (q);                            \
                                                \
    pm1 = ql * dh;                              \
    pm2 = qh * dl;                              \
    ph  = qh * dh;                              \
                                                \
    pm_l = LOW32 (pm1) + LOW32 (pm2);           \
                                                \
    (h) = ph + HIGH32 (pm1) + HIGH32 (pm2)      \
      + HIGH32 (pm_l) + ((pm_l << 32) > l);     \
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
    ASSERT (HIGH32(d) == 0);                    \
                                                \
    pm = HIGH32(q) * d;                         \
    (h) = HIGH32(pm) + ((pm << 32) > l);        \
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
