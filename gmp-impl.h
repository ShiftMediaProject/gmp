/* Include file for internal GNU MP types and definitions.

   THE CONTENTS OF THIS FILE ARE FOR INTERNAL USE AND ARE ALMOST CERTAIN TO
   BE SUBJECT TO INCOMPATIBLE CHANGES IN FUTURE GNU MP RELEASES.

Copyright 1991, 1993, 1994, 1995, 1996, 1997, 1999, 2000, 2001, 2002 Free
Software Foundation, Inc.

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


#ifndef __GMP_IMPL_H__
#define __GMP_IMPL_H__

/* On Cray vector systems "short" and "unsigned short" might not be the same
   number of bits, making the SHRT_MAX defaults below fail.  (This depends
   on compiler options.)  Instead use limits.h.  */
#if defined _CRAY
#include <limits.h>
#include <intrinsics.h>  /* for _popcnt */
#endif

#if ! __GMP_WITHIN_CONFIGURE
#include "config.h"
#include "gmp-mparam.h"
#include "fib_table.h"
#include "mp_bases.h"
#endif

#ifdef __cplusplus
#include <string>
#endif

/* Might search and replace _PROTO to __GMP_PROTO internally one day, to
   avoid two names for one thing, but no hurry for that.  */
#define _PROTO(x)  __GMP_PROTO(x)

/* The following tries to get a good version of alloca.  The tests are
   adapted from autoconf AC_FUNC_ALLOCA, with a couple of additions.
   Whether this succeeds is tested by GMP_FUNC_ALLOCA and HAVE_ALLOCA will
   be setup appropriately.

   ifndef alloca - a cpp define might already exist.
       glibc <stdlib.h> includes <alloca.h> which uses GCC __builtin_alloca.
       HP cc +Olibcalls adds a #define of alloca to __builtin_alloca.

   GCC __builtin_alloca - preferred whenever available.

   _AIX pragma - IBM compilers need a #pragma in "each module that needs to
       use alloca".  Pragma indented to protect pre-ANSI cpp's.  _IBMR2 was
       used in past versions of GMP, retained still in case it matters.

       The autoconf manual says this pragma needs to be at the start of a C
       file, apart from comments and preprocessor directives.  Is that true?
       xlc on aix 4.xxx doesn't seem to mind it being after prototypes etc
       from gmp.h.
*/

#ifndef alloca
# ifdef __GNUC__
#  define alloca __builtin_alloca
# else
#  ifdef __DECC
#   define alloca(x) __ALLOCA(x)
#  else
#   ifdef _MSC_VER
#    include <malloc.h>
#    define alloca _alloca
#   else
#    if HAVE_ALLOCA_H
#     include <alloca.h>
#    else
#     if defined (_AIX) || defined (_IBMR2)
 #pragma alloca
#     else
       char *alloca ();
#     endif
#    endif
#   endif
#  endif
# endif
#endif

/* const and signed must match __gmp_const and __gmp_signed, so follow the
   decision made for those in gmp.h.    */
#if ! __GMP_HAVE_CONST
#define const   /* empty */
#define signed  /* empty */
#endif

/* "const" basically means a function does nothing but examine its arguments
   and give a return value, it doesn't read or write any memory (neither
   global nor pointed to by arguments), and has no other side-effects.  This
   is more restrictive than "pure".  See info node "(gcc)Function
   Attributes".  */
#if HAVE_ATTRIBUTE_CONST
#define ATTRIBUTE_CONST  __attribute__ ((const))
#else
#define ATTRIBUTE_CONST
#endif

#if HAVE_ATTRIBUTE_NORETURN
#define ATTRIBUTE_NORETURN  __attribute__ ((noreturn))
#else
#define ATTRIBUTE_NORETURN
#endif

/* "malloc" means a function behaves like malloc in that the pointer it
   returns doesn't alias anything.  */
#if HAVE_ATTRIBUTE_MALLOC
#define ATTRIBUTE_MALLOC  __attribute__ ((malloc))
#else
#define ATTRIBUTE_MALLOC
#endif


#if ! HAVE_STRCHR
#define strchr(s,c)  index(s,c)
#endif

#if ! HAVE_MEMSET
#define memset(p, c, n)                 \
  do {                                  \
    ASSERT (n >= 0);                    \
    int  __i;                           \
    for (__i = 0; __i < (n); __i++)     \
      (p)[__i] = (c);                   \
  } while (0)
#endif

/* va_copy is standard in C99, and gcc provides __va_copy when in strict C89
   mode.  Falling back to a memcpy will give maximum portability, since it
   works no matter whether va_list is a pointer, struct or array.  */
#if ! defined (va_copy) && defined (__va_copy)
#define va_copy(dst,src)  __va_copy(dst,src)
#endif
#if ! defined (va_copy)
#define va_copy(dst,src) \
  do { memcpy (&(dst), &(src), sizeof (va_list)); } while (0)
#endif


#if defined (__cplusplus)
extern "C" {
#endif


/* Usage: TMP_DECL (marker);
          TMP_MARK (marker);
          ptr = TMP_ALLOC (bytes);
          TMP_FREE (marker);

   TMP_DECL just declares a variable, but might be empty and so must be last
   in a list of variables.  TMP_MARK must be done before any TMP_ALLOC.
   TMP_ALLOC(0) is not allowed.  TMP_FREE doesn't need to be done if a
   TMP_MARK was made, but then no TMP_ALLOCs.

   The name "marker" isn't used by the malloc-reentrant and debug methods,
   instead they hardcode a name which TMP_ALLOC will know.  For that reason
   two TMP_DECLs are not allowed, unless one is in a nested "{ }" block, and
   in that case TMP_MARK, TMP_ALLOC and TMP_FREE will refer to the TMP_DECL
   which is in scope, irrespective of the marker name given.  */

/* The alignment in bytes, used for TMP_ALLOCed blocks, when alloca or
   __gmp_allocate_func doesn't already determine it.  Currently TMP_ALLOC
   isn't used for "double"s, so that's not in the union.  */
union tmp_align_t {
  mp_limb_t  l;
  char       *p;
};
#define __TMP_ALIGN  sizeof (union tmp_align_t)

/* Return "a" rounded upwards to a multiple of "m", if it isn't already.
   "a" must be an unsigned type.  */
#define ROUND_UP_MULTIPLE(a,m)  ((a) + (-(a))%(m))

#if WANT_TMP_ALLOCA
/* Each TMP_ALLOC is simply an alloca(), and nothing else is needed.
   This is the preferred method.  */
#define TMP_DECL(m)
#define TMP_ALLOC(x) alloca(x)
#define TMP_MARK(m)
#define TMP_FREE(m)
#endif

#if WANT_TMP_REENTRANT
/* See tal-reent.c for some comments. */
struct tmp_reentrant_t {
  struct tmp_reentrant_t  *next;
  size_t                  size;   /* bytes, including header */
};
void *__gmp_tmp_reentrant_alloc _PROTO ((struct tmp_reentrant_t **, size_t)) ATTRIBUTE_MALLOC;
void  __gmp_tmp_reentrant_free _PROTO ((struct tmp_reentrant_t *));
#define TMP_DECL(marker)   struct tmp_reentrant_t *__tmp_marker
/* don't demand NULL, just cast a zero */
#define TMP_MARK(marker) \
  do { __tmp_marker = (struct tmp_reentrant_t *) 0; } while (0)
#define TMP_ALLOC(size)    __gmp_tmp_reentrant_alloc (&__tmp_marker, size)
#define TMP_FREE(marker)   __gmp_tmp_reentrant_free  (__tmp_marker)
#endif

#if WANT_TMP_NOTREENTRANT
/* See tal-notreent.c for some comments. */
struct tmp_marker
{
  struct tmp_stack *which_chunk;
  void *alloc_point;
};
typedef struct tmp_marker tmp_marker;
void *__gmp_tmp_alloc _PROTO ((unsigned long)) ATTRIBUTE_MALLOC;
void __gmp_tmp_mark _PROTO ((tmp_marker *));
void __gmp_tmp_free _PROTO ((tmp_marker *));
#define TMP_DECL(marker) tmp_marker marker
/* gcc recognises "(-(8*n))%8" or the like is always zero, which means the
   rounding up is a noop for allocs of whole limbs. */
#define TMP_ALLOC(size) \
  __gmp_tmp_alloc (ROUND_UP_MULTIPLE ((unsigned long) (size), __TMP_ALIGN))
#define TMP_MARK(marker) __gmp_tmp_mark (&marker)
#define TMP_FREE(marker) __gmp_tmp_free (&marker)
#endif

#if WANT_TMP_DEBUG
/* See tal-debug.c for some comments. */
struct tmp_debug_t {
  struct tmp_debug_entry_t  *list;
  const char                *file;
  int                       line;
};
struct tmp_debug_entry_t {
  struct tmp_debug_entry_t  *next;
  char                      *block;
  size_t                    size;
};
void  __gmp_tmp_debug_mark  _PROTO ((const char *, int, struct tmp_debug_t **,
                                     struct tmp_debug_t *,
                                     const char *, const char *));
void *__gmp_tmp_debug_alloc _PROTO ((const char *, int, int,
                                     struct tmp_debug_t **, const char *,
                                     size_t)) ATTRIBUTE_MALLOC;
void  __gmp_tmp_debug_free  _PROTO ((const char *, int, int,
                                     struct tmp_debug_t **,
                                     const char *, const char *));
#if HAVE_STRINGIZE
#define TMP_DECL(marker) TMP_DECL_NAME(marker, #marker)
#define TMP_MARK(marker) TMP_MARK_NAME(marker, #marker)
#define TMP_FREE(marker) TMP_FREE_NAME(marker, #marker)
#else
#define TMP_DECL(marker) TMP_DECL_NAME(marker, "marker")
#define TMP_MARK(marker) TMP_MARK_NAME(marker, "marker")
#define TMP_FREE(marker) TMP_FREE_NAME(marker, "marker")
#endif
/* The marker variable is designed to provoke an uninitialized varialble
   warning from the compiler if TMP_FREE is used without a TMP_MARK.
   __tmp_marker_inscope does the same for TMP_ALLOC.  Runtime tests pick
   these things up too.  */
#define TMP_DECL_NAME(marker, marker_name)                      \
  int marker;                                                   \
  int __tmp_marker_inscope;                                     \
  const char *__tmp_marker_name = marker_name;                  \
  struct tmp_debug_t  __tmp_marker_struct;                      \
  /* don't demand NULL, just cast a zero */                     \
  struct tmp_debug_t  *__tmp_marker = (struct tmp_debug_t *) 0
#define TMP_MARK_NAME(marker, marker_name)                      \
  do {                                                          \
    marker = 1;                                                 \
    __tmp_marker_inscope = 1;                                   \
    __gmp_tmp_debug_mark  (ASSERT_FILE, ASSERT_LINE,            \
                           &__tmp_marker, &__tmp_marker_struct, \
                           __tmp_marker_name, marker_name);     \
  } while (0)
#define TMP_ALLOC(size)                                                 \
  __gmp_tmp_debug_alloc (ASSERT_FILE, ASSERT_LINE,                      \
                         __tmp_marker_inscope,                          \
                         &__tmp_marker, __tmp_marker_name, size)
#define TMP_FREE_NAME(marker, marker_name)                      \
  do {                                                          \
    __gmp_tmp_debug_free  (ASSERT_FILE, ASSERT_LINE,            \
                           marker, &__tmp_marker,               \
                           __tmp_marker_name, marker_name);     \
  } while (0)
#endif


/* Allocating various types. */
#define TMP_ALLOC_TYPE(n,type) ((type *) TMP_ALLOC ((n) * sizeof (type)))
#define TMP_ALLOC_LIMBS(n)     TMP_ALLOC_TYPE(n,mp_limb_t)
#define TMP_ALLOC_MP_PTRS(n)   TMP_ALLOC_TYPE(n,mp_ptr)

/* It's more efficient to allocate one block than two.  This is certainly
   true of the malloc methods, but it can even be true of alloca if that
   involves copying a chunk of stack (various RISCs), or a call to a stack
   bounds check (mingw).  In any case, when debugging keep separate blocks
   so a redzoning malloc debugger can protect each individually.  */
#if WANT_TMP_DEBUG
#define TMP_ALLOC_LIMBS_2(xp,xsize, yp,ysize)   \
  do {                                          \
    (xp) = TMP_ALLOC_LIMBS (xsize);             \
    (yp) = TMP_ALLOC_LIMBS (ysize);             \
  } while (0)
#else
#define TMP_ALLOC_LIMBS_2(xp,xsize, yp,ysize)   \
  do {                                          \
    (xp) = TMP_ALLOC_LIMBS ((xsize) + (ysize)); \
    (yp) = (xp) + (xsize);                      \
  } while (0)
#endif


/* From gmp.h, nicer names for internal use. */
#define CRAY_Pragma(str)               __GMP_CRAY_Pragma(str)
#define MPN_CMP(result, xp, yp, size)  __GMPN_CMP(result, xp, yp, size)
#define LIKELY(cond)                   __GMP_LIKELY(cond)
#define UNLIKELY(cond)                 __GMP_UNLIKELY(cond)

#define ABS(x) ((x) >= 0 ? (x) : -(x))
#define MIN(l,o) ((l) < (o) ? (l) : (o))
#define MAX(h,i) ((h) > (i) ? (h) : (i))
#define numberof(x)  (sizeof (x) / sizeof ((x)[0]))

/* Field access macros.  */
#define SIZ(x) ((x)->_mp_size)
#define ABSIZ(x) ABS (SIZ (x))
#define PTR(x) ((x)->_mp_d)
#define LIMBS(x) ((x)->_mp_d)
#define EXP(x) ((x)->_mp_exp)
#define PREC(x) ((x)->_mp_prec)
#define ALLOC(x) ((x)->_mp_alloc)

/* n-1 inverts any low zeros and the lowest one bit.  If n&(n-1) leaves zero
   then that lowest one bit must have been the only bit set.  n==0 will
   return true though, so avoid that.  */
#define POW2_P(n)  (((n) & ((n) - 1)) == 0)


/* Might be already defined by gmp-mparam.h, otherwise use what's in gmp.h. */
#ifndef BITS_PER_MP_LIMB
#define BITS_PER_MP_LIMB  __GMP_BITS_PER_MP_LIMB
#endif


/* The "short" defines are a bit different because shorts are promoted to
   ints by ~ or >> etc.  */

#ifndef ULONG_MAX
#define ULONG_MAX   __GMP_ULONG_MAX
#endif
#ifndef UINT_MAX
#define UINT_MAX    __GMP_UINT_MAX
#endif
#ifndef USHRT_MAX
#define USHRT_MAX   __GMP_USHRT_MAX
#endif
#define MP_LIMB_T_MAX      (~ (mp_limb_t) 0)

/* Must cast ULONG_MAX etc to unsigned long etc, since they might not be
   unsigned on a K&R compiler.  In particular the HP-UX 10 bundled K&R cc
   treats the plain decimal values in <limits.h> as signed.  */
#define ULONG_HIGHBIT      (ULONG_MAX ^ ((unsigned long) ULONG_MAX >> 1))
#define UINT_HIGHBIT       (UINT_MAX ^ ((unsigned) UINT_MAX >> 1))
#define USHRT_HIGHBIT      ((unsigned short) (USHRT_MAX ^ ((unsigned short) USHRT_MAX >> 1)))
#define GMP_LIMB_HIGHBIT  (MP_LIMB_T_MAX ^ (MP_LIMB_T_MAX >> 1))

#ifndef LONG_MIN
#define LONG_MIN           ((long) ULONG_HIGHBIT)
#endif
#ifndef LONG_MAX
#define LONG_MAX           (-(LONG_MIN+1))
#endif

#ifndef INT_MIN
#define INT_MIN            ((int) UINT_HIGHBIT)
#endif
#ifndef INT_MAX
#define INT_MAX            (-(INT_MIN+1))
#endif

#ifndef SHRT_MIN
#define SHRT_MIN           ((short) USHRT_HIGHBIT)
#endif
#ifndef SHRT_MAX
#define SHRT_MAX           ((short) (-(SHRT_MIN+1)))
#endif

#if __GMP_MP_SIZE_T_INT
#define MP_SIZE_T_MAX      INT_MAX
#define MP_SIZE_T_MIN      INT_MIN
#else
#define MP_SIZE_T_MAX      LONG_MAX
#define MP_SIZE_T_MIN      LONG_MIN
#endif

#define LONG_HIGHBIT       LONG_MIN
#define INT_HIGHBIT        INT_MIN
#define SHRT_HIGHBIT       SHRT_MIN


#define GMP_NUMB_HIGHBIT  (CNST_LIMB(1) << (GMP_NUMB_BITS-1))

#if GMP_NAIL_BITS == 0
#define GMP_NAIL_LOWBIT   CNST_LIMB(0)
#else
#define GMP_NAIL_LOWBIT   (CNST_LIMB(1) << GMP_NUMB_BITS)
#endif

#if GMP_NAIL_BITS != 0
/* Set various *_THRESHOLD values to be used for nails.  Thus we avoid using
   code that has not yet been qualified.  */

#undef DIV_SB_PREINV_THRESHOLD
#undef DIV_DC_THRESHOLD
#undef POWM_THRESHOLD
#define DIV_SB_PREINV_THRESHOLD           MP_SIZE_T_MAX
#define DIV_DC_THRESHOLD                 50
#define POWM_THRESHOLD                    0

#undef GCD_ACCEL_THRESHOLD
#undef GCDEXT_THRESHOLD
#define GCD_ACCEL_THRESHOLD               3
#define GCDEXT_THRESHOLD                 20

#undef DIVREM_1_NORM_THRESHOLD
#undef DIVREM_1_UNNORM_THRESHOLD
#undef MOD_1_NORM_THRESHOLD
#undef MOD_1_UNNORM_THRESHOLD
#undef USE_PREINV_DIVREM_1
#undef USE_PREINV_MOD_1
#undef DIVREM_2_THRESHOLD
#undef DIVEXACT_1_THRESHOLD
#undef MODEXACT_1_ODD_THRESHOLD
#define DIVREM_1_NORM_THRESHOLD           MP_SIZE_T_MAX  /* preinv always */
#define DIVREM_1_UNNORM_THRESHOLD         MP_SIZE_T_MAX  /* always */
#define MOD_1_NORM_THRESHOLD              MP_SIZE_T_MAX  /* always */
#define MOD_1_UNNORM_THRESHOLD            MP_SIZE_T_MAX  /* always */
#define USE_PREINV_DIVREM_1               0  /* preinv never */
#define USE_PREINV_MOD_1                  0  /* preinv never */
#define DIVREM_2_THRESHOLD                MP_SIZE_T_MAX  /* preinv never */
#define DIVEXACT_1_THRESHOLD              MP_SIZE_T_MAX  /* always */
#define MODEXACT_1_ODD_THRESHOLD          MP_SIZE_T_MAX  /* always */

#undef GET_STR_DC_THRESHOLD
#undef GET_STR_PRECOMPUTE_THRESHOLD
#undef SET_STR_THRESHOLD
#define GET_STR_DC_THRESHOLD             22
#define GET_STR_PRECOMPUTE_THRESHOLD     42
#define SET_STR_THRESHOLD              3259

#undef MUL_FFT_TABLE
#undef MUL_FFT_MODF_THRESHOLD
#undef MUL_FFT_THRESHOLD
#define MUL_FFT_TABLE  { 400, 928, 1856, 3840, 7168, 20480, 0 }
#define MUL_FFT_MODF_THRESHOLD          416
#define MUL_FFT_THRESHOLD                MP_SIZE_T_MAX

#undef  SQR_FFT_TABLE
#undef  SQR_FFT_MODF_THRESHOLD
#undef  SQR_FFT_THRESHOLD
#define SQR_FFT_TABLE  { 400, 992, 1984, 3840, 9216, 20480, 0 }
#define SQR_FFT_MODF_THRESHOLD          416
#define SQR_FFT_THRESHOLD                MP_SIZE_T_MAX

#endif

/* Swap macros. */

#define MP_LIMB_T_SWAP(x, y)                    \
  do {                                          \
    mp_limb_t __mp_limb_t_swap__tmp = (x);      \
    (x) = (y);                                  \
    (y) = __mp_limb_t_swap__tmp;                \
  } while (0)
#define MP_SIZE_T_SWAP(x, y)                    \
  do {                                          \
    mp_size_t __mp_size_t_swap__tmp = (x);      \
    (x) = (y);                                  \
    (y) = __mp_size_t_swap__tmp;                \
  } while (0)

#define MP_PTR_SWAP(x, y)               \
  do {                                  \
    mp_ptr __mp_ptr_swap__tmp = (x);    \
    (x) = (y);                          \
    (y) = __mp_ptr_swap__tmp;           \
  } while (0)
#define MP_SRCPTR_SWAP(x, y)                    \
  do {                                          \
    mp_srcptr __mp_srcptr_swap__tmp = (x);      \
    (x) = (y);                                  \
    (y) = __mp_srcptr_swap__tmp;                \
  } while (0)

#define MPN_PTR_SWAP(xp,xs, yp,ys)      \
  do {                                  \
    MP_PTR_SWAP (xp, yp);               \
    MP_SIZE_T_SWAP (xs, ys);            \
  } while(0)
#define MPN_SRCPTR_SWAP(xp,xs, yp,ys)   \
  do {                                  \
    MP_SRCPTR_SWAP (xp, yp);            \
    MP_SIZE_T_SWAP (xs, ys);            \
  } while(0)

#define MPZ_PTR_SWAP(x, y)              \
  do {                                  \
    mpz_ptr __mpz_ptr_swap__tmp = (x);  \
    (x) = (y);                          \
    (y) = __mpz_ptr_swap__tmp;          \
  } while (0)
#define MPZ_SRCPTR_SWAP(x, y)                   \
  do {                                          \
    mpz_srcptr __mpz_srcptr_swap__tmp = (x);    \
    (x) = (y);                                  \
    (y) = __mpz_srcptr_swap__tmp;               \
  } while (0)


void *__gmp_default_allocate _PROTO ((size_t));
void *__gmp_default_reallocate _PROTO ((void *, size_t, size_t));
void __gmp_default_free _PROTO ((void *, size_t));

#define __GMP_ALLOCATE_FUNC_TYPE(n,type) \
  ((type *) (*__gmp_allocate_func) ((n) * sizeof (type)))
#define __GMP_ALLOCATE_FUNC_LIMBS(n)   __GMP_ALLOCATE_FUNC_TYPE (n, mp_limb_t)

#define __GMP_REALLOCATE_FUNC_TYPE(p, old_size, new_size, type) \
  ((type *) (*__gmp_reallocate_func)                            \
   (p, (old_size) * sizeof (type), (new_size) * sizeof (type)))
#define __GMP_REALLOCATE_FUNC_LIMBS(p, old_size, new_size) \
  __GMP_REALLOCATE_FUNC_TYPE(p, old_size, new_size, mp_limb_t)

#define __GMP_FREE_FUNC_TYPE(p,n,type) (*__gmp_free_func) (p, (n) * sizeof (type))
#define __GMP_FREE_FUNC_LIMBS(p,n)     __GMP_FREE_FUNC_TYPE (p, n, mp_limb_t)

#define __GMP_REALLOCATE_FUNC_MAYBE(ptr, oldsize, newsize)      \
  do {                                                          \
    if ((oldsize) != (newsize))                                 \
      (ptr) = (*__gmp_reallocate_func) (ptr, oldsize, newsize); \
  } while (0)

#define __GMP_REALLOCATE_FUNC_MAYBE_TYPE(ptr, oldsize, newsize, type)   \
  do {                                                                  \
    if ((oldsize) != (newsize))                                         \
      (ptr) = (type *) (*__gmp_reallocate_func)                         \
        (ptr, (oldsize) * sizeof (type), (newsize) * sizeof (type));    \
  } while (0)


/* Dummy for non-gcc, code involving it will go dead. */
#ifndef __GNUC__
#define __builtin_constant_p(x)   0
#endif


/* In gcc 2.96 and up on i386, tail calls are optimized to jumps if the
   stack usage is compatible.  __attribute__ ((regparm (N))) helps by
   putting leading parameters in registers, avoiding extra stack.  */

#if HAVE_HOST_CPU_FAMILY_x86 && __GMP_GNUC_PREREQ (2,96)
#define USE_LEADING_REGPARM 1
#else
#define USE_LEADING_REGPARM 0
#endif

/* Macros for altering parameter order according to regparm usage. */
#if USE_LEADING_REGPARM
#define REGPARM_2_1(a,b,x)    x,a,b
#define REGPARM_3_1(a,b,c,x)  x,a,b,c
#define REGPARM_ATTR(n) __attribute__ ((regparm (n)))
#else
#define REGPARM_2_1(a,b,x)    a,b,x
#define REGPARM_3_1(a,b,c,x)  a,b,c,x
#define REGPARM_ATTR(n)
#endif


/* ASM_L gives a local label for a gcc asm block, for use when temporary
   local labels like "1:" might not be available, which is the case for
   instance on the x86s (the SCO assembler doesn't support them).

   The label generated is made unique by including "%=" which is a unique
   number for each insn.  This ensures the same name can be used in multiple
   asm blocks, perhaps via a macro.  Since jumps between asm blocks are not
   allowed there's no need for a label to be usable outside a single
   block.  */

#define ASM_L(name)  LSYM_PREFIX "asm_%=_" #name


#if defined (__GNUC__) && HAVE_HOST_CPU_FAMILY_x86
#if 0
/* FIXME: Check that these actually improve things.
   FIXME: Need a cld after each std.
   FIXME: Can't have inputs in clobbered registers, must describe them as
   dummy outputs, and add volatile. */
#define MPN_COPY_INCR(DST, SRC, N)					\
  __asm__ ("cld\n\trep\n\tmovsl" : :					\
	   "D" (DST), "S" (SRC), "c" (N) :				\
	   "cx", "di", "si", "memory")
#define MPN_COPY_DECR(DST, SRC, N)					\
  __asm__ ("std\n\trep\n\tmovsl" : :					\
	   "D" ((DST) + (N) - 1), "S" ((SRC) + (N) - 1), "c" (N) :	\
	   "cx", "di", "si", "memory")
#endif
#endif


void __gmpz_aorsmul_1 _PROTO ((REGPARM_3_1 (mpz_ptr w, mpz_srcptr u, mp_limb_t v, mp_size_t sub))) REGPARM_ATTR(1);
#define mpz_aorsmul_1(w,u,v,sub)  __gmpz_aorsmul_1 (REGPARM_3_1 (w, u, v, sub))

#define mpz_n_pow_ui __gmpz_n_pow_ui
void    mpz_n_pow_ui _PROTO ((mpz_ptr, mp_srcptr, mp_size_t, unsigned long));


#define mpn_add_nc __MPN(add_nc)
__GMP_DECLSPEC mp_limb_t mpn_add_nc __GMP_PROTO ((mp_ptr, mp_srcptr, mp_srcptr, mp_size_t, mp_limb_t));

#define mpn_addmul_1c __MPN(addmul_1c)
__GMP_DECLSPEC mp_limb_t mpn_addmul_1c __GMP_PROTO ((mp_ptr, mp_srcptr, mp_size_t, mp_limb_t, mp_limb_t));

#define mpn_addsub_n __MPN(addsub_n)
__GMP_DECLSPEC mp_limb_t mpn_addsub_n __GMP_PROTO ((mp_ptr, mp_ptr, mp_srcptr, mp_srcptr, mp_size_t));

#define mpn_addsub_nc __MPN(addsub_nc)
__GMP_DECLSPEC mp_limb_t mpn_addsub_nc __GMP_PROTO ((mp_ptr, mp_ptr, mp_srcptr, mp_srcptr, mp_size_t, mp_limb_t));

#define mpn_divrem_1c __MPN(divrem_1c)
__GMP_DECLSPEC mp_limb_t mpn_divrem_1c __GMP_PROTO ((mp_ptr, mp_size_t, mp_srcptr, mp_size_t, mp_limb_t, mp_limb_t));

#define mpn_dump __MPN(dump)
__GMP_DECLSPEC void mpn_dump __GMP_PROTO ((mp_srcptr, mp_size_t));

#define mpn_fib2_ui __MPN(fib2_ui)
mp_size_t mpn_fib2_ui _PROTO ((mp_ptr, mp_ptr, unsigned long));

/* Remap names of internal mpn functions.  */
#define __clz_tab               __MPN(clz_tab)
#define mpn_udiv_w_sdiv		__MPN(udiv_w_sdiv)

#define mpn_gcd_finda	__MPN(gcd_finda)
mp_limb_t mpn_gcd_finda _PROTO((const mp_limb_t cp[2])) __GMP_ATTRIBUTE_PURE;

#define mpn_jacobi_base __MPN(jacobi_base)
int mpn_jacobi_base _PROTO ((mp_limb_t a, mp_limb_t b, int result_bit1)) ATTRIBUTE_CONST;

#define mpn_mod_1c __MPN(mod_1c)
__GMP_DECLSPEC mp_limb_t mpn_mod_1c __GMP_PROTO ((mp_srcptr, mp_size_t, mp_limb_t, mp_limb_t)) __GMP_ATTRIBUTE_PURE;

#define mpn_mul_1c __MPN(mul_1c)
__GMP_DECLSPEC mp_limb_t mpn_mul_1c __GMP_PROTO ((mp_ptr, mp_srcptr, mp_size_t, mp_limb_t, mp_limb_t));

#define mpn_mul_2 __MPN(mul_2)
mp_limb_t mpn_mul_2 _PROTO ((mp_ptr, mp_srcptr, mp_size_t, mp_srcptr));

#define mpn_mul_basecase __MPN(mul_basecase)
__GMP_DECLSPEC void mpn_mul_basecase __GMP_PROTO ((mp_ptr, mp_srcptr, mp_size_t, mp_srcptr, mp_size_t));

#define mpn_sqr_n __MPN(sqr_n)
__GMP_DECLSPEC void mpn_sqr_n __GMP_PROTO ((mp_ptr, mp_srcptr, mp_size_t));

#define mpn_sqr_basecase __MPN(sqr_basecase)
__GMP_DECLSPEC void mpn_sqr_basecase __GMP_PROTO ((mp_ptr, mp_srcptr, mp_size_t));

#define mpn_sub_nc __MPN(sub_nc)
__GMP_DECLSPEC mp_limb_t mpn_sub_nc __GMP_PROTO ((mp_ptr, mp_srcptr, mp_srcptr, mp_size_t, mp_limb_t));

#define mpn_submul_1c __MPN(submul_1c)
__GMP_DECLSPEC mp_limb_t mpn_submul_1c __GMP_PROTO ((mp_ptr, mp_srcptr, mp_size_t, mp_limb_t, mp_limb_t));


typedef __gmp_randstate_struct *gmp_randstate_ptr;

#define _gmp_rand __gmp_rand
__GMP_DECLSPEC void _gmp_rand _PROTO ((mp_ptr, gmp_randstate_t, unsigned long int));


/* __gmp_rands is the global state for the old-style random functions, and
   is also used in the test programs (hence the __GMP_DECLSPEC).

   There's no seeding here, so mpz_random etc will generate the same
   sequence every time.  This is not unlike the C library random functions
   if you don't seed them, so perhaps it's acceptable.  Digging up a seed
   from /dev/random or the like would work on many systems, but might
   encourage a false confidence, since it'd be pretty much impossible to do
   something that would work reliably everywhere.  In any case the new style
   functions are recommended to applications which care about randomness, so
   the old functions aren't too important.  */

__GMP_DECLSPEC extern char             __gmp_rands_initialized;
__GMP_DECLSPEC extern gmp_randstate_t  __gmp_rands;

#define RANDS                                   \
  ((__gmp_rands_initialized ? 0                 \
    : (__gmp_rands_initialized = 1,             \
       gmp_randinit_default (__gmp_rands), 0)), \
   __gmp_rands)

/* this is used by the test programs, to free memory */
#define RANDS_CLEAR()                   \
  do {                                  \
    if (__gmp_rands_initialized)        \
      {                                 \
        __gmp_rands_initialized = 0;    \
        gmp_randclear (__gmp_rands);    \
      }                                 \
  } while (0)


/* kara uses n+1 limbs of temporary space and then recurses with the
   balance, so need (n+1) + (ceil(n/2)+1) + (ceil(n/4)+1) + ... */
#define MPN_KARA_MUL_N_TSIZE(n)   (2*((n)+BITS_PER_MP_LIMB))
#define MPN_KARA_SQR_N_TSIZE(n)   (2*((n)+BITS_PER_MP_LIMB))

/* toom3 uses 4*(ceil(n/3)) of temporary space and then recurses with the
   balance either into itself or kara.  The following might be
   overestimates. */
#define MPN_TOOM3_MUL_N_TSIZE(n)  (2*(n) + 3*BITS_PER_MP_LIMB)
#define MPN_TOOM3_SQR_N_TSIZE(n)  (2*(n) + 3*BITS_PER_MP_LIMB)

/* need 2 so that n2>=1 */
#define MPN_KARA_MUL_N_MINSIZE    2
#define MPN_KARA_SQR_N_MINSIZE    2

/* Need l>=1, ls>=1, and 2*ls > l (the latter for the tD MPN_INCR_U) */
#define MPN_TOOM3_MUL_N_MINSIZE   11
#define MPN_TOOM3_SQR_N_MINSIZE   11

#define mpn_sqr_diagonal __MPN(sqr_diagonal)
void mpn_sqr_diagonal _PROTO ((mp_ptr, mp_srcptr, mp_size_t));

#define mpn_kara_mul_n	__MPN(kara_mul_n)
void mpn_kara_mul_n _PROTO((mp_ptr, mp_srcptr, mp_srcptr, mp_size_t, mp_ptr));

#define mpn_kara_sqr_n  __MPN(kara_sqr_n)
void mpn_kara_sqr_n _PROTO ((mp_ptr, mp_srcptr, mp_size_t, mp_ptr));

#define mpn_toom3_mul_n  __MPN(toom3_mul_n)
void mpn_toom3_mul_n _PROTO ((mp_ptr, mp_srcptr, mp_srcptr, mp_size_t,mp_ptr));

#define mpn_toom3_sqr_n  __MPN(toom3_sqr_n)
void mpn_toom3_sqr_n _PROTO((mp_ptr, mp_srcptr, mp_size_t, mp_ptr));


#define mpn_fft_best_k __MPN(fft_best_k)
int     mpn_fft_best_k _PROTO ((mp_size_t n, int sqr)) ATTRIBUTE_CONST;

#define mpn_mul_fft  __MPN(mul_fft)
void mpn_mul_fft _PROTO ((mp_ptr op, mp_size_t pl,
                          mp_srcptr n, mp_size_t nl,
                          mp_srcptr m, mp_size_t ml,
                          int k));

#define mpn_mul_fft_full  __MPN(mul_fft_full)
void mpn_mul_fft_full _PROTO ((mp_ptr op,
                               mp_srcptr n, mp_size_t nl,
                               mp_srcptr m, mp_size_t ml));

#define   mpn_fft_next_size __MPN(fft_next_size)
mp_size_t mpn_fft_next_size _PROTO ((mp_size_t pl, int k)) ATTRIBUTE_CONST;

#define mpn_sb_divrem_mn  __MPN(sb_divrem_mn)
mp_limb_t mpn_sb_divrem_mn _PROTO ((mp_ptr, mp_ptr, mp_size_t,
                                    mp_srcptr, mp_size_t));

#define mpn_dc_divrem_n  __MPN(dc_divrem_n)
mp_limb_t mpn_dc_divrem_n _PROTO ((mp_ptr, mp_ptr, mp_srcptr, mp_size_t));

/* #define mpn_tdiv_q  __MPN(tdiv_q) */
/* void mpn_tdiv_q _PROTO ((mp_ptr, mp_size_t, mp_srcptr, mp_size_t, mp_srcptr, mp_size_t)); */

#define mpz_divexact_gcd  __gmpz_divexact_gcd
void mpz_divexact_gcd _PROTO ((mpz_ptr q, mpz_srcptr a, mpz_srcptr d));

#define mpz_inp_str_nowhite __gmpz_inp_str_nowhite
#ifdef _GMP_H_HAVE_FILE
size_t mpz_inp_str_nowhite _PROTO ((mpz_ptr x, FILE *stream, int base, int c, size_t nread));
#endif

#define mpn_divisible_p __MPN(divisible_p)
int     mpn_divisible_p _PROTO ((mp_srcptr ap, mp_size_t asize,
                                 mp_srcptr dp, mp_size_t dsize)) __GMP_ATTRIBUTE_PURE;

#define mpn_rootrem __gmpn_rootrem
mp_size_t mpn_rootrem _PROTO ((mp_ptr, mp_ptr, mp_srcptr, mp_size_t, mp_limb_t));


/* from gmp.h */
#if HAVE_HOST_CPU_FAMILY_power || HAVE_HOST_CPU_FAMILY_powerpc
#define MPN_COPY_INCR(dst, src, size)                   \
  do {                                                  \
    ASSERT ((size) >= 0);                               \
    ASSERT (MPN_SAME_OR_INCR_P (dst, src, size));       \
    __GMPN_COPY_INCR (dst, src, size);                  \
  } while (0)
#endif

#if defined (_CRAY)
#define MPN_COPY_INCR(dst, src, n)					\
  do {									\
    int __i;		/* Faster on some Crays with plain int */	\
    _Pragma ("_CRI ivdep");						\
    for (__i = 0; __i < (n); __i++)					\
      (dst)[__i] = (src)[__i];						\
  } while (0)
#endif

#define mpn_copyi __MPN(copyi)
void mpn_copyi _PROTO ((mp_ptr, mp_srcptr, mp_size_t));

#if ! defined (MPN_COPY_INCR) && HAVE_NATIVE_mpn_copyi
#define MPN_COPY_INCR(dst, src, size)                   \
  do {                                                  \
    ASSERT ((size) >= 0);                               \
    ASSERT (MPN_SAME_OR_INCR_P (dst, src, size));       \
    mpn_copyi (dst, src, size);                         \
  } while (0)
#endif

/* Copy N limbs from SRC to DST incrementing, N==0 allowed.  */
#if ! defined (MPN_COPY_INCR)
#define MPN_COPY_INCR(dst, src, n)                      \
  do {                                                  \
    ASSERT ((n) >= 0);                                  \
    ASSERT (MPN_SAME_OR_INCR_P (dst, src, n));          \
    if ((n) != 0)                                       \
      {                                                 \
	mp_size_t __n = (n) - 1;                        \
	mp_ptr __dst = (dst);                           \
	mp_srcptr __src = (src);                        \
	mp_limb_t __x;                                  \
	__x = *__src++;                                 \
	if (__n != 0)                                   \
	  {                                             \
	    do                                          \
	      {                                         \
		*__dst++ = __x;                         \
		__x = *__src++;                         \
	      }                                         \
	    while (--__n);                              \
	  }                                             \
	*__dst++ = __x;                                 \
      }                                                 \
  } while (0)
#endif


/* As per __GMPN_COPY_INCR in gmp.h. */
#if HAVE_HOST_CPU_FAMILY_power || HAVE_HOST_CPU_FAMILY_powerpc
#define MPN_COPY_DECR(dst, src, size)                   \
  do {                                                  \
    ASSERT ((size) >= 0);                               \
    ASSERT (MPN_SAME_OR_DECR_P (dst, src, size));       \
    if ((size) != 0)                                    \
      {                                                 \
        mp_ptr     __dst = (dst) + (size);              \
        mp_srcptr  __src = (src) + (size);              \
        mp_size_t  __size = (size);                     \
        do                                              \
          *--__dst = *--__src;                          \
        while (--__size != 0);                          \
      }                                                 \
  } while (0)
#endif

#if defined (_CRAY)
#define MPN_COPY_DECR(dst, src, n)					\
  do {									\
    int __i;		/* Faster on some Crays with plain int */	\
    _Pragma ("_CRI ivdep");						\
    for (__i = (n) - 1; __i >= 0; __i--)				\
      (dst)[__i] = (src)[__i];						\
  } while (0)
#endif

#define mpn_copyd __MPN(copyd)
void mpn_copyd _PROTO ((mp_ptr, mp_srcptr, mp_size_t));

#if ! defined (MPN_COPY_DECR) && HAVE_NATIVE_mpn_copyd
#define MPN_COPY_DECR(dst, src, size)                   \
  do {                                                  \
    ASSERT ((size) >= 0);                               \
    ASSERT (MPN_SAME_OR_DECR_P (dst, src, size));       \
    mpn_copyd (dst, src, size);                         \
  } while (0)
#endif

/* Copy N limbs from SRC to DST decrementing, N==0 allowed.  */
#if ! defined (MPN_COPY_DECR)
#define MPN_COPY_DECR(dst, src, n)                      \
  do {                                                  \
    ASSERT ((n) >= 0);                                  \
    ASSERT (MPN_SAME_OR_DECR_P (dst, src, n));          \
    if ((n) != 0)                                       \
      {                                                 \
	mp_size_t __n = (n) - 1;                        \
	mp_ptr __dst = (dst) + __n;                     \
	mp_srcptr __src = (src) + __n;                  \
	mp_limb_t __x;                                  \
	__x = *__src--;                                 \
	if (__n != 0)                                   \
	  {                                             \
	    do                                          \
	      {                                         \
		*__dst-- = __x;                         \
		__x = *__src--;                         \
	      }                                         \
	    while (--__n);                              \
	  }                                             \
	*__dst-- = __x;                                 \
      }                                                 \
  } while (0)
#endif


#ifndef MPN_COPY
#define MPN_COPY(d,s,n)                         \
  do {                                          \
    ASSERT (MPN_SAME_OR_SEPARATE_P (d, s, n));  \
    MPN_COPY_INCR (d, s, n);                    \
  } while (0)
#endif


/* Set {dst,size} to the limbs of {src,size} in reverse order. */
#define MPN_REVERSE(dst, src, size)                     \
  do {                                                  \
    mp_ptr     __dst = (dst);                           \
    mp_size_t  __size = (size);                         \
    mp_srcptr  __src = (src) + __size - 1;              \
    mp_size_t  __i;                                     \
    ASSERT ((size) >= 0);                               \
    ASSERT (! MPN_OVERLAP_P (dst, size, src, size));    \
    CRAY_Pragma ("_CRI ivdep");                         \
    for (__i = 0; __i < __size; __i++)                  \
      {                                                 \
        *__dst = *__src;                                \
        __dst++;                                        \
        __src--;                                        \
      }                                                 \
  } while (0)


/* Zero n limbs at dst.

   For power and powerpc we want an inline stu/bdnz loop for zeroing.  On
   ppc630 for instance this is optimal since it can sustain only 1 store per
   cycle.

   gcc 2.95.x (for powerpc64 -maix64, or powerpc32) doesn't recognise the
   "for" loop in the generic code below can become stu/bdnz.  The do/while
   here helps it get to that.  The same caveat about plain -mpowerpc64 mode
   applies here as to __GMPN_COPY_INCR in gmp.h.

   xlc 3.1 already generates stu/bdnz from the generic C, and does so from
   this loop too.

   Enhancement: GLIBC does some trickery with dcbz to zero whole cache lines
   at a time.  MPN_ZERO isn't all that important in GMP, so it might be more
   trouble than it's worth to do the same, though perhaps a call to memset
   would be good when on a GNU system.  */

#if HAVE_HOST_CPU_FAMILY_power || HAVE_HOST_CPU_FAMILY_powerpc
#define MPN_ZERO(dst, n)			\
  do {						\
    ASSERT ((n) >= 0);				\
    if ((n) != 0)				\
      {						\
	mp_ptr __dst = (dst) - 1;		\
	mp_size_t __n = (n);			\
	do					\
	  *++__dst = 0;				\
	while (--__n);				\
      }						\
  } while (0)
#endif

#ifndef MPN_ZERO
#define MPN_ZERO(dst, n)			\
  do {						\
    ASSERT ((n) >= 0);				\
    if ((n) != 0)				\
      {						\
	mp_ptr __dst = (dst);			\
	mp_size_t __n = (n);			\
	do					\
	  *__dst++ = 0;				\
	while (--__n);				\
      }						\
  } while (0)
#endif


/* On the x86s repe/scasl doesn't seem useful, since it takes many cycles to
   start up and would need to strip a lot of zeros before it'd be faster
   than a simple cmpl loop.  Here are some times in cycles for
   std/repe/scasl/cld and cld/repe/scasl (the latter would be for stripping
   low zeros).

                std   cld
           P5    18    16
           P6    46    38
           K6    36    13
           K7    21    20
*/
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
#define MPN_NORMALIZE_NOT_ZERO(DST, NLIMBS)     \
  do {                                          \
    ASSERT ((NLIMBS) >= 1);                     \
    while (1)                                   \
      {                                         \
	if ((DST)[(NLIMBS) - 1] != 0)           \
	  break;                                \
	NLIMBS--;                               \
      }                                         \
  } while (0)
#endif

/* Strip least significant zero limbs from {ptr,size} by incrementing ptr
   and decrementing size.  low should be ptr[0], and will be the new ptr[0]
   on returning.  The number in {ptr,size} must be non-zero, ie. size!=0 and
   somewhere a non-zero limb.  */
#define MPN_STRIP_LOW_ZEROS_NOT_ZERO(ptr, size, low)    \
  do {                                                  \
    ASSERT ((size) >= 1);                               \
    ASSERT ((low) == (ptr)[0]);                         \
                                                        \
    while ((low) == 0)                                  \
      {                                                 \
        (size)--;                                       \
        ASSERT ((size) >= 1);                           \
        (ptr)++;                                        \
        (low) = *(ptr);                                 \
      }                                                 \
  } while (0)

/* Initialize X of type mpz_t with space for NLIMBS limbs.  X should be a
   temporary variable; it will be automatically cleared out at function
   return.  We use __x here to make it possible to accept both mpz_ptr and
   mpz_t arguments.  */
#define MPZ_TMP_INIT(X, NLIMBS)                                         \
  do {                                                                  \
    mpz_ptr __x = (X);                                                  \
    ASSERT ((NLIMBS) >= 1);                                             \
    __x->_mp_alloc = (NLIMBS);                                          \
    __x->_mp_d = (mp_ptr) TMP_ALLOC ((NLIMBS) * BYTES_PER_MP_LIMB);     \
  } while (0)

/* Realloc for an mpz_t WHAT if it has less than NEEDED limbs.  */
#define MPZ_REALLOC(z,n) ((n) > ALLOC(z) ? _mpz_realloc(z,n) : PTR(z))

#define MPZ_EQUAL_1_P(z)  (SIZ(z)==1 && PTR(z)[0] == 1)


/* MPN_FIB2_SIZE(n) is the size in limbs required by mpn_fib2_ui for fp and
   f1p.

   From Knuth vol 1 section 1.2.8, F[n] = phi^n/sqrt(5) rounded to the
   nearest integer, where phi=(1+sqrt(5))/2 is the golden ratio.  So the
   number of bits required is n*log_2((1+sqrt(5))/2) = n*0.6942419.

   The multiplier used is 23/32=0.71875 for efficient calculation on CPUs
   without good floating point.  There's +2 for rounding up, and a further
   +2 since at the last step x limbs are doubled into a 2x+1 limb region
   whereas the actual F[2k] value might be only 2x-1 limbs.

   Note that a division is done first, since on a 32-bit system it's at
   least conceivable to go right up to n==ULONG_MAX.  (F[2^32-1] would be
   about 380Mbytes, plus temporary workspace of about 1.2Gbytes here and
   whatever a multiply of two 190Mbyte numbers takes.)

   Enhancement: When GMP_NUMB_BITS is not a power of 2 the division could be
   worked into the multiplier.  */

#define MPN_FIB2_SIZE(n) \
  ((mp_size_t) ((n) / 32 * 23 / GMP_NUMB_BITS) + 4)


/* FIB_TABLE(n) returns the Fibonacci number F[n].  Must have n in the range
   -1 <= n <= FIB_TABLE_LIMIT (that constant in fib_table.h).

   FIB_TABLE_LUCNUM_LIMIT (in fib_table.h) is the largest n for which L[n] =
   F[n] + 2*F[n-1] fits in a limb.  */

__GMP_DECLSPEC extern const mp_limb_t __gmp_fib_table[];
#define FIB_TABLE(n)  (__gmp_fib_table[(n)+1])


/* For a threshold between algorithms A and B, size>=thresh is where B
   should be used.  Special value MP_SIZE_T_MAX means only ever use A, or
   value 0 means only ever use B.  The tests for these special values will
   be compile-time constants, so the compiler should be able to eliminate
   the code for the unwanted algorithm.  */

#define ABOVE_THRESHOLD(size,thresh)    \
  ((thresh) == 0                        \
   || ((thresh) != MP_SIZE_T_MAX        \
       && (size) >= (thresh)))
#define BELOW_THRESHOLD(size,thresh)  (! ABOVE_THRESHOLD (size, thresh))


/* If MUL_KARATSUBA_THRESHOLD is not already defined, define it to a
   value which is good on most machines.  */
#ifndef MUL_KARATSUBA_THRESHOLD
#define MUL_KARATSUBA_THRESHOLD 32
#endif

/* If MUL_TOOM3_THRESHOLD is not already defined, define it to a
   value which is good on most machines.  */
#ifndef MUL_TOOM3_THRESHOLD
#define MUL_TOOM3_THRESHOLD 256
#endif

/* SQR_BASECASE_THRESHOLD is where mpn_sqr_basecase should take over from
   mpn_mul_basecase in mpn_sqr_n.  Default is to use mpn_sqr_basecase
   always.  (Note that we certainly always want it if there's a native
   assembler mpn_sqr_basecase.)

   If it turns out that mpn_kara_sqr_n becomes faster than mpn_mul_basecase
   before mpn_sqr_basecase does, then SQR_BASECASE_THRESHOLD is the
   karatsuba threshold and SQR_KARATSUBA_THRESHOLD is 0.  This oddity arises
   more or less because SQR_KARATSUBA_THRESHOLD represents the size up to
   which mpn_sqr_basecase should be used, and that may be never.  */

#ifndef SQR_BASECASE_THRESHOLD
#define SQR_BASECASE_THRESHOLD 0
#endif

#ifndef SQR_KARATSUBA_THRESHOLD
#define SQR_KARATSUBA_THRESHOLD (2*MUL_KARATSUBA_THRESHOLD)
#endif

#ifndef SQR_TOOM3_THRESHOLD
#define SQR_TOOM3_THRESHOLD (2*MUL_TOOM3_THRESHOLD)
#endif

/* First k to use for an FFT modF multiply.  A modF FFT is an order
   log(2^k)/log(2^(k-1)) algorithm, so k=3 is merely 1.5 like karatsuba,
   whereas k=4 is 1.33 which is faster than toom3 at 1.485.    */
#define FFT_FIRST_K  4

/* Threshold at which FFT should be used to do a modF NxN -> N multiply. */
#ifndef MUL_FFT_MODF_THRESHOLD
#define MUL_FFT_MODF_THRESHOLD   (MUL_TOOM3_THRESHOLD * 3)
#endif
#ifndef SQR_FFT_MODF_THRESHOLD
#define SQR_FFT_MODF_THRESHOLD   (SQR_TOOM3_THRESHOLD * 3)
#endif

/* Threshold at which FFT should be used to do an NxN -> 2N multiply.  This
   will be a size where FFT is using k=7 or k=8, since an FFT-k used for an
   NxN->2N multiply and not recursing into itself is an order
   log(2^k)/log(2^(k-2)) algorithm, so it'll be at least k=7 at 1.39 which
   is the first better than toom3.  */
#ifndef MUL_FFT_THRESHOLD
#define MUL_FFT_THRESHOLD   (MUL_FFT_MODF_THRESHOLD * 10)
#endif
#ifndef SQR_FFT_THRESHOLD
#define SQR_FFT_THRESHOLD   (SQR_FFT_MODF_THRESHOLD * 10)
#endif

/* Table of thresholds for successive modF FFT "k"s.  The first entry is
   where FFT_FIRST_K+1 should be used, the second FFT_FIRST_K+2,
   etc.  See mpn_fft_best_k(). */
#ifndef MUL_FFT_TABLE
#define MUL_FFT_TABLE                           \
  { MUL_TOOM3_THRESHOLD * 4,   /* k=5 */        \
    MUL_TOOM3_THRESHOLD * 8,   /* k=6 */        \
    MUL_TOOM3_THRESHOLD * 16,  /* k=7 */        \
    MUL_TOOM3_THRESHOLD * 32,  /* k=8 */        \
    MUL_TOOM3_THRESHOLD * 96,  /* k=9 */        \
    MUL_TOOM3_THRESHOLD * 288, /* k=10 */       \
    0 }
#endif
#ifndef SQR_FFT_TABLE
#define SQR_FFT_TABLE                           \
  { SQR_TOOM3_THRESHOLD * 4,   /* k=5 */        \
    SQR_TOOM3_THRESHOLD * 8,   /* k=6 */        \
    SQR_TOOM3_THRESHOLD * 16,  /* k=7 */        \
    SQR_TOOM3_THRESHOLD * 32,  /* k=8 */        \
    SQR_TOOM3_THRESHOLD * 96,  /* k=9 */        \
    SQR_TOOM3_THRESHOLD * 288, /* k=10 */       \
    0 }
#endif

#ifndef FFT_TABLE_ATTRS
#define FFT_TABLE_ATTRS   static const
#endif

#define MPN_FFT_TABLE_SIZE  16


/* mpn_dc_divrem_n(n) calls 2*mul(n/2)+2*div(n/2), thus to be faster than
   div(n) = 4*div(n/2), we need mul(n/2) to be faster than the classic way,
   i.e. n/2 >= MUL_KARATSUBA_THRESHOLD

   Measured values are between 2 and 4 times MUL_KARATSUBA_THRESHOLD, so go
   for 3 as an average.  */

#ifndef DIV_DC_THRESHOLD
#define DIV_DC_THRESHOLD    (3 * MUL_KARATSUBA_THRESHOLD)
#endif


/* Return non-zero if xp,xsize and yp,ysize overlap.
   If xp+xsize<=yp there's no overlap, or if yp+ysize<=xp there's no
   overlap.  If both these are false, there's an overlap. */
#define MPN_OVERLAP_P(xp, xsize, yp, ysize) \
  ((xp) + (xsize) > (yp) && (yp) + (ysize) > (xp))
#define MEM_OVERLAP_P(xp, xsize, yp, ysize)     \
  (   (char *) (xp) + (xsize) > (char *) (yp)   \
   && (char *) (yp) + (ysize) > (char *) (xp))

/* Return non-zero if xp,xsize and yp,ysize are either identical or not
   overlapping.  Return zero if they're partially overlapping. */
#define MPN_SAME_OR_SEPARATE_P(xp, yp, size)    \
  MPN_SAME_OR_SEPARATE2_P(xp, size, yp, size)
#define MPN_SAME_OR_SEPARATE2_P(xp, xsize, yp, ysize)           \
  ((xp) == (yp) || ! MPN_OVERLAP_P (xp, xsize, yp, ysize))

/* Return non-zero if dst,dsize and src,ssize are either identical or
   overlapping in a way suitable for an incrementing/decrementing algorithm.
   Return zero if they're partially overlapping in an unsuitable fashion. */
#define MPN_SAME_OR_INCR2_P(dst, dsize, src, ssize)             \
  ((dst) <= (src) || ! MPN_OVERLAP_P (dst, dsize, src, ssize))
#define MPN_SAME_OR_INCR_P(dst, src, size)      \
  MPN_SAME_OR_INCR2_P(dst, size, src, size)
#define MPN_SAME_OR_DECR2_P(dst, dsize, src, ssize)             \
  ((dst) >= (src) || ! MPN_OVERLAP_P (dst, dsize, src, ssize))
#define MPN_SAME_OR_DECR_P(dst, src, size)      \
  MPN_SAME_OR_DECR2_P(dst, size, src, size)


/* ASSERT() is a private assertion checking scheme, similar to <assert.h>.
   ASSERT() does the check only if WANT_ASSERT is selected, ASSERT_ALWAYS()
   does it always.  Generally assertions are meant for development, but
   might help when looking for a problem later too.

   Note that strings shouldn't be used within the ASSERT expression,
   eg. ASSERT(strcmp(s,"notgood")!=0), since the quotes upset the "expr"
   used in the !HAVE_STRINGIZE case (ie. K&R).  */

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

void __gmp_assert_header _PROTO ((const char *filename, int linenum));
__GMP_DECLSPEC void __gmp_assert_fail _PROTO ((const char *filename, int linenum, const char *expr)) ATTRIBUTE_NORETURN;

#if HAVE_STRINGIZE
#define ASSERT_FAIL(expr)  __gmp_assert_fail (ASSERT_FILE, ASSERT_LINE, #expr)
#else
#define ASSERT_FAIL(expr)  __gmp_assert_fail (ASSERT_FILE, ASSERT_LINE, "expr")
#endif

#define ASSERT_ALWAYS(expr)     \
  do {                          \
    if (!(expr))                \
      ASSERT_FAIL (expr);       \
  } while (0)

#if WANT_ASSERT
#define ASSERT(expr)   ASSERT_ALWAYS (expr)
#else
#define ASSERT(expr)   do {} while (0)
#endif


/* ASSERT_CARRY checks the expression is non-zero, and ASSERT_NOCARRY checks
   that it's zero.  In both cases if assertion checking is disabled the
   expression is still evaluated.  These macros are meant for use with
   routines like mpn_add_n() where the return value represents a carry or
   whatever that should or shouldn't occur in some context.  For example,
   ASSERT_NOCARRY (mpn_add_n (rp, s1p, s2p, size)); */
#if WANT_ASSERT
#define ASSERT_CARRY(expr)     ASSERT_ALWAYS ((expr) != 0)
#define ASSERT_NOCARRY(expr)   ASSERT_ALWAYS ((expr) == 0)
#else
#define ASSERT_CARRY(expr)     (expr)
#define ASSERT_NOCARRY(expr)   (expr)
#endif


/* ASSERT_CODE includes code when assertion checking is wanted.  This is the
   same as writing "#if WANT_ASSERT", but more compact.  */
#if WANT_ASSERT
#define ASSERT_CODE(expr)  expr
#else
#define ASSERT_CODE(expr)
#endif


/* Test that an mpq_t is in fully canonical form.  This can be used as
   protection on routines like mpq_equal which give wrong results on
   non-canonical inputs.  */
#if WANT_ASSERT
#define ASSERT_MPQ_CANONICAL(q)                         \
  do {                                                  \
    ASSERT (q->_mp_den._mp_size > 0);                   \
    if (q->_mp_num._mp_size == 0)                       \
      {                                                 \
        /* zero should be 0/1 */                        \
        ASSERT (mpz_cmp_ui (mpq_denref(q), 1L) == 0);   \
      }                                                 \
    else                                                \
      {                                                 \
        /* no common factors */                         \
        mpz_t  __g;                                     \
        mpz_init (__g);                                 \
        mpz_gcd (__g, mpq_numref(q), mpq_denref(q));    \
        ASSERT (mpz_cmp_ui (__g, 1) == 0);              \
        mpz_clear (__g);                                \
      }                                                 \
  } while (0)
#else
#define ASSERT_MPQ_CANONICAL(q)  do {} while (0)
#endif

/* Check that the nail parts are zero. */
#define ASSERT_ALWAYS_LIMB(limb)                \
  do {                                          \
    mp_limb_t  __nail = (limb) & GMP_NAIL_MASK; \
    ASSERT_ALWAYS (__nail == 0);                \
  } while (0)
#define ASSERT_ALWAYS_MPN(ptr, size)            \
  do {                                          \
    /* let whole loop go dead when no nails */  \
    if (GMP_NAIL_BITS != 0)                     \
      {                                         \
        mp_size_t  __i;                         \
        for (__i = 0; __i < (size); __i++)      \
          ASSERT_ALWAYS_LIMB ((ptr)[__i]);      \
      }                                         \
  } while (0)
#if WANT_ASSERT
#define ASSERT_LIMB(limb)       ASSERT_ALWAYS_LIMB (limb)
#define ASSERT_MPN(ptr, size)   ASSERT_ALWAYS_MPN (ptr, size)
#else
#define ASSERT_LIMB(limb)       do {} while (0)
#define ASSERT_MPN(ptr, size)   do {} while (0)
#endif


/* Assert that an mpn region {ptr,size} is zero, or non-zero.
   size==0 is allowed, and in that case {ptr,size} considered to be zero.  */
#if WANT_ASSERT
#define ASSERT_MPN_ZERO_P(ptr,size)     \
  do {                                  \
    mp_size_t  __i;                     \
    ASSERT ((size) >= 0);               \
    for (__i = 0; __i < (size); __i++)  \
      ASSERT ((ptr)[__i] == 0);         \
  } while (0)
#define ASSERT_MPN_NONZERO_P(ptr,size)  \
  do {                                  \
    mp_size_t  __i;                     \
    int        __nonzero = 0;           \
    ASSERT ((size) >= 0);               \
    for (__i = 0; __i < (size); __i++)  \
      if ((ptr)[__i] != 0)              \
        {                               \
          __nonzero = 1;                \
          break;                        \
        }                               \
    ASSERT (__nonzero);                 \
  } while (0)
#else
#define ASSERT_MPN_ZERO_P(ptr,size)     do {} while (0)
#define ASSERT_MPN_NONZERO_P(ptr,size)  do {} while (0)
#endif


#if HAVE_NATIVE_mpn_com_n
#define mpn_com_n __MPN(com_n)
void mpn_com_n _PROTO ((mp_ptr, mp_srcptr, mp_size_t));
#else
#define mpn_com_n(d,s,n)                                \
  do {                                                  \
    mp_ptr     __d = (d);                               \
    mp_srcptr  __s = (s);                               \
    mp_size_t  __n = (n);                               \
    ASSERT (__n >= 1);                                  \
    ASSERT (MPN_SAME_OR_SEPARATE_P (__d, __s, __n));    \
    do                                                  \
      *__d++ = (~ *__s++) & GMP_NUMB_MASK;              \
    while (--__n);                                      \
  } while (0)
#endif

#define MPN_LOGOPS_N_INLINE(d, s1, s2, n, operation)    \
  do {                                                  \
    mp_ptr	 __d = (d);                             \
    mp_srcptr	 __s1 = (s1);                           \
    mp_srcptr	 __s2 = (s2);                           \
    mp_size_t	 __n = (n);                             \
    ASSERT (__n >= 1);                                  \
    ASSERT (MPN_SAME_OR_SEPARATE_P (__d, __s1, __n));   \
    ASSERT (MPN_SAME_OR_SEPARATE_P (__d, __s2, __n));   \
    do                                                  \
      operation;                                        \
    while (--__n);                                      \
  } while (0)

#if HAVE_NATIVE_mpn_and_n
#define mpn_and_n __MPN(and_n)
void mpn_and_n _PROTO ((mp_ptr, mp_srcptr, mp_srcptr, mp_size_t));
#else
#define mpn_and_n(d, s1, s2, n) \
  MPN_LOGOPS_N_INLINE (d, s1, s2, n, *__d++ = *__s1++ & *__s2++)
#endif

#if HAVE_NATIVE_mpn_andn_n
#define mpn_andn_n __MPN(andn_n)
void mpn_andn_n _PROTO ((mp_ptr, mp_srcptr, mp_srcptr, mp_size_t));
#else
#define mpn_andn_n(d, s1, s2, n) \
  MPN_LOGOPS_N_INLINE (d, s1, s2, n, *__d++ = *__s1++ & ~*__s2++)
#endif

#if HAVE_NATIVE_mpn_nand_n
#define mpn_nand_n __MPN(nand_n)
void mpn_nand_n _PROTO ((mp_ptr, mp_srcptr, mp_srcptr, mp_size_t));
#else
#define mpn_nand_n(d, s1, s2, n) \
  MPN_LOGOPS_N_INLINE (d, s1, s2, n, *__d++ = ~ (*__s1++ & *__s2++))
#endif

#if HAVE_NATIVE_mpn_ior_n
#define mpn_ior_n __MPN(ior_n)
void mpn_ior_n _PROTO ((mp_ptr, mp_srcptr, mp_srcptr, mp_size_t));
#else
#define mpn_ior_n(d, s1, s2, n) \
  MPN_LOGOPS_N_INLINE (d, s1, s2, n, *__d++ = *__s1++ | *__s2++)
#endif

#if HAVE_NATIVE_mpn_iorn_n
#define mpn_iorn_n __MPN(iorn_n)
void mpn_iorn_n _PROTO ((mp_ptr, mp_srcptr, mp_srcptr, mp_size_t));
#else
#define mpn_iorn_n(d, s1, s2, n) \
  MPN_LOGOPS_N_INLINE (d, s1, s2, n, *__d++ = *__s1++ | ~*__s2++)
#endif

#if HAVE_NATIVE_mpn_nior_n
#define mpn_nior_n __MPN(nior_n)
void mpn_nior_n _PROTO ((mp_ptr, mp_srcptr, mp_srcptr, mp_size_t));
#else
#define mpn_nior_n(d, s1, s2, n) \
  MPN_LOGOPS_N_INLINE (d, s1, s2, n, *__d++ = ~ (*__s1++ | *__s2++))
#endif

#if HAVE_NATIVE_mpn_xor_n
#define mpn_xor_n __MPN(xor_n)
void mpn_xor_n _PROTO ((mp_ptr, mp_srcptr, mp_srcptr, mp_size_t));
#else
#define mpn_xor_n(d, s1, s2, n) \
  MPN_LOGOPS_N_INLINE (d, s1, s2, n, *__d++ = *__s1++ ^ *__s2++)
#endif

#if HAVE_NATIVE_mpn_xnor_n
#define mpn_xnor_n __MPN(xnor_n)
void mpn_xnor_n _PROTO ((mp_ptr, mp_srcptr, mp_srcptr, mp_size_t));
#else
#define mpn_xnor_n(d, s1, s2, n) \
  MPN_LOGOPS_N_INLINE (d, s1, s2, n, *__d++ = ~ (*__s1++ ^ *__s2++))
#endif


/* ADDC_LIMB sets w=x+y and cout to 0 or 1 for a carry from that addition. */
#if GMP_NAIL_BITS == 0
#define ADDC_LIMB(cout, w, x, y)        \
  do {                                  \
    mp_limb_t  __x = (x);               \
    mp_limb_t  __y = (y);               \
    mp_limb_t  __w = __x + __y;         \
    (w) = __w;                          \
    (cout) = __w < __x;                 \
  } while (0)
#else
#define ADDC_LIMB(cout, w, x, y)        \
  do {                                  \
    mp_limb_t  __w;                     \
    ASSERT_LIMB (x);                    \
    ASSERT_LIMB (y);                    \
    __w = (x) + (y);                    \
    (w) = __w & GMP_NUMB_MASK;          \
    (cout) = __w >> GMP_NUMB_BITS;      \
  } while (0)
#endif

/* SUBC_LIMB sets w=x-y and cout to 0 or 1 for a borrow from that
   subtract.  */
#if GMP_NAIL_BITS == 0
#define SUBC_LIMB(cout, w, x, y)        \
  do {                                  \
    mp_limb_t  __x = (x);               \
    mp_limb_t  __y = (y);               \
    mp_limb_t  __w = __x - __y;         \
    (w) = __w;                          \
    (cout) = __w > __x;                 \
  } while (0)
#else
#define SUBC_LIMB(cout, w, x, y)        \
  do {                                  \
    mp_limb_t  __w = (x) - (y);         \
    (w) = __w & GMP_NUMB_MASK;          \
    (cout) = __w >> (GMP_LIMB_BITS-1);  \
  } while (0)
#endif


/* MPN_INCR_U does {ptr,size} += n, MPN_DECR_U does {ptr,size} -= n, both
   expecting no carry (or borrow) from that.

   The size parameter is only for the benefit of assertion checking.  In a
   normal build it's unused and the carry/borrow is just propagated as far
   as it needs to go.

   On random data, usually only one or two limbs of {ptr,size} get updated,
   so there's no need for any sophisticated looping, just something compact
   and sensible.

   FIXME: Do the generic MPN_{INCR,DECR}_U with a block of code like
   mpn_incr_u but with the assertions built in, rather than the separate
   add_1 and sub_1 when assertion checking.

   FIXME: Switch all code from mpn_{incr,decr}_u to MPN_{INCR,DECR}_U,
   declaring their operand sizes, then remove the former.  This is purely
   for the benefit of assertion checking.  */

#if defined (__GNUC__) && HAVE_HOST_CPU_FAMILY_x86 && GMP_NAIL_BITS == 0      \
  && BITS_PER_MP_LIMB == 32 && ! defined (NO_ASM) && ! WANT_ASSERT
/* Better flags handling than the generic C gives on i386, saving a few
   bytes of code and maybe a cycle or two.  */

#define MPN_IORD_U(ptr, incr, aors)                     \
  do {                                                  \
    mp_ptr  __ptr_dummy;                                \
    if (__builtin_constant_p (incr) && (incr) == 1)     \
      {                                                 \
        __asm__ __volatile__                            \
          ("\n" ASM_L(top) ":\n"                        \
           "\t" aors " $1, (%0)\n"                      \
           "\tleal 4(%0),%0\n"                          \
           "\tjc " ASM_L(top)                           \
           : "=r" (__ptr_dummy)                         \
           : "0"  (ptr)                                 \
           : "memory");                                 \
      }                                                 \
    else                                                \
      {                                                 \
        __asm__ __volatile__                            \
          (   aors  " %2,(%0)\n"                        \
           "\tjnc " ASM_L(done) "\n"                    \
           ASM_L(top) ":\n"                             \
           "\t" aors " $1,4(%0)\n"                      \
           "\tleal 4(%0),%0\n"                          \
           "\tjc " ASM_L(top) "\n"                      \
           ASM_L(done) ":\n"                            \
           : "=r" (__ptr_dummy)                         \
           : "0"  (ptr),                                \
             "ri" (incr)                                \
           : "memory");                                 \
      }                                                 \
  } while (0)

#define MPN_INCR_U(ptr, size, incr)  MPN_IORD_U (ptr, incr, "addl")
#define MPN_DECR_U(ptr, size, incr)  MPN_IORD_U (ptr, incr, "subl")
#define mpn_incr_u(ptr, incr)  MPN_INCR_U (ptr, 0, incr)
#define mpn_decr_u(ptr, incr)  MPN_DECR_U (ptr, 0, incr)
#endif

#if GMP_NAIL_BITS == 0
#ifndef mpn_incr_u
#define mpn_incr_u(p,incr)                              \
  do {                                                  \
    mp_limb_t __x;                                      \
    mp_ptr __p = (p);                                   \
    if (__builtin_constant_p (incr) && (incr) == 1)     \
      {                                                 \
        while (++(*(__p++)) == 0)                       \
          ;                                             \
      }                                                 \
    else                                                \
      {                                                 \
        __x = *__p + (incr);                            \
        *__p = __x;                                     \
        if (__x < (incr))                               \
          while (++(*(++__p)) == 0)                     \
            ;                                           \
      }                                                 \
  } while (0)
#endif
#ifndef mpn_decr_u
#define mpn_decr_u(p,incr)                              \
  do {                                                  \
    mp_limb_t __x;                                      \
    mp_ptr __p = (p);                                   \
    if (__builtin_constant_p (incr) && (incr) == 1)     \
      {                                                 \
        while ((*(__p++))-- == 0)                       \
          ;                                             \
      }                                                 \
    else                                                \
      {                                                 \
        __x = *__p;                                     \
        *__p = __x - (incr);                            \
        if (__x < (incr))                               \
          while ((*(++__p))-- == 0)                     \
            ;                                           \
      }                                                 \
  } while (0)
#endif
#endif

#if GMP_NAIL_BITS >= 1
#ifndef mpn_incr_u
#define mpn_incr_u(p,incr)                              \
  do {							\
    mp_limb_t __x;					\
    mp_ptr __p = (p);					\
    if (__builtin_constant_p (incr) && (incr) == 1)	\
      {							\
	do						\
	  {						\
	    __x = (*__p + 1) & GMP_NUMB_MASK;		\
	    *__p++ = __x;				\
	  }						\
	while (__x == 0);				\
      }							\
    else						\
      {							\
	__x = (*__p + (incr));				\
	*__p++ = __x & GMP_NUMB_MASK;			\
	if (__x >> GMP_NUMB_BITS != 0)			\
	  {						\
	    do						\
	      {						\
		__x = (*__p + 1) & GMP_NUMB_MASK;	\
		*__p++ = __x;				\
	      }						\
	    while (__x == 0);				\
	  }						\
      }							\
  } while (0)
#endif
#ifndef mpn_decr_u
#define mpn_decr_u(p,incr)				\
  do {							\
    mp_limb_t __x;					\
    mp_ptr __p = (p);					\
    if (__builtin_constant_p (incr) && (incr) == 1)	\
      {							\
	do						\
	  {						\
	    __x = *__p;					\
	    *__p++ = (__x - 1) & GMP_NUMB_MASK;		\
	  }						\
	while (__x == 0);				\
      }							\
    else						\
      {							\
	__x = *__p - (incr);				\
	*__p++ = __x & GMP_NUMB_MASK;			\
	if (__x >> GMP_NUMB_BITS != 0)			\
	  {						\
	    do						\
	      {						\
		__x = *__p;				\
		*__p++ = (__x - 1) & GMP_NUMB_MASK;	\
	      }						\
	    while (__x == 0);				\
	  }						\
      }							\
  } while (0)
#endif
#endif

#ifndef MPN_INCR_U
#if WANT_ASSERT
#define MPN_INCR_U(ptr, size, n)                        \
  do {                                                  \
    ASSERT ((size) >= 1);                               \
    ASSERT_NOCARRY (mpn_add_1 (ptr, ptr, size, n));     \
  } while (0)
#else
#define MPN_INCR_U(ptr, size, n)   mpn_incr_u (ptr, n)
#endif
#endif

#ifndef MPN_DECR_U
#if WANT_ASSERT
#define MPN_DECR_U(ptr, size, n)                        \
  do {                                                  \
    ASSERT ((size) >= 1);                               \
    ASSERT_NOCARRY (mpn_sub_1 (ptr, ptr, size, n));     \
  } while (0)
#else
#define MPN_DECR_U(ptr, size, n)   mpn_decr_u (ptr, n)
#endif
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

#define mp_bases __MPN(bases)
#define __mp_bases __MPN(bases)
__GMP_DECLSPEC extern const struct bases mp_bases[257];


/* For power of 2 bases this is exact.  For other bases the result is either
   exact or one too big.

   To be exact always it'd be necessary to examine all the limbs of the
   operand, since numbers like 100..000 and 99...999 generally differ only
   in the lowest limb.  It'd be possible to examine just a couple of high
   limbs to increase the probability of being exact, but that doesn't seem
   worth bothering with.  */

#define MPN_SIZEINBASE(result, ptr, size, base)                         \
  do {                                                                  \
    int       __lb_base, __cnt;                                         \
    mp_size_t __totbits;                                                \
                                                                        \
    ASSERT ((size) >= 0);                                               \
    ASSERT ((base) >= 2);                                               \
    ASSERT ((base) < numberof (mp_bases));                              \
                                                                        \
    /* Special case for X == 0.  */                                     \
    if ((size) == 0)                                                    \
      (result) = 1;                                                     \
    else                                                                \
      {                                                                 \
        /* Calculate the total number of significant bits of X.  */     \
        count_leading_zeros (__cnt, (ptr)[(size)-1]);                   \
        __totbits = (size) * GMP_NUMB_BITS - (__cnt - GMP_NAIL_BITS);   \
                                                                        \
        if (POW2_P (base))                                              \
          {                                                             \
            __lb_base = mp_bases[base].big_base;                        \
            (result) = (__totbits + __lb_base - 1) / __lb_base;         \
          }                                                             \
        else                                                            \
          (result) = (size_t)                                           \
            (__totbits * mp_bases[base].chars_per_bit_exactly) + 1;     \
      }                                                                 \
  } while (0)

/* eliminate mp_bases lookups for base==16 */
#define MPN_SIZEINBASE_16(result, ptr, size)                            \
  do {                                                                  \
    int       __cnt;                                                    \
    mp_size_t __totbits;                                                \
                                                                        \
    ASSERT ((size) >= 0);                                               \
                                                                        \
    /* Special case for X == 0.  */                                     \
    if ((size) == 0)                                                    \
      (result) = 1;                                                     \
    else                                                                \
      {                                                                 \
        /* Calculate the total number of significant bits of X.  */     \
        count_leading_zeros (__cnt, (ptr)[(size)-1]);                   \
        __totbits = (size) * GMP_NUMB_BITS - (__cnt - GMP_NAIL_BITS);   \
        (result) = (__totbits + 4 - 1) / 4;                             \
      }                                                                 \
  } while (0)


#if HAVE_HOST_CPU_FAMILY_x86
#define TARGET_REGISTER_STARVED 1
#else
#define TARGET_REGISTER_STARVED 0
#endif

/* Use a library function for invert_limb, if available. */
#if ! defined (invert_limb) && HAVE_NATIVE_mpn_invert_limb
#define mpn_invert_limb  __MPN(invert_limb)
mp_limb_t mpn_invert_limb _PROTO ((mp_limb_t)) ATTRIBUTE_CONST;
#define invert_limb(invxl,xl)  (invxl = mpn_invert_limb (xl))
#endif

#ifndef invert_limb
#define invert_limb(invxl,xl)                   \
  do {                                          \
    mp_limb_t dummy;                            \
    ASSERT ((xl) != 0);                         \
    if (xl << 1 == 0)                           \
      invxl = ~(mp_limb_t) 0;                   \
    else                                        \
      udiv_qrnnd (invxl, dummy, -xl, 0, xl);    \
  } while (0)
#endif

/* Divide the two-limb number in (NH,,NL) by D, with DI being the largest
   limb not larger than (2**(2*BITS_PER_MP_LIMB))/D - (2**BITS_PER_MP_LIMB).
   If this would yield overflow, DI should be the largest possible number
   (i.e., only ones).  For correct operation, the most significant bit of D
   has to be set.  Put the quotient in Q and the remainder in R.  */
#define udiv_qrnnd_preinv(q, r, nh, nl, d, di)                            \
  do {                                                                    \
    mp_limb_t _q, _ql, _r;                                                \
    mp_limb_t _xh, _xl;                                                   \
    ASSERT ((d) != 0);                                                    \
    umul_ppmm (_q, _ql, (nh), (di));                                      \
    _q += (nh);                 /* DI is 2**BITS_PER_MP_LIMB too small */ \
    umul_ppmm (_xh, _xl, _q, (d));                                        \
    sub_ddmmss (_xh, _r, (nh), (nl), _xh, _xl);                           \
    if (_xh != 0)                                                         \
      {                                                                   \
	sub_ddmmss (_xh, _r, _xh, _r, 0, (d));                            \
	_q += 1;                                                          \
	if (_xh != 0)                                                     \
	  {                                                               \
	    _r -= (d);                                                    \
	    _q += 1;                                                      \
	  }                                                               \
      }                                                                   \
    if (_r >= (d))                                                        \
      {                                                                   \
	_r -= (d);                                                        \
	_q += 1;                                                          \
      }                                                                   \
    (r) = _r;                                                             \
    (q) = _q;                                                             \
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


#define mpn_preinv_divrem_1  __MPN(preinv_divrem_1)
mp_limb_t mpn_preinv_divrem_1 _PROTO ((mp_ptr, mp_size_t, mp_srcptr, mp_size_t, mp_limb_t, mp_limb_t, int));


/* USE_PREINV_DIVREM_1 is whether to use mpn_preinv_divrem_1, as opposed to
   the plain mpn_divrem_1.  Likewise USE_PREINV_MOD_1 chooses between
   mpn_preinv_mod_1 and plain mpn_mod_1.  The default for both is yes, since
   the few CISC chips where preinv is not good have defines saying so.  */
#ifndef USE_PREINV_DIVREM_1
#define USE_PREINV_DIVREM_1   1
#endif
#ifndef USE_PREINV_MOD_1
#define USE_PREINV_MOD_1   1
#endif

#if USE_PREINV_DIVREM_1
#define MPN_DIVREM_OR_PREINV_DIVREM_1(qp,xsize,ap,size,d,dinv,shift)    \
  mpn_preinv_divrem_1 (qp, xsize, ap, size, d, dinv, shift)
#else
#define MPN_DIVREM_OR_PREINV_DIVREM_1(qp,xsize,ap,size,d,dinv,shift)    \
  mpn_divrem_1 (qp, xsize, ap, size, d)
#endif

#if USE_PREINV_MOD_1
#define MPN_MOD_OR_PREINV_MOD_1(src,size,divisor,inverse)       \
  mpn_preinv_mod_1 (src, size, divisor, inverse)
#else
#define MPN_MOD_OR_PREINV_MOD_1(src,size,divisor,inverse)       \
  mpn_mod_1 (src, size, divisor)
#endif


#define mpn_mod_34lsub1 __MPN(mod_34lsub1)
mp_limb_t mpn_mod_34lsub1 _PROTO ((mp_srcptr, mp_size_t)) __GMP_ATTRIBUTE_PURE;


/* DIVEXACT_1_THRESHOLD is at what size to use mpn_divexact_1, as opposed to
   plain mpn_divrem_1.  Likewise MODEXACT_1_ODD_THRESHOLD for
   mpn_modexact_1_odd against plain mpn_mod_1.  On most CPUs divexact and
   modexact are faster at all sizes, so the defaults are 0.  Those CPUs
   where this is not right have a tuned threshold.  */
#ifndef DIVEXACT_1_THRESHOLD
#define DIVEXACT_1_THRESHOLD  0
#endif
#ifndef MODEXACT_1_ODD_THRESHOLD
#define MODEXACT_1_ODD_THRESHOLD  0
#endif

#define mpn_divexact_1 __MPN(divexact_1)
void    mpn_divexact_1 _PROTO ((mp_ptr, mp_srcptr, mp_size_t, mp_limb_t));

#define MPN_DIVREM_OR_DIVEXACT_1(dst, src, size, divisor)                     \
  do {                                                                        \
    if (BELOW_THRESHOLD (size, DIVEXACT_1_THRESHOLD))                         \
      ASSERT_NOCARRY (mpn_divrem_1 (dst, (mp_size_t) 0, src, size, divisor)); \
    else                                                                      \
      {                                                                       \
        ASSERT (mpn_mod_1 (src, size, divisor) == 0);                         \
        mpn_divexact_1 (dst, src, size, divisor);                             \
      }                                                                       \
  } while (0)

#define mpn_modexact_1c_odd  __MPN(modexact_1c_odd)
mp_limb_t mpn_modexact_1c_odd _PROTO ((mp_srcptr src, mp_size_t size,
                                       mp_limb_t divisor, mp_limb_t c)) __GMP_ATTRIBUTE_PURE;

#if HAVE_NATIVE_mpn_modexact_1_odd
#define mpn_modexact_1_odd   __MPN(modexact_1_odd)
mp_limb_t mpn_modexact_1_odd _PROTO ((mp_srcptr src, mp_size_t size,
                                      mp_limb_t divisor)) __GMP_ATTRIBUTE_PURE;
#else
#define mpn_modexact_1_odd(src,size,divisor) \
  mpn_modexact_1c_odd (src, size, divisor, CNST_LIMB(0))
#endif

#define MPN_MOD_OR_MODEXACT_1_ODD(src,size,divisor)     \
  (ABOVE_THRESHOLD (size, MODEXACT_1_ODD_THRESHOLD)     \
   ? mpn_modexact_1_odd (src, size, divisor)            \
   : mpn_mod_1 (src, size, divisor))


/* modlimb_invert() sets inv to the multiplicative inverse of n modulo
   2^GMP_NUMB_BITS, ie. satisfying inv*n == 1 mod 2^GMP_NUMB_BITS.
   n must be odd (otherwise such an inverse doesn't exist).

   This is not to be confused with invert_limb(), which is completely
   different.

   The table lookup gives an inverse with the low 8 bits valid, and each
   multiply step doubles the number of bits.  See Jebelean "An algorithm for
   exact division" end of section 4 (reference in gmp.texi).

   Possible enhancement: Could use UHWtype until the last step, if half-size
   multiplies are faster (might help under _LONG_LONG_LIMB).

   Alternative: As noted in Granlund and Montgomery "Division by Invariant
   Integers using Multiplication" (reference in gmp.texi), n itself gives a
   3-bit inverse immediately, and could be used instead of a table lookup.
   A 4-bit inverse can be obtained effectively from xoring bits 1 and 2 into
   bit 3, for instance with (((n + 2) & 4) << 1) ^ n.  */

#define modlimb_invert_table  __gmp_modlimb_invert_table
__GMP_DECLSPEC extern const unsigned char  modlimb_invert_table[128];

#define modlimb_invert(inv,n)                                           \
  do {                                                                  \
    mp_limb_t  __n = (n);                                               \
    mp_limb_t  __inv;                                                   \
    ASSERT ((__n & 1) == 1);                                            \
                                                                        \
    __inv = modlimb_invert_table[(__n/2) & 0x7F]; /*  8 */              \
    if (GMP_NUMB_BITS > 8)   __inv = 2 * __inv - __inv * __inv * __n;   \
    if (GMP_NUMB_BITS > 16)  __inv = 2 * __inv - __inv * __inv * __n;   \
    if (GMP_NUMB_BITS > 32)  __inv = 2 * __inv - __inv * __inv * __n;   \
                                                                        \
    if (GMP_NUMB_BITS > 64)                                             \
      {                                                                 \
        int  __invbits = 64;                                            \
        do {                                                            \
          __inv = 2 * __inv - __inv * __inv * __n;                      \
          __invbits *= 2;                                               \
        } while (__invbits < GMP_NUMB_BITS);                            \
      }                                                                 \
                                                                        \
    ASSERT ((__inv * __n & GMP_NUMB_MASK) == 1);                        \
    (inv) = __inv & GMP_NUMB_MASK;                                      \
  } while (0)

/* Multiplicative inverse of 3, modulo 2^GMP_NUMB_BITS.
   Eg. 0xAAAAAAAB for 32 bits, 0xAAAAAAAAAAAAAAAB for 64 bits.
   GMP_NUMB_MAX/3*2+1 is right when GMP_NUMB_BITS is even, but when it's odd
   we need to start from GMP_NUMB_MAX>>1. */
#define MODLIMB_INVERSE_3 (((GMP_NUMB_MAX >> (GMP_NUMB_BITS % 2)) / 3) * 2 + 1)

/* ceil(GMP_NUMB_MAX/3) and ceil(2*GMP_NUMB_MAX/3).
   These expressions work because GMP_NUMB_MAX%3 != 0 for all GMP_NUMB_BITS. */
#define GMP_NUMB_CEIL_MAX_DIV3   (GMP_NUMB_MAX / 3 + 1)
#define GMP_NUMB_CEIL_2MAX_DIV3  ((GMP_NUMB_MAX>>1) / 3 + 1 + GMP_NUMB_HIGHBIT)


/* Set r to -a mod d.  a>=d is allowed.  Can give r>d.  All should be limbs.

   It's not clear whether this is the best way to do this calculation.
   Anything congruent to -a would be fine for the one limb congruence
   tests.  */ 

#define NEG_MOD(r, a, d)                                        \
  do {                                                          \
    ASSERT ((d) != 0);                                          \
    ASSERT_LIMB (a);                                            \
    ASSERT_LIMB (d);                                            \
                                                                \
    if ((a) <= (d))                                             \
      {                                                         \
        /* small a is reasonably likely */                      \
        (r) = (d) - (a);                                        \
      }                                                         \
    else                                                        \
      {                                                         \
        unsigned   __twos;                                      \
        mp_limb_t  __dnorm;                                     \
        count_leading_zeros (__twos, d);                        \
        __twos -= GMP_NAIL_BITS;                                \
        __dnorm = (d) << __twos;                                \
        (r) = ((a) <= __dnorm ? __dnorm : 2*__dnorm) - (a);     \
      }                                                         \
                                                                \
    ASSERT_LIMB (r);                                            \
  } while (0)

/* A bit mask of all the least significant zero bits of n, or -1 if n==0. */
#define LOW_ZEROS_MASK(n)  (((n) & -(n)) - 1)


/* Set "p" to 1 if there's an odd number of 1 bits in "n", or to 0 if
   there's an even number.  */

#if defined (__GNUC__) && ! defined (NO_ASM) && HAVE_HOST_CPU_FAMILY_x86
#define ULONG_PARITY(p, n)              \
  do {                                  \
    char           __p;                 \
    unsigned long  __n = (n);           \
    __n ^= (__n >> 16);                 \
    asm ("xorb   %h1, %b1\n"            \
         "setpo  %0\n"                  \
         : "=qm" (__p), "=q" (__n)      \
         : "1" (__n));                  \
    (p) = __p;                          \
  } while (0)
#else
#define ULONG_PARITY(p, n)                      \
  do {                                          \
    unsigned long  __n = (n);                   \
    int  __p = 0;                               \
    do                                          \
      {                                         \
        __p ^= 0x96696996L >> (__n & 0x1F);     \
        __n >>= 5;                              \
      }                                         \
    while (__n != 0);                           \
                                                \
    (p) = __p;                                  \
  } while (0)
#endif


/* 3 cycles on 604 or 750 since shifts and rlwimi's can pair.  gcc (as of
   version 3.1 at least) doesn't seem to know how to generate rlwimi for
   anything other than bit-fields, so use "asm".  */
#if defined (__GNUC__) && ! defined (NO_ASM)                    \
  && HAVE_HOST_CPU_FAMILY_powerpc && BITS_PER_MP_LIMB == 32
#define BSWAP_LIMB(dst, src)                                            \
  do {                                                                  \
    mp_limb_t  __bswapl_src = (src);                                    \
    mp_limb_t  __tmp1 = __bswapl_src >> 24;            /* low byte */   \
    mp_limb_t  __tmp2 = __bswapl_src << 24;            /* high byte */  \
    asm ("rlwimi %0, %2, 24, 16, 23"                   /* 2nd low */    \
         : "=r" (__tmp1) : "0" (__tmp1), "r" (__bswapl_src));           \
    asm ("rlwimi %0, %2,  8,  8, 15"                   /* 3nd high */   \
         : "=r" (__tmp2) : "0" (__tmp2), "r" (__bswapl_src));           \
    (dst) = __tmp1 | __tmp2;                           /* whole */      \
  } while (0)
#endif

/* bswap is available on i486 and up and is fast.  A combination rorw $8 /
   roll $16 / rorw $8 is used in glibc for plain i386 (and in the linux
   kernel with xchgb instead of rorw), but this is not done here, because
   i386 means generic x86 and mixing word and dword operations will cause
   partial register stalls on P6 chips.  */
#if defined (__GNUC__) && ! defined (NO_ASM)            \
  && HAVE_HOST_CPU_FAMILY_x86 && ! HAVE_HOST_CPU_i386   \
  && BITS_PER_MP_LIMB == 32
#define BSWAP_LIMB(dst, src)                    \
  do {                                          \
    asm ("bswap %0" : "=r" (dst) : "0" (src));  \
  } while (0)
#endif

#if defined (__GNUC__) && ! defined (NO_ASM)    \
  && defined (__ia64) && BITS_PER_MP_LIMB == 64
#define BSWAP_LIMB(dst, src)                                    \
  do {                                                          \
    asm ("mux1 %0 = %1, @rev" : "=r" (dst) :  "r" (src));       \
  } while (0)
#endif

/* As per glibc. */
#if defined (__GNUC__) && ! defined (NO_ASM)                    \
  && HAVE_HOST_CPU_FAMILY_m68k && BITS_PER_MP_LIMB == 32
#define BSWAP_LIMB(dst, src)            \
  do {                                  \
    mp_limb_t  __bswapl_src = (src);    \
    asm ("ror%.w %#8, %0\n"             \
         "swap   %0\n"                  \
         "ror%.w %#8, %0"               \
         : "=d" (dst)                   \
         : "0" (__bswapl_src));         \
  } while (0)
#endif

#if ! defined (BSWAP_LIMB)
#if BITS_PER_MP_LIMB == 8
#define BSWAP_LIMB(dst, src)            \
  do { (dst) = (src); } while (0)
#endif
#if BITS_PER_MP_LIMB == 16
#define BSWAP_LIMB(dst, src)                    \
  do {                                          \
    (dst) = ((src) << 8) + ((src) >> 8);        \
  } while (0)
#endif
#if BITS_PER_MP_LIMB == 32
#define BSWAP_LIMB(dst, src)    \
  do {                          \
    (dst) =                     \
      ((src) << 24)             \
      + (((src) & 0xFF00) << 8) \
      + (((src) >> 8) & 0xFF00) \
      + ((src) >> 24);          \
  } while (0)
#endif
#if BITS_PER_MP_LIMB == 64
#define BSWAP_LIMB(dst, src)            \
  do {                                  \
    (dst) =                             \
      ((src) << 56)                     \
      + (((src) & 0xFF00) << 40)        \
      + (((src) & 0xFF0000) << 24)      \
      + (((src) & 0xFF000000) << 8)     \
      + (((src) >> 8) & 0xFF000000)     \
      + (((src) >> 24) & 0xFF0000)      \
      + (((src) >> 40) & 0xFF00)        \
      + ((src) >> 56);                  \
  } while (0)
#endif
#endif

#if ! defined (BSWAP_LIMB)
#define BSWAP_LIMB(dst, src)                            \
  do {                                                  \
    mp_limb_t  __bswapl_src = (src);                    \
    mp_limb_t  __dst = 0;                               \
    int        __i;                                     \
    for (__i = 0; __i < BYTES_PER_MP_LIMB; __i++)       \
      {                                                 \
        __dst = (__dst << 8) | (__bswapl_src & 0xFF);   \
        __bswapl_src >>= 8;                             \
      }                                                 \
    (dst) = __dst;                                      \
  } while (0)
#endif


/* Apparently lwbrx might be slow on some PowerPC chips, so restrict it to
   those we know are fast.  */
#if defined (__GNUC__) && ! defined (NO_ASM)                            \
  && BITS_PER_MP_LIMB == 32 && HAVE_LIMB_BIG_ENDIAN                     \
  && (HAVE_HOST_CPU_powerpc604                                          \
      || HAVE_HOST_CPU_powerpc604e                                      \
      || HAVE_HOST_CPU_powerpc750                                       \
      || HAVE_HOST_CPU_powerpc7400)
#define BSWAP_LIMB_FETCH(limb, src)     \
  do {                                  \
    mp_srcptr  __blf_src = (src);       \
    mp_limb_t  __limb;                  \
    __asm__ ("lwbrx %0, 0, %1"          \
             : "=r" (__limb)            \
             : "r" (__blf_src),         \
               "m" (*__blf_src));       \
    (limb) = __limb;                    \
  } while (0)
#endif

#if ! defined (BSWAP_LIMB_FETCH)
#define BSWAP_LIMB_FETCH(limb, src)  BSWAP_LIMB (limb, *(src))
#endif


/* On the same basis that lwbrx might be slow, restrict stwbrx to those we
   know are fast.  FIXME: Is this necessary?  */
#if defined (__GNUC__) && ! defined (NO_ASM)                            \
  && BITS_PER_MP_LIMB == 32 && HAVE_LIMB_BIG_ENDIAN                     \
  && (HAVE_HOST_CPU_powerpc604                                          \
      || HAVE_HOST_CPU_powerpc604e                                      \
      || HAVE_HOST_CPU_powerpc750                                       \
      || HAVE_HOST_CPU_powerpc7400)
#define BSWAP_LIMB_STORE(dst, limb)     \
  do {                                  \
    mp_ptr     __dst = (dst);           \
    mp_limb_t  __limb = (limb);         \
    __asm__ ("stwbrx %1, 0, %2"         \
             : "=m" (*__dst)            \
             : "r" (__limb),            \
               "r" (__dst));            \
  } while (0)
#endif

#if ! defined (BSWAP_LIMB_STORE)
#define BSWAP_LIMB_STORE(dst, limb)  BSWAP_LIMB (*(dst), limb)
#endif


/* Byte swap limbs from {src,size} and store at {dst,size}. */
#define MPN_BSWAP(dst, src, size)                       \
  do {                                                  \
    mp_ptr     __dst = (dst);                           \
    mp_srcptr  __src = (src);                           \
    mp_size_t  __size = (size);                         \
    mp_size_t  __i;                                     \
    ASSERT ((size) >= 0);                               \
    ASSERT (MPN_SAME_OR_SEPARATE_P (dst, src, size));   \
    CRAY_Pragma ("_CRI ivdep");                         \
    for (__i = 0; __i < __size; __i++)                  \
      {                                                 \
        BSWAP_LIMB_FETCH (*__dst, __src);               \
        __dst++;                                        \
        __src++;                                        \
      }                                                 \
  } while (0)

/* Byte swap limbs from {dst,size} and store in reverse order at {src,size}. */
#define MPN_BSWAP_REVERSE(dst, src, size)               \
  do {                                                  \
    mp_ptr     __dst = (dst);                           \
    mp_size_t  __size = (size);                         \
    mp_srcptr  __src = (src) + __size - 1;              \
    mp_size_t  __i;                                     \
    ASSERT ((size) >= 0);                               \
    ASSERT (! MPN_OVERLAP_P (dst, size, src, size));    \
    CRAY_Pragma ("_CRI ivdep");                         \
    for (__i = 0; __i < __size; __i++)                  \
      {                                                 \
        BSWAP_LIMB_FETCH (*__dst, __src);               \
        __dst++;                                        \
        __src--;                                        \
      }                                                 \
  } while (0)


/* No processor claiming to be SPARC v9 compliant seems to
   implement the POPC instruction.  Disable pattern for now.  */
#if 0
#if defined __GNUC__ && defined __sparc_v9__ && BITS_PER_MP_LIMB == 64
#define popc_limb(result, input)                        \
  do {                                                  \
    DItype __res;                                       \
    asm ("popc %1,%0" : "=r" (result) : "rI" (input));  \
  } while (0)
#endif
#endif

/* Cray intrinsic. */
#ifdef _CRAY
#define popc_limb(result, input)        \
  do {                                  \
    (result) = _popcnt (input);         \
  } while (0)
#endif

/* Cool population count of an mp_limb_t.
   You have to figure out how this works, We won't tell you!

   The constants could also be expressed as:
     0xAA... = [2^(N+1) / 3] = [(2^N-1)/3*2]
     0x33... = [2^N / 5]     = [(2^N-1)/5]
     0x0f... = [2^N / 17]    = [(2^N-1)/17]
     (N is BITS_PER_MP_LIMB, [] denotes truncation.) */

#if ! defined (popc_limb) && BITS_PER_MP_LIMB == 64
#define popc_limb(result, input)                                \
  do {                                                          \
    mp_limb_t  __x = (input);                                   \
    __x -= (__x & CNST_LIMB(0xaaaaaaaaaaaaaaaa)) >> 1;          \
    __x = ((__x >> 2) & CNST_LIMB(0x3333333333333333))          \
      +    (__x       & CNST_LIMB(0x3333333333333333));         \
    __x = ((__x >> 4) + __x) & CNST_LIMB(0x0f0f0f0f0f0f0f0f);   \
    __x = ((__x >> 8) + __x);                                   \
    __x = ((__x >> 16) + __x);                                  \
    __x = ((__x >> 32) + __x) & 0xff;                           \
    (result) = __x;                                             \
  } while (0)
#endif
#if ! defined (popc_limb) && BITS_PER_MP_LIMB == 32
#define popc_limb(result, input)                                \
  do {                                                          \
    mp_limb_t  __x = (input);                                   \
    __x -= (__x & 0xaaaaaaaaL) >> 1;                            \
    __x = ((__x >> 2) & 0x33333333L) + (__x & 0x33333333L);     \
    __x = ((__x >> 4) + __x) & 0x0f0f0f0fL;                     \
    __x = ((__x >> 8) + __x);                                   \
    __x = ((__x >> 16) + __x) & 0xff;                           \
    (result) = __x;                                             \
  } while (0)
#endif
#if ! defined (popc_limb) && BITS_PER_MP_LIMB == 16
#define popc_limb(result, input)                        \
  do {                                                  \
    mp_limb_t  __x = (input);                           \
    __x -= (__x & 0xaaaa) >> 1;                         \
    __x = ((__x >> 2) & 0x3333) + (__x & 0x3333);       \
    __x = ((__x >> 4) + __x) & 0x0f0f;                  \
    __x = ((__x >> 8) + __x) & 0xff;                    \
    (result) = __x;                                     \
  } while (0)
#endif
#if ! defined (popc_limb) && BITS_PER_MP_LIMB == 8
#define popc_limb(result, input)                \
  do {                                          \
    mp_limb_t  __x = (input);                   \
    __x -= (__x & 0xaa) >> 1;                   \
    __x = ((__x >> 2) & 0x33) + (__x & 0x33);   \
    __x = ((__x >> 4) + __x) & 0xf;             \
    (result) = __x;                             \
  } while (0)
#endif
#if ! defined (popc_limb) && BITS_PER_MP_LIMB == 4
#define popc_limb(result, input)                                              \
  do {                                                                        \
    mp_limb_t  __x = (input);                                                 \
    __x = (__x & 1) + ((__x >> 1) & 1) + ((__x >> 2) & 1) + ((__x >> 3) & 1); \
    (result) = __x;                                                           \
  } while (0)
#endif

#if ! defined (popc_limb)
#define popc_limb(result, input)                \
  do {                                          \
    mp_limb_t  __x = (input);                   \
    int        __result = 0;                    \
    int        __i;                             \
    for (__i = 0; __i < GMP_LIMB_BITS; __i++)   \
      {                                         \
        __result += (int) (__x & 1);            \
        __x >>= 1;                              \
      }                                         \
    (result) = __result;                        \
  } while (0)
#endif


/* Define stuff for longlong.h.  */
#if HAVE_ATTRIBUTE_MODE
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
#else /* Assume `long' gives us a wide enough type.  Needed for hppa2.0w.  */
typedef long int DItype;
typedef unsigned long int UDItype;
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
 || defined (__ia64)							\
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
#if defined (__mc68000__) || defined (__mc68020__) || defined (__m68k__)\
    || defined(mc68020)
#define _GMP_IEEE_FLOATS 1
union ieee_double_extract
{
  struct
    {
      /* "int" might be only 16 bits, so use "long" */
      unsigned long sig:1;
      unsigned long exp:11;
      unsigned long manh:20;
      unsigned long manl:32;
    } s;
  double d;
};
#else
#if defined (_BIG_ENDIAN) || defined (__BIG_ENDIAN__)			\
 || defined (__a29k__) || defined (_AM29K)				\
 || defined (__arm__)							\
 || (defined (__convex__) && defined (_IEEE_FLOAT_))			\
 || defined (_CRAYMPP) || defined (_CRAYIEEE)				\
 || defined (__i370__) || defined (__mvs__)				\
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
#endif

/* Use (4.0 * ...) instead of (2.0 * ...) to work around buggy compilers
   that don't convert ulong->double correctly (eg. SunOS 4 native cc).  */
#define MP_BASE_AS_DOUBLE (4.0 * ((mp_limb_t) 1 << (GMP_NUMB_BITS - 2)))
/* Maximum number of limbs it will take to store any `double'.
   We assume doubles have 53 mantissam bits.  */
#define LIMBS_PER_DOUBLE ((53 + GMP_NUMB_BITS - 1) / GMP_NUMB_BITS + 1)

double __gmp_scale2 _PROTO ((double, int)) ATTRIBUTE_CONST;
int __gmp_extract_double _PROTO ((mp_ptr, double));

extern int __gmp_junk;
extern const int __gmp_0;
void __gmp_exception _PROTO ((int)) ATTRIBUTE_NORETURN;
void __gmp_divide_by_zero _PROTO ((void)) ATTRIBUTE_NORETURN;
void __gmp_sqrt_of_negative _PROTO ((void)) ATTRIBUTE_NORETURN;
#define GMP_ERROR(code)   __gmp_exception (code)
#define DIVIDE_BY_ZERO    __gmp_divide_by_zero ()
#define SQRT_OF_NEGATIVE  __gmp_sqrt_of_negative ()

#if defined _LONG_LONG_LIMB
#if __GMP_HAVE_TOKEN_PASTE
#define CNST_LIMB(C) ((mp_limb_t) C##LL)
#else
#define CNST_LIMB(C) ((mp_limb_t) C/**/LL)
#endif
#else /* not _LONG_LONG_LIMB */
#if __GMP_HAVE_TOKEN_PASTE
#define CNST_LIMB(C) ((mp_limb_t) C##L)
#else
#define CNST_LIMB(C) ((mp_limb_t) C/**/L)
#endif
#endif /* _LONG_LONG_LIMB */

/* Stuff used by mpn/generic/perfsqr.c and mpz/prime_p.c */
#if GMP_NUMB_BITS == 2
#define PP 0x3					/* 3 */
#define PP_FIRST_OMITTED 5
#endif
#if GMP_NUMB_BITS == 4
#define PP 0xF					/* 3 x 5 */
#define PP_FIRST_OMITTED 7
#endif
#if GMP_NUMB_BITS == 8
#define PP 0x69					/* 3 x 5 x 7 */
#define PP_FIRST_OMITTED 11
#endif
#if GMP_NUMB_BITS == 16
#define PP 0x3AA7				/* 3 x 5 x 7 x 11 x 13 */
#define PP_FIRST_OMITTED 17
#endif
#if GMP_NUMB_BITS == 32
#define PP 0xC0CFD797L				/* 3 x 5 x 7 x 11 x ... x 29 */
#define PP_INVERTED 0x53E5645CL
#define PP_FIRST_OMITTED 31
#endif
#if GMP_NUMB_BITS == 64
#define PP CNST_LIMB(0xE221F97C30E94E1D)	/* 3 x 5 x 7 x 11 x ... x 53 */
#define PP_INVERTED CNST_LIMB(0x21CFE6CFC938B36B)
#define PP_FIRST_OMITTED 59
#endif
#ifndef PP_FIRST_OMITTED
#define PP_FIRST_OMITTED 3
#endif



/* BIT1 means a result value in bit 1 (second least significant bit), with a
   zero bit representing +1 and a one bit representing -1.  Bits other than
   bit 1 are garbage.  These are meant to be kept in "int"s, and casts are
   used to ensure the expressions are "int"s even if a and/or b might be
   other types.

   JACOBI_TWOS_U_BIT1 and JACOBI_RECIP_UU_BIT1 are used in mpn_jacobi_base
   and their speed is important.  Expressions are used rather than
   conditionals to accumulate sign changes, which effectively means XORs
   instead of conditional JUMPs. */

/* (a/0), with a signed; is 1 if a=+/-1, 0 otherwise */
#define JACOBI_S0(a)   (((a) == 1) | ((a) == -1))

/* (a/0), with a unsigned; is 1 if a=+/-1, 0 otherwise */
#define JACOBI_U0(a)   ((a) == 1)

/* (a/0), with a given by low and size;
   is 1 if a=+/-1, 0 otherwise */
#define JACOBI_LS0(alow,asize) \
  (((asize) == 1 || (asize) == -1) && (alow) == 1)

/* (a/0), with a an mpz_t;
   fetch of low limb always valid, even if size is zero */
#define JACOBI_Z0(a)   JACOBI_LS0 (PTR(a)[0], SIZ(a))

/* (0/b), with b unsigned; is 1 if b=+/-1, 0 otherwise */
#define JACOBI_0U(b)   ((b) == 1)

/* (0/b), with b unsigned; is 1 if b=+/-1, 0 otherwise */
#define JACOBI_0S(b)   ((b) == 1 || (b) == -1)

/* (0/b), with b given by low and size; is 1 if b=+/-1, 0 otherwise */
#define JACOBI_0LS(blow,bsize) \
  (((bsize) == 1 || (bsize) == -1) && (blow) == 1)

/* Convert a bit1 to +1 or -1. */
#define JACOBI_BIT1_TO_PN(result_bit1) \
  (1 - ((int) (result_bit1) & 2))

/* (2/b), with b unsigned and odd;
   is (-1)^((b^2-1)/8) which is 1 if b==1,7mod8 or -1 if b==3,5mod8 and
   hence obtained from (b>>1)^b */
#define JACOBI_TWO_U_BIT1(b) \
  ((int) (((b) >> 1) ^ (b)))

/* (2/b)^twos, with b unsigned and odd */
#define JACOBI_TWOS_U_BIT1(twos, b) \
  ((int) ((twos) << 1) & JACOBI_TWO_U_BIT1 (b))

/* (2/b)^twos, with b unsigned and odd */
#define JACOBI_TWOS_U(twos, b) \
  (JACOBI_BIT1_TO_PN (JACOBI_TWOS_U_BIT1 (twos, b)))

/* (-1/b), with b odd (signed or unsigned);
   is (-1)^((b-1)/2) */
#define JACOBI_N1B_BIT1(b) \
  ((int) (b))

/* (a/b) effect due to sign of a: signed/unsigned, b odd;
   is (-1/b) if a<0, or +1 if a>=0 */
#define JACOBI_ASGN_SU_BIT1(a, b) \
  ((((a) < 0) << 1) & JACOBI_N1B_BIT1(b))

/* (a/b) effect due to sign of b: signed/signed;
   is -1 if a and b both negative, +1 otherwise */
#define JACOBI_BSGN_SS_BIT1(a, b) \
  ((((a)<0) & ((b)<0)) << 1)

/* (a/b) effect due to sign of b: signed/mpz;
   is -1 if a and b both negative, +1 otherwise */
#define JACOBI_BSGN_SZ_BIT1(a, b) \
  JACOBI_BSGN_SS_BIT1 (a, SIZ(b))

/* (a/b) effect due to sign of b: mpz/signed;
   is -1 if a and b both negative, +1 otherwise */
#define JACOBI_BSGN_ZS_BIT1(a, b) \
  JACOBI_BSGN_SZ_BIT1 (b, a)

/* (a/b) reciprocity to switch to (b/a), a,b both unsigned and odd;
   is (-1)^((a-1)*(b-1)/4), which means +1 if either a,b==1mod4, or -1 if
   both a,b==3mod4, achieved in bit 1 by a&b.  No ASSERT()s about a,b odd
   because this is used in a couple of places with only bit 1 of a or b
   valid. */
#define JACOBI_RECIP_UU_BIT1(a, b) \
  ((int) ((a) & (b)))

/* Strip low zero limbs from {b_ptr,b_size} by incrementing b_ptr and
   decrementing b_size.  b_low should be b_ptr[0] on entry, and will be
   updated for the new b_ptr.  result_bit1 is updated according to the
   factors of 2 stripped, as per (a/2).  */
#define JACOBI_STRIP_LOW_ZEROS(result_bit1, a, b_ptr, b_size, b_low)    \
  do {                                                                  \
    ASSERT ((b_size) >= 1);                                             \
    ASSERT ((b_low) == (b_ptr)[0]);                                     \
                                                                        \
    while (UNLIKELY ((b_low) == 0))                                     \
      {                                                                 \
        (b_size)--;                                                     \
        ASSERT ((b_size) >= 1);                                         \
        (b_ptr)++;                                                      \
        (b_low) = *(b_ptr);                                             \
                                                                        \
        ASSERT (((a) & 1) != 0);                                        \
        if ((GMP_NUMB_BITS % 2) == 1)                                   \
          (result_bit1) ^= JACOBI_TWO_U_BIT1(a);                        \
      }                                                                 \
  } while (0)

/* Set a_rem to {a_ptr,a_size} reduced modulo b, either using mod_1 or
   modexact_1_odd, but in either case leaving a_rem<b.  b must be odd and
   unsigned.  modexact_1_odd effectively calculates -a mod b, and
   result_bit1 is adjusted for the factor of -1.

   The way mpn_modexact_1_odd sometimes bases its remainder on a_size and
   sometimes on a_size-1 means if GMP_NUMB_BITS is odd we can't know what
   factor to introduce into result_bit1, so for that case use mpn_mod_1
   unconditionally.

   FIXME: mpn_modexact_1_odd is more efficient, so some way to get it used
   for odd GMP_NUMB_BITS would be good.  Perhaps it could mung its result,
   or not skip a divide step, or something. */

#define JACOBI_MOD_OR_MODEXACT_1_ODD(result_bit1, a_rem, a_ptr, a_size, b) \
  do {                                                                     \
    mp_srcptr  __a_ptr  = (a_ptr);                                         \
    mp_size_t  __a_size = (a_size);                                        \
    mp_limb_t  __b      = (b);                                             \
                                                                           \
    ASSERT (__a_size >= 1);                                                \
    ASSERT (__b & 1);                                                      \
                                                                           \
    if ((GMP_NUMB_BITS % 2) != 0                                           \
        || BELOW_THRESHOLD (__a_size, MODEXACT_1_ODD_THRESHOLD))           \
      {                                                                    \
        (a_rem) = mpn_mod_1 (__a_ptr, __a_size, __b);                      \
      }                                                                    \
    else                                                                   \
      {                                                                    \
        (result_bit1) ^= JACOBI_N1B_BIT1 (__b);                            \
        (a_rem) = mpn_modexact_1_odd (__a_ptr, __a_size, __b);             \
      }                                                                    \
  } while (0)


/* __GMPF_BITS_TO_PREC applies a minimum 53 bits, rounds upwards to a whole
   limb and adds an extra limb.  __GMPF_PREC_TO_BITS drops that extra limb,
   hence giving back the user's size in bits rounded up.  Notice that
   converting prec->bits->prec gives an unchanged value.  */
#define __GMPF_BITS_TO_PREC(n)						\
  ((mp_size_t) ((__GMP_MAX (53, n) + 2 * GMP_NUMB_BITS - 1) / GMP_NUMB_BITS))
#define __GMPF_PREC_TO_BITS(n) \
  ((unsigned long) (n) * GMP_NUMB_BITS - GMP_NUMB_BITS)

extern mp_size_t __gmp_default_fp_limb_precision;


/* Set n to the number of significant digits an mpf of the given _mp_prec
   field, in the given base.  This is a rounded up value, designed to ensure
   there's enough digits to reproduce all the guaranteed part of the value.

   There are prec many limbs, but the high might be only "1" so forget it
   and just count prec-1 limbs into chars.  +1 rounds that upwards, and a
   further +1 is because the limbs usually won't fall on digit boundaries.

   FIXME: If base is a power of 2 and the bits per digit divides
   BITS_PER_MP_LIMB then the +2 is unnecessary.  This happens always for
   base==2, and in base==16 with the current 32 or 64 bit limb sizes. */

#define MPF_SIGNIFICANT_DIGITS(n, base, prec)                           \
  do {                                                                  \
    ASSERT (base >= 2 && base < numberof (mp_bases));                   \
    (n) = 2 + (size_t) ((((prec) - 1) * BITS_PER_MP_LIMB)               \
                        * mp_bases[(base)].chars_per_bit_exactly);      \
  } while (0)


#define DOPRNT_CONV_FIXED        1
#define DOPRNT_CONV_SCIENTIFIC   2
#define DOPRNT_CONV_GENERAL      3

#define DOPRNT_JUSTIFY_NONE      0
#define DOPRNT_JUSTIFY_LEFT      1
#define DOPRNT_JUSTIFY_RIGHT     2
#define DOPRNT_JUSTIFY_INTERNAL  3

#define DOPRNT_SHOWBASE_YES      1
#define DOPRNT_SHOWBASE_NO       2
#define DOPRNT_SHOWBASE_NONZERO  3

struct doprnt_params_t {
  int         base;          /* negative for upper case */
  int         conv;          /* choices above */
  const char  *expfmt;       /* exponent format */
  int         exptimes4;     /* exponent multiply by 4 */
  char        fill;          /* character */
  int         justify;       /* choices above */
  int         prec;          /* prec field, or -1 for all digits */
  int         showbase;      /* choices above */
  int         showpoint;     /* if radix point always shown */
  int         showtrailing;  /* if trailing zeros wanted */
  char        sign;          /* '+', ' ', or '\0' */
  int         width;         /* width field */
};

#if _GMP_H_HAVE_VA_LIST

typedef int (*doprnt_format_t) _PROTO ((void *data, const char *fmt, va_list ap));
typedef int (*doprnt_memory_t) _PROTO ((void *data, const char *str, size_t len));
typedef int (*doprnt_reps_t) _PROTO ((void *data, int c, int reps));
typedef int (*doprnt_final_t) _PROTO ((void *data));

struct doprnt_funs_t {
  doprnt_format_t  format;
  doprnt_memory_t  memory;
  doprnt_reps_t    reps;
  doprnt_final_t   final;   /* NULL if not required */
};

extern const struct doprnt_funs_t  __gmp_fprintf_funs;
extern const struct doprnt_funs_t  __gmp_sprintf_funs;
extern const struct doprnt_funs_t  __gmp_snprintf_funs;
extern const struct doprnt_funs_t  __gmp_obstack_printf_funs;
extern const struct doprnt_funs_t  __gmp_ostream_funs;

/* "buf" is a __gmp_allocate_func block of "alloc" many bytes.  The first
   "size" of these have been written.  "alloc > size" is maintained, so
   there's room to store a '\0' at the end.  "result" is where the
   application wants the final block pointer.  */
struct gmp_asprintf_t {
  char    **result;
  char    *buf;
  size_t  size;
  size_t  alloc;
};

#define GMP_ASPRINTF_T_INIT(d, output)                          \
  do {                                                          \
    (d).result = (output);                                      \
    (d).alloc = 256;                                            \
    (d).buf = (char *) (*__gmp_allocate_func) ((d).alloc);      \
    (d).size = 0;                                               \
  } while (0)

/* If a realloc is necessary, use twice the size actually required, so as to
   avoid repeated small reallocs.  */
#define GMP_ASPRINTF_T_NEED(d, n)                                       \
  do {                                                                  \
    size_t  alloc, newsize, newalloc;                                   \
    ASSERT ((d)->alloc >= (d)->size + 1);                               \
                                                                        \
    alloc = (d)->alloc;                                                 \
    newsize = (d)->size + (n);                                          \
    if (alloc <= newsize)                                               \
      {                                                                 \
        newalloc = 2*newsize;                                           \
        (d)->alloc = newalloc;                                          \
        (d)->buf = __GMP_REALLOCATE_FUNC_TYPE ((d)->buf,                \
                                               alloc, newalloc, char);  \
      }                                                                 \
  } while (0)

__GMP_DECLSPEC int __gmp_asprintf_memory _PROTO ((struct gmp_asprintf_t *d, const char *str, size_t len));
__GMP_DECLSPEC int __gmp_asprintf_reps _PROTO ((struct gmp_asprintf_t *d, int c, int reps));
__GMP_DECLSPEC int __gmp_asprintf_final _PROTO ((struct gmp_asprintf_t *d));

/* buf is where to write the next output, and size is how much space is left
   there.  If the application passed size==0 then that's what we'll have
   here, and nothing at all should be written.  */
struct gmp_snprintf_t {
  char    *buf;
  size_t  size;
};

/* Add the bytes printed by the call to the total retval, or bail out on an
   error.  */
#define DOPRNT_ACCUMULATE(call) \
  do {                          \
    int  __ret;                 \
    __ret = call;               \
    if (__ret == -1)            \
      goto error;               \
    retval += __ret;            \
  } while (0)
#define DOPRNT_ACCUMULATE_FUN(fun, params)      \
  do {                                          \
    ASSERT ((fun) != NULL);                     \
    DOPRNT_ACCUMULATE ((*(fun)) params);        \
  } while (0)
    
#define DOPRNT_FORMAT(fmt, ap)                          \
  DOPRNT_ACCUMULATE_FUN (funs->format, (data, fmt, ap))
#define DOPRNT_MEMORY(ptr, len)                                 \
  DOPRNT_ACCUMULATE_FUN (funs->memory, (data, ptr, len))
#define DOPRNT_REPS(c, n)                               \
  DOPRNT_ACCUMULATE_FUN (funs->reps, (data, c, n))

#define DOPRNT_STRING(str)      DOPRNT_MEMORY (str, strlen (str))

#define DOPRNT_REPS_MAYBE(c, n) \
  do {                          \
    if ((n) != 0)               \
      DOPRNT_REPS (c, n);       \
  } while (0)
#define DOPRNT_MEMORY_MAYBE(ptr, len)   \
  do {                                  \
    if ((len) != 0)                     \
      DOPRNT_MEMORY (ptr, len);         \
  } while (0)

__GMP_DECLSPEC int __gmp_doprnt _PROTO ((const struct doprnt_funs_t *, void *, const char *, va_list));
__GMP_DECLSPEC int __gmp_doprnt_integer _PROTO ((const struct doprnt_funs_t *, void *, const struct doprnt_params_t *, const char *));
__GMP_DECLSPEC int __gmp_doprnt_mpf _PROTO ((const struct doprnt_funs_t *, void *, const struct doprnt_params_t *, mpf_srcptr));
/* why no __GMP_DECLSPEC here??? */
int __gmp_replacement_vsnprintf _PROTO ((char *, size_t, const char *, va_list));
#endif /* _GMP_H_HAVE_VA_LIST */


typedef int (*gmp_doscan_scan_t)  _PROTO ((void *, const char *, ...));
typedef void *(*gmp_doscan_step_t) _PROTO ((void *, int));
typedef int (*gmp_doscan_get_t)   _PROTO ((void *));
typedef int (*gmp_doscan_unget_t) _PROTO ((int, void *));

struct gmp_doscan_funs_t {
  gmp_doscan_scan_t   scan;
  gmp_doscan_step_t   step;
  gmp_doscan_get_t    get;
  gmp_doscan_unget_t  unget;
};
extern const struct gmp_doscan_funs_t  __gmp_fscanf_funs;
extern const struct gmp_doscan_funs_t  __gmp_sscanf_funs;

#if _GMP_H_HAVE_VA_LIST
int __gmp_doscan _PROTO ((const struct gmp_doscan_funs_t *, void *,
                          const char *, va_list));
#endif


/* For testing and debugging.  */
#define MPZ_CHECK_FORMAT(z)					\
  do {                                                          \
    ASSERT_ALWAYS (SIZ(z) == 0 || PTR(z)[ABSIZ(z) - 1] != 0);	\
    ASSERT_ALWAYS (ALLOC(z) >= ABSIZ(z));			\
    ASSERT_ALWAYS_MPN (PTR(z), ABSIZ(z));                       \
  } while (0)

#define MPQ_CHECK_FORMAT(q)                             \
  do {                                                  \
    MPZ_CHECK_FORMAT (mpq_numref (q));                  \
    MPZ_CHECK_FORMAT (mpq_denref (q));                  \
    ASSERT_ALWAYS (SIZ(mpq_denref(q)) >= 1);            \
                                                        \
    if (SIZ(mpq_numref(q)) == 0)                        \
      {                                                 \
        /* should have zero as 0/1 */                   \
        ASSERT_ALWAYS (SIZ(mpq_denref(q)) == 1          \
                       && PTR(mpq_denref(q))[0] == 1);  \
      }                                                 \
    else                                                \
      {                                                 \
        /* should have no common factors */             \
        mpz_t  g;                                       \
        mpz_init (g);                                   \
        mpz_gcd (g, mpq_numref(q), mpq_denref(q));      \
        ASSERT_ALWAYS (mpz_cmp_ui (g, 1) == 0);         \
        mpz_clear (g);                                  \
      }                                                 \
  } while (0)

#define MPF_CHECK_FORMAT(f)                             \
  do {                                                  \
    ASSERT_ALWAYS (PREC(f) >= __GMPF_BITS_TO_PREC(53)); \
    ASSERT_ALWAYS (ABSIZ(f) <= PREC(f)+1);              \
    if (SIZ(f) == 0)                                    \
      ASSERT_ALWAYS (EXP(f) == 0);                      \
    if (SIZ(f) != 0)                                    \
      ASSERT_ALWAYS (PTR(f)[ABSIZ(f) - 1] != 0);        \
  } while (0)


#define MPZ_PROVOKE_REALLOC(z)					\
  do { ALLOC(z) = ABSIZ(z); } while (0)


#if TUNE_PROGRAM_BUILD
/* Some extras wanted when recompiling some .c files for use by the tune
   program.  Not part of a normal build. */

extern mp_size_t  mul_threshold[];
extern mp_size_t  fft_modf_mul_threshold;
extern mp_size_t  sqr_threshold[];
extern mp_size_t  fft_modf_sqr_threshold;
extern mp_size_t  sb_preinv_threshold[];
extern mp_size_t  dc_threshold[];
extern mp_size_t  powm_threshold[];
extern mp_size_t  gcd_accel_threshold[];
extern mp_size_t  gcdext_threshold[];
extern mp_size_t  divrem_1_norm_threshold[];
extern mp_size_t  divrem_1_unnorm_threshold[];
extern mp_size_t  divrem_2_threshold[];
extern mp_size_t  mod_1_norm_threshold[];
extern mp_size_t  mod_1_unnorm_threshold[];
extern mp_size_t  get_str_basecase_threshold[];
extern mp_size_t  get_str_precompute_threshold[];

#undef MUL_KARATSUBA_THRESHOLD
#undef MUL_TOOM3_THRESHOLD
#undef MUL_FFT_TABLE
#undef MUL_FFT_THRESHOLD
#undef MUL_FFT_MODF_THRESHOLD
#undef SQR_BASECASE_THRESHOLD
#undef SQR_KARATSUBA_THRESHOLD
#undef SQR_TOOM3_THRESHOLD
#undef SQR_FFT_TABLE
#undef SQR_FFT_THRESHOLD
#undef SQR_FFT_MODF_THRESHOLD
#undef DIV_DC_THRESHOLD
#undef POWM_THRESHOLD
#undef GCD_ACCEL_THRESHOLD
#undef GCDEXT_THRESHOLD
#undef DIVREM_1_NORM_THRESHOLD
#undef DIVREM_1_UNNORM_THRESHOLD
#undef MOD_1_NORM_THRESHOLD
#undef MOD_1_UNNORM_THRESHOLD
#undef GET_STR_DC_THRESHOLD
#undef GET_STR_PRECOMPUTE_THRESHOLD

#define MUL_KARATSUBA_THRESHOLD   mul_threshold[0]
#define MUL_TOOM3_THRESHOLD       mul_threshold[1]
#define MUL_FFT_TABLE             { 0 }
#define MUL_FFT_THRESHOLD         mul_threshold[2]
#define MUL_FFT_MODF_THRESHOLD    fft_modf_mul_threshold
#define SQR_BASECASE_THRESHOLD    sqr_threshold[0]
#define SQR_KARATSUBA_THRESHOLD   sqr_threshold[1]
#define SQR_TOOM3_THRESHOLD       sqr_threshold[2]
#define SQR_FFT_TABLE             { 0 }
#define SQR_FFT_THRESHOLD         sqr_threshold[3]
#define SQR_FFT_MODF_THRESHOLD    fft_modf_sqr_threshold
#define DIV_DC_THRESHOLD              dc_threshold[0]
#define POWM_THRESHOLD            powm_threshold[0]
#define GCD_ACCEL_THRESHOLD       gcd_accel_threshold[0]
#define GCDEXT_THRESHOLD          gcdext_threshold[0]
#define DIVREM_1_NORM_THRESHOLD   divrem_1_norm_threshold[0]
#define DIVREM_1_UNNORM_THRESHOLD divrem_1_unnorm_threshold[0]
#define MOD_1_NORM_THRESHOLD      mod_1_norm_threshold[0]
#define MOD_1_UNNORM_THRESHOLD    mod_1_unnorm_threshold[0]
#define GET_STR_DC_THRESHOLD   get_str_basecase_threshold[0]
#define GET_STR_PRECOMPUTE_THRESHOLD get_str_precompute_threshold[0]

#if ! UDIV_PREINV_ALWAYS
#undef DIV_SB_PREINV_THRESHOLD
#undef DIVREM_2_THRESHOLD
#define DIV_SB_PREINV_THRESHOLD       sb_preinv_threshold[0]
#define DIVREM_2_THRESHOLD        divrem_2_threshold[0]
#endif

/* Sizes the tune program tests up to, used in a couple of recompilations. */
#define SQR_KARATSUBA_MAX_GENERIC  200
#define MUL_TOOM3_THRESHOLD_LIMIT  700
#define GET_STR_THRESHOLD_LIMIT    500

#undef  FFT_TABLE_ATTRS
#define FFT_TABLE_ATTRS
extern mp_size_t mpn_fft_table[2][MPN_FFT_TABLE_SIZE];

#if TUNE_PROGRAM_BUILD_SQR
#undef SQR_KARATSUBA_THRESHOLD
#define SQR_KARATSUBA_THRESHOLD  SQR_KARATSUBA_MAX_GENERIC
#endif

#endif /* TUNE_PROGRAM_BUILD */

#if defined (__cplusplus)
}
#endif


#ifdef __cplusplus

/* A little helper for a null-terminated __gmp_allocate_func string.
   The destructor ensures it's freed even if an exception is thrown.  */
class gmp_allocated_string {
 public:
  char *str;
  gmp_allocated_string(char *arg) { str = arg; }
  ~gmp_allocated_string() { (*__gmp_free_func) (str, strlen(str)+1); }
};

int __gmp_istream_set_base (std::istream &, char &, bool &, bool &);
void __gmp_istream_set_digits (std::string &, std::istream &, char &, bool &, int);
void __gmp_doprnt_params_from_ios (struct doprnt_params_t *p, std::ios &o);
std::ostream& __gmp_doprnt_integer_ostream (std::ostream &o, struct doprnt_params_t *p, char *s);
extern const struct doprnt_funs_t  __gmp_asprintf_funs_noformat;

#endif /* __cplusplus */

#endif /* __GMP_IMPL_H__ */
