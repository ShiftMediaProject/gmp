/* Include file for internal GNU MP types and definitions.

Copyright (C) 1991, 1993, 1994, 1995, 1996, 1997, 1999, 2000 Free Software
Foundation, Inc.

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "gmp-mparam.h"
/* #include "longlong.h" */

/* When using gcc, make sure to use its builtin alloca.  */
#if ! defined (alloca) && defined (__GNUC__)
#define alloca __builtin_alloca
#define HAVE_ALLOCA
#endif

/* When using cc, do whatever necessary to allow use of alloca.  For many
   machines, this means including alloca.h.  IBM's compilers need a #pragma
   in "each module that needs to use alloca".  */
#if ! defined (alloca)
/* We need lots of variants for MIPS, to cover all versions and perversions
   of OSes for MIPS.  */
#if defined (__mips) || defined (MIPSEL) || defined (MIPSEB) \
 || defined (_MIPSEL) || defined (_MIPSEB) || defined (__sgi) \
 || defined (__alpha) || defined (__sparc) || defined (sparc) \
 || defined (__ksr__)
#include <alloca.h>
#define HAVE_ALLOCA
#endif
#if defined (_IBMR2)
#pragma alloca
#define HAVE_ALLOCA
#endif
#if defined (__DECC)
#define alloca(x) __ALLOCA(x)
#define HAVE_ALLOCA
#endif
#endif

#if defined (alloca)
#define HAVE_ALLOCA
#endif

#if ! defined (HAVE_ALLOCA) || USE_STACK_ALLOC
#include "stack-alloc.h"
#else
#define TMP_DECL(m)
#define TMP_ALLOC(x) alloca(x)
#define TMP_MARK(m)
#define TMP_FREE(m)
#endif

#ifndef NULL
#define NULL ((void *) 0)
#endif

#if ! defined (__GNUC__)	/* FIXME: Test for C++ compilers here,
				   __DECC understands __inline */
#define inline			/* Empty */
#endif

#define ABS(x) (x >= 0 ? x : -x)
#define MIN(l,o) ((l) < (o) ? (l) : (o))
#define MAX(h,i) ((h) > (i) ? (h) : (i))

/* Field access macros.  */
#define SIZ(x) ((x)->_mp_size)
#define ABSIZ(x) ABS (SIZ (x))
#define PTR(x) ((x)->_mp_d)
#define LIMBS(x) ((x)->_mp_d)
#define EXP(x) ((x)->_mp_exp)
#define PREC(x) ((x)->_mp_prec)
#define ALLOC(x) ((x)->_mp_alloc)

/* FIXME: These are purely internal, so do a search and replace to change
   them to __gmp forms, rather than using these macros. */
#define _mp_allocate_func      __gmp_allocate_func
#define _mp_reallocate_func    __gmp_reallocate_func
#define _mp_free_func          __gmp_free_func
#define _mp_default_allocate   __gmp_default_allocate
#define _mp_default_reallocate __gmp_default_reallocate
#define _mp_default_free       __gmp_default_free

#if (__STDC__-0) || defined (__cplusplus)
void *malloc (size_t);
void *realloc (void *, size_t);
void free (void *);

extern void *	(*_mp_allocate_func) (size_t);
extern void *	(*_mp_reallocate_func) (void *, size_t, size_t);
extern void	(*_mp_free_func) (void *, size_t);

void *_mp_default_allocate (size_t);
void *_mp_default_reallocate (void *, size_t, size_t);
void _mp_default_free (void *, size_t);

#else

#define const			/* Empty */
#define signed			/* Empty */

void *malloc ();
void *realloc ();
void free ();

extern void *	(*_mp_allocate_func) ();
extern void *	(*_mp_reallocate_func) ();
extern void	(*_mp_free_func) ();

void *_mp_default_allocate ();
void *_mp_default_reallocate ();
void _mp_default_free ();
#endif

#if defined (__GNUC__) && defined (__i386__)
#if 0			/* check that these actually improve things */
#define MPN_COPY_INCR(DST, SRC, N)					\
  __asm__ ("cld\n\trep\n\tmovsl" : :					\
	   "D" (DST), "S" (SRC), "c" (N) :				\
	   "cx", "di", "si", "memory")
#define MPN_COPY_DECR(DST, SRC, N)					\
  __asm__ ("std\n\trep\n\tmovsl" : :					\
	   "D" ((DST) + (N) - 1), "S" ((SRC) + (N) - 1), "c" (N) :	\
	   "cx", "di", "si", "memory")
#define MPN_NORMALIZE_NOT_ZERO(P, N)					\
  do {									\
    __asm__ ("std\n\trepe\n\tscasl" : "=c" (N) :			\
	     "a" (0), "D" ((P) + (N) - 1), "0" (N) :			\
	     "cx", "di");						\
    (N)++;								\
  } while (0)
#endif
#endif

#if HAVE_NATIVE_mpn_copyi
#define mpn_copyi __MPN(copyi)
void mpn_copyi _PROTO ((mp_ptr, mp_srcptr, mp_size_t));
#endif

/* Remap names of internal mpn functions.  */
#define __clz_tab               __MPN(clz_tab)
#define mpn_udiv_w_sdiv		__MPN(udiv_w_sdiv)
#define mpn_kara_sqr_n		__MPN(kara_sqr_n)
#define mpn_toom3_sqr_n		__MPN(toom3_sqr_n)
#define mpn_kara_mul_n		__MPN(kara_mul_n)
#define mpn_toom3_mul_n		__MPN(toom3_mul_n)
#define mpn_reciprocal		__MPN(reciprocal)

#define mpn_sb_divrem_mn	__MPN(sb_divrem_mn)
#define mpn_bz_divrem_n		__MPN(bz_divrem_n)
#define mpn_tdiv_qr		__MPN(tdiv_qr)
/* #define mpn_tdiv_q		__MPN(tdiv_q) */

mp_limb_t mpn_sb_divrem_mn _PROTO ((mp_ptr, mp_ptr, mp_size_t, mp_srcptr, mp_size_t));
mp_limb_t mpn_bz_divrem_n _PROTO ((mp_ptr, mp_ptr, mp_srcptr, mp_size_t));
void mpn_tdiv_qr _PROTO ((mp_ptr, mp_ptr, mp_size_t, mp_srcptr, mp_size_t, mp_srcptr, mp_size_t));
/* void mpn_tdiv_q _PROTO ((mp_ptr, mp_size_t, mp_srcptr, mp_size_t, mp_srcptr, mp_size_t)); */

/* Copy NLIMBS *limbs* from SRC to DST, NLIMBS==0 allowed.  */
#ifndef MPN_COPY_INCR
#if HAVE_NATIVE_mpn_copyi
#define MPN_COPY_INCR(DST, SRC, NLIMBS)   mpn_copyi (DST, SRC, NLIMBS)
#else
#define MPN_COPY_INCR(DST, SRC, NLIMBS) \
  do {									\
    mp_size_t __i;							\
    for (__i = 0; __i < (NLIMBS); __i++)				\
      (DST)[__i] = (SRC)[__i];						\
  } while (0)
#endif
#endif

#if HAVE_NATIVE_mpn_copyd
#define mpn_copyd __MPN(copyd)
void mpn_copyd _PROTO ((mp_ptr, mp_srcptr, mp_size_t));
#endif

/* NLIMBS==0 allowed */
#ifndef MPN_COPY_DECR
#if HAVE_NATIVE_mpn_copyd
#define MPN_COPY_DECR(DST, SRC, NLIMBS)   mpn_copyd (DST, SRC, NLIMBS)
#else
#define MPN_COPY_DECR(DST, SRC, NLIMBS) \
  do {									\
    mp_size_t __i;							\
    for (__i = (NLIMBS) - 1; __i >= 0; __i--)				\
      (DST)[__i] = (SRC)[__i];						\
  } while (0)
#endif
#endif

/* Define MPN_COPY for vector computers.  Since #pragma cannot be in a macro,
   rely on function inlining. */
#if defined (_CRAY) || defined (__uxp__)
static inline void
_MPN_COPY (d, s, n) mp_ptr d; mp_srcptr s; mp_size_t n;
{
  int i;				/* Faster for Cray with plain int */
#pragma _CRI ivdep			/* Cray PVP systems */
#pragma loop noalias d,s		/* Fujitsu VPP systems */
  for (i = 0; i < n; i++)
    d[i] = s[i];
}
#define MPN_COPY _MPN_COPY
#endif

#ifndef MPN_COPY
#define MPN_COPY MPN_COPY_INCR
#endif

/* Zero NLIMBS *limbs* AT DST.  */
#ifndef MPN_ZERO
#define MPN_ZERO(DST, NLIMBS) \
  do {									\
    mp_size_t __i;							\
    for (__i = 0; __i < (NLIMBS); __i++)				\
      (DST)[__i] = 0;							\
  } while (0)
#endif

#ifndef MPN_NORMALIZE
#define MPN_NORMALIZE(DST, NLIMBS) \
  do {									\
    while (NLIMBS > 0)							\
      {									\
	if ((DST)[(NLIMBS) - 1] != 0)					\
	  break;							\
	NLIMBS--;							\
      }									\
  } while (0)
#endif
#ifndef MPN_NORMALIZE_NOT_ZERO
#define MPN_NORMALIZE_NOT_ZERO(DST, NLIMBS) \
  do {									\
    while (1)								\
      {									\
	if ((DST)[(NLIMBS) - 1] != 0)					\
	  break;							\
	NLIMBS--;							\
      }									\
  } while (0)
#endif

/* Initialize X of type mpz_t with space for NLIMBS limbs.  X should be a
   temporary variable; it will be automatically cleared out at function
   return.  We use __x here to make it possible to accept both mpz_ptr and
   mpz_t arguments.  */
#define MPZ_TMP_INIT(X, NLIMBS) \
  do {									\
    mpz_ptr __x = (X);							\
    __x->_mp_alloc = (NLIMBS);						\
    __x->_mp_d = (mp_ptr) TMP_ALLOC ((NLIMBS) * BYTES_PER_MP_LIMB);	\
  } while (0)

/* Realloc for an mpz_t WHAT if it has less thann NEEDED limbs.  */
#define MPZ_REALLOC(what,needed) \
  do {								\
    if ((needed) > ALLOC (what))				\
      _mpz_realloc (what, needed);				\
  } while (0)

/* If KARATSUBA_MUL_THRESHOLD is not already defined, define it to a
   value which is good on most machines.  */
#ifndef KARATSUBA_MUL_THRESHOLD
#define KARATSUBA_MUL_THRESHOLD 32
#endif

/* If TOOM3_MUL_THRESHOLD is not already defined, define it to a
   value which is good on most machines.  */
#ifndef TOOM3_MUL_THRESHOLD
#define TOOM3_MUL_THRESHOLD 256
#endif

#ifndef KARATSUBA_SQR_THRESHOLD
#define KARATSUBA_SQR_THRESHOLD (2*KARATSUBA_MUL_THRESHOLD)
#endif

#ifndef TOOM3_SQR_THRESHOLD
#define TOOM3_SQR_THRESHOLD (2*TOOM3_MUL_THRESHOLD)
#endif

/* Return non-zero if xp,xsize and yp,ysize overlap.
   If xp+xsize<=yp there's no overlap, or if yp+ysize<=xp there's no
   overlap.  If both these are false, there's an overlap. */
#define MPN_OVERLAP_P(xp, xsize, yp, ysize) \
  ((xp) + (xsize) > (yp) && (yp) + (ysize) > (xp))


/* ASSERT() is a private assertion checking scheme, similar to <assert.h>.
   ASSERT() does the check only if WANT_ASSERT is selected, ASSERT_ALWAYS()
   does it always.  Generally assertions are meant for development, but
   might help when looking for a problem later too.

   ASSERT_NOCARRY() uses ASSERT() to check the expression is zero, but if
   assertion checking is disabled, the expression is still evaluated.  This
   is meant for use with routines like mpn_add_n() where the return value
   represents a carry or whatever that shouldn't occur.  For example,
   ASSERT_NOCARRY (mpn_add_n (rp, s1p, s2p, size)); */

#ifdef __LINE__
#define ASSERT_LINE  __LINE__
#else
#define ASSERT_LINE  -1
#endif

#ifdef __FILE__
#define ASSERT_FILE  __FILE__
#else
#define ASSERT_FILE  ""
#endif

#if HAVE_STRINGIZE
#define ASSERT_FAIL(expr)  __gmp_assert_fail (ASSERT_FILE, ASSERT_LINE, #expr)
#else
#define ASSERT_FAIL(expr)  __gmp_assert_fail (ASSERT_FILE, ASSERT_LINE, "expr")
#endif

#if HAVE_VOID
#define CAST_TO_VOID        (void)
#else
#define CAST_TO_VOID        
#endif

#define ASSERT_ALWAYS(expr) ((expr) ? 0 : ASSERT_FAIL (expr))

#if WANT_ASSERT
#define ASSERT(expr)           ASSERT_ALWAYS (expr)
#define ASSERT_NOCARRY(expr)   ASSERT_ALWAYS ((expr) == 0)
#else
#define ASSERT(expr)           (CAST_TO_VOID 0)
#define ASSERT_NOCARRY(expr)   (expr)
#endif

int __gmp_assert_fail _PROTO((const char *filename, int linenum,
                              const char *expr));


#if HAVE_NATIVE_mpn_com_n
#define mpn_com_n __MPN(com_n)
void mpn_com_n _PROTO ((mp_ptr, mp_srcptr, mp_size_t));
#else
#define mpn_com_n(d,s,n)        \
  {                             \
    mp_ptr     __d = (d);       \
    mp_srcptr  __s = (s);       \
    mp_size_t  __n = (n);       \
    do                          \
      *__d++ = *__s++;          \
    while (--__n);              \
  } while (0)
#endif

#define MPN_LOGOPS_N_INLINE(d,s1,s2,n,dop,op,s2op)      \
  {                                                     \
    mp_ptr     __d = (d);                               \
    mp_srcptr  __s1 = (s1);                             \
    mp_srcptr  __s2 = (s2);                             \
    mp_size_t  __n = (n);                               \
    do                                                  \
      *__d++ = dop (*__s1++ op s2op *__s2++);           \
    while (--__n);                                      \
  } while (0)

#if HAVE_NATIVE_mpn_and_n
#define mpn_and_n __MPN(and_n)
void mpn_and_n _PROTO ((mp_ptr, mp_srcptr, mp_srcptr, mp_size_t));
#else
#define mpn_and_n(d,s1,s2,n)  MPN_LOGOPS_N_INLINE(d,s1,s2,n, ,&, )
#endif

#if HAVE_NATIVE_mpn_andn_n
#define mpn_andn_n __MPN(andn_n)
void mpn_andn_n _PROTO ((mp_ptr, mp_srcptr, mp_srcptr, mp_size_t));
#else
#define mpn_andn_n(d,s1,s2,n) MPN_LOGOPS_N_INLINE(d,s1,s2,n, ,&,~)
#endif

#if HAVE_NATIVE_mpn_nand_n
#define mpn_nand_n __MPN(nand_n)
void mpn_nand_n _PROTO ((mp_ptr, mp_srcptr, mp_srcptr, mp_size_t));
#else
#define mpn_nand_n(d,s1,s2,n) MPN_LOGOPS_N_INLINE(d,s1,s2,n,~,&, )
#endif

#if HAVE_NATIVE_mpn_ior_n
#define mpn_ior_n __MPN(ior_n)
void mpn_ior_n _PROTO ((mp_ptr, mp_srcptr, mp_srcptr, mp_size_t));
#else
#define mpn_ior_n(d,s1,s2,n)  MPN_LOGOPS_N_INLINE(d,s1,s2,n, ,|, )
#endif

#if HAVE_NATIVE_mpn_iorn_n
#define mpn_iorn_n __MPN(iorn_n)
void mpn_iorn_n _PROTO ((mp_ptr, mp_srcptr, mp_srcptr, mp_size_t));
#else
#define mpn_iorn_n(d,s1,s2,n) MPN_LOGOPS_N_INLINE(d,s1,s2,n, ,|,~)
#endif

#if HAVE_NATIVE_mpn_nior_n
#define mpn_nior_n __MPN(nior_n)
void mpn_nior_n _PROTO ((mp_ptr, mp_srcptr, mp_srcptr, mp_size_t));
#else
#define mpn_nior_n(d,s1,s2,n) MPN_LOGOPS_N_INLINE(d,s1,s2,n,~,|, )
#endif

#if HAVE_NATIVE_mpn_xor_n
#define mpn_xor_n __MPN(xor_n)
void mpn_xor_n _PROTO ((mp_ptr, mp_srcptr, mp_srcptr, mp_size_t));
#else
#define mpn_xor_n(d,s1,s2,n)  MPN_LOGOPS_N_INLINE(d,s1,s2,n, ,^, )
#endif

#if HAVE_NATIVE_mpn_xnor_n
#define mpn_xnor_n __MPN(xnor_n)
void mpn_xnor_n _PROTO ((mp_ptr, mp_srcptr, mp_srcptr, mp_size_t));
#else
#define mpn_xnor_n(d,s1,s2,n) MPN_LOGOPS_N_INLINE(d,s1,s2,n,~,^, )
#endif

/* Structure for conversion between internal binary format and
   strings in base 2..36.  */
struct bases
{
  /* Number of digits in the conversion base that always fits in an mp_limb_t.
     For example, for base 10 on a machine where a mp_limb_t has 32 bits this
     is 9, since 10**9 is the largest number that fits into a mp_limb_t.  */
  int chars_per_limb;

  /* log(2)/log(conversion_base) */
  double chars_per_bit_exactly;

  /* base**chars_per_limb, i.e. the biggest number that fits a word, built by
     factors of base.  Exception: For 2, 4, 8, etc, big_base is log2(base),
     i.e. the number of bits used to represent each digit in the base.  */
  mp_limb_t big_base;

  /* A BITS_PER_MP_LIMB bit approximation to 1/big_base, represented as a
     fixed-point number.  Instead of dividing by big_base an application can
     choose to multiply by big_base_inverted.  */
  mp_limb_t big_base_inverted;
};

#define __mp_bases __MPN(mp_bases)
extern const struct bases __mp_bases[];
extern mp_size_t __gmp_default_fp_limb_precision;

#if defined (__i386__)
#define TARGET_REGISTER_STARVED 1
#else
#define TARGET_REGISTER_STARVED 0
#endif

#if defined (__alpha)
mp_limb_t __MPN(invert_limb) ();
#define invert_limb(invxl,xl) invxl = __MPN(invert_limb) (xl)
#endif

#ifndef invert_limb
#define invert_limb(invxl,xl) \
  do {									\
    mp_limb_t dummy;							\
    if (xl << 1 == 0)							\
      invxl = ~(mp_limb_t) 0;						\
    else								\
      udiv_qrnnd (invxl, dummy, -xl, 0, xl);				\
  } while (0)
#endif

/* Divide the two-limb number in (NH,,NL) by D, with DI being the largest
   limb not larger than (2**(2*BITS_PER_MP_LIMB))/D - (2**BITS_PER_MP_LIMB).
   If this would yield overflow, DI should be the largest possible number
   (i.e., only ones).  For correct operation, the most significant bit of D
   has to be set.  Put the quotient in Q and the remainder in R.  */
#define udiv_qrnnd_preinv(q, r, nh, nl, d, di) \
  do {									\
    mp_limb_t _q, _ql, _r;						\
    mp_limb_t _xh, _xl;							\
    umul_ppmm (_q, _ql, (nh), (di));					\
    _q += (nh);			/* DI is 2**BITS_PER_MP_LIMB too small */\
    umul_ppmm (_xh, _xl, _q, (d));					\
    sub_ddmmss (_xh, _r, (nh), (nl), _xh, _xl);				\
    if (_xh != 0)							\
      {									\
	sub_ddmmss (_xh, _r, _xh, _r, 0, (d));				\
	_q += 1;							\
	if (_xh != 0)							\
	  {								\
	    sub_ddmmss (_xh, _r, _xh, _r, 0, (d));			\
	    _q += 1;							\
	  }								\
      }									\
    if (_r >= (d))							\
      {									\
	_r -= (d);							\
	_q += 1;							\
      }									\
    (r) = _r;								\
    (q) = _q;								\
  } while (0)
/* Like udiv_qrnnd_preinv, but for for any value D.  DNORM is D shifted left
   so that its most significant bit is set.  LGUP is ceil(log2(D)).  */
#define udiv_qrnnd_preinv2gen(q, r, nh, nl, d, di, dnorm, lgup) \
  do {									\
    mp_limb_t _n2, _n10, _n1, _nadj, _q1;				\
    mp_limb_t _xh, _xl;							\
    _n2 = ((nh) << (BITS_PER_MP_LIMB - (lgup))) + ((nl) >> 1 >> (l - 1));\
    _n10 = (nl) << (BITS_PER_MP_LIMB - (lgup));				\
    _n1 = ((mp_limb_signed_t) _n10 >> (BITS_PER_MP_LIMB - 1));		\
    _nadj = _n10 + (_n1 & (dnorm));					\
    umul_ppmm (_xh, _xl, di, _n2 - _n1);				\
    add_ssaaaa (_xh, _xl, _xh, _xl, 0, _nadj);				\
    _q1 = ~(_n2 + _xh);							\
    umul_ppmm (_xh, _xl, _q1, d);					\
    add_ssaaaa (_xh, _xl, _xh, _xl, nh, nl);				\
    _xh -= (d);								\
    (r) = _xl + ((d) & _xh);						\
    (q) = _xh - _q1;							\
  } while (0)
/* Exactly like udiv_qrnnd_preinv, but branch-free.  It is not clear which
   version to use.  */
#define udiv_qrnnd_preinv2norm(q, r, nh, nl, d, di) \
  do {									\
    mp_limb_t _n2, _n10, _n1, _nadj, _q1;				\
    mp_limb_t _xh, _xl;							\
    _n2 = (nh);								\
    _n10 = (nl);							\
    _n1 = ((mp_limb_signed_t) _n10 >> (BITS_PER_MP_LIMB - 1));		\
    _nadj = _n10 + (_n1 & (d));						\
    umul_ppmm (_xh, _xl, di, _n2 - _n1);				\
    add_ssaaaa (_xh, _xl, _xh, _xl, 0, _nadj);				\
    _q1 = ~(_n2 + _xh);							\
    umul_ppmm (_xh, _xl, _q1, d);					\
    add_ssaaaa (_xh, _xl, _xh, _xl, nh, nl);				\
    _xh -= (d);								\
    (r) = _xl + ((d) & _xh);						\
    (q) = _xh - _q1;							\
  } while (0)

/* The `mode' attribute was introduced in GCC 2.2, but we can only distinguish
   between GCC 2 releases from 2.5, since __GNUC_MINOR__ wasn't introduced
   until then.  */
#if __GNUC__ - 0 > 2 || defined (__GNUC_MINOR__) && ! defined (__APPLE_CC__)
/* Define stuff for longlong.h.  */
typedef unsigned int UQItype	__attribute__ ((mode (QI)));
typedef		 int SItype	__attribute__ ((mode (SI)));
typedef unsigned int USItype	__attribute__ ((mode (SI)));
typedef		 int DItype	__attribute__ ((mode (DI)));
typedef unsigned int UDItype	__attribute__ ((mode (DI)));
#else
typedef unsigned char UQItype;
typedef		 long SItype;
typedef unsigned long USItype;
#if defined _LONGLONG || defined _LONG_LONG_LIMB
typedef	long long int DItype;
typedef unsigned long long int UDItype;
#endif
#endif

typedef mp_limb_t UWtype;
typedef unsigned int UHWtype;
#define W_TYPE_SIZE BITS_PER_MP_LIMB

/* Define ieee_double_extract and _GMP_IEEE_FLOATS.  */

#if (defined (__arm__) && (defined (__ARMWEL__) || defined (__linux__)))
/* Special case for little endian ARM since floats remain in big-endian.  */
#define _GMP_IEEE_FLOATS 1
union ieee_double_extract
{
  struct
    {
      unsigned int manh:20;
      unsigned int exp:11;
      unsigned int sig:1;
      unsigned int manl:32;
    } s;
  double d;
};
#else
#if defined (_LITTLE_ENDIAN) || defined (__LITTLE_ENDIAN__)		\
 || defined (__alpha)							\
 || defined (__clipper__)						\
 || defined (__cris)							\
 || defined (__i386__)							\
 || defined (__i860__)							\
 || defined (__i960__)							\
 || defined (MIPSEL) || defined (_MIPSEL)				\
 || defined (__ns32000__)						\
 || defined (__WINNT) || defined (_WIN32)
#define _GMP_IEEE_FLOATS 1
union ieee_double_extract
{
  struct
    {
      unsigned int manl:32;
      unsigned int manh:20;
      unsigned int exp:11;
      unsigned int sig:1;
    } s;
  double d;
};
#else /* Need this as an #else since the tests aren't made exclusive.  */
#if defined (_BIG_ENDIAN) || defined (__BIG_ENDIAN__)			\
 || defined (__a29k__) || defined (_AM29K)				\
 || defined (__arm__)							\
 || (defined (__convex__) && defined (_IEEE_FLOAT_))			\
 || defined (_CRAYMPP)							\
 || defined (__i370__) || defined (__mvs__)				\
 || defined (__mc68000__) || defined (__mc68020__) || defined (__m68k__)\
    || defined(mc68020)							\
 || defined (__m88000__)						\
 || defined (MIPSEB) || defined (_MIPSEB)				\
 || defined (__hppa) || defined (__hppa__)				\
 || defined (__pyr__)							\
 || defined (__ibm032__)						\
 || defined (_IBMR2) || defined (_ARCH_PPC)				\
 || defined (__sh__)							\
 || defined (__sparc) || defined (sparc)				\
 || defined (__we32k__)
#define _GMP_IEEE_FLOATS 1
union ieee_double_extract
{
  struct
    {
      unsigned int sig:1;
      unsigned int exp:11;
      unsigned int manh:20;
      unsigned int manl:32;
    } s;
  double d;
};
#endif
#endif
#endif

#define MP_BASE_AS_DOUBLE (2.0 * ((mp_limb_t) 1 << (BITS_PER_MP_LIMB - 1)))
#if BITS_PER_MP_LIMB == 64
#define LIMBS_PER_DOUBLE 2
#else
#define LIMBS_PER_DOUBLE 3
#endif

double __gmp_scale2 _PROTO ((double, int));
int __gmp_extract_double _PROTO ((mp_ptr, double));

extern int __gmp_junk;
extern const int __gmp_0;
#define GMP_ERROR(code)   (gmp_errno |= (code), __gmp_junk = 10/__gmp_0)
#define DIVIDE_BY_ZERO    GMP_ERROR(GMP_ERROR_DIVISION_BY_ZERO)
#define SQRT_OF_NEGATIVE  GMP_ERROR(GMP_ERROR_SQRT_OF_NEGATIVE)

#if defined _LONG_LONG_LIMB
#if defined (__STDC__)
#define CNST_LIMB(C) C##LL
#else
#define CNST_LIMB(C) C/**/LL
#endif
#else /* not _LONG_LONG_LIMB */
#if defined (__STDC__)
#define CNST_LIMB(C) C##L
#else
#define CNST_LIMB(C) C/**/L
#endif
#endif /* _LONG_LONG_LIMB */

/*** Stuff used by mpn/generic/prefsqr.c and mpn/generic/next_prime.c ***/
#if BITS_PER_MP_LIMB == 32
#define PP 0xC0CFD797L		/* 3 x 5 x 7 x 11 x 13 x ... x 29 */
#define PP_INVERTED 0x53E5645CL
#define PP_MAXPRIME 29
#define PP_MASK 0x208A28A8L
#endif

#if BITS_PER_MP_LIMB == 64
#define PP CNST_LIMB(0xE221F97C30E94E1D)	/* 3 x 5 x 7 x 11 x 13 x ... x 53 */
#define PP_INVERTED CNST_LIMB(0x21CFE6CFC938B36B)
#define PP_MAXPRIME 53
#define PP_MASK CNST_LIMB(0x208A20A08A28A8)
#endif

/* For testing and debugging.  */
#define MPZ_CHECK_FORMAT(z)                                     	\
  (ASSERT_ALWAYS (SIZ(z) == 0 || PTR(z)[ABSIZ(z) - 1] != 0),    	\
   ASSERT_ALWAYS (ALLOC(z) >= ABSIZ(z)))
#define MPZ_PROVOKE_REALLOC(z)						\
  do { ALLOC(z) = ABSIZ(z); } while (0)

#if TUNE_PROGRAM_BUILD
/* Some extras wanted when recompiling some .c files for use by the tune
   program.  Not part of a normal build. */
extern mp_size_t  tune_mul_threshold[];
extern mp_size_t  tune_sqr_threshold[];
#endif
