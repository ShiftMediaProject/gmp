/* Reference mpn functions, designed to be simple, portable and independent
   of the normal gmp code.  Speed isn't a consideration.

   Most routines have assertions representing what the mpn routines are
   supposed to accept.  Many of these reference routines do sensible things
   outside these ranges (eg. for size==0), but the assertions are present to
   pick up bad parameters passed here that are about to be passed the same
   to a real mpn routine being compared.  */

/* always do assertion checking */
#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
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
  p = malloc (size * BYTES_PER_MP_LIMB);
  assert (p != NULL);
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
  assert (size >= 1);
  for (i = 0; i < size; i++)
    ptr[i] = value;
}


void
refmpn_copyi (mp_ptr rp, mp_srcptr sp, mp_size_t size)
{
  mp_size_t i;

  assert (refmpn_overlap_low_to_high_p (rp, sp, size));
  assert (size >= 1);

  for (i = 0; i < size; i++)
    rp[i] = sp[i];
}

void
refmpn_copyd (mp_ptr rp, mp_srcptr sp, mp_size_t size)
{
  mp_size_t i;

  assert (refmpn_overlap_high_to_low_p (rp, sp, size));
  assert (size >= 1);

  for (i = size-1; i >= 0; i--)
    rp[i] = sp[i];
}

void
refmpn_com_n (mp_ptr rp, mp_srcptr sp, mp_size_t size)
{
  mp_size_t i;

  assert (refmpn_overlap_fullonly_p (rp, sp, size));
  assert (size >= 1);

  for (i = 0; i < size; i++)
    rp[i] = ~sp[i];
}


mp_limb_t
refmpn_cmp (mp_srcptr s1p, mp_srcptr s2p, mp_size_t size)
{
#define COMPARE(x,y)            \
  do {                          \
    if ((x) > (y))  return 1;   \
    if ((x) < (y))  return -1;  \
  } while (0)

  mp_size_t  i;

  assert (size >= 1);

  for (i = size-1; i >= 0; i--)
    COMPARE (s1p[i], s2p[i]);
  return 0;
}


#define LOGOPS(function, operation)                                     \
  void                                                                  \
  function (mp_ptr rp, mp_srcptr s1p, mp_srcptr s2p, mp_size_t size)    \
  {                                                                     \
    mp_size_t  i;                                                       \
                                                                        \
    assert (refmpn_overlap_fullonly_two_p (rp, s1p, s2p, size));        \
    assert (size >= 1);                                                 \
                                                                        \
    for (i = 0; i < size; i++)                                          \
      rp[i] = operation;                                                \
  }
LOGOPS(refmpn_and_n,    s1p[i] &  s2p[i])
LOGOPS(refmpn_andn_n,   s1p[i] & ~s2p[i])
LOGOPS(refmpn_nand_n, ~(s1p[i] &  s2p[i]))
LOGOPS(refmpn_ior_n,    s1p[i] |  s2p[i])
LOGOPS(refmpn_iorn_n,   s1p[i] | ~s2p[i])
LOGOPS(refmpn_nior_n, ~(s1p[i] |  s2p[i]))
LOGOPS(refmpn_xor_n,    s1p[i] ^  s2p[i])
LOGOPS(refmpn_xnor_n, ~(s1p[i] ^  s2p[i]))


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
  assert (c == 0 || c == 1);
  r = add (w, x, y);
  return r + add (w, *w, c);
}

/* set *w to x-y-c (where c is 0 or 1), return 0 or 1 borrow */
mp_limb_t
sbb (mp_limb_t *w, mp_limb_t x, mp_limb_t y, mp_limb_t c)
{
  mp_limb_t  r;
  assert (c == 0 || c == 1);
  r = sub (w, x, y);
  return r + sub (w, *w, c);
}


#define AORS_1(function,operation)                                      \
  mp_limb_t                                                             \
  function (mp_ptr rp, mp_srcptr sp, mp_size_t size, mp_limb_t n)       \
  {                                                                     \
    mp_limb_t  i;                                                       \
                                                                        \
    assert (refmpn_overlap_fullonly_p (rp, sp, size));                  \
    assert (size >= 1);                                                 \
                                                                        \
    for (i = 0; i < size; i++)                                          \
      n = operation (&rp[i], sp[i], n);                                 \
    return n;                                                           \
  }
AORS_1 (refmpn_add_1, add)
AORS_1 (refmpn_sub_1, sub)


#define AORS_NC(function, operation)                                    \
  mp_limb_t                                                             \
  function (mp_ptr rp, mp_srcptr s1p, mp_srcptr s2p, mp_size_t size,    \
            mp_limb_t carry)                                            \
  {                                                                     \
    mp_size_t  i;                                                       \
                                                                        \
    assert (refmpn_overlap_fullonly_two_p (rp, s1p, s2p, size));        \
    assert (carry == 0 || carry == 1);                                  \
    assert (size >= 1);                                                 \
                                                                        \
    for (i = 0; i < size; i++)                                          \
      carry = operation (&rp[i], s1p[i], s2p[i], carry);                \
    return carry;                                                       \
  }
AORS_NC (refmpn_add_nc, adc)
AORS_NC (refmpn_sub_nc, sbb)


#define AORS_N(function, function_nc)                                   \
  mp_limb_t                                                             \
  function (mp_ptr rp, mp_srcptr s1p, mp_srcptr s2p, mp_size_t size)    \
  {                                                                     \
    return function_nc (rp, s1p, s2p, size, 0);                         \
  }
AORS_N (refmpn_add_n, refmpn_add_nc)
AORS_N (refmpn_sub_n, refmpn_sub_nc)


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
  assert_nocarry (add (hi, *hi, add (lo, *lo, SHIFTHIGH(LOWPART(s)))));
  assert_nocarry (add (hi, *hi, HIGHPART(s)));

  s = HIGHPART(x) * LOWPART(y);
  assert_nocarry (add (hi, *hi, add (lo, *lo, SHIFTHIGH(LOWPART(s)))));
  assert_nocarry (add (hi, *hi, HIGHPART(s)));
}

mp_limb_t
refmpn_mul_1c (mp_ptr rp, mp_srcptr sp, mp_size_t size, mp_limb_t multiplier,
               mp_limb_t carry)
{
  mp_size_t  i;
  mp_limb_t  hi, lo;
  
  assert (refmpn_overlap_fullonly_p (rp, sp, size));
  assert (size >= 1);
                                                                          
  for (i = 0; i < size; i++)
    {
      mul (&hi, &lo, sp[i], multiplier);
      assert_nocarry (add (&hi, hi, add (&lo, lo, carry)));
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


#define AORSMUL_1C(function, operation_n)                                  \
  mp_limb_t                                                                \
  function (mp_ptr rp, mp_srcptr sp, mp_size_t size, mp_limb_t multiplier, \
            mp_limb_t carry)                                               \
  {                                                                        \
    mp_ptr     p = refmpn_malloc_limbs (size);                             \
    mp_limb_t  ret;                                                        \
                                                                           \
    assert (refmpn_overlap_fullonly_p (rp, sp, size));                    \
    assert (size >= 1);                                                    \
                                                                           \
    ret = refmpn_mul_1c (p, sp, size, multiplier, carry);                  \
    ret += operation_n (rp, rp, p, size);                                  \
                                                                           \
    free (p);                                                              \
    return ret;                                                            \
  }
AORSMUL_1C(refmpn_addmul_1c, refmpn_add_n)
AORSMUL_1C(refmpn_submul_1c, refmpn_sub_n)

#define AORSMUL_1(function, function_1c)                                   \
  mp_limb_t                                                                \
  function (mp_ptr rp, mp_srcptr sp, mp_size_t size, mp_limb_t multiplier) \
  {                                                                        \
    return function_1c (rp, sp, size, multiplier, 0);                      \
  }
AORSMUL_1(refmpn_addmul_1, refmpn_addmul_1c)
AORSMUL_1(refmpn_submul_1, refmpn_submul_1c)


mp_limb_t
refmpn_addsub_n (mp_ptr r1p, mp_ptr r2p,
		 mp_srcptr s1p, mp_srcptr s2p, mp_size_t size)
{
  mp_ptr p;
  mp_limb_t acy, scy;

  /* Destinations can't overlap at all. */
  assert (! MPN_OVERLAP_P (r1p, size, r2p, size));
  assert (refmpn_overlap_fullonly_two_p (r1p, s1p, s2p, size));
  assert (refmpn_overlap_fullonly_two_p (r2p, s1p, s2p, size));
  assert (size >= 1);

  p = refmpn_malloc_limbs (size);
  acy = mpn_add_n (p, s1p, s2p, size);
  scy = mpn_sub_n (r2p, s1p, s2p, size);
  refmpn_copyi (r1p, p, size);
  free (p);
  return 2 * acy + scy;
}


/* Right shift hi,lo and return the low limb of the result.
   Note a shift by BITS_PER_MP_LIMB isn't assumed to work (doesn't on x86). */
mp_limb_t
rshift_make (mp_limb_t hi, mp_limb_t lo, unsigned shift)
{
  assert (shift >= 0 && shift < BITS_PER_MP_LIMB);
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
  assert (shift >= 0 && shift < BITS_PER_MP_LIMB);
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

  assert (refmpn_overlap_low_to_high_p (rp, sp, size));
  assert (size >= 1);
  assert (shift >= 1 && shift < BITS_PER_MP_LIMB);

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

  assert (refmpn_overlap_high_to_low_p (rp, sp, size));
  assert (size >= 1);
  assert (shift >= 1 && shift < BITS_PER_MP_LIMB);

  ret = lshift_make (0, sp[size-1], shift);

  for (i = size-2; i >= 0; i--)
    rp[i+1] = lshift_make (sp[i+1], sp[i], shift);

  rp[i+1] = lshift_make (sp[i+1], 0, shift);
  return ret;
}


#define MP_LIMB_T_HIGHBIT   ((mp_limb_t) 1 << (BITS_PER_MP_LIMB-1))

/* Divide h,l by d, producing a quotient *q and remainder *r.
   Must have h < d.
   __udiv_qrnnd_c isn't simple, and it's a bit slow, but it works. */
void
div (mp_limb_t *q, mp_limb_t *r, mp_limb_t h, mp_limb_t l, mp_limb_t d)
{
  int  n;

  assert (d != 0);
  assert (h < d);

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
  mp_ptr     sp_orig = refmpn_memdup_limbs (sp, size);
  mp_ptr     prod = refmpn_malloc_limbs (size);
  mp_limb_t  carry_orig = carry;

  mp_size_t  i;

  assert (refmpn_overlap_fullonly_p (rp, sp, size));
  assert (size >= 1);
  assert (carry < divisor);

  for (i = size-1; i >= 0; i--)
    div (&rp[i], &carry, carry, sp[i], divisor);

  /* check by multiplying back */
#if 0
  printf ("size=%ld divisor=0x%lX carry=0x%lX remainder=0x%lX\n",
          size, divisor, carry_orig, carry);
  mpn_trace("s",sp_copy,size);
  mpn_trace("r",rp,size);
  printf ("mul_1c %lX\n", refmpn_mul_1c (prod, rp, size, divisor, carry));
  mpn_trace("p",prod,size);
#endif
  assert (refmpn_mul_1c (prod, rp, size, divisor, carry) == carry_orig);
  assert (refmpn_cmp (prod, sp_orig, size) == 0);
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
refmpn_divrem_1c (mp_ptr rp, mp_size_t xsize,
                  mp_srcptr sp, mp_size_t size, mp_limb_t divisor,
                  mp_limb_t carry)
{
  mp_ptr  z;

  carry = refmpn_divmod_1c (rp+xsize, sp, size, divisor, carry);
  if (xsize != 0)
    {
      z = refmpn_malloc_limbs (xsize);
      refmpn_fill (z, xsize, 0);
      carry = refmpn_divmod_1c (rp, z, xsize, divisor, carry);
      free (z);
    }
  return carry;
}  

mp_limb_t
refmpn_divrem_1 (mp_ptr rp, mp_size_t xsize,
                 mp_srcptr sp, mp_size_t size, mp_limb_t divisor)
{
  return refmpn_divrem_1c (rp, xsize, sp, size, divisor, 0);
}


/* The same as mpn/generic/mul_basecase.c, but using refmpn functions. */
void
refmpn_mul_basecase (mp_ptr prodp,
                     mp_srcptr up, mp_size_t usize,
                     mp_srcptr vp, mp_size_t vsize)
{
  mp_size_t i;

  assert (! MPN_OVERLAP_P (prodp, usize+vsize, up, usize));
  assert (! MPN_OVERLAP_P (prodp, usize+vsize, vp, vsize));
  assert (usize >= vsize);
  assert (vsize >= 1);

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
