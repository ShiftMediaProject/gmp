/* Reference mpn functions, designed to be simple, portable and independent
   of the normal gmp code.  Speed isn't a consideration.  */

/*
Copyright (C) 1996, 1997, 1998, 1999, 2000 Free Software Foundation, Inc.

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
MA 02111-1307, USA.
*/


/* Most routines have assertions representing what the mpn routines are
   supposed to accept.  Many of these reference routines do sensible things
   outside these ranges (eg. for size==0), but the assertions are present to
   pick up bad parameters passed here that are about to be passed the same
   to a real mpn routine being compared.  */

/* always do assertion checking */
#define WANT_ASSERT  1

#include <stdio.h>  /* for NULL */
#include <stdlib.h> /* for malloc */
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"

#include "ref.h"


/* Check overlap for a routine defined to work low to high. */
int
refmpn_overlap_low_to_high_p (mp_srcptr dst, mp_srcptr src, mp_size_t size)
{
  return (!MPN_OVERLAP_P (dst, size, src, size) || dst <= src);
}

/* Check overlap for a routine defined to work high to low. */
int
refmpn_overlap_high_to_low_p (mp_srcptr dst, mp_srcptr src, mp_size_t size)
{
  return (!MPN_OVERLAP_P (dst, size, src, size) || dst >= src);
}

/* Check overlap for a standard routine requiring equal or separate. */
int
refmpn_overlap_fullonly_p (mp_srcptr dst, mp_srcptr src, mp_size_t size)
{
  return (dst == src || !MPN_OVERLAP_P (dst, size, src, size));
}
int
refmpn_overlap_fullonly_two_p (mp_srcptr dst, mp_srcptr src1, mp_srcptr src2,
                               mp_size_t size)
{
  return (refmpn_overlap_fullonly_p (dst, src1, size)
          && refmpn_overlap_fullonly_p (dst, src2, size));
}


mp_ptr
refmpn_malloc_limbs (mp_size_t size)
{
  mp_ptr  p;
  ASSERT (size >= 0);
  if (size == 0)
    size = 1;
  p = (mp_ptr) malloc (size * BYTES_PER_MP_LIMB);
  ASSERT (p != NULL);
  return p;
}

mp_ptr
refmpn_memdup_limbs (mp_srcptr ptr, mp_size_t size)
{
  mp_ptr  p;
  p = refmpn_malloc_limbs (size);
  refmpn_copyi (p, ptr, size);
  return p;
}

void
refmpn_fill (mp_ptr ptr, mp_size_t size, mp_limb_t value)
{
  mp_size_t  i;
  ASSERT (size >= 0);
  for (i = 0; i < size; i++)
    ptr[i] = value;
}

int
refmpn_zero_p (mp_srcptr ptr, mp_size_t size)
{
  mp_size_t  i;
  for (i = 0; i < size; i++)
    if (ptr[i] != 0)
      return 0;
  return 1;
}

mp_limb_t
refmpn_msbone (mp_limb_t x)
{
  mp_limb_t  n = (mp_limb_t) 1 << (BITS_PER_MP_LIMB-1);

  while (n != 0)
    {
      if (x & n)
        break;
      n >>= 1;
    }
  return n;
}

/* a mask of the MSB one bit and all bits below */
mp_limb_t
refmpn_msbone_mask (mp_limb_t x)
{
  if (x == 0)
    return 0;

  return (refmpn_msbone (x) << 1) - 1;
}

void
refmpn_copyi (mp_ptr rp, mp_srcptr sp, mp_size_t size)
{
  mp_size_t i;

  ASSERT (refmpn_overlap_low_to_high_p (rp, sp, size));
  ASSERT (size >= 0);

  for (i = 0; i < size; i++)
    rp[i] = sp[i];
}

void
refmpn_copyd (mp_ptr rp, mp_srcptr sp, mp_size_t size)
{
  mp_size_t i;

  ASSERT (refmpn_overlap_high_to_low_p (rp, sp, size));
  ASSERT (size >= 0);

  for (i = size-1; i >= 0; i--)
    rp[i] = sp[i];
}

void
refmpn_com_n (mp_ptr rp, mp_srcptr sp, mp_size_t size)
{
  mp_size_t i;

  ASSERT (refmpn_overlap_fullonly_p (rp, sp, size));
  ASSERT (size >= 1);

  for (i = 0; i < size; i++)
    rp[i] = ~sp[i];
}


int
refmpn_cmp (mp_srcptr xp, mp_srcptr yp, mp_size_t size)
{
  mp_size_t  i;

  ASSERT (size >= 1);

  for (i = size-1; i >= 0; i--)
    {
      if (xp[i] > yp[i])  return 1;
      if (xp[i] < yp[i])  return -1;
    }
  return 0;
}

int
refmpn_cmp_twosizes (mp_srcptr xp, mp_size_t xsize,
                     mp_srcptr yp, mp_size_t ysize)
{
  int  opp, cmp;

  opp = (xsize < ysize);
  if (opp)
    MPN_SRCPTR_SWAP (xp,xsize, yp,ysize);
  
  if (! refmpn_zero_p (xp+ysize, xsize-ysize))
    cmp = 1;
  else
    cmp = refmpn_cmp (xp, yp, ysize);

  return (opp ? -cmp : cmp);
}


#define LOGOPS(operation)                                               \
  {                                                                     \
    mp_size_t  i;                                                       \
                                                                        \
    ASSERT (refmpn_overlap_fullonly_two_p (rp, s1p, s2p, size));        \
    ASSERT (size >= 1);                                                 \
                                                                        \
    for (i = 0; i < size; i++)                                          \
      rp[i] = operation;                                                \
  }

void
refmpn_and_n (mp_ptr rp, mp_srcptr s1p, mp_srcptr s2p, mp_size_t size)
{
  LOGOPS (s1p[i] &  s2p[i]);
}
void
refmpn_andn_n (mp_ptr rp, mp_srcptr s1p, mp_srcptr s2p, mp_size_t size)
{
  LOGOPS (s1p[i] & ~s2p[i]);
}
void
refmpn_nand_n (mp_ptr rp, mp_srcptr s1p, mp_srcptr s2p, mp_size_t size)
{
  LOGOPS (~(s1p[i] &  s2p[i]));
}
void
refmpn_ior_n (mp_ptr rp, mp_srcptr s1p, mp_srcptr s2p, mp_size_t size)
{
  LOGOPS (s1p[i] |  s2p[i]);
}
void
refmpn_iorn_n (mp_ptr rp, mp_srcptr s1p, mp_srcptr s2p, mp_size_t size)
{
  LOGOPS (s1p[i] | ~s2p[i]);
}
void
refmpn_nior_n (mp_ptr rp, mp_srcptr s1p, mp_srcptr s2p, mp_size_t size)
{
  LOGOPS (~(s1p[i] |  s2p[i]));
}
void
refmpn_xor_n (mp_ptr rp, mp_srcptr s1p, mp_srcptr s2p, mp_size_t size)
{
  LOGOPS (s1p[i] ^  s2p[i]);
}
void
refmpn_xnor_n (mp_ptr rp, mp_srcptr s1p, mp_srcptr s2p, mp_size_t size)
{
  LOGOPS (~(s1p[i] ^  s2p[i]));
}

/* set *w to x+y, return 0 or 1 carry */
mp_limb_t
add (mp_limb_t *w, mp_limb_t x, mp_limb_t y)
{
  *w = x + y;
  return *w < x;
}

/* set *w to x-y, return 0 or 1 borrow */
mp_limb_t
sub (mp_limb_t *w, mp_limb_t x, mp_limb_t y)
{
  *w = x - y;
  return *w > x;
}

/* set *w to x+y+c (where c is 0 or 1), return 0 or 1 carry */
mp_limb_t
adc (mp_limb_t *w, mp_limb_t x, mp_limb_t y, mp_limb_t c)
{
  mp_limb_t  r;
  ASSERT (c == 0 || c == 1);
  r = add (w, x, y);
  return r + add (w, *w, c);
}

/* set *w to x-y-c (where c is 0 or 1), return 0 or 1 borrow */
mp_limb_t
sbb (mp_limb_t *w, mp_limb_t x, mp_limb_t y, mp_limb_t c)
{
  mp_limb_t  r;
  ASSERT (c == 0 || c == 1);
  r = sub (w, x, y);
  return r + sub (w, *w, c);
}


#define AORS_1(operation)                               \
  {                                                     \
    mp_limb_t  i;                                       \
                                                        \
    ASSERT (refmpn_overlap_fullonly_p (rp, sp, size));  \
    ASSERT (size >= 1);                                 \
                                                        \
    for (i = 0; i < size; i++)                          \
      n = operation (&rp[i], sp[i], n);                 \
    return n;                                           \
  }

mp_limb_t
refmpn_add_1 (mp_ptr rp, mp_srcptr sp, mp_size_t size, mp_limb_t n)
{
  AORS_1 (add);
}
mp_limb_t
refmpn_sub_1 (mp_ptr rp, mp_srcptr sp, mp_size_t size, mp_limb_t n)
{
  AORS_1 (sub);
}

#define AORS_NC(operation)                                              \
  {                                                                     \
    mp_size_t  i;                                                       \
                                                                        \
    ASSERT (refmpn_overlap_fullonly_two_p (rp, s1p, s2p, size));        \
    ASSERT (carry == 0 || carry == 1);                                  \
    ASSERT (size >= 1);                                                 \
                                                                        \
    for (i = 0; i < size; i++)                                          \
      carry = operation (&rp[i], s1p[i], s2p[i], carry);                \
    return carry;                                                       \
  }

mp_limb_t
refmpn_add_nc (mp_ptr rp, mp_srcptr s1p, mp_srcptr s2p, mp_size_t size,
               mp_limb_t carry)
{
  AORS_NC (adc);
}
mp_limb_t
refmpn_sub_nc (mp_ptr rp, mp_srcptr s1p, mp_srcptr s2p, mp_size_t size,
               mp_limb_t carry)
{
  AORS_NC (sbb);
}


mp_limb_t
refmpn_add_n (mp_ptr rp, mp_srcptr s1p, mp_srcptr s2p, mp_size_t size)
{
  return refmpn_add_nc (rp, s1p, s2p, size, 0);
}
mp_limb_t
refmpn_sub_n (mp_ptr rp, mp_srcptr s1p, mp_srcptr s2p, mp_size_t size)
{
  return refmpn_sub_nc (rp, s1p, s2p, size, 0);
}


#define AORS(aors_n, aors_1)                                    \
  {                                                             \
    mp_limb_t  c;                                               \
    ASSERT (s1size >= s2size);                                  \
    ASSERT (s2size >= 1);                                       \
    c = aors_n (rp, s1p, s2p, s2size);                          \
    if (s1size-s2size != 0)                                     \
      c = aors_1 (rp+s2size, s1p+s2size, s1size-s2size, c);     \
    return c;                                                   \
  }
mp_limb_t
refmpn_add (mp_ptr rp,
            mp_srcptr s1p, mp_size_t s1size,
            mp_srcptr s2p, mp_size_t s2size)
{
  AORS (refmpn_add_n, refmpn_add_1);
}
mp_limb_t
refmpn_sub (mp_ptr rp,
            mp_srcptr s1p, mp_size_t s1size,
            mp_srcptr s2p, mp_size_t s2size)
{
  AORS (refmpn_sub_n, refmpn_sub_1);
}


#define SHIFTHIGH(x) ((x) << BITS_PER_MP_LIMB/2)
#define SHIFTLOW(x)  ((x) >> BITS_PER_MP_LIMB/2)

#define LOWMASK   (((mp_limb_t) 1 << BITS_PER_MP_LIMB/2)-1)
#define HIGHMASK  SHIFTHIGH(LOWMASK)

#define LOWPART(x)   ((x) & LOWMASK)
#define HIGHPART(x)  SHIFTLOW((x) & HIGHMASK)

/* set *hi,*lo to x*y */
void
mul (mp_limb_t *hi, mp_limb_t *lo, mp_limb_t x, mp_limb_t y)
{
  mp_limb_t s;
  
  *lo = LOWPART(x) * LOWPART(y);
  *hi = HIGHPART(x) * HIGHPART(y);

  s = LOWPART(x) * HIGHPART(y);
  ASSERT_NOCARRY (add (hi, *hi, add (lo, *lo, SHIFTHIGH(LOWPART(s)))));
  ASSERT_NOCARRY (add (hi, *hi, HIGHPART(s)));

  s = HIGHPART(x) * LOWPART(y);
  ASSERT_NOCARRY (add (hi, *hi, add (lo, *lo, SHIFTHIGH(LOWPART(s)))));
  ASSERT_NOCARRY (add (hi, *hi, HIGHPART(s)));
}

mp_limb_t
refmpn_mul_1c (mp_ptr rp, mp_srcptr sp, mp_size_t size, mp_limb_t multiplier,
               mp_limb_t carry)
{
  mp_size_t  i;
  mp_limb_t  hi, lo;
  
  ASSERT (refmpn_overlap_fullonly_p (rp, sp, size));
  ASSERT (size >= 1);
                                                                          
  for (i = 0; i < size; i++)
    {
      mul (&hi, &lo, sp[i], multiplier);
      ASSERT_NOCARRY (add (&hi, hi, add (&lo, lo, carry)));
      rp[i] = lo;
      carry = hi;
    }
  
  return carry;
}

mp_limb_t
refmpn_mul_1 (mp_ptr rp, mp_srcptr sp, mp_size_t size, mp_limb_t multiplier)
{
  return refmpn_mul_1c (rp, sp, size, multiplier, 0);
}


#define AORSMUL_1C(operation_n)                                 \
  {                                                             \
    mp_ptr     p = refmpn_malloc_limbs (size);                  \
    mp_limb_t  ret;                                             \
                                                                \
    ASSERT (refmpn_overlap_fullonly_p (rp, sp, size));          \
    ASSERT (size >= 1);                                         \
                                                                \
    ret = refmpn_mul_1c (p, sp, size, multiplier, carry);       \
    ret += operation_n (rp, rp, p, size);                       \
                                                                \
    free (p);                                                   \
    return ret;                                                 \
  }

mp_limb_t
refmpn_addmul_1c (mp_ptr rp, mp_srcptr sp, mp_size_t size,
                  mp_limb_t multiplier, mp_limb_t carry)
{
  AORSMUL_1C (refmpn_add_n);
}
mp_limb_t
refmpn_submul_1c (mp_ptr rp, mp_srcptr sp, mp_size_t size,
                  mp_limb_t multiplier, mp_limb_t carry)
{
  AORSMUL_1C (refmpn_sub_n);
}


mp_limb_t
refmpn_addmul_1 (mp_ptr rp, mp_srcptr sp, mp_size_t size, mp_limb_t multiplier)
{
  return refmpn_addmul_1c (rp, sp, size, multiplier, 0);
}
mp_limb_t
refmpn_submul_1 (mp_ptr rp, mp_srcptr sp, mp_size_t size, mp_limb_t multiplier)
{
  return refmpn_submul_1c (rp, sp, size, multiplier, 0);
}


mp_limb_t
refmpn_addsub_nc (mp_ptr r1p, mp_ptr r2p,
                  mp_srcptr s1p, mp_srcptr s2p, mp_size_t size,
                  mp_limb_t carry)
{
  mp_ptr p;
  mp_limb_t acy, scy;

  /* Destinations can't overlap. */
  ASSERT (! MPN_OVERLAP_P (r1p, size, r2p, size));
  ASSERT (refmpn_overlap_fullonly_two_p (r1p, s1p, s2p, size));
  ASSERT (refmpn_overlap_fullonly_two_p (r2p, s1p, s2p, size));
  ASSERT (size >= 1);

  p = refmpn_malloc_limbs (size);
  acy = refmpn_add_nc (p, s1p, s2p, size, carry >> 1);
  scy = refmpn_sub_nc (r2p, s1p, s2p, size, carry & 1);
  refmpn_copyi (r1p, p, size);
  free (p);
  return 2 * acy + scy;
}

mp_limb_t
refmpn_addsub_n (mp_ptr r1p, mp_ptr r2p,
		 mp_srcptr s1p, mp_srcptr s2p, mp_size_t size)
{
  return refmpn_addsub_nc (r1p, r2p, s1p, s2p, size, 0);
}


/* Right shift hi,lo and return the low limb of the result.
   Note a shift by BITS_PER_MP_LIMB isn't assumed to work (doesn't on x86). */
mp_limb_t
rshift_make (mp_limb_t hi, mp_limb_t lo, unsigned shift)
{
  ASSERT (shift >= 0 && shift < BITS_PER_MP_LIMB);
  if (shift == 0)
    return lo;
  else
    return (hi << (BITS_PER_MP_LIMB-shift)) | (lo >> shift);
}

/* Left shift hi,lo and return the high limb of the result.
   Note a shift by BITS_PER_MP_LIMB isn't assumed to work (doesn't on x86). */
mp_limb_t
lshift_make (mp_limb_t hi, mp_limb_t lo, unsigned shift)
{
  ASSERT (shift >= 0 && shift < BITS_PER_MP_LIMB);
  if (shift == 0)
    return hi;
  else
    return (hi << shift) | (lo >> (BITS_PER_MP_LIMB-shift));
}


mp_limb_t
refmpn_rshift (mp_ptr rp, mp_srcptr sp, mp_size_t size, unsigned shift)
{
  mp_limb_t  ret;
  mp_size_t  i;

  ASSERT (refmpn_overlap_low_to_high_p (rp, sp, size));
  ASSERT (size >= 1);
  ASSERT (shift >= 1 && shift < BITS_PER_MP_LIMB);

  ret = rshift_make (sp[0], 0, shift);

  for (i = 0; i < size-1; i++)
    rp[i] = rshift_make (sp[i+1], sp[i], shift);

  rp[i] = rshift_make (0, sp[i], shift);
  return ret;
}

mp_limb_t
refmpn_lshift (mp_ptr rp, mp_srcptr sp, mp_size_t size, unsigned shift)
{
  mp_limb_t  ret;
  mp_size_t  i;

  ASSERT (refmpn_overlap_high_to_low_p (rp, sp, size));
  ASSERT (size >= 1);
  ASSERT (shift >= 1 && shift < BITS_PER_MP_LIMB);

  ret = lshift_make (0, sp[size-1], shift);

  for (i = size-2; i >= 0; i--)
    rp[i+1] = lshift_make (sp[i+1], sp[i], shift);

  rp[i+1] = lshift_make (sp[i+1], 0, shift);
  return ret;
}


/* Divide h,l by d, producing a quotient *q and remainder *r.
   Must have h < d.
   __udiv_qrnnd_c isn't simple, and it's a bit slow, but it works. */
void
div1 (mp_limb_t *q, mp_limb_t *r, mp_limb_t h, mp_limb_t l, mp_limb_t d)
{
  int  n;

  ASSERT (d != 0);
  ASSERT (h < d);

#if 0
  udiv_qrnnd (*q, *r, h, l, d);
  return;
#endif

  for (n = 0; !(d & MP_LIMB_T_HIGHBIT); n++)
    d <<= 1;

  h = lshift_make (h, l, n);
  l <<= n;

  __udiv_qrnnd_c (*q, *r, h, l, d);
  *r >>= n;
}

mp_limb_t
refmpn_divmod_1c (mp_ptr rp, mp_srcptr sp, mp_size_t size, mp_limb_t divisor,
                 mp_limb_t carry)
{
  mp_ptr     sp_orig;
  mp_ptr     prod;
  mp_limb_t  carry_orig;
  mp_size_t  i;

  ASSERT (refmpn_overlap_fullonly_p (rp, sp, size));
  ASSERT (size >= 0);
  ASSERT (carry < divisor);

  if (size == 0)
    return carry;

  sp_orig = refmpn_memdup_limbs (sp, size);
  prod = refmpn_malloc_limbs (size);
  carry_orig = carry;

  for (i = size-1; i >= 0; i--)
    div1 (&rp[i], &carry, carry, sp[i], divisor);

  /* check by multiplying back */
#if 0
  printf ("size=%ld divisor=0x%lX carry=0x%lX remainder=0x%lX\n",
          size, divisor, carry_orig, carry);
  mpn_trace("s",sp_copy,size);
  mpn_trace("r",rp,size);
  printf ("mul_1c %lX\n", refmpn_mul_1c (prod, rp, size, divisor, carry));
  mpn_trace("p",prod,size);
#endif
  ASSERT (refmpn_mul_1c (prod, rp, size, divisor, carry) == carry_orig);
  ASSERT (refmpn_cmp (prod, sp_orig, size) == 0);
  free (sp_orig);
  free (prod);

  return carry;
}

mp_limb_t
refmpn_divmod_1 (mp_ptr rp, mp_srcptr sp, mp_size_t size, mp_limb_t divisor)
{
  return refmpn_divmod_1c (rp, sp, size, divisor, 0);
}


mp_limb_t
refmpn_mod_1c (mp_srcptr sp, mp_size_t size, mp_limb_t divisor,
               mp_limb_t carry)
{
  mp_ptr  p = refmpn_malloc_limbs (size);
  carry = refmpn_divmod_1c (p, sp, size, divisor, carry);
  free (p);
  return carry;
}  

mp_limb_t
refmpn_mod_1 (mp_srcptr sp, mp_size_t size, mp_limb_t divisor)
{
  return refmpn_mod_1c (sp, size, divisor, 0);
}

mp_limb_t
refmpn_mod_1_rshift (mp_srcptr sp, mp_size_t size,
                     unsigned shift, mp_limb_t divisor)
{
  mp_limb_t  r;
  mp_ptr     p = refmpn_malloc_limbs (size);
  refmpn_rshift (p, sp, size, shift);
  r = refmpn_mod_1 (p, size, divisor);
  free (p);
  return r;
}


mp_limb_t
refmpn_divrem_1c (mp_ptr rp, mp_size_t xsize,
                  mp_srcptr sp, mp_size_t size, mp_limb_t divisor,
                  mp_limb_t carry)
{
  mp_ptr  z;

  z = refmpn_malloc_limbs (xsize);
  refmpn_fill (z, xsize, 0);
  
  carry = refmpn_divmod_1c (rp+xsize, sp, size, divisor, carry);
  carry = refmpn_divmod_1c (rp, z, xsize, divisor, carry);
  
  free (z);
  return carry;
}  

mp_limb_t
refmpn_divrem_1 (mp_ptr rp, mp_size_t xsize,
                 mp_srcptr sp, mp_size_t size, mp_limb_t divisor)
{
  return refmpn_divrem_1c (rp, xsize, sp, size, divisor, 0);
}


/* As given in the manual, the divexact method gives quotient q and return
   value c satisfying

           c*b^n + a-i == 3*q

   where a=dividend, i=initial carry, b=2^BITS_PER_MP_LIMB, and n=size.

   If a-i is divisible by 3 then c==0 and a plain divmod gives the quotient.
   If (a-i)%3==r then c is a high limb tacked on that will turn r into 0.
   Because 2^BITS_PER_MP_LIMB==1mod3 (so long as BITS_PER_MP_LIMB is even)
   it's enough to set c=3-r, ie. if r=1 then c=2, or if r=2 then c=1.

   If a-i produces a borrow then refmpn_sub_1 leaves a twos complement
   negative, ie. b^n+a-i, and the calculation produces c1 satisfying
  
           c1*b^n + b^n+a-i == 3*q

   From which clearly c=c1+1, so it's enough to just add any borrow to the
   return value otherwise calculated.

   A borrow only occurs when a==0 or a==1, and, by the same reasoning as in
   mpn/generic/diveby3.c, the c1 that results in those cases will only be 0
   or 1 respectively, so with 1 added the final return value is still in the
   prescribed range 0 to 2. */

mp_limb_t
refmpn_divexact_by3c (mp_ptr rp, mp_srcptr sp, mp_size_t size, mp_limb_t carry)
{
  mp_ptr     spcopy;
  mp_limb_t  c, cs;

  ASSERT (refmpn_overlap_fullonly_p (rp, sp, size));
  ASSERT (size >= 1);
  ASSERT (carry <= 2);

  spcopy = refmpn_memdup_limbs (sp, size);
  cs = refmpn_sub_1 (spcopy, spcopy, size, carry);

  c = refmpn_divmod_1 (rp, spcopy, size, 3);
  if (c != 0)
    {
      ASSERT ((BITS_PER_MP_LIMB % 2) == 0);
      c = 3-c;
      ASSERT_NOCARRY (refmpn_divmod_1c (rp, spcopy, size, 3, c));
    }

  c += cs;
  ASSERT (c <= 2);

  free (spcopy);
  return c;
}

mp_limb_t
refmpn_divexact_by3 (mp_ptr rp, mp_srcptr sp, mp_size_t size)
{
  return refmpn_divexact_by3c (rp, sp, size, 0);
}


/* The same as mpn/generic/mul_basecase.c, but using refmpn functions. */
void
refmpn_mul_basecase (mp_ptr prodp,
                     mp_srcptr up, mp_size_t usize,
                     mp_srcptr vp, mp_size_t vsize)
{
  mp_size_t i;

  ASSERT (! MPN_OVERLAP_P (prodp, usize+vsize, up, usize));
  ASSERT (! MPN_OVERLAP_P (prodp, usize+vsize, vp, vsize));
  ASSERT (usize >= vsize);
  ASSERT (vsize >= 1);

  prodp[usize] = refmpn_mul_1 (prodp, up, usize, vp[0]);
  for (i = 1; i < vsize; i++)
    prodp[usize+i] = refmpn_addmul_1 (prodp+i, up, usize, vp[i]);
}

void
refmpn_mul_n (mp_ptr prodp, mp_srcptr up, mp_srcptr vp, mp_size_t size)
{
  refmpn_mul_basecase (prodp, up, size, vp, size);
}

void
refmpn_sqr (mp_ptr dst, mp_srcptr src, mp_size_t size)
{
  refmpn_mul_basecase (dst, src, size, src, size);
}


mp_limb_t
refmpn_gcd_1 (mp_srcptr xp, mp_size_t xsize, mp_limb_t y)
{
  mp_limb_t  x;
  int  twos;

  ASSERT (y != 0);
  ASSERT (! refmpn_zero_p (xp, xsize));

  x = mpn_mod_1 (xp, xsize, y);
  if (x == 0)
    return y;

  twos = 0;
  while ((x & 1) == 0 && (y & 1) == 0)
    {
      x >>= 1;
      y >>= 1;
      twos++;
    }

  for (;;)
    {
      while ((x & 1) == 0)  x >>= 1;
      while ((y & 1) == 0)  y >>= 1;

      if (x < y)
        MP_LIMB_T_SWAP (x, y);

      x -= y;
      if (x == 0)
        break;
    }

  return y << twos;
}


unsigned
refmpn_count_trailing_zeros (mp_limb_t x)
{
  unsigned  n = 0;

  ASSERT (x != 0);
  while ((x & 1) == 0)
    {
      x >>= 1;
      n++;
    }
  return n;
}

mp_size_t
refmpn_strip_twos (mp_ptr p, mp_size_t size)
{
  mp_size_t  limbs;
  unsigned   shift;

  ASSERT (size >= 1);
  ASSERT (! refmpn_zero_p (p, size));

  for (limbs = 0; p[0] == 0; limbs++)
    {
      MPN_COPY_INCR (p, p+1, size-1);
      p[size-1] = 0;
    }

  shift = refmpn_count_trailing_zeros (p[0]);
  if (shift)
    refmpn_rshift (p, p, size, shift);

  return limbs*BITS_PER_MP_LIMB + shift;
}

mp_limb_t
refmpn_gcd (mp_ptr gp, mp_ptr xp, mp_size_t xsize, mp_ptr yp, mp_size_t ysize)
{
  int       cmp;

  ASSERT (ysize >= 1);
  ASSERT (xsize >= ysize);
  ASSERT ((xp[0] & 1) != 0);
  ASSERT ((yp[0] & 1) != 0);
  ASSERT (xp[xsize-1] != 0);
  ASSERT (yp[ysize-1] != 0);
  ASSERT (refmpn_overlap_fullonly_p (gp, xp, xsize));
  ASSERT (refmpn_overlap_fullonly_p (gp, yp, ysize));
  ASSERT (! MPN_OVERLAP_P (xp, xsize, yp, ysize));
  if (xsize == ysize)
    ASSERT (refmpn_msbone (xp[xsize-1]) >= refmpn_msbone (yp[ysize-1]));

  refmpn_strip_twos (xp, xsize);
  MPN_NORMALIZE (xp, xsize);
  MPN_NORMALIZE (yp, ysize);

  for (;;)
    {
      cmp = refmpn_cmp_twosizes (xp, xsize, yp, ysize);
      if (cmp == 0)
        break;
      if (cmp < 0)
        MPN_PTR_SWAP (xp,xsize, yp,ysize);

      ASSERT_NOCARRY (refmpn_sub (xp, xp, xsize, yp, ysize));

      refmpn_strip_twos (xp, xsize);
      MPN_NORMALIZE (xp, xsize);
    }

  refmpn_copyi (gp, xp, xsize);
  return xsize;
}


unsigned long
refmpn_popcount (mp_srcptr sp, mp_size_t size)
{
  unsigned long  count = 0;
  mp_size_t  i;
  int        j;
  mp_limb_t  l;

  ASSERT (size >= 0);
  for (i = 0; i < size; i++)
    {
      l = sp[i];
      for (j = 0; j < BITS_PER_MP_LIMB; j++)
        {
          count += (l & 1);
          l >>= 1;
        }
    }
  return count;
}

unsigned long
refmpn_hamdist (mp_srcptr s1p, mp_srcptr s2p, mp_size_t size)
{
  mp_ptr  d;
  unsigned long  count;

  if (size == 0)
    return 0;

  d = refmpn_malloc_limbs (size);
  refmpn_xor_n (d, s1p, s2p, size);
  count = refmpn_popcount (d, size);
  free (d);
  return count;
}
