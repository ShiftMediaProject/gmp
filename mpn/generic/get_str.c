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

/* Conversion of U {up,un} to a string in base b.  Basic algorithms:

  A) Divide U repeatedly by b, generating an integer quotient and remainder.
     Digits come out from right to left.  (Used in mpn_sb_divrem_mn.)

  B) Divide U by b^g, for g such that 1/b <= U/b^g < 1, generating a fraction.
     Then develop digits by multiplying the fraction repeatedly by b.  Digits
     come out from left to right.  (Currently not used herein.)

  C) Compute b^1, b^2, b^4, ..., b^(2^s), for s such that b^(2^s) > sqrt(U).
     Then divide U by b^(2^k), generating an integer quotient and remainder.
     Recursively convert the quotient, then the remainder, using the
     precomputed powers.  Digits come out from left to right.  (Used in
     mpn_dc_get_str.)

  When using algorithm C, algorithm B might be suitable for basecase code,
  since the required b^g power will be readily accessible.

  Optimization ideas:
  1. Obviously, b should normally be replaced by big_base, the largest power of
     b that fits in a limb.
  2. The recursive function of (C) could avoid TMP allocation:
     a) Overwrite dividend with quotient and remainder, just as permitted by
        mpn_sb_divrem_mn.
     b) If TMP memory is anyway needed, pass it as a parameter, similarly to
        how we do it in Karatsuba multiplication.
  3. Store the powers of (C) normalized, with the normalization count.
     Quotients will usually need to be left-shifted before each divide, and
     remainders will either need to be left-shifted of right-shifted.
  4. When b is even, the powers will end up with lots of low zero limbs.  Can
     we take advantage of that?

  Basic structure of (C):
    mpn_get_str:
      if POW2_P (n)
	...
      else
	if (un < GET_STR_PRECOMPUTE_THRESHOLD)
	  mpn_sb_get_str (str, base, up, un);
	else
	  precompute_power_tables
	  mpn_dc_get_str

    mpn_dc_get_str:
	mpn_tdiv_qr
	if (qn < GET_STR_BASECASE_THRESHOLD)
	  mpn_sb_get_str
	else
	  mpn_dc_get_str
	if (rn < GET_STR_BASECASE_THRESHOLD)
	  mpn_sb_get_str
	else
	  mpn_dc_get_str


  The reason for the two threshold values is the cost of
  precompute_power_tables.  GET_STR_PRECOMPUTE_THRESHOLD will be considerably
  larger than GET_STR_PRECOMPUTE_THRESHOLD.  Do you think I should change
  mpn_dc_get_str to instead look like the following?  */


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

/* When to stop divide-and-conquer and call the basecase mpn_get_str.  */
#ifndef GET_STR_BASECASE_THRESHOLD
#define GET_STR_BASECASE_THRESHOLD 15
#endif
/* Whether to bother at all with precomputing powers of the base, or go
   to the basecase mpn_get_str directly.  */
#ifndef GET_STR_PRECOMPUTE_THRESHOLD
#define GET_STR_PRECOMPUTE_THRESHOLD 30
#endif

struct powers
{
  size_t digits_in_base;
  mp_ptr p;
  mp_size_t n;		/* mpz_struct uses int for sizes, but not mpn! */
  int base;
};
typedef struct powers powers_t;


/* Convert {UP,UN} to a string with a base as represented in POWTAB, and put
   the string in STR.  Generate LEN characters, possibly padding with zeros to
   the left.  If LEN is zero, generate as many characters as required.
   Return a pointer immediately after the last digit of the result string.
   Complexity is O(UN^2) and is intended for small conversions.  */
static unsigned char *
mpn_sb_get_str (unsigned char *str, size_t len,
		mp_ptr up, mp_size_t un,
		powers_t *powtab)
{
  mp_limb_t rl, ul;
  unsigned char *s;
  int base;

#if TUNE_PROGRAM_BUILD
#define USE_MULTILIMB  1
#else
#define USE_MULTILIMB  (GET_STR_BASECASE_THRESHOLD > 2)
#endif

#if USE_MULTILIMB
#if TUNE_PROGRAM_BUILD
#define BUF_ALLOC (GET_STR_THRESHOLD_LIMIT * BITS_PER_MP_LIMB)
#else
#define BUF_ALLOC (GET_STR_PRECOMPUTE_THRESHOLD * BITS_PER_MP_LIMB)
#endif
  base = powtab->base;
  if (base == 10)
    {
      /* Special case code for base==10 so that the compiler has a
	 chance to optimize divisions by 10 in udiv_qrnd_unnorm.  */
      size_t l;
      unsigned char buf[BUF_ALLOC];

      s = buf + BUF_ALLOC;
      while (un > 1)
	{
	  int i;
	  ul = MPN_DIVREM_OR_PREINV_DIVREM_1
	    (up, (mp_size_t) 0, up, un, MP_BASES_BIG_BASE_10,
	     MP_BASES_BIG_BASE_INVERTED_10,
	     MP_BASES_NORMALIZATION_STEPS_10);
	  un -= up[un - 1] == 0;

	  /* Convert ul from big_base to a string of digits in base using
	     single precision operations.  */
	  i = MP_BASES_CHARS_PER_LIMB_10;
	  do
	    {
	      udiv_qrnd_unnorm (ul, rl, ul, 10);
	      *--s = rl;
	    }
	  while (--i != 0);
	}

      ul = up[0];
      while (ul != 0)
	{
	  udiv_qrnd_unnorm (ul, rl, ul, 10);
	  *--s = rl;
	}
      l = buf + BUF_ALLOC - s;
      while (l < len)
        {
          *str++ = 0;
          len--;
        }
      while (l != 0)
	{
	  *str++ = *s++;
	  l--;
	}
      return str;
    }
  else
    {
      size_t l;
      unsigned char buf[BUF_ALLOC];
      unsigned chars_per_limb = __mp_bases[base].chars_per_limb;
      mp_limb_t big_base = __mp_bases[base].big_base;
#if USE_PREINV_DIVREM_1
      unsigned normalization_steps;
      mp_limb_t big_base_inverted = __mp_bases[base].big_base_inverted;
      count_leading_zeros (normalization_steps, big_base);
#endif

      s = buf + BUF_ALLOC;
      while (un > 1)
	{
	  int i;
	  ul = MPN_DIVREM_OR_PREINV_DIVREM_1
	    (up, (mp_size_t) 0, up, un, big_base,
	     big_base_inverted, normalization_steps);
	  un -= up[un - 1] == 0;

	  /* Convert ul from big_base to a string of digits in base using
	     single precision operations.  */
	  i = chars_per_limb;
	  do
	    {
	      udiv_qrnd_unnorm (ul, rl, ul, base);
	      *--s = rl;
	    }
	  while (--i != 0);
	}

      ul = up[0];
      while (ul != 0)
	{
	  udiv_qrnd_unnorm (ul, rl, ul, base);
	  *--s = rl;
	}
      l = buf + BUF_ALLOC - s;
      while (l < len)
        {
          *str++ = 0;
          len--;
        }
      while (l != 0)
	{
	  *str++ = *s++;
	  l--;
	}
      return str;
    }
#else
  ASSERT_ALWAYS (un == 1);

  base = powtab->base;
  ul = up[0];
  if (len == 0)
    {
      /* We're about to output the leftmost little block of the entire number.
	 Executed once per converted number.  Optimize something else!  */
      unsigned char buf[BITS_PER_MP_LIMB];
      s = buf + BITS_PER_MP_LIMB;
      while (ul >= base)
	{
	  udiv_qrnd_unnorm (ul, rl, ul, base);
	  *--s = rl;
	}
      *--s = ul;
      while (s != buf + BITS_PER_MP_LIMB)
	*str++ = *s++;
      return str;
    }
  else
    {
      int i;
      s = str + len;
      if (base == 10)
	{
	  for (i = len; i > 0; i--)
	    {
	      udiv_qrnd_unnorm (ul, rl, ul, 10);
	      *--s = rl;
	    }
	}
      else
	{
	  for (i = len; i > 0; i--)
	    {
	      udiv_qrnd_unnorm (ul, rl, ul, base);
	      *--s = rl;
	    }
	}
      return str + len;
    }
#endif
}


/* Convert {UP,UN} to a string with a base as represented in POWTAB, and put
   the string in STR.  Generate LEN characters, possibly padding with zeros to
   the left.  If LEN is zero, generate as many characters as required.
   Return a pointer immediately after the last digit of the result string.
   This uses divide-and-conquer and is intended for large conversions.  */
static unsigned char *
mpn_dc_get_str (unsigned char *str, size_t len,
		mp_ptr up, mp_size_t un,
		powers_t *powtab)
{
  if (un < GET_STR_BASECASE_THRESHOLD)
    {
      if (un != 0)
	str = mpn_sb_get_str (str, len, up, un, powtab);
      else
	{
	  while (len != 0)
	    {
	      *str++ = 0;
	      len--;
	    }
	}
    }
  else
    {
      mp_ptr pwp, qp, rp;
      mp_size_t pwn, qn;

      pwp = powtab->p;
      pwn = powtab->n;

      if (un < pwn || (un == pwn && mpn_cmp (up, pwp, un) < 0))
	{
	  str = mpn_dc_get_str (str, len, up, un, powtab - 1);
	}
      else
	{
	  TMP_DECL (marker);
	  TMP_MARK (marker);
	  qp = TMP_ALLOC_LIMBS (un - pwn + 1);
	  rp = TMP_ALLOC_LIMBS (pwn);

	  mpn_tdiv_qr (qp, rp, 0L, up, un, pwp, pwn);
	  qn = un - pwn; qn += qp[qn] != 0;		/* quotient size */
	  if (len != 0)
	    len = len - powtab->digits_in_base;
	  str = mpn_dc_get_str (str, len, qp, qn, powtab - 1);
	  str = mpn_dc_get_str (str, powtab->digits_in_base, rp, pwn, powtab - 1);
	  TMP_FREE (mark);
	}
    }
  return str;
}


/* There are no leading zeros on the digits generated at str, but that's not
   currently a documented feature.  */

size_t
mpn_get_str (unsigned char *str, int base, mp_ptr up, mp_size_t un)
{
  mp_ptr powtab_mem, powtab_mem_ptr;
  mp_limb_t big_base;
  size_t digits_in_base;
  powers_t powtab[30];
  int pi;
  mp_size_t n;
  mp_ptr p, t;
  size_t out_len;

  /* Special case zero, as the code below doesn't handle it.  */
  if (un == 0)
    {
      str[0] = 0;
      return 1;
    }

  if (POW2_P (base))
    {
      /* The base is a power of 2.  Make conversion from most
	 significant side.  */
      mp_limb_t n1, n0;
      int bits_per_digit = __mp_bases[base].big_base;
      int cnt;
      int bit_pos;
      mp_size_t i;
      unsigned char *s = str;

      n1 = up[un - 1];
      count_leading_zeros (cnt, n1);

      /* BIT_POS should be R when input ends in least sign. nibble,
         R + bits_per_digit * n when input ends in n:th least significant
         nibble. */

      {
	unsigned long bits;

	bits = BITS_PER_MP_LIMB * un - cnt;
	cnt = bits % bits_per_digit;
	if (cnt != 0)
	  bits += bits_per_digit - cnt;
	bit_pos = bits - (un - 1) * BITS_PER_MP_LIMB;
      }

      /* Fast loop for bit output.  */
      i = un - 1;
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
	  n1 = up[i];
	  bit_pos += BITS_PER_MP_LIMB;
	  *s++ = n0 | (n1 >> bit_pos);
	}

      *s = 0;

      return s - str;
    }

  /* General case.  The base is not a power of 2.  */

  if (un < GET_STR_PRECOMPUTE_THRESHOLD)
    {
      struct powers ptab[1];
      ptab[0].base = base;
      return mpn_sb_get_str (str, (size_t) 0, up, un, ptab) - str;
    }

  /* Allocate one large block for the powers of big_base.  With the current
     scheme, we need to allocate twice as much as would be possible if a
     minimal set of powers were generated.  */
#define ALLOC_SIZE (2 * un + 30)
  powtab_mem = __GMP_ALLOCATE_FUNC_LIMBS (ALLOC_SIZE);
  powtab_mem_ptr = powtab_mem;

  /* Compute a table of powers: big_base^1, big_base^2, big_base^4, ...,
     big_base^(2^k), for k such that the biggest power is between U and
     sqrt(U).  */

  big_base = __mp_bases[base].big_base;
  digits_in_base = __mp_bases[base].chars_per_limb;

  powtab[0].base = base; /* FIXME: hack for getting base to mpn_sb_get_str */
  powtab[1].p = &big_base;
  powtab[1].n = 1;
  powtab[1].digits_in_base = digits_in_base;
  powtab[1].base = base;
  powtab[2].p = &big_base;
  powtab[2].n = 1;
  powtab[2].digits_in_base = digits_in_base;
  powtab[2].base = base;
  n = 1;
  pi = 2;
  p = &big_base;
  for (;;)
    {
      ++pi;
      t = powtab_mem_ptr;
      powtab_mem_ptr += 2 * n;
      mpn_sqr_n (t, p, n);
      n *= 2; n -= t[n - 1] == 0;
      digits_in_base *= 2;
      p = t;
      powtab[pi].p = p;
      powtab[pi].n = n;
      powtab[pi].digits_in_base = digits_in_base;
      powtab[pi].base = base;

      if (2 * n > un)
	break;
    }
  ASSERT_ALWAYS (ALLOC_SIZE > powtab_mem_ptr - powtab_mem);

  /* Using our precomputed powers, now in powtab[], convert our number.  */
  out_len = mpn_dc_get_str (str, 0, up, un, powtab + pi) - str;

  __GMP_FREE_FUNC_LIMBS (powtab_mem, ALLOC_SIZE);

  return out_len;
}
