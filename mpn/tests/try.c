/* Run some tests on various mpn routines.

   THIS IS A TEST PROGRAM USED ONLY FOR DEVELOPMENT.  IT'S ALMOST CERTAIN TO
   BE SUBJECT TO INCOMPATIBLE CHANGES IN FUTURE VERSIONS OF GMP.  */

/*
Copyright (C) 2000 Free Software Foundation, Inc.

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


/* Usage: try [options] <function>...

   For example, "./try mpn_add_n" to run tests of that function.

   Combinations of alignments and overlaps are tested, with redzones above
   or below the destinations, and with the sources write-protected.
  
   The number of tests performed becomes ridiculously large with all the
   combinations, and for that reason this can't be a part of a "make check",
   it's meant only for development.  The code isn't very pretty either.

   During development it can help to disable the redzones, since seeing the
   rest of the destination written can show where the wrong part is, or if
   the dst pointers are off by 1 or whatever.  The magic DEADVAL initial
   fill (see below) will show locations never written.

   The -s option can be used to test only certain size operands, which is
   useful if some new code doesn't yet support say sizes less than the
   unrolling, or whatever.

   When a problem occurs it'll of course be necessary to run the program
   under gdb to find out quite where, how and why it's going wrong.  Disable
   the spinner with the -W option when doing this, or single stepping won't
   work.  Using -1 to run with simple data can be useful.

   New functions to test can be added by defining a TRY_TYPE_, adding an
   entry to try_array[] and adding a call to the call() function (if the
   type isn't already supported).  Extra TRY_TYPE_ bits can be easily added
   if necessary.


   Future:

   Automatically detect gdb and disable the spinner timer (use -W for now).

   Make a way to re-run a failing case in the debugger.  Have an option to
   snapshot each test case before it's run so the data is available if a
   segv occurs.  (This should be more reliable than the current print_all()
   in the signal handler.)

   When alignment means a dst isn't hard against the redzone, check the
   space in between remains unchanged.

   See if the 80x86 debug registers can do redzones on byte boundaries.

   When a source overlaps a destination, don't run both s[i].high 0 and 1,
   as s[i].high has no effect.  Maybe encode s[i].high into overlap->s[i].

   When partial overlaps aren't done, don't loop over source alignments
   during overlaps.

*/


/* always do assertion checking */
#define WANT_ASSERT 1

#include "config.h"

#if HAVE_GETOPT_H
#include <getopt.h>  /* for getopt_long() */
#endif
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>

#include "gmp.h"
#include "gmp-impl.h"

#include "ref.h"
#include "try.h"

#if HAVE_SPA_EXTRAS
#include "spa-out.asm.h"
#endif

#if !HAVE_DECL_OPTARG
extern char *optarg;
extern int optind, opterr;
#endif


#define DEFAULT_REPETITIONS  10

int  option_repetitions = DEFAULT_REPETITIONS;
int  option_spinner = 1;
int  option_redzones = 1;
int  option_firstsize = 0;
int  option_lastsize = 500;
int  option_firstsize2 = 0;

#define ALIGNMENTS          4
#define OVERLAPS            4
#define CARRY_RANDOMS       5
#define MULTIPLIER_RANDOMS  5
#define DIVISOR_RANDOMS     5
#define XSIZE_COUNT         4

int  option_print = 0;

#define DATA_TRAND  0
#define DATA_ZEROS  1
#define DATA_SEQ    2
#define DATA_FFS    3
#define DATA_2FD    4
int  option_data = DATA_TRAND;


mp_size_t  pagesize;
#define PAGESIZE_LIMBS  (pagesize / BYTES_PER_MP_LIMB)

/* must be a multiple of the page size */
#define REDZONE_BYTES   (pagesize * 16)
#define REDZONE_LIMBS   (REDZONE_BYTES / BYTES_PER_MP_LIMB)


#define MAX3(x,y,z)   (MAX (x, MAX (y, z)))

#if BITS_PER_MP_LIMB == 32
#define DEADVAL  CNST_LIMB(0xDEADBEEF)
#else
#define DEADVAL  CNST_LIMB(0xDEADBEEFBADDCAFE)
#endif


#define TRY_RETVAL           (1<<0)
#define TRY_SIZE2            (1<<1)
#define TRY_SHIFT            (1<<2)
#define TRY_CARRYBIT         (1<<3)
#define TRY_CARRY3           (1<<4)
#define TRY_CARRY4           (1<<4)
#define TRY_CARRYLIMB        (1<<5)
#define TRY_MULTIPLIER       (1<<6)
#define TRY_DIVISOR          (1<<7)
#define TRY_DOUBLE_DST       (1<<8)
#define TRY_DST0_INIT        (1<<9)
#define TRY_XSIZE            (1<<10)
#define TRY_SIZE_ZERO        (1<<11)
#define TRY_DST_SIZE_RETVAL  (1<<12)
#define TRY_SRC1_GCDDATA     (1<<13)

#define TRY_OVERLAP_LOW_TO_HIGH  (1<<15) /* Default is allow full overlap. */
#define TRY_OVERLAP_HIGH_TO_LOW  (1<<16)
#define TRY_OVERLAP_NONE         (1<<17)
#define TRY_OVERLAP_NOTSRCS      (1<<18)

#define TRY_SRC0        (1<<20)
#define TRY_SRC1        (TRY_SRC0 << 1)

#define TRY_DST0        (1<<24)
#define TRY_DST1        (TRY_DST0 << 1)


#define TRY_SRC(n)      (TRY_SRC0 << (n))
#define TRY_DST(n)      (TRY_DST0 << (n))

#define TRY_CARRYANY  (TRY_CARRYBIT | TRY_CARRY3 | TRY_CARRY4 | TRY_CARRYLIMB)


#define TRY_TYPE_AORS_N      (TRY_RETVAL | TRY_DST0 | TRY_SRC0 | TRY_SRC1)
#define TRY_TYPE_AORS_NC     (TRY_TYPE_AORS_N | TRY_CARRYBIT)

#define TRY_TYPE_AORSMUL_1   (TRY_TYPE_MUL_1  | TRY_DST0_INIT)
#define TRY_TYPE_AORSMUL_1C  (TRY_TYPE_MUL_1C | TRY_DST0_INIT)

#define TRY_TYPE_LOGOPS_N    (TRY_DST0 | TRY_SRC0 | TRY_SRC1)

#define TRY_TYPE_ADDSUB_N \
  (TRY_RETVAL | TRY_DST0 | TRY_DST1 | TRY_SRC0 | TRY_SRC1)
#define TRY_TYPE_ADDSUB_NC \
  (TRY_TYPE_ADDSUB_N | TRY_CARRY4)

#define TRY_TYPE_COPYI \
  (TRY_DST0 | TRY_SRC0 | TRY_OVERLAP_LOW_TO_HIGH | TRY_SIZE_ZERO)
#define TRY_TYPE_COPYD \
  (TRY_DST0 | TRY_SRC0 | TRY_OVERLAP_HIGH_TO_LOW | TRY_SIZE_ZERO)
#define TRY_TYPE_COM_N   (TRY_DST0 | TRY_SRC0)

#define TRY_TYPE_MOD_1   (TRY_RETVAL | TRY_SRC0 | TRY_DIVISOR | TRY_SIZE_ZERO)
#define TRY_TYPE_MOD_1C       (TRY_TYPE_MOD_1 | TRY_CARRYLIMB)
#define TRY_TYPE_DIVMOD_1     (TRY_TYPE_MOD_1  | TRY_DST0)
#define TRY_TYPE_DIVMOD_1C    (TRY_TYPE_MOD_1C | TRY_DST0)
#define TRY_TYPE_DIVREM_1     (TRY_TYPE_DIVMOD_1  | TRY_XSIZE)
#define TRY_TYPE_DIVREM_1C    (TRY_TYPE_DIVMOD_1C | TRY_XSIZE)
#define TRY_TYPE_MOD_1_RSHIFT (TRY_RETVAL | TRY_SRC0 | TRY_SHIFT | TRY_DIVISOR)

#define TRY_TYPE_DIVEXACT_BY3   (TRY_RETVAL | TRY_DST0 | TRY_SRC0)
#define TRY_TYPE_DIVEXACT_BY3C  (TRY_TYPE_DIVEXACT_BY3 | TRY_CARRY3)

#define TRY_TYPE_GCD_1   (TRY_RETVAL | TRY_SRC0 | TRY_DIVISOR)
#define TRY_TYPE_GCD                                            \
  (TRY_RETVAL | TRY_DST0 | TRY_SRC0 | TRY_SRC1 | TRY_SIZE2      \
   | TRY_DST_SIZE_RETVAL | TRY_OVERLAP_NOTSRCS | TRY_SRC1_GCDDATA)

#define TRY_TYPE_MUL_1   (TRY_RETVAL | TRY_DST0 | TRY_SRC0 | TRY_MULTIPLIER)
#define TRY_TYPE_MUL_1C  (TRY_TYPE_MUL_1 | TRY_CARRYLIMB)

#define TRY_TYPE_MUL_BASECASE \
  (TRY_DST0 | TRY_SRC0 | TRY_SRC1 | TRY_SIZE2 | TRY_OVERLAP_NONE)
#define TRY_TYPE_MUL_N \
  (TRY_DST0 | TRY_SRC0 | TRY_SRC1 | TRY_DOUBLE_DST | TRY_OVERLAP_NONE)
#define TRY_TYPE_SQR \
  (TRY_DST0 | TRY_SRC0 | TRY_DOUBLE_DST | TRY_OVERLAP_NONE)

#define TRY_TYPE_RSHIFT \
  (TRY_RETVAL | TRY_DST0 | TRY_SRC0 | TRY_SHIFT | TRY_OVERLAP_LOW_TO_HIGH)
#define TRY_TYPE_LSHIFT \
  (TRY_RETVAL | TRY_DST0 | TRY_SRC0 | TRY_SHIFT | TRY_OVERLAP_HIGH_TO_LOW)

#define TRY_TYPE_POPCOUNT   (TRY_RETVAL | TRY_SRC0 | TRY_SIZE_ZERO)
#define TRY_TYPE_HAMDIST    (TRY_TYPE_POPCOUNT | TRY_SRC1)


/* The following are macros if there's no native versions, so wrap them in
   functions that can be in try_array[]. */

void
MPN_COPY_INCR_fun (mp_ptr rp, mp_srcptr sp, mp_size_t size)
{ MPN_COPY_INCR (rp, sp, size); }

void
MPN_COPY_DECR_fun (mp_ptr rp, mp_srcptr sp, mp_size_t size)
{ MPN_COPY_DECR (rp, sp, size); }

void
mpn_com_n_fun (mp_ptr rp, mp_srcptr sp, mp_size_t size)
{ mpn_com_n (rp, sp, size); }

void
mpn_and_n_fun (mp_ptr rp, mp_srcptr s1, mp_srcptr s2, mp_size_t size)
{ mpn_and_n (rp, s1, s2, size); }

void
mpn_andn_n_fun (mp_ptr rp, mp_srcptr s1, mp_srcptr s2, mp_size_t size)
{ mpn_andn_n (rp, s1, s2, size); }

void
mpn_nand_n_fun (mp_ptr rp, mp_srcptr s1, mp_srcptr s2, mp_size_t size)
{ mpn_nand_n (rp, s1, s2, size); }

void
mpn_ior_n_fun (mp_ptr rp, mp_srcptr s1, mp_srcptr s2, mp_size_t size)
{ mpn_ior_n (rp, s1, s2, size); }

void
mpn_iorn_n_fun (mp_ptr rp, mp_srcptr s1, mp_srcptr s2, mp_size_t size)
{ mpn_iorn_n (rp, s1, s2, size); }

void
mpn_nior_n_fun (mp_ptr rp, mp_srcptr s1, mp_srcptr s2, mp_size_t size)
{ mpn_nior_n (rp, s1, s2, size); }

void
mpn_xor_n_fun (mp_ptr rp, mp_srcptr s1, mp_srcptr s2, mp_size_t size)
{ mpn_xor_n (rp, s1, s2, size); }

void
mpn_xnor_n_fun (mp_ptr rp, mp_srcptr s1, mp_srcptr s2, mp_size_t size)
{ mpn_xnor_n (rp, s1, s2, size); }

void
mpn_divexact_by3_fun (mp_ptr rp, mp_srcptr sp, mp_size_t size)
{ mpn_divexact_by3 (rp, sp, size); }


struct try_t {
  struct try_one_t {
    tryfun_t    function;
    const char  *name;
  } ref, fun;
  int         flag;
  mp_size_t   minsize;
};

#if HAVE_STRINGIZE
#define TRY(fun)        { (tryfun_t) fun,       #fun }
#define TRY_FUNFUN(fun) { (tryfun_t) fun##_fun, #fun }
#else
#define TRY(fun)        { (tryfun_t) fun,         "fun" }
#define TRY_FUNFUN(fun) { (tryfun_t) fun/**/_fun, "fun" }
#endif

struct try_t try_array[] = {
  { TRY(refmpn_add_n),     TRY(mpn_add_n),     TRY_TYPE_AORS_N  },
  { TRY(refmpn_sub_n),     TRY(mpn_sub_n),     TRY_TYPE_AORS_N  },
#if HAVE_NATIVE_mpn_add_nc
  { TRY(refmpn_add_nc),    TRY(mpn_add_nc),    TRY_TYPE_AORS_NC },
#endif
#if HAVE_NATIVE_mpn_sub_nc
  { TRY(refmpn_sub_nc),    TRY(mpn_sub_nc),    TRY_TYPE_AORS_NC },
#endif

  { TRY(refmpn_addmul_1),  TRY(mpn_addmul_1),  TRY_TYPE_AORSMUL_1  },
  { TRY(refmpn_submul_1),  TRY(mpn_submul_1),  TRY_TYPE_AORSMUL_1  },
#if HAVE_NATIVE_mpn_addmul_1c
  { TRY(refmpn_addmul_1c), TRY(mpn_addmul_1c), TRY_TYPE_AORSMUL_1C },
#endif
#if HAVE_NATIVE_mpn_submul_1c
  { TRY(refmpn_submul_1c), TRY(mpn_submul_1c), TRY_TYPE_AORSMUL_1C },
#endif

  { TRY(refmpn_com_n),    TRY_FUNFUN(mpn_com_n),  TRY_TYPE_COM_N },

  { TRY(refmpn_copyi),    TRY_FUNFUN(MPN_COPY_INCR), TRY_TYPE_COPYI },
  { TRY(refmpn_copyd),    TRY_FUNFUN(MPN_COPY_DECR), TRY_TYPE_COPYD },

  { TRY(refmpn_and_n),    TRY_FUNFUN(mpn_and_n),  TRY_TYPE_LOGOPS_N },
  { TRY(refmpn_andn_n),   TRY_FUNFUN(mpn_andn_n), TRY_TYPE_LOGOPS_N },
  { TRY(refmpn_nand_n),   TRY_FUNFUN(mpn_nand_n), TRY_TYPE_LOGOPS_N },
  { TRY(refmpn_ior_n),    TRY_FUNFUN(mpn_ior_n),  TRY_TYPE_LOGOPS_N },
  { TRY(refmpn_iorn_n),   TRY_FUNFUN(mpn_iorn_n), TRY_TYPE_LOGOPS_N },
  { TRY(refmpn_nior_n),   TRY_FUNFUN(mpn_nior_n), TRY_TYPE_LOGOPS_N },
  { TRY(refmpn_xor_n),    TRY_FUNFUN(mpn_xor_n),  TRY_TYPE_LOGOPS_N },
  { TRY(refmpn_xnor_n),   TRY_FUNFUN(mpn_xnor_n), TRY_TYPE_LOGOPS_N },

  { TRY(refmpn_divrem_1),     TRY(mpn_divrem_1),     TRY_TYPE_DIVREM_1 },
  { TRY(refmpn_mod_1),        TRY(mpn_mod_1),        TRY_TYPE_MOD_1 },
  { TRY(refmpn_mod_1_rshift), TRY(mpn_mod_1_rshift), TRY_TYPE_MOD_1_RSHIFT },
#if HAVE_NATIVE_mpn_divrem_1c
  { TRY(refmpn_divrem_1c),    TRY(mpn_divrem_1c),    TRY_TYPE_DIVREM_1C },
#endif
#if HAVE_NATIVE_mpn_mod_1c
  { TRY(refmpn_mod_1c),       TRY(mpn_mod_1c),       TRY_TYPE_MOD_1C },
#endif
  { TRY(refmpn_divexact_by3), TRY_FUNFUN(mpn_divexact_by3),
                                                     TRY_TYPE_DIVEXACT_BY3 },
  { TRY(refmpn_divexact_by3c),TRY(mpn_divexact_by3c),TRY_TYPE_DIVEXACT_BY3C },

  { TRY(refmpn_mul_1),    TRY(mpn_mul_1),      TRY_TYPE_MUL_1 },
#if HAVE_NATIVE_mpn_mul_1c
  { TRY(refmpn_mul_1c),   TRY(mpn_mul_1c),     TRY_TYPE_MUL_1C },
#endif

  { TRY(refmpn_rshift),   TRY(mpn_rshift),     TRY_TYPE_RSHIFT },
  { TRY(refmpn_lshift),   TRY(mpn_lshift),     TRY_TYPE_LSHIFT },


  { TRY(refmpn_mul_basecase), TRY(mpn_mul_basecase), TRY_TYPE_MUL_BASECASE },
  { TRY(refmpn_sqr),          TRY(mpn_sqr_basecase), TRY_TYPE_SQR },

  { TRY(refmpn_mul_basecase), TRY(mpn_mul),    TRY_TYPE_MUL_BASECASE },
  { TRY(refmpn_mul_n),        TRY(mpn_mul_n),  TRY_TYPE_MUL_N },
  { TRY(refmpn_sqr),          TRY(mpn_sqr_n),  TRY_TYPE_SQR },

  { TRY(refmpn_gcd_1),    TRY(mpn_gcd_1),      TRY_TYPE_GCD_1 },
  { TRY(refmpn_gcd),      TRY(mpn_gcd),        TRY_TYPE_GCD   },

  { TRY(refmpn_popcount), TRY(mpn_popcount),   TRY_TYPE_POPCOUNT },
  { TRY(refmpn_hamdist),  TRY(mpn_hamdist),    TRY_TYPE_HAMDIST },

#if 0
  /* need wrapper functions since they take workspace arguments */
  { TRY(refmpn_mul_n), TRY_FUNFUN(mpn_kara_mul_n),  TRY_TYPE_MUL_N,
                                                      MPN_KARA_MINSIZE},
  { TRY(refmpn_sqr),   TRY_FUNFUN(mpn_kara_sqr_n),  TRY_TYPE_SQR,
                                                      MPN_KARA_MINSIZE},
  { TRY(refmpn_mul_n), TRY_FUNFUN(mpn_toom3_mul_n), TRY_TYPE_MUL_N,
                                                      MPN_TOOM3_MINSIZE},
  { TRY(refmpn_sqr),   TRY_FUNFUN(mpn_toom3_sqr_n), TRY_TYPE_SQR,
                                                      MPN_TOOM3_MINSIZE },
#endif

#if HAVE_SPA_EXTRAS
#include "spa-out.t-table.i"
#endif
};

struct try_t *tr = &try_array[0];


struct region_t {
  mp_ptr     ptr;
  mp_size_t  size;
};


#define TRAP_NOWHERE 0
#define TRAP_REF     1
#define TRAP_FUN     2
#define TRAP_SETUPS  3
int trap_location = TRAP_NOWHERE;


/* Find least significant limb position where p1,size and p2,size differ.  */
mp_size_t
mpn_diff_lowest (mp_srcptr p1, mp_srcptr p2, mp_size_t size)
{
  mp_size_t  i;

  for (i = 0; i < size; i++)
    if (p1[i] != p2[i])
      return i;

  /* no differences */
  return -1;
}


/* Find most significant limb position where p1,size and p2,size differ.  */
mp_size_t
mpn_diff_highest (mp_srcptr p1, mp_srcptr p2, mp_size_t size)
{
  mp_size_t  i;

  for (i = size-1; i >= 0; i--)
    if (p1[i] != p2[i])
      return i;

  /* no differences */
  return -1;
}


/* Return p advanced to the next multiple of "align" bytes.  "align" must be
   a power of 2.  Care is taken not to assume sizeof(int)==sizeof(pointer).  */
void *
align_pointer (void *p, size_t align)
{
  unsigned  d;
  d = ((unsigned) p) & (align-1);
  d = (d != 0 ? align-d : 0);
  return (void *) (((char *) p) + d);
}

/* malloc n limbs on a multiple of m bytes boundary */
mp_ptr
malloc_limbs_aligned (size_t n, size_t m)
{
  return (mp_ptr) align_pointer (refmpn_malloc_limbs (n + m-1), m);
}

void
mprotect_maybe (void *addr, size_t len, int prot)
{
  if (!option_redzones)
    return;

  if (mprotect (addr, len, prot) != 0)
    {
      fprintf (stderr, "Cannot mprotect %p 0x%X 0x%X\n", addr, len, prot);
      exit (1);
    }
}

/* round "a" up to a multiple of "m" */
size_t
round_up_multiple (size_t a, size_t m)
{
  unsigned long  r;

  r = a % m;
  if (r == 0)
    return a;
  else
    return a + (m - r);
}

void
malloc_region (struct region_t *r, mp_size_t n)
{
  mp_ptr  p;

  ASSERT ((pagesize % BYTES_PER_MP_LIMB) == 0);

  r->size = round_up_multiple (n, PAGESIZE_LIMBS);
  p = malloc_limbs_aligned (r->size + REDZONE_LIMBS*2, pagesize);
  mprotect_maybe (p, REDZONE_BYTES, PROT_NONE);

  r->ptr = p + REDZONE_LIMBS;
  mprotect_maybe (r->ptr + r->size, REDZONE_BYTES, PROT_NONE);
}

void
mprotect_region (const struct region_t *r, int prot)
{
  mprotect_maybe (r->ptr, r->size, prot);
}


#define NUM_SOURCES  2
#define NUM_DESTS    2

struct source_t {
  struct region_t  region;
  int        high;
  mp_size_t  align;
  mp_ptr     p;
};

struct source_t  s[NUM_SOURCES];

struct dest_t {
  int        high;
  mp_size_t  align;
};

struct dest_t  d[NUM_SOURCES];

struct source_each_t {
  mp_ptr     p;
};

struct dest_each_t {
  struct region_t  region;
  mp_ptr     p;
};

mp_size_t  size;
mp_size_t  size2;
mp_size_t  dsize;
unsigned long   shift;

struct each_t {
  const char  *name;
  struct dest_each_t    d[numberof(d)];
  struct source_each_t  s[numberof(s)];
  mp_limb_t  retval;
};

struct each_t  ref = { "Ref" };
struct each_t  fun = { "Fun" };

#define SRC_SIZE(n) \
  ((n) == 1 && (tr->flag & (TRY_SIZE2|TRY_XSIZE)) ? size2 : size)


/* First four entries must be 0,1,2,3 for TRY_CARRYBIT, TRY_CARRY3, and
   TRY_CARRY4 */
mp_limb_t  carry_array[] = {
  0, 1, 2, 3,
  4,
  (mp_limb_t) 1 << 8,
  (mp_limb_t) 1 << 16,
  (mp_limb_t) -1
};
mp_limb_t  carry;
int        carry_index;

#define CARRY_COUNT                                                     \
  ((tr->flag & TRY_CARRYBIT) ? 2                                        \
   : (tr->flag & TRY_CARRY3) ? 3                                        \
   : (tr->flag & TRY_CARRY4) ? 4                                        \
   : (tr->flag & TRY_CARRYLIMB) ? numberof(carry_array) + CARRY_RANDOMS \
   : 1)

#define MPN_RANDOM_ALT(index,dst,size) \
  (((index) & 1) ? mpn_random (dst, size) : mpn_random2 (dst, size))

/* The dummy value after MPN_RANDOM_ALT ensures both sides of the ":" have
   the same type */
#define CARRY_ITERATION                                                 \
  for (carry_index = 0;                                                 \
       (carry_index < numberof (carry_array)                            \
        ? (carry = carry_array[carry_index])                            \
        : (MPN_RANDOM_ALT (carry_index, &carry, 1), (mp_limb_t) 0)),    \
       carry_index < CARRY_COUNT;                                       \
       carry_index++)


mp_limb_t  multiplier_array[] = {
  0, 1, 2, 3,
  (mp_limb_t) 1 << 8,
  (mp_limb_t) 1 << 16,
  (mp_limb_t) -3,
  (mp_limb_t) -2,
  (mp_limb_t) -1,
};
mp_limb_t  multiplier;
int        multiplier_index;

mp_limb_t  divisor_array[] = {
  1, 2, 3,
  (mp_limb_t) 1 << 8,
  (mp_limb_t) 1 << 16,
  (mp_limb_t) -3,
  (mp_limb_t) -2,
  (mp_limb_t) -1,
};

mp_limb_t  divisor;
int        divisor_index;

/* The dummy value after MPN_RANDOM_ALT ensures both sides of the ":" have
   the same type */
#define ARRAY_ITERATION(var, index, limit, array, randoms, cond)        \
  for (index = 0;                                                       \
       (index < numberof (array)                                        \
        ? CAST_TO_VOID (var = array[index])                             \
        : (MPN_RANDOM_ALT (index, &var, 1), (mp_limb_t) 0)),            \
       index < limit;                                                   \
       index++)

#define MULTIPLIER_COUNT                                \
  ((tr->flag & TRY_MULTIPLIER)                          \
    ? numberof (multiplier_array) + MULTIPLIER_RANDOMS  \
    : 1)

#define MULTIPLIER_ITERATION                                            \
  ARRAY_ITERATION(multiplier, multiplier_index, MULTIPLIER_COUNT,       \
                  multiplier_array, MULTIPLIER_RANDOMS, TRY_MULTIPLIER)

#define DIVISOR_COUNT                           \
  ((tr->flag & TRY_DIVISOR)                    \
   ? numberof (divisor_array) + DIVISOR_RANDOMS \
   : 1)

#define DIVISOR_ITERATION                                               \
  ARRAY_ITERATION(divisor, divisor_index, DIVISOR_COUNT, divisor_array, \
                  DIVISOR_RANDOMS, TRY_DIVISOR)


/* overlap_array[].s[i] is where s[i] should be, 0 or 1 means overlapping
   d[0] or d[1] respectively, -1 means a separate (write-protected)
   location. */

struct overlap_t {
  int  s[NUM_SOURCES];
} overlap_array[] = {
  { { -1, -1 } },
  { {  0, -1 } },
  { { -1,  0 } },
  { {  0,  0 } },
  { {  1, -1 } },
  { { -1,  1 } },
  { {  1,  1 } },
  { {  0,  1 } },
  { {  1,  0 } },
};

struct overlap_t  *overlap, *overlap_limit;

#define OVERLAP_COUNT                   \
  (tr->flag & TRY_OVERLAP_NONE ? 1      \
   : tr->flag & TRY_OVERLAP_NOTSRCS ? 3 \
   : tr->flag & TRY_DST1 ? 9            \
   : tr->flag & TRY_SRC1 ? 4            \
   : tr->flag & TRY_DST0 ? 2            \
   : 1)

#define OVERLAP_ITERATION                               \
  for (overlap = &overlap_array[0],                     \
    overlap_limit = &overlap_array[OVERLAP_COUNT];      \
    overlap < overlap_limit;                            \
    overlap++)


#define T_RAND_COUNT  2
int  t_rand;

void
t_random (mp_ptr ptr, mp_size_t n)
{
  if (size == 0)
    return;

  switch (option_data) {
  case DATA_TRAND:
    switch (t_rand) {
    case 0: mpn_random (ptr, n); break;
    case 1: mpn_random2 (ptr, n); break;
    default: abort();
    }
    break;
  case DATA_SEQ:
    {
      static mp_limb_t  counter = 0;
      mp_size_t  i;
      for (i = 0; i < n; i++)
        ptr[i] = ++counter;
    }
    break;
  case DATA_ZEROS:
    refmpn_fill (ptr, n, (mp_limb_t) 0);
    break;
  case DATA_FFS:
    refmpn_fill (ptr, n, (mp_limb_t) -1);
    break;
  case DATA_2FD:
    /* Special value 0x2FFF...FFFD, which divided by 3 gives 0xFFF...FFF,
       inducing the q1_ff special case in the mul-by-inverse part of some
       versions of divrem_1 and mod_1. */
    refmpn_fill (ptr, n, (mp_limb_t) -1);
    ptr[n-1] = 2;
    ptr[0] -= 2;
    break;

  default:
    abort();
  }
}
#define T_RAND_ITERATION \
  for (t_rand = 0; t_rand < T_RAND_COUNT; t_rand++)


void 
print_each (const struct each_t *e)
{
  int  i;

  printf ("%s %s\n", e->name, e == &ref ? tr->ref.name : tr->fun.name);
  if (tr->flag & TRY_RETVAL)
    printf ("   retval %08lX\n", e->retval);

  for (i = 0; i < numberof (e->d); i++)
    { 
      if (tr->flag & TRY_DST(i))
        {
          mpn_tracen ("   d[%d]", i, e->d[i].p, dsize);
          printf ("        located %p\n", e->d[i].p);
        }
    }

  for (i = 0; i < numberof (e->s); i++)
    if (tr->flag & TRY_SRC(i))
      printf ("   s[%d] located %p\n", i, e->s[i].p);
}

void
print_all (void)
{
  int  i;

  printf ("\n");
  printf ("size  %ld\n", size);
  if (tr->flag & (TRY_SIZE2|TRY_XSIZE))
    printf ("size2 %ld\n", size2);
  if (dsize != size)
    printf ("dsize %ld\n", dsize);
  if (tr->flag & TRY_MULTIPLIER)
    printf ("   multiplier 0x%lX\n", multiplier);
  if (tr->flag & TRY_DIVISOR)
    printf ("   divisor 0x%lX\n", divisor);
  if (tr->flag & TRY_SHIFT)
    printf ("   shift %lu\n", shift);
  if (tr->flag & TRY_CARRYANY)
    printf ("   carry %lX\n", carry);

  for (i = 0; i < numberof (d); i++)
    if (tr->flag & TRY_DST(i))
      printf ("   d[%d] %s, align %ld\n",
              i, d[i].high ? "high" : "low", d[i].align);

  for (i = 0; i < numberof (s); i++)
    {
      if (tr->flag & TRY_SRC(i))
        {
          printf ("   s[%d] %s, align %ld, ",
                  i, s[i].high ? "high" : "low", s[i].align);
          switch (overlap->s[i]) {
          case -1:
            printf ("no overlap\n");
            break;
          default:
            printf ("==d[%d]%s\n",
                    overlap->s[i],
                    tr->flag & TRY_OVERLAP_LOW_TO_HIGH ? "+a"
                    : tr->flag & TRY_OVERLAP_HIGH_TO_LOW ? "-a"
                    : "");
            break;
          }
          mpn_tracen ("   s[%d]", i, s[i].p, SRC_SIZE(i));
        }
    }

  if (tr->flag & TRY_DST0_INIT)
    mpn_trace ("   d[0]", s[1].region.ptr, size);

  print_each (&ref);
  print_each (&fun);
}

void
compare (void)
{ 
  int  error = 0;
  int  i;

  if ((tr->flag & TRY_RETVAL) && ref.retval != fun.retval)
    {
      printf ("Different return values\n");
      error = 1;
    }

  if (! CALLING_CONVENTIONS_CHECK ())
    error = 1;

  if (tr->flag & TRY_DST_SIZE_RETVAL)
    dsize = ref.retval;

  for (i = 0; i < numberof (ref.d); i++)
    {
      if (!(tr->flag & TRY_DST(i)))
        continue;

      if (dsize != 0 && refmpn_cmp (ref.d[i].p, fun.d[i].p, dsize) != 0)
        {
          printf ("Different d[%d] data results, low diff at %ld, high diff at %ld\n",
                  i,
                  mpn_diff_lowest (ref.d[i].p, fun.d[i].p, dsize),
                  mpn_diff_highest (ref.d[i].p, fun.d[i].p, dsize));
          error = 1;
        }
    }

  if (error)
    {
      print_all();
      abort();
    }
}

void
call (struct each_t *e, tryfun_t function)
{
  switch (tr->flag) {
  case TRY_TYPE_AORS_N:
    e->retval = CALLING_CONVENTIONS (function)
      (e->d[0].p, e->s[0].p, e->s[1].p, size);
    break;
  case TRY_TYPE_AORS_NC:
    e->retval = CALLING_CONVENTIONS (function)
      (e->d[0].p, e->s[0].p, e->s[1].p, size, carry);
    break;

  case TRY_TYPE_LOGOPS_N:
    CALLING_CONVENTIONS (function) (e->d[0].p, e->s[0].p, e->s[1].p, size);
    break;

  case TRY_TYPE_ADDSUB_N:
    e->retval = CALLING_CONVENTIONS (function)
      (e->d[0].p, e->d[1].p, e->s[0].p, e->s[1].p, size);
    break;
  case TRY_TYPE_ADDSUB_NC:
    e->retval = CALLING_CONVENTIONS (function)
      (e->d[0].p, e->d[1].p, e->s[0].p, e->s[1].p, size, carry);
    break;

  case TRY_TYPE_COPYI:
  case TRY_TYPE_COPYD:
  case TRY_TYPE_COM_N:
    CALLING_CONVENTIONS (function) (e->d[0].p, e->s[0].p, size);
    break;

  case TRY_TYPE_DIVEXACT_BY3:
    e->retval = CALLING_CONVENTIONS (function) (e->d[0].p, e->s[0].p, size);
    break;
  case TRY_TYPE_DIVEXACT_BY3C:
    e->retval = CALLING_CONVENTIONS (function) (e->d[0].p, e->s[0].p, size,
                                                carry);
    break;

  case TRY_TYPE_DIVMOD_1:
    e->retval = CALLING_CONVENTIONS (function)
      (e->d[0].p, e->s[0].p, size, divisor);
    break;
  case TRY_TYPE_DIVMOD_1C:
    e->retval = CALLING_CONVENTIONS (function)
      (e->d[0].p, e->s[0].p, size, divisor, carry);
    break;
  case TRY_TYPE_DIVREM_1:
    e->retval = CALLING_CONVENTIONS (function)
      (e->d[0].p, size2, e->s[0].p, size, divisor);
    break;
  case TRY_TYPE_DIVREM_1C:
    e->retval = CALLING_CONVENTIONS (function)
      (e->d[0].p, size2, e->s[0].p, size, divisor, carry);
    break;
  case TRY_TYPE_MOD_1:
    e->retval = CALLING_CONVENTIONS (function)
      (e->s[0].p, size, divisor);
    break;
  case TRY_TYPE_MOD_1C:
    e->retval = CALLING_CONVENTIONS (function)
      (e->s[0].p, size, divisor, carry);
    break;
  case TRY_TYPE_MOD_1_RSHIFT:
    e->retval = CALLING_CONVENTIONS (function)
      (e->s[0].p, size, shift, divisor);
    break;

  case TRY_TYPE_GCD_1:
    /* Must have a non-zero src, but this probably isn't the best way to do
       it. */
    if (refmpn_zero_p (e->s[0].p, size))
      e->retval = 0;
    else
      e->retval = CALLING_CONVENTIONS (function) (e->s[0].p, size, divisor);
    break;

  case TRY_TYPE_GCD:
    /* Sources are destroyed, so they're saved and replaced, but a general
       approach to this might be better.  Note that it's still e->s[0].p and
       e->s[1].p that are passed, to get the desired alignments. */
    {
      mp_ptr  s0 = refmpn_malloc_limbs (size);
      mp_ptr  s1 = refmpn_malloc_limbs (size2);
      refmpn_copyi (s0, e->s[0].p, size);
      refmpn_copyi (s1, e->s[1].p, size2);

      mprotect_region (&s[0].region, PROT_READ|PROT_WRITE);
      mprotect_region (&s[1].region, PROT_READ|PROT_WRITE);
      e->retval = CALLING_CONVENTIONS (function) (e->d[0].p,
                                                  e->s[0].p, size,
                                                  e->s[1].p, size2);
      refmpn_copyi (e->s[0].p, s0, size);
      refmpn_copyi (e->s[1].p, s1, size2);
      free (s0);
      free (s1);
    }
    break;

  case TRY_TYPE_MUL_1:
  case TRY_TYPE_AORSMUL_1:
    e->retval = CALLING_CONVENTIONS (function)
      (e->d[0].p, e->s[0].p, size, multiplier);
    break;
  case TRY_TYPE_MUL_1C:
  case TRY_TYPE_AORSMUL_1C:
    /* TRY_TYPE_AORSMUL_1C same */
    e->retval = CALLING_CONVENTIONS (function)
      (e->d[0].p, e->s[0].p, size, multiplier, carry);
    break;

  case TRY_TYPE_MUL_BASECASE:
    CALLING_CONVENTIONS (function)
      (e->d[0].p, e->s[0].p, size, e->s[1].p, size2);
    break;
  case TRY_TYPE_MUL_N:
    CALLING_CONVENTIONS (function) (e->d[0].p, e->s[0].p, e->s[1].p, size);
    break;
  case TRY_TYPE_SQR:
    CALLING_CONVENTIONS (function) (e->d[0].p, e->s[0].p, size);
    break;

  case TRY_TYPE_LSHIFT:
  case TRY_TYPE_RSHIFT:
    e->retval = CALLING_CONVENTIONS (function)
      (e->d[0].p, e->s[0].p, size, shift);
    break;

  case TRY_TYPE_POPCOUNT:
    e->retval = CALLING_CONVENTIONS (function) (e->s[0].p, size);
    break;
  case TRY_TYPE_HAMDIST:
    e->retval = CALLING_CONVENTIONS (function) (e->s[0].p, e->s[1].p, size);
    break;

  default:
    printf ("Unknown routine type 0x%X\n", tr->flag);
    abort ();
    break;
  }
}


void
pointer_setup (struct each_t *e)
{
  int  i, j;

  if (tr->flag & TRY_DOUBLE_DST)
    dsize = 2*size;
  else if (tr->flag & (TRY_SIZE2|TRY_XSIZE))
    dsize = size+size2;
  else
    dsize = size;

  /* establish e->d[].p destinations */
  for (i = 0; i < numberof (e->d); i++)
    {
      mp_size_t  offset = 0;

      /* possible room for overlapping sources */
      for (j = 0; j < numberof (overlap->s); j++)
        if (overlap->s[j] == i)
          offset = MAX (offset, s[j].align);

      if (d[i].high)
        {
          e->d[i].p = e->d[i].region.ptr + e->d[i].region.size
            - dsize - d[i].align;
          if (tr->flag & TRY_OVERLAP_LOW_TO_HIGH)
            e->d[i].p -= offset;
        }
      else
        {
          e->d[i].p = e->d[i].region.ptr + d[i].align;
          if (tr->flag & TRY_OVERLAP_HIGH_TO_LOW)
            e->d[i].p += offset;
        }
    }

  /* establish e->s[].p sources */
  for (i = 0; i < numberof (s); i++)
    {
      int  o = overlap->s[i];
      switch (o) {
      case -1:
        /* no overlap */
        e->s[i].p = s[i].p;
        break;
      case 0:
      case 1:
        /* overlap with d[o] */
        if (tr->flag & TRY_OVERLAP_HIGH_TO_LOW)
          e->s[i].p = e->d[o].p - s[i].align;
        else if (tr->flag & TRY_OVERLAP_LOW_TO_HIGH)
          e->s[i].p = e->d[o].p + s[i].align;
        else if (tr->flag & TRY_XSIZE)
          e->s[i].p = e->d[o].p + size2;
        else
          e->s[i].p = e->d[o].p;
        break;
      default:
        abort();
        break;
      }
    }
}


void
try_one (void)
{
  int  i;

  if (option_spinner)
    spinner();
  spinner_count++;

  trap_location = TRAP_SETUPS;

  for (i = 0; i < numberof (s); i++)
    {
      if (s[i].high)
        s[i].p = s[i].region.ptr + s[i].region.size - SRC_SIZE(i) - s[i].align;
      else
        s[i].p = s[i].region.ptr + s[i].align;
    }

  pointer_setup (&ref);
  pointer_setup (&fun);

  if (tr->flag & TRY_DST0_INIT)
    {
      t_random (s[1].region.ptr, dsize);
      MPN_COPY (fun.d[0].p, s[1].region.ptr, dsize);
      MPN_COPY (ref.d[0].p, s[1].region.ptr, dsize);
    }
  else if (tr->flag & TRY_DST0)
    {
      refmpn_fill (ref.d[0].p, dsize, DEADVAL);
      refmpn_fill (fun.d[0].p, dsize, DEADVAL);
    }
  for (i = 1; i < numberof (d); i++)
    {
      if (!(tr->flag & TRY_DST(i)))
        continue;

      refmpn_fill (ref.d[i].p, dsize, DEADVAL);
      refmpn_fill (fun.d[i].p, dsize, DEADVAL);
    }

  ref.retval = 0x04152637;
  fun.retval = 0x8C9DAEBF;

  for (i = 0; i < numberof (s); i++)
    {
      if (!(tr->flag & TRY_SRC(i)))
        continue;

      mprotect_region (&s[i].region, PROT_READ|PROT_WRITE);
      t_random (s[i].p, SRC_SIZE(i));

      if (tr->flag & TRY_SRC1_GCDDATA)
        {
          /* s[1] no more bits than s[0] */
          if (i == 1 && size2 == size)
            s[1].p[size-1] &= refmpn_msbone_mask (s[0].p[size-1]);

          /* normalized */
          s[i].p[SRC_SIZE(i)-1] += (s[i].p[SRC_SIZE(i)-1] == 0);

          /* odd */
          s[i].p[0] |= 1;
        }

      mprotect_region (&s[i].region, PROT_READ);

      if (ref.s[i].p != s[i].p)
        {
          MPN_COPY (ref.s[i].p, s[i].p, SRC_SIZE(i));
          MPN_COPY (fun.s[i].p, s[i].p, SRC_SIZE(i));
        }
    }
  
  /* special requirement of divmod_1c,divrem_1c,mod_1c */
  if (tr->flag == TRY_TYPE_DIVMOD_1C
      || tr->flag == TRY_TYPE_DIVREM_1C
      || tr->flag == TRY_TYPE_MOD_1C)
    carry %= divisor;

  if (option_print)
    print_all();

  trap_location = TRAP_REF;
  call (&ref, tr->ref.function);
  trap_location = TRAP_FUN;
  call (&fun, tr->fun.function);
  trap_location = TRAP_NOWHERE;

  compare ();
}


#define SIZE_ITERATION                                          \
  for (size = MAX3 (option_firstsize,                           \
                    tr->minsize,                                \
                    (tr->flag & TRY_SIZE_ZERO) ? 0 : 1);        \
       size <= option_lastsize;                                 \
       size++)

#define SIZE2_FIRST                             \
  (option_firstsize2 != 0 ? option_firstsize2   \
   : tr->flag & TRY_SIZE2 ? 1                   \
   : tr->flag & TRY_XSIZE ? 0                   \
   : 0)
#define SIZE2_LAST                              \
  (tr->flag & TRY_SIZE2 ? size                  \
   : tr->flag & TRY_XSIZE ? XSIZE_COUNT-1       \
   : 0)

#define SIZE2_ITERATION \
  for (size2 = SIZE2_FIRST; size2 <= SIZE2_LAST; size2++)

#define ALIGN_COUNT(cond)  ((cond) ? ALIGNMENTS : 1)
#define ALIGN_ITERATION(w,n,cond) \
  for (w[n].align = 0; w[n].align < ALIGN_COUNT(cond); w[n].align++)

#define HIGH_LIMIT(cond)  ((cond) != 0)
#define HIGH_COUNT(cond)  (HIGH_LIMIT (cond) + 1)
#define HIGH_ITERATION(w,n,cond) \
  for (w[n].high = 0; w[n].high <= HIGH_LIMIT(cond); w[n].high++)

#define SHIFT_LIMIT \
  ((unsigned long) ((tr->flag & TRY_SHIFT) ? BITS_PER_MP_LIMB-1 : 1))

#define SHIFT_ITERATION \
  for (shift = 1; shift <= SHIFT_LIMIT; shift++)


void
try_many (void)
{
  int   i;

  {
    unsigned long  total = 1;

    total *= option_repetitions;
    total *= option_lastsize;
    if (tr->flag & TRY_SIZE2) total *= (option_lastsize+1)/2;
    if (tr->flag & TRY_XSIZE) total *= XSIZE_COUNT;

    total *= SHIFT_LIMIT;
    total *= MULTIPLIER_COUNT;
    total *= DIVISOR_COUNT;
    total *= CARRY_COUNT;
    total *= T_RAND_COUNT;

    total *= HIGH_COUNT (tr->flag & TRY_DST0);
    total *= HIGH_COUNT (tr->flag & TRY_DST1);
    total *= HIGH_COUNT (tr->flag & TRY_SRC0);
    total *= HIGH_COUNT (tr->flag & TRY_SRC1);

    total *= ALIGN_COUNT (tr->flag & TRY_DST0);
    total *= ALIGN_COUNT (tr->flag & TRY_DST1);
    total *= ALIGN_COUNT (tr->flag & TRY_SRC0);
    total *= ALIGN_COUNT (tr->flag & TRY_SRC1);

    total *= OVERLAP_COUNT;

    printf ("%s %lu\n", tr->fun.name, total);
  }

  spinner_count = 0;

  for (i = 0; i < option_repetitions; i++)
    SIZE_ITERATION
      SIZE2_ITERATION

      SHIFT_ITERATION
      MULTIPLIER_ITERATION
      DIVISOR_ITERATION
      CARRY_ITERATION /* must be after divisor */
      T_RAND_ITERATION

      HIGH_ITERATION(d,0, tr->flag & TRY_DST0)
      HIGH_ITERATION(d,1, tr->flag & TRY_DST1)
      HIGH_ITERATION(s,0, tr->flag & TRY_SRC0)
      HIGH_ITERATION(s,1, tr->flag & TRY_SRC1)

      ALIGN_ITERATION(d,0, tr->flag & TRY_DST0)
      ALIGN_ITERATION(d,1, tr->flag & TRY_DST1)
      ALIGN_ITERATION(s,0, tr->flag & TRY_SRC0)
      ALIGN_ITERATION(s,1, tr->flag & TRY_SRC1)

      OVERLAP_ITERATION
      try_one();

  printf("\n");
}


/* Usually print_all() doesn't show much, but it might give a hint as to
   where the function was up to when it died. */
void
trap (int sig)
{
  const char *name = "noname";

  switch (sig) {
  case SIGILL:  name = "SIGILL";  break;
#ifdef SIGBUS
  case SIGBUS:  name = "SIGBUS";  break;
#endif
  case SIGSEGV: name = "SIGSEGV"; break;
  case SIGFPE:  name = "SIGFPE";  break;
  }

  printf ("\n\nSIGNAL TRAP: %s\n", name);

  switch (trap_location) {
  case TRAP_REF:
    printf ("  in reference function: %s\n", tr->ref.name);
    break;
  case TRAP_FUN:
    printf ("  in test function: %s\n", tr->fun.name);
    print_all ();
    break;
  case TRAP_SETUPS:
    printf ("  in parameter setups\n");
    print_all ();
    break;
  default:
    printf ("  somewhere unknown\n");
    break;
  }
  exit (1);
}


void
try_init (void)
{
#if HAVE_GETPAGESIZE
  /* Prefer getpagesize() over sysconf(), since on SunOS 4 sysconf() doesn't
     know _SC_PAGESIZE. */
  pagesize = getpagesize ();
#else
#if HAVE_SYSCONF
  if ((pagesize = sysconf (_SC_PAGESIZE)) == -1)
    {
      /* According to the linux man page, sysconf doesn't set errno */
      fprintf (stderr, "Cannot get sysconf _SC_PAGESIZE\n");
      exit (1);
    }
#else
Error, error, cannot get page size
#endif
#endif

  printf ("pagesize is 0x%lX bytes\n", pagesize);

  signal (SIGILL,  trap);
#ifdef SIGBUS
  signal (SIGBUS,  trap);
#endif
  signal (SIGSEGV, trap);
  signal (SIGFPE,  trap);

  {
    int  i;

    for (i = 0; i < numberof (s); i++)
      {
        malloc_region (&s[i].region, 2*option_lastsize+ALIGNMENTS-1);
        printf ("s[%d] %p to %p (0x%lX bytes)\n",
                i, s[i].region.ptr,
                s[i].region.ptr + s[i].region.size,
                s[i].region.size * BYTES_PER_MP_LIMB);
      }

#define INIT_EACH(e,es)                                                 \
    for (i = 0; i < numberof (e.d); i++)                                \
      {                                                                 \
        malloc_region (&e.d[i].region, 2*option_lastsize+ALIGNMENTS-1); \
        printf ("%s d[%d] %p to %p (0x%lX bytes)\n",                    \
                es, i, e.d[i].region.ptr,                               \
                e.d[i].region.ptr + e.d[i].region.size,                 \
                e.d[i].region.size * BYTES_PER_MP_LIMB);                \
      }

    INIT_EACH(ref, "ref");
    INIT_EACH(fun, "fun");
  }
}

int
strmatch_wild (const char *pattern, const char *str)
{
  size_t  plen, slen;

  /* wildcard at start */
  if (pattern[0] == '*')
    {
      pattern++;
      plen = strlen (pattern);
      slen = strlen (str);
      return (plen == 0
              || (slen >= plen && memcmp (pattern, str+slen-plen, plen) == 0));
    }

  /* wildcard at end */
  plen = strlen (pattern);
  if (plen >= 1 && pattern[plen-1] == '*')
    return (memcmp (pattern, str, plen-1) == 0);

  /* no wildcards */
  return (strcmp (pattern, str) == 0);
}

void
try_name (const char *name)
{
  int  found = 0;
  int  i;

  for (i = 0; i < numberof (try_array); i++)
    {
      if (strmatch_wild (name, try_array[i].fun.name))
        {
          tr = &try_array[i];
          try_many ();
          found = 1;
        }
    }

  if (!found)
    {
      printf ("%s unknown\n", name);
      /* exit (1); */
    }
}


void
usage (const char *prog)
{
  int  col = 0;
  int  i;

  printf ("Usage: %s [options] function...\n\
    -1        use limb data 1,2,3,etc\n\
    -9        use limb data all 0xFF..FFs\n\
    -a zeros  use limb data all zeros\n\
    -a ffs    use limb data all 0xFF..FFs (same as -9)\n\
    -a 2fd    use data 0x2FFF...FFFD\n\
    -p        print each case tried (try this if seg faulting)\n\
    -R        seed random numbers from time()\n\
    -r reps   set repetitions (default %d)\n\
    -S seed   randomize from given seed\n\
    -s size   starting size to test\n\
    -s s1-s2  range of sizes to test\n\
    -W        don't show the spinner (use this in gdb)\n\
    -z        disable mprotect() redzones\n\
Default data is mpn_random() and mpn_random2().\n\
\n\
Functions that can be tested:\n\
", prog, DEFAULT_REPETITIONS);

  for (i = 0; i < numberof (try_array); i++)
    {
      if (col + 1 + strlen (try_array[i].fun.name) > 79)
        {
          printf ("\n");
          col = 0;
        }
      printf (" %s", try_array[i].fun.name);
      col += 1 + strlen (try_array[i].fun.name);
    }
  printf ("\n");

  exit(1);
}


int
main (int argc, char *argv[])
{
  int  i;

  /* unbuffered output */
  setbuf (stdout, NULL);
  setbuf (stderr, NULL);

  /* always trace in hex, upper-case so can paste into bc */
  mp_trace_base = -16;

  {
    unsigned  seed = 123;
    int   opt;

    while ((opt = getopt(argc, argv, "19a:pRr:S:s:Wz")) != EOF)
      {
        switch (opt) {
        case '1':
          /* use limb data values 1, 2, 3, ... etc */
          option_data = DATA_SEQ;
          break;
        case '9':
          /* use limb data values 0xFFF...FFF always */
          option_data = DATA_FFS;
          break;
        case 'a':
          if (strcmp (optarg, "zeros") == 0)     option_data = DATA_ZEROS;
          else if (strcmp (optarg, "seq") == 0)  option_data = DATA_SEQ;
          else if (strcmp (optarg, "ffs") == 0)  option_data = DATA_FFS;
          else if (strcmp (optarg, "2fd") == 0)  option_data = DATA_2FD;
          else
            {
              fprintf (stderr, "unrecognised data option: %s\n", optarg);
              exit (1);
            }
          break;
        case 'p':
          option_print = 1;
          break;
        case 'R':
          /* randomize */
	  seed = time (NULL);
          break;
        case 'r':
	  option_repetitions = atoi (optarg);
          break;
        case 's':
          {
            char  *p;
            option_firstsize = atoi (optarg);
            if ((p = strchr (optarg, '-')) != NULL)
              option_lastsize = atoi (p+1);
          }
          break;
        case 'S':
          /* -S <size> sets the starting size for the second of a two size
             routine (like mpn_mul_basecase) */
	  option_firstsize2 = atoi (optarg);
          break;
        case 'W':
          /* use this when running in the debugger */
          option_spinner = 0;
          break;
        case 'z':
          /* disable redzones */
          option_redzones = 0;
          break;
        case '?':
          usage (argv[0]);
          break;
        }
      }

    srandom (seed);
  }

  try_init();

  if (argc <= optind)
    usage (argv[0]);

  for (i = optind; i < argc; i++)
    try_name (argv[i]);

  return 0;
} 
