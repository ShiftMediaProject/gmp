/* GMP module external subroutines.

Copyright 2001 Free Software Foundation, Inc.

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


/* Notes:

   Routines are grouped with the alias feature and a table of function
   pointers where possible, since each xsub routine ends up with quite a bit
   of overhead.  Different combinations of arguments and return values have
   to be separate though.

   The "INTERFACE:" feature isn't available in perl 5.005 and so isn't used.
   "ALIAS:" requires a table lookup with CvXSUBANY(cv).any_i32 ("ix")
   whereas "INTERFACE:" would have CvXSUBANY(cv).any_dptr as the function
   pointer immediately.

   Mixed-type swapped-order assignments like "$a = 123; $a += mpz(456);"
   invoke the plain overloaded "+", not "+=", which makes life easier.

   The various mpz_assume types are used with the overloaded operators since
   we know they always pass a class object as the first argument and we can
   save an sv_derived_from() lookup.  There's assert()s in MPX_ASSUME() to
   check though.

   The overload_constant routines reached via overload::constant get 4
   arguments in perl 5.6, not the 3 as documented.  This is apparently a
   bug, "..." lets us ignore the extra one.

   There's only a few "si" functions in gmp, so generally SvIV values get
   handled with an mpz_set_si into a temporary and then a full precision mpz
   routine.  This is reasonably efficient.

   Strings are identified with "SvPOK(sv)||SvPOKp(sv)" so that magic
   SVt_PVLV returns from substr() will work.  SvPV() always gives a plain
   actual string.

   Bugs:

   Should IV's and/or NV's be identified with the same dual test as for
   strings?

   The memory leak detection attempted in GMP::END() doesn't work when mpz's
   are created as constants because END() is called before they're
   destroyed.  What's the right place to hook such a check?  */


/* Comment this out to get assertion checking. */
#define NDEBUG

/* Change this to "#define TRACE(x) x" for some diagnostics. */
#define TRACE(x) 


#include <assert.h>

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "patchlevel.h"

#include "gmp.h"


/* Code which doesn't check anything itself, but exists to support other
   assert()s.  */
#ifdef NDEBUG
#define assert_support(x)
#else
#define assert_support(x) x
#endif

/* sv_derived_from etc in 5.005 took "char *" rather than "const char *".
   Avoid some compiler warnings by using const only where it works.  */
#if PERL_REVISION > 5 || (PERL_REVISION == 5 && PERL_VERSION >= 6)
#define classconst const
#else
#define classconst
#endif

#define GMP_MALLOC_ID  42

static classconst char mpz_class[]  = "GMP::Mpz";
static classconst char mpq_class[]  = "GMP::Mpq";
static classconst char mpf_class[]  = "GMP::Mpf";
static classconst char rand_class[] = "GMP::Rand";


assert_support (static long mpz_count = 0;)
assert_support (static long mpq_count = 0;)
assert_support (static long mpf_count = 0;)
assert_support (static long rand_count = 0;)

#define TRACE_ACTIVE()                                                   \
  assert_support                                                         \
  (TRACE (printf ("  active %ld mpz, %ld mpq, %ld mpf, %ld randstate\n", \
                  mpz_count, mpq_count, mpf_count, rand_count)))


/* Each "struct mpz_elem" etc is an mpz_t with a link field tacked on the
   end so they can be held on a linked list.  */

#define CREATE_MPX(type)                                \
                                                        \
  struct type##_elem {                                  \
    type##_t            m;                              \
    struct type##_elem  *next;                          \
  };                                                    \
  typedef struct type##_elem  *type;                    \
  typedef struct type##_elem  *type##_assume;           \
  typedef type##_ptr          type##_coerce;            \
  typedef type##_ptr          type##_mutate;            \
                                                        \
  static type type##_freelist = NULL;                   \
                                                        \
  static type                                           \
  new_##type (void)                                     \
  {                                                     \
    type p;                                             \
    TRACE (printf ("new %s\n", type##_class));          \
    if (type##_freelist != NULL)                        \
      {                                                 \
        p = type##_freelist;                            \
        type##_freelist = type##_freelist->next;        \
      }                                                 \
    else                                                \
      {                                                 \
        New (GMP_MALLOC_ID, p, 1, struct type##_elem);  \
        type##_init (p->m);                             \
      }                                                 \
    TRACE (printf ("  p=%p\n", p));                     \
    assert_support (type##_count++);                    \
    TRACE_ACTIVE ();                                    \
    return p;                                           \
  }                                                     \

CREATE_MPX (mpz)
CREATE_MPX (mpq)

typedef mpf_ptr  mpf;
typedef mpf_ptr  mpf_assume;
typedef mpf_ptr  mpf_coerce_st0;
typedef mpf_ptr  mpf_coerce_def;


static mpf
new_mpf (unsigned long prec)
{
  mpf p;
  New (GMP_MALLOC_ID, p, 1, __mpf_struct);
  mpf_init2 (p, prec);
  TRACE (printf ("  mpf p=%p\n", p));
  assert_support (mpf_count++);
  TRACE_ACTIVE ();
  return p;
}


/* tmp_mpf_t records an allocated precision with an mpf_t so changes of
   precision can be done with just an mpf_set_prec_raw.  */

struct tmp_mpf_struct {
  mpf_t          m;
  unsigned long  allocated_prec;
};
typedef const struct tmp_mpf_struct  *tmp_mpf_srcptr;
typedef struct tmp_mpf_struct        *tmp_mpf_ptr;
typedef struct tmp_mpf_struct        tmp_mpf_t[1];

#define tmp_mpf_init(f)                         \
  do {                                          \
    mpf_init (f->m);                            \
    f->allocated_prec = mpf_get_prec (f->m);    \
  } while (0)

void
tmp_mpf_grow (tmp_mpf_ptr f, unsigned long prec)
{
  mpf_set_prec_raw (f->m, f->allocated_prec);
  mpf_set_prec (f->m, prec);
  f->allocated_prec = mpf_get_prec (f->m);
}

#define tmp_mpf_shrink(f)  tmp_mpf_grow (f, 1L)

#define tmp_mpf_set_prec(f,prec)        \
  do {                                  \
    if (prec > f->allocated_prec)       \
      tmp_mpf_grow (f, prec);           \
    else                                \
      mpf_set_prec_raw (f->m, prec);    \
  } while (0)


static mpz_t  tmp_mpz_0, tmp_mpz_1, tmp_mpz_2;
static mpq_t  tmp_mpq_0, tmp_mpq_1;
static tmp_mpf_t tmp_mpf_0, tmp_mpf_1;


#define FREE_MPX_FREELIST(p,type)               \
  do {                                          \
    TRACE (printf ("free %s\n", type##_class)); \
    p->next = type##_freelist;                  \
    type##_freelist = p;                        \
    assert_support (type##_count--);            \
    TRACE_ACTIVE ();                            \
    assert (type##_count >= 0);                 \
  } while (0)

/* this version for comparison, if desired */
#define FREE_MPX_NOFREELIST(p,type)             \
  do {                                          \
    TRACE (printf ("free %s\n", type##_class)); \
    type##_clear (p->m);                        \
    Safefree (p);                               \
    assert_support (type##_count--);            \
    TRACE_ACTIVE ();                            \
    assert (type##_count >= 0);                 \
  } while (0)

#define free_mpz(z)    FREE_MPX_FREELIST (z, mpz)
#define free_mpq(q)    FREE_MPX_FREELIST (q, mpq)


/* Aliases for use in typemaps */
typedef char           *malloced_string;
typedef const char     *const_string;
typedef const char     *const_string_assume;
typedef char           *string;
typedef SV             *order_noswap;
typedef SV             *dummy;
typedef SV             *SV_copy_0;
typedef unsigned long  ulong_coerce;
typedef __gmp_randstate_struct *randstate;

#define SvMPX(s,type)  ((type) SvIV((SV*) SvRV(s)))
#define SvMPZ(s)       SvMPX(s,mpz)
#define SvMPQ(s)       SvMPX(s,mpq)
#define SvMPF(s)       SvMPX(s,mpf)
#define SvRANDSTATE(s) SvMPX(s,randstate)

#define MPX_ASSUME(x,sv,type)                           \
  do {                                                  \
    assert (sv_derived_from (sv, type##_class));        \
    x = SvMPX(sv,type);                                 \
  } while (0)

#define MPZ_ASSUME(z,sv)    MPX_ASSUME(z,sv,mpz)
#define MPQ_ASSUME(q,sv)    MPX_ASSUME(q,sv,mpq)
#define MPF_ASSUME(f,sv)    MPX_ASSUME(f,sv,mpf)

#define numberof(x)  (sizeof (x) / sizeof ((x)[0]))
#define SGN(x)       ((x)<0 ? -1 : (x) != 0)
#define ABS(x)       ((x)>=0 ? (x) : -(x))
#define double_integer_p(d)  (floor (d) == (d))

#define x_mpq_integer_p(q) \
  (mpz_cmp_ui (mpq_denref(q), 1L) == 0)
#define x_mpq_equal_si(q,n,d) \
  (mpz_cmp_si (mpq_numref(q), n) == 0 && mpz_cmp_ui (mpq_denref(q), d) == 0)
#define x_mpq_equal_z(q,z) \
  (x_mpq_integer_p(q) && mpz_cmp (mpq_numref(q), z) == 0)

#define assert_table(ix)  assert (ix >= 0 && ix < numberof (table))

#define SV_PTR_SWAP(x,y) \
  do { SV *__tmp = (x); (x) = (y); (y) = __tmp; } while (0)
#define MPF_PTR_SWAP(x,y) \
  do { mpf_ptr __tmp = (x); (x) = (y); (y) = __tmp; } while (0)

#define SvPOKorp(sv)  (SvPOK(sv) || SvPOKp(sv))

static void
class_or_croak (SV *sv, classconst char *class)
{
  if (! sv_derived_from (sv, class))
    croak("not type %s", class);
}


/* These are macros, wrap them in functions. */
static int
x_mpz_odd_p (mpz_srcptr z)
{
  return mpz_odd_p (z);
}
static int
x_mpz_even_p (mpz_srcptr z)
{
  return mpz_even_p (z);
}

static void
x_mpq_pow_ui (mpq_ptr r, mpq_srcptr b, unsigned long e)
{
  mpz_pow_ui (mpq_numref(r), mpq_numref(b), e);
  mpz_pow_ui (mpq_denref(r), mpq_denref(b), e);
}


static void *
my_gmp_alloc (size_t n)
{
  void *p;
  TRACE (printf ("my_gmp_alloc %u\n", n));
  New (GMP_MALLOC_ID, p, n, char);
  TRACE (printf ("  p=%p\n", p));
  return p;
}

static void *
my_gmp_realloc (void *p, size_t oldsize, size_t newsize)
{
  TRACE (printf ("my_gmp_realloc %p, %u to %u\n", p, oldsize, newsize));
  Renew (p, newsize, char);
  TRACE (printf ("  p=%p\n", p));
  return p;
}

static void
my_gmp_free (void *p, size_t n)
{
  TRACE (printf ("my_gmp_free %p %u\n", p, n));
  Safefree (p);
}


#define my_mpx_set_svstr(type)                                  \
  static void                                                   \
  my_##type##_set_svstr (type##_ptr x, SV *sv)                  \
  {                                                             \
    const char  *str;                                           \
    STRLEN      len;                                            \
    TRACE (printf ("  my_" #type "_set_svstr\n"));              \
    assert (SvPOKorp (sv));                                     \
    str = SvPV (sv, len);                                       \
    TRACE (printf ("  str \"%s\"\n", str));                     \
    if (type##_set_str (x, str, 0) != 0)                        \
      croak ("%s: invalid string: %s", type##_class, str);      \
  }

my_mpx_set_svstr(mpz)
my_mpx_set_svstr(mpq)
my_mpx_set_svstr(mpf)


/* very slack */
static int
x_mpq_cmp_si (mpq_srcptr x, long yn, unsigned long yd)
{
  mpq  y;
  int  ret;
  y = new_mpq ();
  mpq_set_si (y->m, yn, yd);
  ret = mpq_cmp (x, y->m);
  free_mpq (y);
  return ret;
}

static int
x_mpq_fits_slong_p (mpq_srcptr q)
{
  return x_mpq_cmp_si (q, LONG_MIN, 1L) >= 0
    && mpq_cmp_ui (q, LONG_MAX, 1L) <= 0;
}

static int
x_mpz_cmp_q (mpz_ptr x, mpq_srcptr y)
{
  int  ret;
  mpz_set_ui (mpq_denref(tmp_mpq_0), 1L);
  mpz_swap (mpq_numref(tmp_mpq_0), x);
  ret = mpq_cmp (tmp_mpq_0, y);
  mpz_swap (mpq_numref(tmp_mpq_0), x);
  return ret;
}

static int
x_mpz_cmp_f (mpz_srcptr x, mpf_srcptr y)
{
  tmp_mpf_set_prec (tmp_mpf_0, mpz_sizeinbase (x, 2));
  mpf_set_z (tmp_mpf_0->m, x);
  return mpf_cmp (tmp_mpf_0->m, y);
}


/* Coerce sv to an mpz.  Use tmp to hold the converted value if sv isn't
   already an mpz (or an mpq of which the numerator can be used).  Return
   the chosen mpz (tmp or the contents of sv).  */
static mpz_ptr
coerce_mpz (mpz_ptr tmp, SV *sv)
{
  if (SvIOK(sv))
    {
      mpz_set_si (tmp, SvIVX(sv));
      return tmp;
    }
  if (SvPOKorp(sv))
    {
      my_mpz_set_svstr (tmp, sv);
      return tmp;
    }
  if (SvNOK(sv))
    {
      double d = SvNVX(sv);
      if (! double_integer_p (d))
        croak ("cannot coerce non-integer double to mpz");
      mpz_set_d (tmp, d);
      return tmp;
    }
  if (SvROK(sv))
    {
      if (sv_derived_from (sv, mpz_class))
        {
          return SvMPZ(sv)->m;
        }
      if (sv_derived_from (sv, mpq_class))
        {
          mpq q = SvMPQ(sv);
          if (! x_mpq_integer_p (q->m))
            croak ("cannot coerce non-integer mpq to mpz");
          return mpq_numref(q->m);
        }
      if (sv_derived_from (sv, mpf_class))
        {
          mpf f = SvMPF(sv);
          if (! mpf_integer_p (f))
            croak ("cannot coerce non-integer mpf to mpz");
          mpz_set_f (tmp, f);
          return tmp;
        }
    }
  croak ("cannot coerce to mpz");
}


/* Coerce sv to an mpq.  If sv is an mpq then just return that, otherwise
   use tmp to hold the converted value and return that.  */
static mpq_ptr
coerce_mpq (mpq_ptr tmp, SV *sv)
{
  if (SvIOK(sv))
    {
      mpq_set_si (tmp, SvIVX(sv), 1L);
      return tmp;
    }
  if (SvNOK(sv))
    {
      mpq_set_d (tmp, SvNVX(sv));
      return tmp;
    }
  if (SvPOKorp(sv))
    {
      my_mpq_set_svstr (tmp, sv);
      return tmp;
    }
  if (SvROK(sv))
    {
      if (sv_derived_from (sv, mpz_class))
        {
          mpq_set_z (tmp, SvMPZ(sv)->m);
          return tmp;
        }
      if (sv_derived_from (sv, mpq_class))
        {
          return SvMPQ(sv)->m;
        }
      if (sv_derived_from (sv, mpf_class))
        {
          mpq_set_f (tmp, SvMPF(sv));
          return tmp;
        }
    }
  croak ("cannot coerce to mpq");
}


static void
my_mpf_set_sv (mpf_ptr f, SV *sv)
{
  if (SvIOK(sv))
    mpf_set_si (f, SvIVX(sv));
  else if (SvPOKorp(sv))
    my_mpf_set_svstr (f, sv);
  else if (SvNOK(sv))
    mpf_set_d (f, SvNVX(sv));
  else if (SvROK(sv))
    {
      if (sv_derived_from (sv, mpz_class))
        mpf_set_z (f, SvMPZ(sv)->m);
      else if (sv_derived_from (sv, mpq_class))
        mpf_set_q (f, SvMPQ(sv)->m);
      else if (sv_derived_from (sv, mpf_class))
        mpf_set (f, SvMPF(sv));
      else
        goto invalid;
    }
  else
    {
    invalid:
      croak ("cannot coerce to mpf");
    }
}

/* Coerce sv to an mpf.  If sv is an mpf then just return that, otherwise
   use tmp to hold the converted value (with prec precision).  */
static mpf_ptr
coerce_mpf (tmp_mpf_ptr tmp, SV *sv, unsigned long prec)
{
  if (SvROK(sv) && sv_derived_from (sv, mpf_class))
    return SvMPF(sv);

  tmp_mpf_set_prec (tmp, prec);
  my_mpf_set_sv (tmp->m, sv);
  return tmp->m;
}


/* Coerce xv to an mpf and store the pointer in x, ditto for yv to x.  If
   one of xv or yv is an mpf then use it for the precision, otherwise use
   the default precision.  */
#define COERCE_MPF_PAIR(prec, x,xv, y,yv)                       \
  do {                                                          \
    if (SvROK(xv) && sv_derived_from (xv, mpf_class))           \
      {                                                         \
        x = SvMPF(xv);                                          \
        prec = mpf_get_prec (x);                                \
        y = coerce_mpf (tmp_mpf_0, yv, prec);                   \
      }                                                         \
    else                                                        \
      {                                                         \
        y = coerce_mpf (tmp_mpf_0, yv, mpf_get_default_prec()); \
        prec = mpf_get_prec (y);                                \
        x = coerce_mpf (tmp_mpf_1, xv, prec);                   \
      }                                                         \
  } while (0)
      

static unsigned long
coerce_ulong (SV *sv)
{
  long  n;
  if (SvIOK(sv))
    {
      n = SvIVX(sv);
    negative_check:
      if (n < 0)
        {
        range_error:
          croak ("out of range for ulong");
        }
      return n;
    }
  if (SvNOK(sv))
    {
      double d = SvNVX(sv);
      if (! double_integer_p (d))
        {
        integer_error:
          croak ("not an integer");
        }
      n = SvIV(sv);
      goto negative_check;
    }
  if (SvPOKorp(sv))
    {
      n = SvIV(sv);
      goto negative_check;
    }
  if (SvROK(sv))
    {
      if (sv_derived_from (sv, mpz_class))
        {
          mpz z = SvMPZ(sv);
          if (! mpz_fits_ulong_p (z->m))
            goto range_error;
          return mpz_get_ui (z->m);
        }
      if (sv_derived_from (sv, mpq_class))
        {
          mpq q = SvMPQ(sv);
          if (! x_mpq_integer_p (q->m))
            goto integer_error;
          if (! mpz_fits_ulong_p (mpq_numref (q->m)))
            goto range_error;
          return mpz_get_ui (mpq_numref (q->m));
        }
      if (sv_derived_from (sv, mpf_class))
        {
          mpf f = SvMPF(sv);
          if (! mpf_integer_p (f))
            goto integer_error;
          if (! mpf_fits_ulong_p (f))
            goto range_error;
          return mpf_get_ui (f);
        }
    }
  croak ("cannot coerce to ulong");
}


static long
coerce_long (SV *sv)
{
  if (SvIOK(sv))
    return SvIVX(sv);

  if (SvNOK(sv))
    {
      double d = SvNVX(sv);
      if (! double_integer_p (d))
        {
        integer_error:
          croak ("not an integer");
        }
      return SvIV(sv);
    }

  if (SvPOKorp(sv))
    return SvIV(sv);

  if (SvROK(sv))
    {
      if (sv_derived_from (sv, mpz_class))
        {
          mpz z = SvMPZ(sv);
          if (! mpz_fits_slong_p (z->m))
            {
            range_error:
              croak ("out of range for ulong");
            }
          return mpz_get_si (z->m);
        }
      if (sv_derived_from (sv, mpq_class))
        {
          mpq q = SvMPQ(sv);
          if (! x_mpq_integer_p (q->m))
            goto integer_error;
          if (! mpz_fits_slong_p (mpq_numref (q->m)))
            goto range_error;
          return mpz_get_si (mpq_numref (q->m));
        }
      if (sv_derived_from (sv, mpf_class))
        {
          mpf f = SvMPF(sv);
          if (! mpf_integer_p (f))
            goto integer_error;
          if (! mpf_fits_slong_p (f))
            goto range_error;
          return mpf_get_si (f);
        }
    }
  croak ("cannot coerce to long");
}


#define mpx_set_maybe(dst,src,type) \
  do { if ((dst) != (src)) type##_set (dst, src); } while (0)

#define coerce_mpx_into(p,sv,type)                      \
  do {                                                  \
    type##_ptr  __new_p = coerce_##type (p, sv);        \
    mpx_set_maybe (p, __new_p, type);                   \
  } while (0)

/* Like plain coerce_mpz or coerce_mpq, but force the result into p by
   copying if necessary.  */
#define coerce_mpz_into(z,sv)   coerce_mpx_into(z,sv,mpz)
#define coerce_mpq_into(q,sv)   coerce_mpx_into(q,sv,mpq)


/* Prepare sv to be a changable mpz.  If it's not an mpz then turn it into
   one.  If it is an mpz then ensure the reference count is 1.  */
mpz_ptr
mutate_mpz (SV *sv)
{
  mpz  old_z, new_z;

  TRACE (printf ("mutate_mpz %p\n", sv));
  TRACE (printf ("  type %d\n", SvTYPE(sv)));

  if (SvROK (sv) && sv_derived_from (sv, mpz_class))
    {
      old_z = SvMPZ(sv);
      if (SvREFCNT(SvRV(sv)) == 1)
        return SvMPZ(sv)->m;

      TRACE (printf ("mutate_mpz(): forking new mpz\n"));
      new_z = new_mpz ();
      mpz_set (new_z->m, old_z->m);
    }
  else
    {
      TRACE (printf ("mutate_mpz(): coercing new mpz\n"));
      new_z = new_mpz ();
      coerce_mpz_into (new_z->m, sv);
    }
  sv_setref_pv (sv, mpz_class, new_z);
  return new_z->m;
}


/* ------------------------------------------------------------------------- */

MODULE = GMP         PACKAGE = GMP

BOOT:
    TRACE (printf ("GMP boot\n"));
    mp_set_memory_functions (my_gmp_alloc, my_gmp_realloc, my_gmp_free);
    mpz_init (tmp_mpz_0);
    mpz_init (tmp_mpz_1);
    mpz_init (tmp_mpz_2);
    mpq_init (tmp_mpq_0);
    mpq_init (tmp_mpq_1);
    tmp_mpf_init (tmp_mpf_0);
    tmp_mpf_init (tmp_mpf_1);


void
END()
CODE:
    TRACE (printf ("GMP end\n"));
    TRACE_ACTIVE ();
    /* These are not always true, see Bugs at the top of the file. */
    /* assert (mpz_count == 0); */
    /* assert (mpq_count == 0); */
    /* assert (mpf_count == 0); */
    /* assert (rand_count == 0); */


const_string
version()
CODE:
    RETVAL = gmp_version;
OUTPUT:
    RETVAL


bool
fits_slong_p (sv)
    SV *sv
PREINIT:
    mpq_srcptr  q;
CODE:
    if (SvIOK(sv))
      RETVAL = 1;
    else if (SvNOK(sv))
      {
        double  d = SvNVX(sv);
        RETVAL = (d >= LONG_MIN && d <= LONG_MAX);
      }
    else if (SvPOKorp(sv))
      {
        STRLEN len;
        const char *str = SvPV (sv, len);
        if (mpq_set_str (tmp_mpq_0, str, 0) == 0)
          RETVAL = x_mpq_fits_slong_p (tmp_mpq_0);
        else
          {
            /* enough precision for a long */
            tmp_mpf_set_prec (tmp_mpf_0, 2*mp_bits_per_limb);
            if (mpf_set_str (tmp_mpf_0->m, str, 10) != 0)
              croak ("GMP::fits_slong_p invalid string format");
            RETVAL = mpf_fits_slong_p (tmp_mpf_0->m);
          }
      }
    else if (SvROK(sv))
      {
        if (sv_derived_from (sv, mpz_class))
          RETVAL = mpz_fits_slong_p (SvMPZ(sv)->m);
        else if (sv_derived_from (sv, mpq_class))
          RETVAL = x_mpq_fits_slong_p (SvMPQ(sv)->m);
        else if (sv_derived_from (sv, mpf_class))
          RETVAL = mpf_fits_slong_p (SvMPF(sv));
        else
          goto invalid;
      }
    else
      {
      invalid:
        croak ("GMP::fits_slong_p invalid argument");
      }
OUTPUT:
    RETVAL


double
get_d (sv)
    SV *sv
CODE:
    if (SvIOK(sv))
      RETVAL = (double) SvIVX(sv);
    else if (SvNOK(sv))
      RETVAL = SvNVX(sv);
    else if (SvPOKorp(sv))
      {
        STRLEN len;
        RETVAL = atof(SvPV(sv, len));
      }
    else if (SvROK(sv))
      {
        if (sv_derived_from (sv, mpz_class))
          RETVAL = mpz_get_d (SvMPZ(sv)->m);
        else if (sv_derived_from (sv, mpq_class))
          RETVAL = mpq_get_d (SvMPQ(sv)->m);
        else if (sv_derived_from (sv, mpf_class))
          RETVAL = mpf_get_d (SvMPF(sv));
        else
          goto invalid;
      }
    else
      {
      invalid:
        croak ("GMP::get_d invalid argument");
      }
OUTPUT:
    RETVAL


long
get_si (sv)
    SV *sv
CODE:
    if (SvIOK(sv))
      RETVAL = SvIVX(sv);
    else if (SvNOK(sv))
      RETVAL = (long) SvNVX(sv);
    else if (SvPOKorp(sv))
      RETVAL = SvIV(sv);
    else if (SvROK(sv))
      {
        if (sv_derived_from (sv, mpz_class))
          RETVAL = mpz_get_si (SvMPZ(sv)->m);
        else if (sv_derived_from (sv, mpq_class))
          {
            mpz_set_q (tmp_mpz_0, SvMPQ(sv)->m);
            RETVAL = mpz_get_si (tmp_mpz_0);
          }
        else if (sv_derived_from (sv, mpf_class))
          RETVAL = mpf_get_si (SvMPF(sv));
        else
          goto invalid;
      }
    else
      {
      invalid:
        croak ("GMP::get_si invalid argument");
      }
OUTPUT:
    RETVAL


void
get_str_internal (sv, ...)
    SV   *sv
PREINIT:
    char      *str;
    mp_exp_t  exp;
    mpz_ptr   z;
    mpq_ptr   q;
    mpf       f;
    int       base;
PPCODE:
    TRACE (printf ("GMP::get_str_internal\n"));
    if (items >= 2)
      base = coerce_long (ST(1));
    else
      base = 10;
    TRACE (printf (" base=%d\n", base));

    EXTEND (SP, 3);
    PUSHs (sv_2mortal (newSViv (base)));
    
    if (SvIOK(sv))
      {
        mpz_set_si (tmp_mpz_0, SvIVX(sv));
        z = tmp_mpz_0;
        goto get_mpz;
      }
    else if (SvNOK(sv))
      {
        tmp_mpf_set_prec (tmp_mpf_0, 53);
        mpf_set_d (tmp_mpf_0->m, SvNVX(sv));
        f = tmp_mpf_0->m;
        goto get_mpf;
      }
    else if (SvPOKorp(sv))
      {
        /* get_str on a string is not much more than a base conversion */
        STRLEN len;
        str = SvPV (sv, len);
        if (mpz_set_str (tmp_mpz_0, str, 0) == 0)
          {
            z = tmp_mpz_0;
            goto get_mpz;
          }
        else if (mpq_set_str (tmp_mpq_0, str, 0) == 0)
          {
            q = tmp_mpq_0;
            goto get_mpq;
          }
        else
          {
            /* FIXME: Would like perhaps a precision equivalent to the
               number of significant digits of the string, in its given
               base.  */
            tmp_mpf_set_prec (tmp_mpf_0, strlen(str));
            if (mpf_set_str (tmp_mpf_0->m, str, 10) == 0)
              {
                f = tmp_mpf_0->m;
                goto get_mpf;
              }
            else
              croak ("GMP::get_str invalid string format");
          }
      }
    else if (SvROK(sv))
      {
        if (sv_derived_from (sv, mpz_class))
          {
            z = SvMPZ(sv)->m;
          get_mpz:
            str = mpz_get_str (NULL, base, z);
          push_str:
            PUSHs (sv_2mortal (newSVpv (str, 0)));
          }
        else if (sv_derived_from (sv, mpq_class))
          {
            q = SvMPQ(sv)->m;
          get_mpq:
            str = mpq_get_str (NULL, base, q);
            goto push_str;
          }
        else if (sv_derived_from (sv, mpf_class))
          {
            f = SvMPF(sv);
          get_mpf:
            str = mpf_get_str (NULL, &exp, base, 0, f);
            PUSHs (sv_2mortal (newSVpv (str, 0)));
            PUSHs (sv_2mortal (newSViv (exp)));
          }
        else
          goto invalid;
      }
    else
      {
      invalid:
        croak ("GMP::get_str invalid argument");
      }


bool
integer_p (sv)
    SV *sv
CODE:
    if (SvIOK(sv))
      RETVAL = 1;
    else if (SvNOK(sv))
      RETVAL = double_integer_p (SvNVX(sv));
    else if (SvPOKorp(sv))
      {
        /* FIXME: Maybe this should be done by parsing the string, not by an
           actual conversion.  */
        STRLEN len;
        const char *str = SvPV (sv, len);
        if (mpq_set_str (tmp_mpq_0, str, 0) == 0)
          RETVAL = x_mpq_integer_p (tmp_mpq_0);
        else
          {
            /* enough for all digits of the string */
            tmp_mpf_set_prec (tmp_mpf_0, strlen(str)+64);
            if (mpf_set_str (tmp_mpf_0->m, str, 10) == 0)
              RETVAL = mpf_integer_p (tmp_mpf_0->m);
            else
              croak ("GMP::integer_p invalid string format");
          }
      }
    else if (SvROK(sv))
      {
        if (sv_derived_from (sv, mpz_class))
          RETVAL = 1;
        else if (sv_derived_from (sv, mpq_class))
          RETVAL = x_mpq_integer_p (SvMPQ(sv)->m);
        else if (sv_derived_from (sv, mpf_class))
          RETVAL = mpf_integer_p (SvMPF(sv));
        else
          goto invalid;
      }
    else
      {
      invalid:
        croak ("GMP::integer_p invalid argument");
      }
OUTPUT:
    RETVAL


int
sgn (sv)
    SV *sv
CODE:
    if (SvIOK(sv))
      RETVAL = SGN (SvIVX(sv));
    else if (SvNOK(sv))
      RETVAL = SGN (SvNVX(sv));
    else if (SvPOKorp(sv))
      {
        /* FIXME: Maybe this should be done by parsing the string, not by an
           actual conversion.  */
        STRLEN len;
        const char *str = SvPV (sv, len);
        if (mpq_set_str (tmp_mpq_0, str, 0) == 0)
          RETVAL = mpq_sgn (tmp_mpq_0);
        else
          {
            /* enough for all digits of the string */
            tmp_mpf_set_prec (tmp_mpf_0, strlen(str)+64);
            if (mpf_set_str (tmp_mpf_0->m, str, 10) == 0)
              RETVAL = mpf_sgn (tmp_mpf_0->m);
            else
              croak ("GMP::sgn invalid string format");
          }
      }
    else if (SvROK(sv))
      {
        if (sv_derived_from (sv, mpz_class))
          RETVAL = mpz_sgn (SvMPZ(sv)->m);
        else if (sv_derived_from (sv, mpq_class))
          RETVAL = mpq_sgn (SvMPQ(sv)->m);
        else if (sv_derived_from (sv, mpf_class))
          RETVAL = mpf_sgn (SvMPF(sv));
        else
          goto invalid;
      }
    else
      {
      invalid:
        croak ("GMP::sgn invalid argument");
      }
OUTPUT:
    RETVAL


# currently undocumented
void
shrink ()
CODE:
#define x_mpz_shrink(z) \
    mpz_set_ui (z, 0L); _mpz_realloc (z, 1)
#define x_mpq_shrink(q) \
    x_mpz_shrink (mpq_numref(q)); x_mpz_shrink (mpq_denref(q))

    x_mpz_shrink (tmp_mpz_0);
    x_mpz_shrink (tmp_mpz_1);
    x_mpz_shrink (tmp_mpz_2);
    x_mpq_shrink (tmp_mpq_0);
    x_mpq_shrink (tmp_mpq_1);
    tmp_mpf_shrink (tmp_mpf_0);
    tmp_mpf_shrink (tmp_mpf_1);



#------------------------------------------------------------------------------

MODULE = GMP         PACKAGE = GMP::Mpz

mpz
mpz (...)
ALIAS:
    GMP::Mpz::new = 1
PREINIT:
    SV *sv;
CODE:
    TRACE (printf ("%s new, ix=%ld, items=%d\n", mpz_class, ix, items));
    RETVAL = new_mpz();

    switch (items) {
    case 0:
      mpz_set_ui (RETVAL->m, 0L);
      break;
    case 1:
      sv = ST(0);
      if (SvIOK(sv))         mpz_set_si (RETVAL->m, SvIVX(sv));
      else if (SvNOK(sv))    mpz_set_d  (RETVAL->m, SvNVX(sv));
      else if (SvPOKorp(sv)) my_mpz_set_svstr (RETVAL->m, sv);
      else if (SvROK(sv))
        {
          if (sv_derived_from (sv, mpz_class))
            mpz_set   (RETVAL->m, SvMPZ(sv)->m);
          else if (sv_derived_from (sv, mpq_class))
            mpz_set_q (RETVAL->m, SvMPQ(sv)->m);
          else if (sv_derived_from (sv, mpf_class))
            mpz_set_f (RETVAL->m, SvMPF(sv));
          else
            goto invalid;
        }
      else
        goto invalid;
      break;
    default:
    invalid:
      croak ("%s new: invalid arguments", mpz_class);
    }
OUTPUT:
    RETVAL


void
overload_constant (str, pv, d1, ...)
    const_string_assume str
    SV                  *pv
    dummy               d1
PREINIT:
    mpz z;
PPCODE:
    TRACE (printf ("%s constant: %s\n", mpz_class, str));
    z = new_mpz();
    if (mpz_set_str (z->m, str, 0) == 0)
      {
        SV *sv = sv_newmortal(); sv_setref_pv (sv, mpz_class, z); PUSHs(sv);
      }
    else
      {
        free_mpz (z);
        PUSHs(pv);
      }


mpz
overload_copy (z, d1, d2)
    mpz_assume z
    dummy      d1
    dummy      d2
CODE:
    RETVAL = new_mpz();
    mpz_set (RETVAL->m, z->m);
OUTPUT:
    RETVAL


void
DESTROY (z)
    mpz_assume z
CODE:
    TRACE (printf ("%s DESTROY %p\n", mpz_class, z));
    free_mpz (z);


malloced_string
overload_string (z, d1, d2)
    mpz_assume z
    dummy      d1
    dummy      d2
CODE:
    TRACE (printf ("%s overload_string %p\n", mpz_class, z));
    RETVAL = mpz_get_str (NULL, 10, z->m);
OUTPUT:
    RETVAL


mpz
overload_add (xv, yv, order)
    SV *xv
    SV *yv
    SV *order
ALIAS:
    GMP::Mpz::overload_sub = 1
    GMP::Mpz::overload_mul = 2
    GMP::Mpz::overload_div = 3
    GMP::Mpz::overload_rem = 4
    GMP::Mpz::overload_and = 5
    GMP::Mpz::overload_ior = 6
    GMP::Mpz::overload_xor = 7
PREINIT:
    static const struct {
      void (*op) (mpz_ptr, mpz_srcptr, mpz_srcptr);
    } table[] = {
      { mpz_add    }, /* 0 */
      { mpz_sub    }, /* 1 */
      { mpz_mul    }, /* 2 */
      { mpz_tdiv_q }, /* 3 */
      { mpz_tdiv_r }, /* 4 */
      { mpz_and    }, /* 5 */
      { mpz_ior    }, /* 6 */
      { mpz_xor    }, /* 7 */
    };
CODE:
    assert_table (ix);
    if (order == &PL_sv_yes)
      SV_PTR_SWAP (xv, yv);
    RETVAL = new_mpz();
    (*table[ix].op) (RETVAL->m,
                     coerce_mpz (tmp_mpz_0, xv),
                     coerce_mpz (tmp_mpz_1, yv));
OUTPUT:
    RETVAL


void
overload_addeq (x, y, o)
    mpz_assume   x
    mpz_coerce   y
    order_noswap o
ALIAS:
    GMP::Mpz::overload_subeq = 1
    GMP::Mpz::overload_muleq = 2
    GMP::Mpz::overload_diveq = 3
    GMP::Mpz::overload_remeq = 4
    GMP::Mpz::overload_andeq = 5
    GMP::Mpz::overload_ioreq = 6
    GMP::Mpz::overload_xoreq = 7
PREINIT:
    static const struct {
      void (*op) (mpz_ptr, mpz_srcptr, mpz_srcptr);
    } table[] = {
      { mpz_add    }, /* 0 */
      { mpz_sub    }, /* 1 */
      { mpz_mul    }, /* 2 */
      { mpz_tdiv_q }, /* 3 */
      { mpz_tdiv_r }, /* 4 */
      { mpz_and    }, /* 5 */
      { mpz_ior    }, /* 6 */
      { mpz_xor    }, /* 7 */
    };
PPCODE:
    assert_table (ix);
    (*table[ix].op) (x->m, x->m, y);
    XPUSHs (ST(0));


mpz
overload_lshift (zv, nv, order)
    SV *zv
    SV *nv
    SV *order
ALIAS:
    GMP::Mpz::overload_rshift   = 1
    GMP::Mpz::overload_pow      = 2
PREINIT:
    static const struct {
      void (*op) (mpz_ptr, mpz_srcptr, unsigned long);
    } table[] = {
      { mpz_mul_2exp }, /* 0 */
      { mpz_div_2exp }, /* 1 */
      { mpz_pow_ui   }, /* 2 */
    };
CODE:
    assert_table (ix);
    if (order == &PL_sv_yes)
      SV_PTR_SWAP (zv, nv);
    RETVAL = new_mpz();
    (*table[ix].op) (RETVAL->m, coerce_mpz (RETVAL->m, zv), coerce_ulong (nv));
OUTPUT:
    RETVAL


void
overload_lshifteq (z, n, o)
    mpz_assume   z
    ulong_coerce n
    order_noswap o
ALIAS:
    GMP::Mpz::overload_rshifteq   = 1
    GMP::Mpz::overload_poweq      = 2
PREINIT:
    static const struct {
      void (*op) (mpz_ptr, mpz_srcptr, unsigned long);
    } table[] = {
      { mpz_mul_2exp }, /* 0 */
      { mpz_div_2exp }, /* 1 */
      { mpz_pow_ui   }, /* 2 */
    };
PPCODE:
    assert_table (ix);
    (*table[ix].op) (z->m, z->m, n);
    XPUSHs(ST(0));


mpz
overload_abs (z, d1, d2)
    mpz_assume z
    dummy      d1
    dummy      d2
ALIAS:
    GMP::Mpz::overload_neg  = 1
    GMP::Mpz::overload_com  = 2
    GMP::Mpz::overload_sqrt = 3
PREINIT:
    static const struct {
      void (*op) (mpz_ptr w, mpz_srcptr x);
    } table[] = {
      { mpz_abs  }, /* 0 */
      { mpz_neg  }, /* 1 */
      { mpz_com  }, /* 2 */
      { mpz_sqrt }, /* 3 */
    };
CODE:
    assert_table (ix);
    RETVAL = new_mpz();
    (*table[ix].op) (RETVAL->m, z->m);
OUTPUT:
    RETVAL


void
overload_inc (z, d1, d2)
    mpz_assume z
    dummy      d1
    dummy      d2
ALIAS:
    GMP::Mpz::overload_dec = 1
PREINIT:
    static const struct {
      void (*op) (mpz_ptr w, mpz_srcptr x, unsigned long y);
    } table[] = {
      { mpz_add_ui }, /* 0 */
      { mpz_sub_ui }, /* 1 */
    };
CODE:
    assert_table (ix);
    (*table[ix].op) (z->m, z->m, 1L);


int
overload_spaceship (xv, yv, order)
    SV *xv
    SV *yv
    SV *order
PREINIT:
    mpz x;
CODE:
    TRACE (printf ("%s overload_spaceship\n", mpz_class));
    MPZ_ASSUME (x, xv);
    if (SvIOK(yv))
      RETVAL = mpz_cmp_si (x->m, SvIVX(yv));
    else if (SvPOKorp(yv))
      RETVAL = mpz_cmp (x->m, coerce_mpz (tmp_mpz_0, yv));
    else if (SvNOK(yv))
      RETVAL = mpz_cmp_d (x->m, SvNVX(yv));
    else if (SvROK(yv))
      {
        if (sv_derived_from (yv, mpz_class))
          RETVAL = mpz_cmp (x->m, SvMPZ(yv)->m);
        else if (sv_derived_from (yv, mpq_class))
          RETVAL = x_mpz_cmp_q (x->m, SvMPQ(yv)->m);
        else if (sv_derived_from (yv, mpf_class))
          RETVAL = x_mpz_cmp_f (x->m, SvMPF(yv));
        else
          goto invalid;
      }
    else
      {
      invalid:
        croak ("%s <=>: invalid operand", mpz_class);
      }
    RETVAL = SGN (RETVAL);
    if (order == &PL_sv_yes)
      RETVAL = -RETVAL;
OUTPUT:
    RETVAL


bool
overload_bool (z, d1, d2)
    mpz_assume z
    dummy      d1
    dummy      d2
ALIAS:
    GMP::Mpz::overload_not = 1
CODE:
    RETVAL = (mpz_sgn (z->m) != 0) ^ ix;
OUTPUT:
    RETVAL


mpz
bin (n, k)
    mpz_coerce   n
    ulong_coerce k
ALIAS:
    GMP::Mpz::root = 1
PREINIT:
    /* mpz_root returns an int, hence the cast */
    static const struct {
      void (*op) (mpz_ptr, mpz_srcptr, unsigned long);
    } table[] = {
      {                                                mpz_bin_ui }, /* 0 */
      { (void (*)(mpz_ptr, mpz_srcptr, unsigned long)) mpz_root   }, /* 1 */
    };
CODE:
    assert_table (ix);
    RETVAL = new_mpz();
    (*table[ix].op) (RETVAL->m, n, k);
OUTPUT:
    RETVAL


mpz
cdiv (a, d)
    mpz_coerce a
    mpz_coerce d
ALIAS:
    GMP::Mpz::fdiv = 1
    GMP::Mpz::tdiv = 2
PREINIT:
    static const struct {
      void (*op) (mpz_ptr, mpz_ptr, mpz_srcptr, mpz_srcptr);
    } table[] = {
      { mpz_cdiv_qr }, /* 0 */
      { mpz_fdiv_qr }, /* 1 */
      { mpz_tdiv_qr }, /* 2 */
    };
    mpz q, r;
    SV  *sv;
PPCODE:
    assert_table (ix);
    q = new_mpz();
    r = new_mpz();
    (*table[ix].op) (q->m, r->m, a, d);
    EXTEND (SP, 2);
    sv = sv_newmortal(); sv_setref_pv (sv, mpz_class, q); PUSHs(sv);
    sv = sv_newmortal(); sv_setref_pv (sv, mpz_class, r); PUSHs(sv);


mpz
cdiv_2exp (a, d)
    mpz_coerce   a
    ulong_coerce d
ALIAS:
    GMP::Mpz::fdiv_2exp = 1
    GMP::Mpz::tdiv_2exp = 2
PREINIT:
    static const struct {
      void (*q) (mpz_ptr, mpz_srcptr, unsigned long);
      void (*r) (mpz_ptr, mpz_srcptr, unsigned long);
    } table[] = {
      { mpz_cdiv_q_2exp, mpz_cdiv_r_2exp }, /* 0 */
      { mpz_fdiv_q_2exp, mpz_fdiv_r_2exp }, /* 1 */
      { mpz_tdiv_q_2exp, mpz_tdiv_r_2exp }, /* 2 */
    };
    mpz q, r;
    SV  *sv;
PPCODE:
    assert_table (ix);
    q = new_mpz();
    r = new_mpz();
    (*table[ix].q) (q->m, a, d);
    (*table[ix].r) (r->m, a, d);
    EXTEND (SP, 2);
    sv = sv_newmortal(); sv_setref_pv (sv, mpz_class, q); PUSHs(sv);
    sv = sv_newmortal(); sv_setref_pv (sv, mpz_class, r); PUSHs(sv);


bool
congruent_p (a, c, d)
    mpz_coerce a
    mpz_coerce c
    mpz_coerce d
PREINIT:
CODE:
    RETVAL = mpz_congruent_p (a, c, d);
OUTPUT:
    RETVAL


bool
congruent_2exp_p (a, c, d)
    mpz_coerce   a
    mpz_coerce   c
    ulong_coerce d
PREINIT:
CODE:
    RETVAL = mpz_congruent_2exp_p (a, c, d);
OUTPUT:
    RETVAL


mpz
divexact (a, d)
    mpz_coerce a
    mpz_coerce d
ALIAS:
    GMP::Mpz::mod = 1
PREINIT:
    static const struct {
      void (*op) (mpz_ptr, mpz_srcptr, mpz_srcptr);
    } table[] = {
      { mpz_divexact }, /* 0 */
      { mpz_mod      }, /* 1 */
    };
CODE:
    assert_table (ix);
    RETVAL = new_mpz();
    (*table[ix].op) (RETVAL->m, a, d);
OUTPUT:
    RETVAL


bool
divisible_p (a, d)
    mpz_coerce a
    mpz_coerce d
CODE:
    RETVAL = mpz_divisible_p (a, d);
OUTPUT:
    RETVAL


bool
divisible_2exp_p (a, d)
    mpz_coerce   a
    ulong_coerce d
CODE:
    RETVAL = mpz_divisible_2exp_p (a, d);
OUTPUT:
    RETVAL


bool
even_p (z)
    mpz_coerce z
ALIAS:
    GMP::Mpz::odd_p            = 1
    GMP::Mpz::perfect_square_p = 2
    GMP::Mpz::perfect_power_p  = 3
PREINIT:
    static const struct {
      int (*op) (mpz_srcptr z);
    } table[] = {
      { x_mpz_even_p         }, /* 0 */
      { x_mpz_odd_p          }, /* 1 */
      { mpz_perfect_square_p }, /* 2 */
      { mpz_perfect_power_p  }, /* 3 */
    };
CODE:
    assert_table (ix);
    RETVAL = (*table[ix].op) (z);
OUTPUT:
    RETVAL


mpz
fac (n)
    ulong_coerce n
ALIAS:
    GMP::Mpz::fib = 1
PREINIT:
    static const struct {
      void (*op) (mpz_ptr r, unsigned long n);
    } table[] = {
      { mpz_fac_ui }, /* 0 */
      { mpz_fib_ui }, /* 1 */
    };
CODE:
    assert_table (ix);
    RETVAL = new_mpz();
    (*table[ix].op) (RETVAL->m, n);
OUTPUT:
    RETVAL


mpz
gcd (x, ...)
    mpz_coerce x
ALIAS:
    GMP::Mpz::lcm = 1
PREINIT:
    static const struct {
      void (*op) (mpz_ptr w, mpz_srcptr x, mpz_srcptr y);
      void (*op_ui) (mpz_ptr w, mpz_srcptr x, unsigned long y);
    } table[] = {
      /* cast to ignore ulong return from mpz_gcd_ui */
      { mpz_gcd,
        (void (*) (mpz_ptr, mpz_srcptr, unsigned long)) mpz_gcd_ui }, /* 0 */
      { mpz_lcm, mpz_lcm_ui },                                        /* 1 */
    };
    int  i;
    SV   *yv;
CODE:
    assert_table (ix);
    RETVAL = new_mpz();
    if (items == 1)
      mpz_set (RETVAL->m, x);
    else
      {
        for (i = 1; i < items; i++)
          {
            yv = ST(i);
            if (SvIOK(yv))
              (*table[ix].op_ui) (RETVAL->m, x, ABS(SvIVX(yv)));
            else
              (*table[ix].op) (RETVAL->m, x, coerce_mpz (tmp_mpz_1, yv));
            x = RETVAL->m;
          }
      }
OUTPUT:
    RETVAL


mpz
gcdext (a, b)
    mpz_coerce a
    mpz_coerce b
PREINIT:
    mpz g, x, y;
    SV  *sv;
PPCODE:
    g = new_mpz();
    x = new_mpz();
    y = new_mpz();
    mpz_gcdext (g->m, x->m, y->m, a, b);
    EXTEND (SP, 3);
    sv = sv_newmortal(); sv_setref_pv (sv, mpz_class, g); PUSHs(sv);
    sv = sv_newmortal(); sv_setref_pv (sv, mpz_class, x); PUSHs(sv);
    sv = sv_newmortal(); sv_setref_pv (sv, mpz_class, y); PUSHs(sv);


unsigned long
hamdist (x, y)
    mpz_coerce x
    mpz_coerce y
CODE:
    RETVAL = mpz_hamdist (x, y);
OUTPUT:
    RETVAL


mpz
invert (a, m)
    mpz_coerce a
    mpz_coerce m
CODE:
    RETVAL = new_mpz();
    if (! mpz_invert (RETVAL->m, a, m))
      {
        free_mpz (RETVAL);
        XSRETURN_UNDEF;
      }
OUTPUT:
    RETVAL


int
jacobi (a, b)
    mpz_coerce a
    mpz_coerce b
CODE:
    RETVAL = mpz_jacobi (a, b);
OUTPUT:
    RETVAL


int
kronecker (a, b)
    SV *a
    SV *b
CODE:
    if (SvIOK(b))
      RETVAL = mpz_kronecker_si (coerce_mpz(tmp_mpz_0,a), SvIVX(b));
    else if (SvIOK(a))
      RETVAL = mpz_si_kronecker (SvIVX(a), coerce_mpz(tmp_mpz_0,b));
    else
      RETVAL = mpz_kronecker (coerce_mpz(tmp_mpz_0,a),
                              coerce_mpz(tmp_mpz_1,b));
OUTPUT:
    RETVAL


mpz
nextprime (z)
    mpz_coerce z
CODE:
    RETVAL = new_mpz();
    mpz_nextprime (RETVAL->m, z);
OUTPUT:
    RETVAL


unsigned long
popcount (x)
    mpz_coerce x
CODE:
    RETVAL = mpz_popcount (x);
OUTPUT:
    RETVAL


mpz
powm (b, e, m)
    mpz_coerce b
    mpz_coerce e
    mpz_coerce m
CODE:
    RETVAL = new_mpz();
    mpz_powm (RETVAL->m, b, e, m);
OUTPUT:
    RETVAL


bool
probab_prime_p (z, n)
    mpz_coerce   z
    ulong_coerce n
CODE:
    RETVAL = mpz_probab_prime_p (z, n);
OUTPUT:
    RETVAL


# No attempt to coerce here, only an mpz makes sense.
void
realloc (z, limbs)
    mpz z
    int limbs
CODE:
    _mpz_realloc (z->m, limbs);


void
remove (z, f)
    mpz_coerce z
    mpz_coerce f
PREINIT:
    SV             *sv;
    mpz            rem;
    unsigned long  mult;
    dTARG;
PPCODE:
    rem = new_mpz();
    mult = mpz_remove (rem->m, z, f);
    EXTEND (SP, 2);
    sv = sv_newmortal(); sv_setref_pv (sv, mpz_class, rem); PUSHs(sv);
    PUSHs (sv_2mortal (newSViv (mult)));


void
roote (z, n)
    mpz_coerce   z
    ulong_coerce n
PREINIT:
    SV  *sv;
    mpz root;
    int exact;
PPCODE:
    root = new_mpz();
    exact = mpz_root (root->m, z, n);
    EXTEND (SP, 2);
    sv = sv_newmortal(); sv_setref_pv (sv, mpz_class, root); PUSHs(sv);
    sv = (exact ? &PL_sv_yes : &PL_sv_no); sv_2mortal(sv); PUSHs(sv);


unsigned long
scan0 (z, start)
    mpz_coerce   z
    ulong_coerce start
ALIAS:
    GMP::Mpz::scan1 = 1
PREINIT:
    static const struct {
      unsigned long (*op) (mpz_srcptr, unsigned long);
    } table[] = {
      { mpz_scan0  }, /* 0 */
      { mpz_scan1  }, /* 1 */
    };
CODE:
    assert_table (ix);
    RETVAL = (*table[ix].op) (z, start);
OUTPUT:
    RETVAL


void
setbit (z, bit)
    mpz_mutate   z
    ulong_coerce bit
ALIAS:
    GMP::Mpz::clrbit = 1
PREINIT:
    static const struct {
      void (*op) (mpz_ptr, unsigned long);
    } table[] = {
      { mpz_setbit }, /* 0 */
      { mpz_clrbit }, /* 1 */
    };
CODE:
    TRACE (printf ("%s %s\n", mpz_class, (ix==0 ? "setbit" : "clrbit")));
    assert (SvROK(ST(0)) && SvREFCNT(SvRV(ST(0))) == 1);
    assert_table (ix);
    (*table[ix].op) (z, bit);


void
sqrtrem (z)
    mpz_coerce z
PREINIT:
    SV  *sv;
    mpz root;
    mpz rem;
PPCODE:
    root = new_mpz();
    rem = new_mpz();
    mpz_sqrtrem (root->m, rem->m, z);
    EXTEND (SP, 2);
    sv = sv_newmortal(); sv_setref_pv (sv, mpz_class, root); PUSHs(sv);
    sv = sv_newmortal(); sv_setref_pv (sv, mpz_class, rem);  PUSHs(sv);


size_t
sizeinbase (z, base)
    mpz_coerce z
    int        base
CODE:
    RETVAL = mpz_sizeinbase (z, base);
OUTPUT:
    RETVAL


int
tstbit (z, bit)
    mpz_coerce   z
    ulong_coerce bit
CODE:
    RETVAL = mpz_tstbit (z, bit);
OUTPUT:
    RETVAL



#------------------------------------------------------------------------------

MODULE = GMP         PACKAGE = GMP::Mpq


mpq
mpq (...)
ALIAS:
    GMP::Mpq::new = 1
PREINIT:
CODE:
    TRACE (printf ("%s new\n", mpq_class));
    RETVAL = new_mpq();

    switch (items) {
    case 0:
      mpq_set_ui (RETVAL->m, 0L, 1L);
      break;
    case 1:
      coerce_mpq_into (RETVAL->m, ST(0));
      break;
    case 2:
      coerce_mpz_into (mpq_numref(RETVAL->m), ST(0));
      coerce_mpz_into (mpq_denref(RETVAL->m), ST(1));
      break;
    default:
      croak ("%s new: invalid arguments", mpq_class);
    }
OUTPUT:
    RETVAL


void
overload_constant (str, pv, d1, ...)
    const_string_assume str
    SV                  *pv
    dummy               d1
PREINIT:
    SV  *sv;
    mpq q;
PPCODE:
    TRACE (printf ("%s constant: %s\n", mpq_class, str));
    q = new_mpq();
    if (mpq_set_str (q->m, str, 0) == 0)
      { sv = sv_newmortal(); sv_setref_pv (sv, mpq_class, q); }
    else
      { free_mpq (q); sv = pv; }
    XPUSHs(sv);


mpq
overload_copy (q, d1, d2)
    mpq_assume q
    dummy      d1
    dummy      d2
CODE:
    RETVAL = new_mpq();
    mpq_set (RETVAL->m, q->m);
OUTPUT:
    RETVAL


void
DESTROY (q)
    mpq_assume q
CODE:
    TRACE (printf ("%s DESTROY %p\n", mpq_class, q));
    free_mpq (q);


malloced_string
overload_string (q, d1, d2)
    mpq_assume q
    dummy      d1
    dummy      d2
CODE:
    TRACE (printf ("%s overload_string %p\n", mpq_class, q));
    RETVAL = mpq_get_str (NULL, 10, q->m);
OUTPUT:
    RETVAL


mpq
overload_add (xv, yv, order)
    SV *xv
    SV *yv
    SV *order
ALIAS:
    GMP::Mpq::overload_sub   = 1
    GMP::Mpq::overload_mul   = 2
    GMP::Mpq::overload_div   = 3
PREINIT:
    static const struct {
      void (*op) (mpq_ptr, mpq_srcptr, mpq_srcptr);
    } table[] = {
      { mpq_add }, /* 0 */
      { mpq_sub }, /* 1 */
      { mpq_mul }, /* 2 */
      { mpq_div }, /* 3 */
    };
CODE:
    TRACE (printf ("%s binary\n", mpf_class));
    assert_table (ix);
    if (order == &PL_sv_yes)
      SV_PTR_SWAP (xv, yv);
    RETVAL = new_mpq();
    (*table[ix].op) (RETVAL->m,
                     coerce_mpq (tmp_mpq_0, xv),
                     coerce_mpq (tmp_mpq_1, yv));
OUTPUT:
    RETVAL


void
overload_addeq (x, y, o)
    mpq_assume   x
    mpq_coerce   y
    order_noswap o
ALIAS:
    GMP::Mpq::overload_subeq = 1
    GMP::Mpq::overload_muleq = 2
    GMP::Mpq::overload_diveq = 3
PREINIT:
    static const struct {
      void (*op) (mpq_ptr, mpq_srcptr, mpq_srcptr);
    } table[] = {
      { mpq_add    }, /* 0 */
      { mpq_sub    }, /* 1 */
      { mpq_mul    }, /* 2 */
      { mpq_div    }, /* 3 */
    };
PPCODE:
    assert_table (ix);
    (*table[ix].op) (x->m, x->m, y);
    XPUSHs(ST(0));


mpq
overload_lshift (qv, nv, order)
    SV *qv
    SV *nv
    SV *order
ALIAS:
    GMP::Mpq::overload_rshift   = 1
    GMP::Mpq::overload_pow      = 2
PREINIT:
    static const struct {
      void (*op) (mpq_ptr, mpq_srcptr, unsigned long);
    } table[] = {
      { mpq_mul_2exp }, /* 0 */
      { mpq_div_2exp }, /* 1 */
      { x_mpq_pow_ui }, /* 2 */
    };
CODE:
    assert_table (ix);
    if (order == &PL_sv_yes)
      SV_PTR_SWAP (qv, nv);
    RETVAL = new_mpq();
    (*table[ix].op) (RETVAL->m, coerce_mpq (RETVAL->m, qv), coerce_ulong (nv));
OUTPUT:
    RETVAL


void
overload_lshifteq (q, n, o)
    mpq_assume   q
    ulong_coerce n
    order_noswap o
ALIAS:
    GMP::Mpq::overload_rshifteq   = 1
    GMP::Mpq::overload_poweq      = 2
PREINIT:
    static const struct {
      void (*op) (mpq_ptr, mpq_srcptr, unsigned long);
    } table[] = {
      { mpq_mul_2exp }, /* 0 */
      { mpq_div_2exp }, /* 1 */
      { x_mpq_pow_ui }, /* 2 */
    };
PPCODE:
    assert_table (ix);
    (*table[ix].op) (q->m, q->m, n);
    XPUSHs(ST(0));


void
overload_inc (q, d1, d2)
    mpq_assume q
    dummy      d1
    dummy      d2
ALIAS:
    GMP::Mpq::overload_dec = 1
PREINIT:
    static const struct {
      void (*op) (mpz_ptr, mpz_srcptr, mpz_srcptr);
    } table[] = {
      { mpz_add }, /* 0 */
      { mpz_sub }, /* 1 */
    };
CODE:
    assert_table (ix);
    (*table[ix].op) (mpq_numref(q->m), mpq_numref(q->m), mpq_denref(q->m));


mpq
overload_abs (q, d1, d2)
    mpq_assume q
    dummy      d1
    dummy      d2
ALIAS:
    GMP::Mpq::overload_neg = 1
PREINIT:
    static const struct {
      void (*op) (mpq_ptr w, mpq_srcptr x);
    } table[] = {
      { mpq_abs }, /* 0 */
      { mpq_neg }, /* 1 */
    };
CODE:
    assert_table (ix);
    RETVAL = new_mpq();
    (*table[ix].op) (RETVAL->m, q->m);
OUTPUT:
    RETVAL


int
overload_spaceship (x, y, order)
    mpq_assume x
    mpq_coerce y
    SV         *order
CODE:
    RETVAL = mpq_cmp (x->m, y);
    RETVAL = SGN (RETVAL);
    if (order == &PL_sv_yes)
      RETVAL = -RETVAL;
OUTPUT:
    RETVAL


bool
overload_bool (q, d1, d2)
    mpq_assume q
    dummy      d1
    dummy      d2
ALIAS:
    GMP::Mpq::overload_not = 1
CODE:
    RETVAL = (mpq_sgn (q->m) != 0) ^ ix;
OUTPUT:
    RETVAL


bool
overload_eq (x, yv, d)
    mpq_assume x
    SV         *yv
    dummy      d
ALIAS:
    GMP::Mpq::overload_ne = 1
CODE:
    if (SvIOK(yv))
      RETVAL = x_mpq_equal_si (x->m, SvIVX(yv), 1L);
    else if (SvROK(yv))
      {
        if (sv_derived_from (yv, mpz_class))
          RETVAL = x_mpq_equal_z (x->m, SvMPZ(yv)->m);
        else if (sv_derived_from (yv, mpq_class))
          RETVAL = mpq_equal (x->m, SvMPQ(yv)->m);
        else
          goto coerce;
      }
    else
      {
      coerce:
        RETVAL = mpq_equal (x->m, coerce_mpq (tmp_mpq_0, yv));
      }
    RETVAL ^= ix;
OUTPUT:
    RETVAL


void
canonicalize (q)
    mpq q
CODE:
    mpq_canonicalize (q->m);


mpq
inv (q)
    mpq_coerce q
CODE:
    RETVAL = new_mpq();
    mpq_inv (RETVAL->m, q);
OUTPUT:
    RETVAL


mpz
num (q)
    mpq q
ALIAS:
    GMP::Mpq::den = 1
CODE:
    RETVAL = new_mpz();
    mpz_set (RETVAL->m, (ix == 0 ? mpq_numref(q->m) : mpq_denref(q->m)));
OUTPUT:
    RETVAL



#------------------------------------------------------------------------------

MODULE = GMP         PACKAGE = GMP::Mpf


mpf
mpf (...)
ALIAS:
    GMP::Mpf::new = 1
PREINIT:
    unsigned long  prec;
CODE:
    TRACE (printf ("%s new\n", mpf_class));
    if (items > 2)
      croak ("%s new: invalid arguments", mpf_class);
    prec = (items == 2 ? coerce_ulong (ST(1)) : mpf_get_default_prec());
    RETVAL = new_mpf (prec);
    if (items >= 1)
      my_mpf_set_sv (RETVAL, ST(0));
OUTPUT:
    RETVAL


mpf
overload_constant (sv, d1, d2, ...)
    SV     *sv
    dummy  d1
    dummy  d2
PREINIT:
    mpf f;
CODE:
    assert (SvPOK (sv));
    TRACE (printf ("%s constant: %s\n", mpq_class, SvPVX(sv)));
    RETVAL = new_mpf (mpf_get_default_prec());
    my_mpf_set_svstr (RETVAL, sv);
OUTPUT:
    RETVAL


mpf
overload_copy (f, d1, d2)
    mpf_assume f
    dummy      d1
    dummy      d2
CODE:
    TRACE (printf ("%s copy\n", mpf_class));
    RETVAL = new_mpf (mpf_get_prec (f));
    mpf_set (RETVAL, f);
OUTPUT:
    RETVAL


void
DESTROY (f)
    mpf_assume f
CODE:
    TRACE (printf ("%s DESTROY %p\n", mpf_class, f));
    mpf_clear (f);
    Safefree (f);
    assert_support (mpf_count--);
    TRACE_ACTIVE ();


mpf
overload_add (x, y, order)
    mpf_assume     x
    mpf_coerce_st0 y
    SV             *order
ALIAS:
    GMP::Mpf::overload_sub   = 1
    GMP::Mpf::overload_mul   = 2
    GMP::Mpf::overload_div   = 3
PREINIT:
    static const struct {
      void (*op) (mpf_ptr, mpf_srcptr, mpf_srcptr);
    } table[] = {
      { mpf_add }, /* 0 */
      { mpf_sub }, /* 1 */
      { mpf_mul }, /* 2 */
      { mpf_div }, /* 3 */
    };
    unsigned long prec;
CODE:
    assert_table (ix);
    RETVAL = new_mpf (mpf_get_prec (x));
    if (order == &PL_sv_yes)
      MPF_PTR_SWAP (x, y);
    (*table[ix].op) (RETVAL, x, y);
OUTPUT:
    RETVAL


void
overload_addeq (x, y, o)
    mpf_assume     x
    mpf_coerce_st0 y
    order_noswap   o
ALIAS:
    GMP::Mpf::overload_subeq = 1
    GMP::Mpf::overload_muleq = 2
    GMP::Mpf::overload_diveq = 3
PREINIT:
    static const struct {
      void (*op) (mpf_ptr, mpf_srcptr, mpf_srcptr);
    } table[] = {
      { mpf_add }, /* 0 */
      { mpf_sub }, /* 1 */
      { mpf_mul }, /* 2 */
      { mpf_div }, /* 3 */
    };
PPCODE:
    assert_table (ix);
    (*table[ix].op) (x, x, y);
    XPUSHs(ST(0));


mpf
overload_lshift (fv, nv, order)
    SV *fv
    SV *nv
    SV *order
ALIAS:
    GMP::Mpf::overload_rshift = 1
    GMP::Mpf::overload_pow    = 2
PREINIT:
    static const struct {
      void (*op) (mpf_ptr, mpf_srcptr, unsigned long);
    } table[] = {
      { mpf_mul_2exp }, /* 0 */
      { mpf_div_2exp }, /* 1 */
      { mpf_pow_ui   }, /* 2 */
    };
    mpf f;
    unsigned long prec;
CODE:
    assert_table (ix);
    MPF_ASSUME (f, fv);
    prec = mpf_get_prec (f);
    if (order == &PL_sv_yes)
      SV_PTR_SWAP (fv, nv);
    f = coerce_mpf (tmp_mpf_0, fv, prec);
    RETVAL = new_mpf (prec);
    (*table[ix].op) (RETVAL, f, coerce_ulong (nv));
OUTPUT:
    RETVAL


void
overload_lshifteq (f, n, o)
    mpf_assume   f
    ulong_coerce n
    order_noswap o
ALIAS:
    GMP::Mpf::overload_rshifteq   = 1
    GMP::Mpf::overload_poweq      = 2
PREINIT:
    static const struct {
      void (*op) (mpf_ptr, mpf_srcptr, unsigned long);
    } table[] = {
      { mpf_mul_2exp }, /* 0 */
      { mpf_div_2exp }, /* 1 */
      { mpf_pow_ui   }, /* 2 */
    };
PPCODE:
    assert_table (ix);
    (*table[ix].op) (f, f, n);
    XPUSHs(ST(0));


mpf
overload_abs (f, d1, d2)
    mpf_assume f
    dummy      d1
    dummy      d2
ALIAS:
    GMP::Mpf::overload_neg   = 1
    GMP::Mpf::overload_sqrt  = 2
PREINIT:
    static const struct {
      void (*op) (mpf_ptr w, mpf_srcptr x);
    } table[] = {
      { mpf_abs  }, /* 0 */
      { mpf_neg  }, /* 1 */
      { mpf_sqrt }, /* 2 */
    };
CODE:
    assert_table (ix);
    RETVAL = new_mpf (mpf_get_prec (f));
    (*table[ix].op) (RETVAL, f);
OUTPUT:
    RETVAL


void
overload_inc (f, d1, d2)
    mpf_assume f
    dummy      d1
    dummy      d2
ALIAS:
    GMP::Mpf::overload_dec = 1
PREINIT:
    static const struct {
      void (*op) (mpf_ptr w, mpf_srcptr x, unsigned long y);
    } table[] = {
      { mpf_add_ui }, /* 0 */
      { mpf_sub_ui }, /* 1 */
    };
CODE:
    assert_table (ix);
    (*table[ix].op) (f, f, 1L);


int
overload_spaceship (xv, yv, order)
    SV *xv
    SV *yv
    SV *order
PREINIT:
    mpf x;
CODE:
    MPF_ASSUME (x, xv);
    if (SvIOK(yv))
      RETVAL = mpf_cmp_si (x, SvIVX(yv));
    else if (SvNOK(yv))
      RETVAL = mpf_cmp_d (x, SvNVX(yv));
    else if (SvPOKorp(yv))
      {
        STRLEN len;
        const char *str = SvPV (yv, len);
        /* enough for all digits of the string */
        tmp_mpf_set_prec (tmp_mpf_0, strlen(str)+64);
        if (mpf_set_str (tmp_mpf_0->m, str, 10) != 0)
          croak ("%s <=>: invalid string format", mpf_class);
        RETVAL = mpf_cmp (x, tmp_mpf_0->m);
      }
    else if (SvROK(yv))
      {
        if (sv_derived_from (yv, mpz_class))
          RETVAL = - x_mpz_cmp_f (SvMPZ(yv)->m, x);
        else if (sv_derived_from (yv, mpf_class))
          RETVAL = mpf_cmp (x, SvMPF(yv));
        else
          goto use_mpq;
      }
    else
      {
      use_mpq:
        RETVAL = mpq_cmp (coerce_mpq (tmp_mpq_0, xv),
                          coerce_mpq (tmp_mpq_1, yv));
      }
    RETVAL = SGN (RETVAL);
    if (order == &PL_sv_yes)
      RETVAL = -RETVAL;
OUTPUT:
    RETVAL


bool
overload_bool (f, d1, d2)
    mpf_assume f
    dummy      d1
    dummy      d2
ALIAS:
    GMP::Mpf::overload_not = 1
CODE:
    RETVAL = (mpf_sgn (f) != 0) ^ ix;
OUTPUT:
    RETVAL


mpf
ceil (f)
    mpf_coerce_def f
ALIAS:
    GMP::Mpf::floor = 1
    GMP::Mpf::trunc = 2
PREINIT:
    static const struct {
      void (*op) (mpf_ptr w, mpf_srcptr x);
    } table[] = {
      { mpf_ceil  }, /* 0 */
      { mpf_floor }, /* 1 */
      { mpf_trunc }, /* 2 */
    };
CODE:
    assert_table (ix);
    RETVAL = new_mpf (mpf_get_prec (f));
    (*table[ix].op) (RETVAL, f);
OUTPUT:
    RETVAL


unsigned long
get_default_prec ()
CODE:
    RETVAL = mpf_get_default_prec();
OUTPUT:
    RETVAL


unsigned long
get_prec (f)
    mpf_coerce_def f
CODE:
    RETVAL = mpf_get_prec (f);
OUTPUT:
    RETVAL


bool
mpf_eq (xv, yv, bits)
    SV           *xv
    SV           *yv
    ulong_coerce bits
PREINIT:
    mpf  x, y;
    unsigned long  prec;
CODE:
    TRACE (printf ("%s eq\n", mpf_class));
    COERCE_MPF_PAIR (prec, x,xv, y,yv);
    RETVAL = mpf_eq (x, y, bits);
OUTPUT:
    RETVAL


void
mpf_get_str (f, base, digits)
    mpf_coerce_def f
    int            base
    int            digits
PREINIT:
    char      *str;
    mp_exp_t  exp;
PPCODE:
    str = mpf_get_str (NULL, &exp, base, digits, f);
    EXTEND (SP, 2);
    PUSHs (sv_2mortal (newSVpv (str, 0)));
    PUSHs (sv_2mortal (newSViv (exp)));


mpf
reldiff (xv, yv)
    SV *xv
    SV *yv
PREINIT:
    mpf  x, y;
    unsigned long prec;
CODE:
    TRACE (printf ("%s reldiff\n", mpf_class));
    COERCE_MPF_PAIR (prec, x,xv, y,yv);
    RETVAL = new_mpf (prec);
    mpf_reldiff (RETVAL, x, y);
OUTPUT:
    RETVAL


void
set_default_prec (prec)
    ulong_coerce prec
CODE:
    TRACE (printf ("%s set_default_prec %lu\n", mpf_class, prec));
    mpf_set_default_prec (prec);


void
set_prec (sv, prec)
    SV           *sv
    ulong_coerce prec
PREINIT:
    mpf_ptr  old_f, new_f;
CODE:
    TRACE (printf ("%s set_prec to %lu\n", mpf_class, prec));
    if (SvROK (sv) && sv_derived_from (sv, mpf_class))
      {
        old_f = SvMPF(sv);
        if (SvREFCNT(SvRV(sv)) == 1)
          mpf_set_prec (old_f, prec);
        else
          {
            TRACE (printf ("  fork new mpf\n"));
            new_f = new_mpf (prec);
            mpf_set (new_f, old_f);
            goto setref;
          }
      }
    else
      {
        TRACE (printf ("  coerce to mpf\n"));
        new_f = new_mpf (prec);
        my_mpf_set_sv (new_f, sv);
      setref:
        sv_setref_pv (sv, mpf_class, new_f);
      }



#------------------------------------------------------------------------------

MODULE = GMP         PACKAGE = GMP::Rand

randstate
new (...)
ALIAS:
    GMP::Rand::randstate = 1
CODE:
    TRACE (printf ("%s new\n", rand_class));
    New (GMP_MALLOC_ID, RETVAL, 1, __gmp_randstate_struct);
    TRACE (printf ("  RETVAL %p\n", RETVAL));
    assert_support (rand_count++);
    TRACE_ACTIVE ();

    if (items == 0)
      gmp_randinit (RETVAL, GMP_RAND_ALG_DEFAULT, 128);
    else
      {
        STRLEN      len;
        const char  *method = SvPV (ST(0), len);
        if (strcmp (method, "lc") == 0)
          {
            if (items != 2)
              goto invalid;
            gmp_randinit (RETVAL, GMP_RAND_ALG_LC, coerce_ulong (ST(1)));
          }
        else if (strncmp (method, "lc2exp", len) == 0)
          {
            if (items != 4)
              goto invalid;
            gmp_randinit_lc_2exp (RETVAL,
                                  coerce_mpz (tmp_mpz_0, ST(1)),
                                  coerce_ulong (ST(2)),
                                  coerce_ulong (ST(3)));
          }
        else
          {
          invalid:
            croak ("%s new: invalid arguments", rand_class);
          }
      }
OUTPUT:
    RETVAL


void
DESTROY (r)
    randstate r
CODE:
    TRACE (printf ("%s DESTROY\n", rand_class));
    gmp_randclear (r);
    Safefree (r);
    assert_support (rand_count--);
    TRACE_ACTIVE ();


void
seed (r, z)
    randstate  r
    mpz_coerce z
CODE:
    gmp_randseed (r, z);


mpz
mpz_urandomb (r, bits)
    randstate    r
    ulong_coerce bits
ALIAS:
    GMP::Rand::mpz_rrandomb = 1
PREINIT:
    static const struct {
      void (*fun) (mpz_ptr, gmp_randstate_t r, unsigned long bits);
    } table[] = {
      { mpz_urandomb }, /* 0 */
      { mpz_rrandomb }, /* 1 */
    };
CODE:
    assert_table (ix);
    RETVAL = new_mpz();
    (*table[ix].fun) (RETVAL->m, r, bits);
OUTPUT:
    RETVAL


mpz
mpz_urandomm (r, m)
    randstate  r
    mpz_coerce m
CODE:
    RETVAL = new_mpz();
    mpz_urandomm (RETVAL->m, r, m);
OUTPUT:
    RETVAL


mpf
mpf_urandomb (r, bits)
    randstate    r
    ulong_coerce bits
CODE:
    RETVAL = new_mpf (bits);
    mpf_urandomb (RETVAL, r, bits);
OUTPUT:
    RETVAL
