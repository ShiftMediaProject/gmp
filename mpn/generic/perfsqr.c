/* mpn_perfect_square_p(u,usize) -- Return non-zero if U is a perfect square,
   zero otherwise.

Copyright 1991, 1993, 1994, 1996, 1997, 2000, 2001 Free Software Foundation,
Inc.

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

#include <stdio.h> /* for NULL */
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"


/* sq_res_0x100[x mod 0x100] == 1 iff x mod 0x100 is a quadratic residue
   modulo 0x100.  */
static unsigned char const sq_res_0x100[0x100] =
{
  1,1,0,0,1,0,0,0,0,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,
  0,1,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,
  1,1,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,
  0,1,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,
  0,1,0,0,1,0,0,0,0,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,
  0,1,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,
  0,1,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,
  0,1,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0
};

int
mpn_perfect_square_p (mp_srcptr up, mp_size_t usize)
{
  mp_limb_t rem;
  mp_ptr root_ptr;
  int res;
  TMP_DECL (marker);

  ASSERT (usize >= 1);

  /* The first test excludes 212/256 (82.8%) of the perfect square candidates
     in O(1) time.  */
  if ((sq_res_0x100[(unsigned int) up[0] % 0x100]) == 0)
    return 0;

#if 0
  /* Check that we have even multiplicity of 2, and then check that the rest is
     a possible perfect square.  Leave disabled until we can determine this
     really is an improvement.  It it is, it could completely replace the
     simple probe above, since this should through out more non-squares, but at
     the expense of somewhat more cycles.  */
  {
    mp_limb_t lo;
    int cnt;
    lo = up[0];
    while (lo == 0)
      up++, lo = up[0], usize--;
    count_trailing_zeros (cnt, lo);
    if ((cnt & 1) != 0)
      return 0;			/* return of not even multiplicity of 2 */
    lo >>= cnt;			/* shift down to align lowest non-zero bit */
    lo >>= 1;			/* shift away lowest non-zero bit */
    if ((lo & 3) != 0)
      return 0;
  }
#endif

#if defined (PP)
  /* The second test excludes most of the remaining perfect square candidates
     in O(n) time.  Each residue tested keeps (p+1)/2 out of p, and with no
     common factors the tests are independent so the total passed is

              p=M    (p+1)/2
             product  -------
              p=3        p

     which, for M=29 (for 32-bit hosts) is 155520/30808063, or 99.5% excluded,
     and for M=53 (for 64-bit hosts) is 67722117120/742518990138757, or 99.99%
     excluded.

     If U is just a few limbs, computing the square root does not take long
     time, so things might run faster if we suppress this for A less than some
     appropriate threshold.  */


  /* Firstly, compute REM = U mod PP.  */
#if defined (PP_INVERTED)
  rem = MPN_MOD_OR_PREINV_MOD_1 (up, usize, (mp_limb_t) PP,
                                 (mp_limb_t) PP_INVERTED);
#else
  rem = mpn_mod_1 (up, usize, (mp_limb_t) PP);
#endif

  /* Now decide if REM is a quadratic residue modulo the factors in PP.  We
     test prime pairs multiplied together, as long as their products don't
     overflow a limb.  The tests that exclude most false squares are put
     first.  */

  if (0
#if BITS_PER_MP_LIMB == 64
      || ((CNST_LIMB(0x0C22C90530902D3) >> rem % (3 * 19)) & 1) == 0
      || ((CNST_LIMB(0x0230148611CA33) >> rem % (5 * 11)) & 1) == 0
      || ((CNST_LIMB(0x12DD703303AED3) >> rem % 53) & 1) == 0
      || ((CNST_LIMB(0x04351B2753DF) >> rem % 47) & 1) == 0
      || ((CNST_LIMB(0x35883A3EE53) >> rem % 43) & 1) == 0
      || ((CNST_LIMB(0x1B382B50737) >> rem % 41) & 1) == 0
      || ((CNST_LIMB(0x165E211E9B) >> rem % 37) & 1) == 0
      || ((0x121D47B7L >> rem % 31) & 1) == 0
      || ((0x13D122F3L >> rem % 29) & 1) == 0
      || ((0x05335FL >> rem % 23) & 1) == 0
      /* 19 handled above */
      || ((0x1A317L >> rem % 17) & 1) == 0
      || ((0x161BL >> rem % 13) & 1) == 0
      /* 11 handled above */
      || ((0x17L >> rem % 7) & 1) == 0
      /* 5 handled above */
      /* 3 handled above */
#endif
#if BITS_PER_MP_LIMB == 32
      || ((0x058293L >> rem % (3 * 7)) & 1) == 0
      || ((0x13D122F3L >> rem % 29) & 1) == 0
      || ((0x05335FL >> rem % 23) & 1) == 0
      || ((0x30AF3L >> rem % 19) & 1) == 0
      || ((0x1A317L >> rem % 17) & 1) == 0
      || ((0x161BL >> rem % 13) & 1) == 0
      || ((0x23BL >> rem % 11) & 1) == 0
      /* 7 handled above */
      || ((0x13L >> rem % 5) & 1) == 0
      /* 3 handled above */
#endif
#if BITS_PER_MP_LIMB == 16
      || ((0x0653L >> rem % (3 * 5)) & 1) == 0
      || ((0x161BL >> rem % 13) & 1) == 0
      || ((0x23BL >> rem % 11) & 1) == 0
      || ((0x17L >> rem % 7) & 1) == 0
      /* 5 handled above */
      /* 3 handled above */
#endif
#if BITS_PER_MP_LIMB == 8
      || ((0x17L >> rem % 7) & 1) == 0
      || ((0x13L >> rem % 5) & 1) == 0
      || ((0x3L >> rem % 3) & 1) == 0
#endif
#if BITS_PER_MP_LIMB == 4
      || ((0x13L >> rem % 5) & 1) == 0
      || ((0x3L >> rem % 3) & 1) == 0
#endif
#if BITS_PER_MP_LIMB == 2
      || ((0x3L >> rem % 3) & 1) == 0
#endif
      )
    return 0;
#endif /* PP */

  TMP_MARK (marker);

  /* For the third and last test, we finally compute the square root,
     to make sure we've really got a perfect square.  */
  root_ptr = (mp_ptr) TMP_ALLOC ((usize + 1) / 2 * BYTES_PER_MP_LIMB);

  /* Iff mpn_sqrtrem returns zero, the square is perfect.  */
  res = ! mpn_sqrtrem (root_ptr, NULL, up, usize);
  TMP_FREE (marker);
  return res;
}
