/* mpn_divrem_1 -- mpn by limb division.

Copyright 1991, 1993, 1994, 1996, 1998, 1999, 2000, 2002 Free Software
Foundation, Inc.

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


/* The size where udiv_qrnnd_preinv should be used rather than udiv_qrnnd,
   meaning the quotient size where that should happen, the quotient size
   being how many udiv divisions will be done.

   The default is to use preinv always, CPUs where this doesn't suit have
   tuned thresholds.  Note in particular that preinv should certainly be
   used if that's the only division available (USE_PREINV_ALWAYS).  */

#ifndef DIVREM_1_NORM_THRESHOLD
#define DIVREM_1_NORM_THRESHOLD  0
#endif
#ifndef DIVREM_1_UNNORM_THRESHOLD
#define DIVREM_1_UNNORM_THRESHOLD  0
#endif



/* If the cpu only has multiply-by-inverse division (eg. alpha), then NORM
   and UNNORM thresholds are 0 and only the inversion code is included.

   If multiply-by-inverse is never viable, then NORM and UNNORM thresholds
   will be MP_SIZE_T_MAX and only the plain division code is included.

   Otherwise mul-by-inverse is better than plain division above some
   threshold, and best results are obtained by having code for both present.

   The main reason for separating the norm and unnorm cases is that not all
   CPUs give zero for "n0 >> BITS_PER_MP_LIMB" which would arise in the
   unnorm code used on an already normalized divisor.

   If UDIV_NEEDS_NORMALIZATION is false then plain division uses the same
   non-shifting code for both the norm and unnorm cases, though with
   different criteria for skipping a division, and with different thresholds
   of course.  And in fact if inversion is never viable, then that simple
   non-shifting division would be all that's left.

   The NORM and UNNORM thresholds might not differ much, but if there's
   going to be separate code for norm and unnorm then it makes sense to have
   separate thresholds.  One thing that's possible is that the
   mul-by-inverse might be better only for normalized divisors, due to that
   case not needing variable bit shifts.

   Notice that the thresholds are tested after the decision to possibly skip
   one divide step, so they're based on the actual number of divisions done.

   For the unnorm case, it would be possible to call mpn_lshift to adjust
   the dividend all in one go (into the quotient space say), rather than
   limb-by-limb in the loop.  This might help if mpn_lshift is a lot faster
   than what the compiler can generate for EXTRACT.  But this is left to CPU
   specific implementations to consider, especially since EXTRACT isn't on
   the dependent chain.  */

mp_limb_t
mpn_divrem_1 (mp_ptr qp, mp_size_t xsize,
              mp_srcptr ap, mp_size_t size, mp_limb_t d)
{
  mp_size_t  total_size;
  mp_size_t  i;
  mp_limb_t  n1, n0;
  mp_limb_t  r = 0;

  ASSERT (xsize >= 0);
  ASSERT (size >= 0);
  ASSERT (d != 0);
  /* FIXME: What's the correct overlap rule when xsize!=0? */
  ASSERT (MPN_SAME_OR_SEPARATE_P (qp+xsize, ap, size));

  total_size = size + xsize;
  if (total_size == 0)
    return 0;

  qp += (total_size - 1);   /* dest high limb */

  if ((d & MP_LIMB_T_HIGHBIT) != 0)
    {
      if (size != 0)
        {
          /* High quotient limb is 0 or 1, and skip a divide step. */
          mp_limb_t  q;
          r = ap[size-1];
          q = (r >= d);
          *qp-- = q;
          r -= (d & -q);
          total_size--;
          size--;
        }

      if (BELOW_THRESHOLD (total_size, DIVREM_1_NORM_THRESHOLD))
        {
        plain:
          for (i = size-1; i >= 0; i--)
            {
              n0 = ap[i];
              udiv_qrnnd (*qp, r, r, n0, d);
              qp--;
            }
          for (i = 0; i < xsize; i++)
            {
              udiv_qrnnd (*qp, r, r, 0, d);
              qp--;
            }
          return r;
        }
      else
        {
          /* Multiply-by-inverse, divisor already normalized. */
	  mp_limb_t dinv;
	  invert_limb (dinv, d);

          for (i = size-1; i >= 0; i--)
            {
              n0 = ap[i];
              udiv_qrnnd_preinv (*qp, r, r, n0, d, dinv);
              qp--;
            }
          for (i = 0; i < xsize; i++)
            {
              udiv_qrnnd_preinv (*qp, r, r, 0, d, dinv);
              qp--;
            }
	  return r;
        }
    }
  else
    {
      int  norm;

      /* Skip a division if high < divisor (high quotient 0).  Testing here
         before before normalizing will still skip as often as possible.  */
      if (size != 0)
        {
          n1 = ap[size-1];
          if (n1 < d)
            {
              r = n1;
              *qp-- = 0;
              total_size--;
              if (total_size == 0)
                return r;
              size--;
            }
        }

      if (! UDIV_NEEDS_NORMALIZATION
          && BELOW_THRESHOLD (total_size, DIVREM_1_UNNORM_THRESHOLD))
        goto plain;

      count_leading_zeros (norm, d);
      d <<= norm;
      r <<= norm;

#define EXTRACT   ((n1 << norm) | (n0 >> (BITS_PER_MP_LIMB - norm)))

      if (UDIV_NEEDS_NORMALIZATION
          && BELOW_THRESHOLD (total_size, DIVREM_1_UNNORM_THRESHOLD))
        {
          if (size != 0)
            {
              n1 = ap[size-1];
              r |= (n1 >> (BITS_PER_MP_LIMB - norm));
              for (i = size-2; i >= 0; i--)
                {
                  n0 = ap[i];
                  udiv_qrnnd (*qp, r, r, EXTRACT, d);
                  qp--;
                  n1 = n0;
                }
              udiv_qrnnd (*qp, r, r, n1 << norm, d);
              qp--;
            }
          for (i = 0; i < xsize; i++)
            {
              udiv_qrnnd (*qp, r, r, 0, d);
              qp--;
            }
          return r >> norm;
        }
      else
        {
	  mp_limb_t  dinv;
	  invert_limb (dinv, d);
          if (size != 0)
            {
              n1 = ap[size-1];
              r |= (n1 >> (BITS_PER_MP_LIMB - norm));
              for (i = size-2; i >= 0; i--)
                {
                  n0 = ap[i];
                  udiv_qrnnd_preinv (*qp, r, r, EXTRACT, d, dinv);
                  qp--;
                  n1 = n0;
                }
              udiv_qrnnd_preinv (*qp, r, r, n1 << norm, d, dinv);
              qp--;
            }
          for (i = 0; i < xsize; i++)
            {
              udiv_qrnnd_preinv (*qp, r, r, 0, d, dinv);
              qp--;
            }
	  return r >> norm;
        }
    }
}
