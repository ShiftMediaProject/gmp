/* mpn_get_str -- Convert a MSIZE long limb vector pointed to by MPTR
   to a printable string in STR in base BASE.

Copyright 1991, 1992, 1993, 1994, 1996, 2000, 2001, 2002 Free Software
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


/* The x86s and m68020 have a quotient and remainder "div" instruction and
   gcc recognises an adjacent "/" and "%" can be combined using that.
   Elsewhere "/" and "%" are either separate instructions, or separate
   libgcc calls (which unfortunately gcc as of version 3.0 doesn't combine).
   A multiply and subtract should be faster than a "%" in those cases.  */
#if HAVE_HOST_CPU_FAMILY_x86            \
  || HAVE_HOST_CPU_m68020               \
  || HAVE_HOST_CPU_m68030               \
  || HAVE_HOST_CPU_m68040               \
  || HAVE_HOST_CPU_m68060               \
  || HAVE_HOST_CPU_m68360 /* CPU32 */
#define udiv_qrnd_unnorm(q,r,n,d)       \
  do {                                  \
    mp_limb_t  __q = (n) / (d);         \
    mp_limb_t  __r = (n) % (d);         \
    (q) = __q;                          \
    (r) = __r;                          \
  } while (0)
#else
#define udiv_qrnd_unnorm(q,r,n,d)       \
  do {                                  \
    mp_limb_t  __q = (n) / (d);         \
    mp_limb_t  __r = (n) - __q*(d);     \
    (q) = __q;                          \
    (r) = __r;                          \
  } while (0)
#endif


/* Convert the limb vector pointed to by MPTR and MSIZE long to a
   char array, using base BASE for the result array.  Store the
   result in the character array STR.  STR must point to an array with
   space for the largest possible number represented by a MSIZE long
   limb vector + 1 extra character.

   The result is NOT in Ascii, to convert it to printable format, add
   '0' or 'A' depending on the base and range.

   Return the number of digits in the result string.
   This may include some leading zeros.

   The limb vector pointed to by MPTR is clobbered.  */

size_t
mpn_get_str (unsigned char *str, int base, mp_ptr mptr, mp_size_t msize)
{
  mp_limb_t big_base;
  mp_size_t out_len;
  unsigned char *s;

  ASSERT (msize >= 0);
  ASSERT (base >= 2);
  ASSERT (base < numberof (__mp_bases));
  ASSERT (msize == 0 || mptr[msize-1] != 0);

  big_base = __mp_bases[base].big_base;

  s = str;

  /* Special case zero, as the code below doesn't handle it.  */
  if (msize == 0)
    {
      s[0] = 0;
      return 1;
    }

  if ((base & (base - 1)) == 0)
    {
      /* The base is a power of 2.  Make conversion from most
	 significant side.  */
      mp_limb_t n1, n0;
      int bits_per_digit = big_base;
      int x;
      int bit_pos;
      int i;

      n1 = mptr[msize - 1];
      count_leading_zeros (x, n1);

      /* BIT_POS should be R when input ends in least sign. nibble,
         R + bits_per_digit * n when input ends in n:th least significant
         nibble. */

      {
	int bits;

	bits = BITS_PER_MP_LIMB * msize - x;
	x = bits % bits_per_digit;
	if (x != 0)
	  bits += bits_per_digit - x;
	bit_pos = bits - (msize - 1) * BITS_PER_MP_LIMB;
      }

      /* Fast loop for bit output.  */
      i = msize - 1;
      for (;;)
	{
	  bit_pos -= bits_per_digit;
	  while (bit_pos >= 0)
	    {
	      *s++ = (n1 >> bit_pos) & ((1 << bits_per_digit) - 1);
	      bit_pos -= bits_per_digit;
	    }
	  i--;
	  if (i < 0)
	    break;
	  n0 = (n1 << -bit_pos) & ((1 << bits_per_digit) - 1);
	  n1 = mptr[i];
	  bit_pos += BITS_PER_MP_LIMB;
	  *s++ = n0 | (n1 >> bit_pos);
	}

      *s = 0;

      return s - str;
    }
  else
    {
      /* General case.  The base is not a power of 2.  Make conversion
	 from least significant end.  */
      mp_limb_t n1, c;

      out_len = ((size_t) msize * BITS_PER_MP_LIMB
                 * __mp_bases[base].chars_per_bit_exactly) + 1;
      s += out_len;

      if (base == 10)
        {
          /* Special case code for base==10 so that the compiler has a
             chance to optimize divisions by 10 in udiv_qrnd_unnorm.  */
          while (msize > 1)
            {
              int i;
              n1 = MPN_DIVREM_OR_PREINV_DIVREM_1
                (mptr, (mp_size_t) 0, mptr, msize,
                 MP_BASES_BIG_BASE_10,
                 MP_BASES_BIG_BASE_INVERTED_10,
                 MP_BASES_NORMALIZATION_STEPS_10);
              msize -= mptr[msize-1] == 0;

              /* Convert N1 from BIG_BASE to a string of digits in BASE
                 using single precision operations.  */
              i = MP_BASES_CHARS_PER_LIMB_10;
              do
                {
                  udiv_qrnd_unnorm (n1, c, n1, 10);
                  *--s = c;
                  i--;
                }
              while (i != 0);
            }

          n1 = mptr[0];
          while (n1 != 0)
            {
              udiv_qrnd_unnorm (n1, c, n1, 10);
              *--s = c;
            }
        }
      else
        {
          unsigned   dig_per_u = __mp_bases[base].chars_per_limb;
#if USE_PREINV_DIVREM_1
          unsigned   normalization_steps;
          mp_limb_t  big_base_inverted = __mp_bases[base].big_base_inverted;
          count_leading_zeros (normalization_steps, big_base);
#endif
          while (msize > 1)
            {
              int i;
              n1 = MPN_DIVREM_OR_PREINV_DIVREM_1 (mptr, (mp_size_t) 0,
                                                  mptr, msize, big_base,
                                                  big_base_inverted,
                                                  normalization_steps);
              msize -= mptr[msize-1] == 0;

              /* Convert N1 from BIG_BASE to a string of digits in BASE
                 using single precision operations.  */
              i = dig_per_u;
              do
                {
                  udiv_qrnd_unnorm (n1, c, n1, base);
                  *--s = c;
                  i--;
                }
              while (i != 0);
            }

          n1 = mptr[0];
          while (n1 != 0)
            {
              udiv_qrnd_unnorm (n1, c, n1, base);
              *--s = c;
            }
        }

      ASSERT (s >= str);
      while (s != str)
	*--s = 0;

      return out_len;
    }
}
