/* Header for speed and threshold things. */

/*
Copyright (C) 1999, 2000 Free Software Foundation, Inc.

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

#ifndef __SPEED_H__
#define __SPEED_H__


/* sizes of temporary space required */
#define MPN_KARA_MUL_N_TSIZE(n)  (2*((n)+BITS_PER_MP_LIMB))
#define MPN_KARA_SQR_N_TSIZE(n)  (2*((n)+BITS_PER_MP_LIMB))
#define MPN_TOOM3_MUL_N_TSIZE(n) (2*(n) + 3*BITS_PER_MP_LIMB)
#define MPN_TOOM3_SQR_N_TSIZE(n) (2*((n) + BITS_PER_MP_LIMB))


/* Pad ptr,oldsize with zero limbs (at the most significant end) to make it
   newsize long. */
#define MPN_ZERO_EXTEND(ptr, oldsize, newsize)          \
  do {                                                  \
    ASSERT ((newsize) >= (oldsize));                    \
    MPN_ZERO ((ptr)+(oldsize), (newsize)-(oldsize));    \
  } while (0)

/* A mask of the least significant n bits.  Note 1<<32 doesn't give zero on
   x86 family CPUs, hence the separate case for BITS_PER_MP_LIMB. */
#define MP_LIMB_T_LOWBITMASK(n) \
  ((n) == BITS_PER_MP_LIMB ? MP_LIMB_T_MAX : ((mp_limb_t) 1 << (n)) - 1)


/* align must be a power of 2 here, usually CACHE_LINE_SIZE is a good choice */

#define TMP_ALLOC_ALIGNED(bytes, align) \
  align_pointer (TMP_ALLOC ((bytes) + (align)-1), (align))
#define TMP_ALLOC_LIMBS_ALIGNED(limbs, align) \
  ((mp_ptr) TMP_ALLOC_ALIGNED ((limbs)*sizeof(mp_limb_t), align))

/* 32 for pentium, 64 for athlon, might want to configure this for other
   CPUs.  In truth though nothing has yet shown up that cares about cache
   line boundaries.  The only practical effect of this is to restrict the
   range that s->align_xp can take.  Perhaps this could be a variable
   instead. */
#define CACHE_LINE_SIZE    64 /* bytes */

#define SPEED_TMP_ALLOC_ADJUST_MASK  (CACHE_LINE_SIZE/BYTES_PER_MP_LIMB - 1)

#define SPEED_TMP_ALLOC_LIMBS(limbs, align) \
  (speed_tmp_alloc_adjust             \
    (TMP_ALLOC_LIMBS((limbs) + SPEED_TMP_ALLOC_ADJUST_MASK), (align)))


/* This is the size for s->xp_block and s->yp_block, used in certain
   routines that want to run across many different data values and use
   s->size for a different purpose, eg. SPEED_ROUTINE_MPN_GCD_1.

   512 means 2kbytes of data for each of xp_block and yp_block, making 4k
   total, which should fit easily in any L1 data cache. */

#define SPEED_BLOCK_SIZE   512 /* limbs */


extern double  speed_unittime;
extern double  speed_cycletime;
extern int     speed_precision;
extern const char *speed_time_string;
void speed_time_init _PROTO ((void));
void speed_starttime _PROTO ((void));
double speed_endtime _PROTO ((void));

struct speed_params {
  unsigned   reps;      /* how many times to run the routine */
  mp_ptr     xp;        /* first argument */
  mp_ptr     yp;        /* second argument */
  mp_size_t  size;      /* size of both arguments */
  long       r;         /* user supplied parameter */
  mp_size_t  align_xp;  /* alignment of xp */
  mp_size_t  align_yp;  /* alignment of yp */
  mp_size_t  align_wp;  /* intended alignment of wp */
  mp_size_t  align_wp2; /* intended alignment of wp2 */
  mp_ptr     xp_block;  /* first special SPEED_BLOCK_SIZE block */
  mp_ptr     yp_block;  /* second special SPEED_BLOCK_SIZE block */

  double     time_divisor; /* optionally set by the speed routine */
  
  /* used by the cache priming things */
  int        cache;
  unsigned   src_num, dst_num;
  struct {
    mp_ptr    ptr;
    mp_size_t size;
  } src[2], dst[3];
};

typedef double (*speed_function_t) _PROTO ((struct speed_params *s));

double speed_measure _PROTO ((speed_function_t fun, struct speed_params *s));

/* Prototypes for speed measuring routines */

double speed_malloc_free _PROTO ((struct speed_params *s));
double speed_malloc_realloc_free _PROTO ((struct speed_params *s));
double speed_memcpy _PROTO ((struct speed_params *s));
double speed_modlimb_invert _PROTO ((struct speed_params *s));
double speed_mp_allocate_free _PROTO ((struct speed_params *s));
double speed_mp_allocate_reallocate_free _PROTO ((struct speed_params *s));

double speed_mpf_init_clear _PROTO ((struct speed_params *s));

double speed_mpn_add_n _PROTO ((struct speed_params *s));
double speed_mpn_add_n_self _PROTO ((struct speed_params *s));
double speed_mpn_add_n_inplace _PROTO ((struct speed_params *s));
double speed_mpn_and_n _PROTO ((struct speed_params *s));
double speed_mpn_andn_n _PROTO ((struct speed_params *s));
double speed_mpn_addmul_1 _PROTO ((struct speed_params *s));
double speed_mpn_bz_divrem_n _PROTO ((struct speed_params *s));
double speed_mpn_bz_divrem_sb _PROTO ((struct speed_params *s));
double speed_mpn_bz_tdiv_qr _PROTO ((struct speed_params *s));
double speed_MPN_COPY _PROTO ((struct speed_params *s));
double speed_MPN_COPY_DECR _PROTO ((struct speed_params *s));
double speed_MPN_COPY_INCR _PROTO ((struct speed_params *s));
double speed_mpn_divexact_by3 _PROTO ((struct speed_params *s));
double speed_mpn_divmod_1 _PROTO ((struct speed_params *s));
double speed_mpn_divrem_1 _PROTO ((struct speed_params *s));
double speed_mpn_divrem_1f _PROTO ((struct speed_params *s));
double speed_mpn_divrem_1c _PROTO ((struct speed_params *s));
double speed_mpn_divrem_1cf _PROTO ((struct speed_params *s));
double speed_mpn_divrem_2 _PROTO ((struct speed_params *s));
double speed_mpn_gcd _PROTO ((struct speed_params *s));
double speed_mpn_gcd_1 _PROTO ((struct speed_params *s));
double speed_mpn_gcdext _PROTO ((struct speed_params *s));
double speed_mpn_hamdist _PROTO ((struct speed_params *s));
double speed_mpn_ior_n _PROTO ((struct speed_params *s));
double speed_mpn_iorn_n _PROTO ((struct speed_params *s));
double speed_mpn_jacobi_base _PROTO ((struct speed_params *s));
double speed_mpn_kara_mul_n _PROTO ((struct speed_params *s));
double speed_mpn_kara_sqr_n _PROTO ((struct speed_params *s));
double speed_mpn_lshift _PROTO ((struct speed_params *s));
double speed_mpn_mod_1 _PROTO ((struct speed_params *s));
double speed_mpn_mod_1c _PROTO ((struct speed_params *s));
double speed_mpn_mul_1 _PROTO ((struct speed_params *s));
double speed_mpn_mul_basecase _PROTO ((struct speed_params *s));
double speed_mpn_mul_fft _PROTO ((struct speed_params *s));
double speed_mpn_mul_fft_sqr _PROTO ((struct speed_params *s));
double speed_mpn_mul_fft_full _PROTO ((struct speed_params *s));
double speed_mpn_mul_fft_full_sqr _PROTO ((struct speed_params *s));
double speed_mpn_mul_n _PROTO ((struct speed_params *s));
double speed_mpn_mul_n_sqr _PROTO ((struct speed_params *s));
double speed_mpn_mul_n_toom3 _PROTO ((struct speed_params *s));
double speed_mpn_nand_n _PROTO ((struct speed_params *s));
double speed_mpn_nior_n _PROTO ((struct speed_params *s));
double speed_mpn_popcount _PROTO ((struct speed_params *s));
double speed_mpn_rshift _PROTO ((struct speed_params *s));
double speed_mpn_sqr_basecase _PROTO ((struct speed_params *s));
double speed_mpn_sqr_n _PROTO ((struct speed_params *s));
double speed_mpn_sqr_toom3 _PROTO ((struct speed_params *s));
double speed_mpn_sub_n _PROTO ((struct speed_params *s));
double speed_mpn_submul_1 _PROTO ((struct speed_params *s));
double speed_mpn_toom3_mul_n _PROTO ((struct speed_params *s));
double speed_mpn_toom3_sqr_n _PROTO ((struct speed_params *s));
double speed_mpn_udiv_qrnnd _PROTO ((struct speed_params *s));
double speed_mpn_umul_ppmm _PROTO ((struct speed_params *s));
double speed_mpn_xnor_n _PROTO ((struct speed_params *s));
double speed_mpn_xor_n _PROTO ((struct speed_params *s));

double speed_mpq_init_clear _PROTO ((struct speed_params *s));

double speed_mpz_add _PROTO ((struct speed_params *s));
double speed_mpz_bin_uiui _PROTO ((struct speed_params *s));
double speed_mpz_fac_ui _PROTO ((struct speed_params *s));
double speed_mpz_fib_ui _PROTO ((struct speed_params *s));
double speed_mpz_init_clear _PROTO ((struct speed_params *s));
double speed_mpz_init_realloc_clear _PROTO ((struct speed_params *s));
double speed_mpz_powm _PROTO ((struct speed_params *s));

double speed_noop _PROTO ((struct speed_params *s));
double speed_noop_wxs _PROTO ((struct speed_params *s));
double speed_noop_wxys _PROTO ((struct speed_params *s));

double speed_udiv_qrnnd _PROTO ((struct speed_params *s));
double speed_udiv_qrnnd_preinv _PROTO ((struct speed_params *s));
double speed_udiv_qrnnd_preinv2norm _PROTO ((struct speed_params *s));
double speed_umul_ppmm _PROTO ((struct speed_params *s));


/* Prototypes for other routines */

/* low 32-bits in p[0], high 32-bits in p[1] */
void speed_cyclecounter _PROTO ((unsigned p[2]));

void pentium_wbinvd _PROTO ((void));

void noop _PROTO ((void));
void noop_1 _PROTO ((mp_limb_t n));
void noop_wxs _PROTO ((mp_ptr wp, mp_srcptr xp, mp_size_t size));
void noop_wxys _PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                        mp_size_t size));
void mpn_cache_fill _PROTO ((mp_srcptr ptr, mp_size_t size));
void mpn_cache_fill_dummy _PROTO ((mp_limb_t n));
mp_ptr speed_tmp_alloc_adjust _PROTO ((void *ptr, mp_size_t align));
void *_mp_allocate_or_reallocate _PROTO ((void *ptr,
                                          size_t oldsize, size_t newsize));
void *align_pointer _PROTO ((void *p, size_t align));
void *_mp_allocate_func_aligned _PROTO ((size_t bytes, size_t align));
void speed_cache_fill _PROTO ((struct speed_params *s));
void speed_operand_src _PROTO ((struct speed_params *s,
                                mp_ptr ptr, mp_size_t size));
void speed_operand_dst _PROTO ((struct speed_params *s,
                                mp_ptr ptr, mp_size_t size));
void mpz_set_n _PROTO ((mpz_ptr z, mp_srcptr p, mp_size_t size));

extern int  speed_option_addrs;
void speed_option_set _PROTO((const char *s));


#define SPEED_RESTRICT_COND(cond)   if (!(cond)) return -1.0;

/* For mpn_copy or similar. */
#define SPEED_ROUTINE_MPN_COPY_CALL(call)               \
  {                                                     \
    mp_ptr    wp;                                       \
    unsigned  i;                                        \
    double    t;                                        \
    TMP_DECL (marker);                                  \
                                                        \
    SPEED_RESTRICT_COND (s->size >= 0);                 \
                                                        \
    TMP_MARK (marker);                                  \
    wp = SPEED_TMP_ALLOC_LIMBS (s->size, s->align_wp);  \
                                                        \
    speed_operand_src (s, s->xp, s->size);              \
    speed_operand_dst (s, wp, s->size);                 \
    speed_cache_fill (s);                               \
                                                        \
    speed_starttime ();                                 \
    i = s->reps;                                        \
    do                                                  \
      call;                                             \
    while (--i != 0);                                   \
    t = speed_endtime ();                               \
                                                        \
    TMP_FREE (marker);                                  \
    return t;                                           \
  }  

#define SPEED_ROUTINE_MPN_COPY(function) \
  SPEED_ROUTINE_MPN_COPY_CALL(function (wp, s->xp, s->size))
#define SPEED_ROUTINE_MPN_COPYC(function) \
  SPEED_ROUTINE_MPN_COPY_CALL(function (wp, s->xp, s->size, 0))


/* For mpn_add_n, mpn_sub_n, or similar. */
#define SPEED_ROUTINE_MPN_BINARY_N_CALL(call)           \
  {                                                     \
    mp_ptr    wp;                                       \
    unsigned  i;                                        \
    double    t;                                        \
    TMP_DECL (marker);                                  \
                                                        \
    SPEED_RESTRICT_COND (s->size >= 1);                 \
                                                        \
    TMP_MARK (marker);                                  \
    wp = SPEED_TMP_ALLOC_LIMBS (s->size, s->align_wp);  \
                                                        \
    speed_operand_src (s, s->xp, s->size);              \
    speed_operand_src (s, s->yp, s->size);              \
    speed_operand_dst (s, wp, s->size);                 \
    speed_cache_fill (s);                               \
                                                        \
    speed_starttime ();                                 \
    i = s->reps;                                        \
    do                                                  \
      call;                                             \
    while (--i != 0);                                   \
    t = speed_endtime ();                               \
                                                        \
    TMP_FREE (marker);                                  \
    return t;                                           \
  }  

#define SPEED_ROUTINE_MPN_BINARY_N(function) \
   SPEED_ROUTINE_MPN_BINARY_N_CALL ((*function) (wp, s->xp, s->yp, s->size))

#define SPEED_ROUTINE_MPN_BINARY_NC(function) \
   SPEED_ROUTINE_MPN_BINARY_N_CALL ((*function) (wp, s->xp, s->yp, s->size, 0))

#define SPEED_ROUTINE_MPN_BINARY_N_SELF(function) \
   SPEED_ROUTINE_MPN_BINARY_N_CALL ((*function) (wp, s->xp, s->xp, s->size))

#define SPEED_ROUTINE_MPN_BINARY_N_INPLACE(function) \
   SPEED_ROUTINE_MPN_BINARY_N_CALL ((*function) (wp, wp, s->xp, s->size))


/* For mpn_lshift, mpn_rshift, mpn_mul_1, with r, or similar. */
#define SPEED_ROUTINE_MPN_UNARY_1_CALL(call)            \
  {                                                     \
    mp_ptr    wp;                                       \
    unsigned  i;                                        \
    double    t;                                        \
    TMP_DECL (marker);                                  \
                                                        \
    SPEED_RESTRICT_COND (s->size >= 1);                 \
                                                        \
    TMP_MARK (marker);                                  \
    wp = SPEED_TMP_ALLOC_LIMBS (s->size, s->align_wp);  \
                                                        \
    speed_operand_src (s, s->xp, s->size);              \
    speed_operand_dst (s, wp, s->size);                 \
    speed_cache_fill (s);                               \
                                                        \
    speed_starttime ();                                 \
    i = s->reps;                                        \
    do                                                  \
      call;                                             \
    while (--i != 0);                                   \
    t = speed_endtime ();                               \
                                                        \
    TMP_FREE (marker);                                  \
    return t;                                           \
  }  

#define SPEED_ROUTINE_MPN_UNARY_1(function) \
  SPEED_ROUTINE_MPN_UNARY_1_CALL ((*function) (wp, s->xp, s->size, s->r))

#define SPEED_ROUTINE_MPN_UNARY_1C(function) \
  SPEED_ROUTINE_MPN_UNARY_1_CALL ((*function) (wp, s->xp, s->size, s->r, 0))

#define SPEED_ROUTINE_MPN_DIVREM_1(function) \
  SPEED_ROUTINE_MPN_UNARY_1_CALL ((*function) (wp, 0, s->xp, s->size, s->r))

#define SPEED_ROUTINE_MPN_DIVREM_1C(function) \
  SPEED_ROUTINE_MPN_UNARY_1_CALL ((*function) (wp, 0, s->xp, s->size, s->r, 0))

#define SPEED_ROUTINE_MPN_DIVREM_1F(function) \
  SPEED_ROUTINE_MPN_UNARY_1_CALL ((*function) (wp, s->size, s->xp, 0, s->r))

#define SPEED_ROUTINE_MPN_DIVREM_1CF(function) \
  SPEED_ROUTINE_MPN_UNARY_1_CALL ((*function) (wp, s->size, s->xp, 0, s->r, 0))


/* For mpn_mul_basecase, xsize=r, ysize=s->size. */
#define SPEED_ROUTINE_MPN_MUL_BASECASE(function)                \
  {                                                             \
    mp_ptr    wp;                                               \
    mp_size_t  size1;                                           \
    unsigned  i;                                                \
    double    t;                                                \
    TMP_DECL (marker);                                          \
                                                                \
    size1 = (s->r == 0 ? s->size : s->r);                       \
                                                                \
    SPEED_RESTRICT_COND (s->size >= 1);                         \
    SPEED_RESTRICT_COND (size1 >= s->size);                     \
                                                                \
    TMP_MARK (marker);                                          \
    wp = SPEED_TMP_ALLOC_LIMBS (size1 + s->size, s->align_wp);  \
                                                                \
    speed_operand_src (s, s->xp, size1);                        \
    speed_operand_src (s, s->yp, s->size);                      \
    speed_operand_dst (s, wp, size1 + s->size);                 \
    speed_cache_fill (s);                                       \
                                                                \
    speed_starttime ();                                         \
    i = s->reps;                                                \
    do                                                          \
      function (wp, s->xp, size1, s->yp, s->size);              \
    while (--i != 0);                                           \
    t = speed_endtime ();                                       \
                                                                \
    TMP_FREE (marker);                                          \
    return t;                                                   \
  }  


#define SPEED_ROUTINE_MPN_MUL_N_CALL(call)                      \
  {                                                             \
    mp_ptr    wp;                                               \
    unsigned  i;                                                \
    double    t;                                                \
    TMP_DECL (marker);                                          \
                                                                \
    SPEED_RESTRICT_COND (s->size >= 1);                         \
                                                                \
    TMP_MARK (marker);                                          \
    wp = SPEED_TMP_ALLOC_LIMBS (2*s->size, s->align_wp);        \
                                                                \
    speed_operand_src (s, s->xp, s->size);                      \
    speed_operand_src (s, s->yp, s->size);                      \
    speed_operand_dst (s, wp, 2*s->size);                       \
    speed_cache_fill (s);                                       \
                                                                \
    speed_starttime ();                                         \
    i = s->reps;                                                \
    do                                                          \
      call;                                                     \
    while (--i != 0);                                           \
    t = speed_endtime ();                                       \
                                                                \
    TMP_FREE (marker);                                          \
    return t;                                                   \
  }  

#define SPEED_ROUTINE_MPN_MUL_N(function) \
  SPEED_ROUTINE_MPN_MUL_N_CALL (function (wp, s->xp, s->yp, s->size));


#define SPEED_ROUTINE_MPN_MUL_N_TSPACE(call, tsize)             \
  {                                                             \
    mp_ptr    wp, tspace;                                       \
    unsigned  i;                                                \
    double    t;                                                \
    TMP_DECL (marker);                                          \
                                                                \
    SPEED_RESTRICT_COND (s->size >= 1);                         \
                                                                \
    TMP_MARK (marker);                                          \
    wp = SPEED_TMP_ALLOC_LIMBS (2*s->size, s->align_wp);        \
    tspace = SPEED_TMP_ALLOC_LIMBS (tsize, s->align_wp2);       \
                                                                \
    speed_operand_src (s, s->xp, s->size);                      \
    speed_operand_src (s, s->yp, s->size);                      \
    speed_operand_dst (s, wp, 2*s->size);                       \
    speed_operand_dst (s, tspace, tsize);                       \
    speed_cache_fill (s);                                       \
                                                                \
    speed_starttime ();                                         \
    i = s->reps;                                                \
    do                                                          \
      call;                                                     \
    while (--i != 0);                                           \
    t = speed_endtime ();                                       \
                                                                \
    TMP_FREE (marker);                                          \
    return t;                                                   \
  }  

/* FIXME: size restrictions */
#define SPEED_ROUTINE_MPN_KARA_MUL_N(function)          \
  SPEED_ROUTINE_MPN_MUL_N_TSPACE                        \
    (function (wp, s->xp, s->xp, s->size, tspace),      \
     MPN_KARA_MUL_N_TSIZE (s->size))

/* FIXME: size restrictions */
#define SPEED_ROUTINE_MPN_TOOM3_MUL_N(function)         \
  SPEED_ROUTINE_MPN_MUL_N_TSPACE                        \
    (function (wp, s->xp, s->yp, s->size, tspace),      \
     MPN_TOOM3_MUL_N_TSIZE (s->size))


#define SPEED_ROUTINE_MPN_SQR_CALL(call)                        \
  {                                                             \
    mp_ptr    wp;                                               \
    unsigned  i;                                                \
    double    t;                                                \
    TMP_DECL (marker);                                          \
                                                                \
    SPEED_RESTRICT_COND (s->size >= 1);                         \
                                                                \
    TMP_MARK (marker);                                          \
    wp = SPEED_TMP_ALLOC_LIMBS (2*s->size, s->align_wp);        \
                                                                \
    speed_operand_src (s, s->xp, s->size);                      \
    speed_operand_dst (s, wp, 2*s->size);                       \
    speed_cache_fill (s);                                       \
                                                                \
    speed_starttime ();                                         \
    i = s->reps;                                                \
    do                                                          \
      call;                                                     \
    while (--i != 0);                                           \
    t = speed_endtime ();                                       \
                                                                \
    TMP_FREE (marker);                                          \
    return t;                                                   \
  }  

#define SPEED_ROUTINE_MPN_SQR(function) \
  SPEED_ROUTINE_MPN_SQR_CALL (function (wp, s->xp, s->size))


#define SPEED_ROUTINE_MPN_SQR_TSPACE(call, tsize)               \
  {                                                             \
    mp_ptr    wp, tspace;                                       \
    unsigned  i;                                                \
    double    t;                                                \
    TMP_DECL (marker);                                          \
                                                                \
    SPEED_RESTRICT_COND (s->size >= 1);                         \
                                                                \
    TMP_MARK (marker);                                          \
    wp = SPEED_TMP_ALLOC_LIMBS (2*s->size, s->align_wp);        \
    tspace = SPEED_TMP_ALLOC_LIMBS (tsize, s->align_wp2);       \
                                                                \
    speed_operand_src (s, s->xp, s->size);                      \
    speed_operand_dst (s, wp, 2*s->size);                       \
    speed_operand_dst (s, tspace, tsize);                       \
    speed_cache_fill (s);                                       \
                                                                \
    speed_starttime ();                                         \
    i = s->reps;                                                \
    do                                                          \
      call;                                                     \
    while (--i != 0);                                           \
    t = speed_endtime ();                                       \
                                                                \
    TMP_FREE (marker);                                          \
    return t;                                                   \
  }  

/* FIXME: size restrictions */
#define SPEED_ROUTINE_MPN_KARA_SQR_N(function)                          \
  SPEED_ROUTINE_MPN_SQR_TSPACE (function (wp, s->xp, s->size, tspace),  \
                                MPN_KARA_SQR_N_TSIZE (s->size))

/* FIXME: size restrictions */
#define SPEED_ROUTINE_MPN_TOOM3_SQR_N(function)                         \
  SPEED_ROUTINE_MPN_SQR_TSPACE (function (wp, s->xp, s->size, tspace),  \
                                MPN_TOOM3_SQR_N_TSIZE (s->size))


#define SPEED_ROUTINE_MPN_MOD_CALL(call)        \
  {                                             \
    unsigned  i;                                \
                                                \
    SPEED_RESTRICT_COND (s->size >= 0);         \
                                                \
    speed_operand_src (s, s->xp, s->size);      \
    speed_cache_fill (s);                       \
                                                \
    speed_starttime ();                         \
    i = s->reps;                                \
    do                                          \
      call;                                     \
    while (--i != 0);                           \
    return speed_endtime ();                    \
  }  

#define SPEED_ROUTINE_MPN_MOD_1(function)  \
   SPEED_ROUTINE_MPN_MOD_CALL ((*function) (s->xp, s->size, s->r))

#define SPEED_ROUTINE_MPN_MOD_1C(function) \
   SPEED_ROUTINE_MPN_MOD_CALL ((*function) (s->xp, s->size, s->r, 0))


/* A division of 2*s->size by s->size limbs */

#define SPEED_ROUTINE_MPN_BZ_DIVREM_CALL(call)                  \
  {                                                             \
    unsigned   i;                                               \
    mp_ptr     a, d, q, r;                                      \
    double    t;                                                \
    TMP_DECL (marker);                                          \
                                                                \
    SPEED_RESTRICT_COND (s->size >= 1);                         \
                                                                \
    TMP_MARK (marker);                                          \
    a = SPEED_TMP_ALLOC_LIMBS (2*s->size, s->align_xp);         \
    d = SPEED_TMP_ALLOC_LIMBS (s->size,   s->align_yp);         \
    q = SPEED_TMP_ALLOC_LIMBS (s->size+1, s->align_wp);         \
    r = SPEED_TMP_ALLOC_LIMBS (s->size,   s->align_wp2);        \
                                                                \
    MPN_COPY (a, s->xp, s->size);                               \
    MPN_COPY (a+s->size, s->xp, s->size);                       \
                                                                \
    MPN_COPY (d, s->yp, s->size);                               \
                                                                \
    /* normalize the data */                                    \
    d[s->size-1] |= MP_LIMB_T_HIGHBIT;                          \
    a[2*s->size-1] = d[s->size-1] - 1;                          \
                                                                \
    speed_operand_src (s, a, 2*s->size);                        \
    speed_operand_src (s, d, s->size);                          \
    speed_operand_dst (s, q, s->size+1);                        \
    speed_operand_dst (s, r, s->size);                          \
    speed_cache_fill (s);                                       \
                                                                \
    speed_starttime ();                                         \
    i = s->reps;                                                \
    do                                                          \
      call;                                                     \
    while (--i != 0);                                           \
    t = speed_endtime ();                                       \
                                                                \
    TMP_FREE (marker);                                          \
    return t;                                                   \
  }  

#define SPEED_ROUTINE_MPN_BZ_DIVREM_N(function) \
  SPEED_ROUTINE_MPN_BZ_DIVREM_CALL((*function) (q, a, d, s->size))

#define SPEED_ROUTINE_MPN_BZ_DIVREM_SB(function)        \
  SPEED_ROUTINE_MPN_BZ_DIVREM_CALL                      \
    ((*function) (q, a, 2*s->size, d, s->size))

#define SPEED_ROUTINE_MPN_BZ_TDIV_QR(function)          \
  SPEED_ROUTINE_MPN_BZ_DIVREM_CALL                      \
    ((*function) (q, r, 0, a, 2*s->size, d, s->size))


#define SPEED_ROUTINE_MPN_POPCOUNT(function)    \
  {                                             \
    unsigned  i;                                \
                                                \
    SPEED_RESTRICT_COND (s->size >= 1);         \
                                                \
    speed_operand_src (s, s->xp, s->size);      \
    speed_cache_fill (s);                       \
                                                \
    speed_starttime ();                         \
    i = s->reps;                                \
    do                                          \
      function (s->xp, s->size);                \
    while (--i != 0);                           \
    return speed_endtime ();                    \
  }  

#define SPEED_ROUTINE_MPN_HAMDIST(function)     \
  {                                             \
    unsigned  i;                                \
                                                \
    SPEED_RESTRICT_COND (s->size >= 1);         \
                                                \
    speed_operand_src (s, s->xp, s->size);      \
    speed_operand_src (s, s->yp, s->size);      \
    speed_cache_fill (s);                       \
                                                \
    speed_starttime ();                         \
    i = s->reps;                                \
    do                                          \
      function (s->xp, s->yp, s->size);         \
    while (--i != 0);                           \
    return speed_endtime ();                    \
  }  


/* For mpz_fib_ui, mpz_fac_ui, etc */

#define SPEED_ROUTINE_MPZ_UI(function)  \
  {                                     \
    mpz_t     z;                        \
    unsigned  i;                        \
    double    t;                        \
                                        \
    SPEED_RESTRICT_COND (s->size >= 0); \
                                        \
    mpz_init (z);                       \
                                        \
    speed_starttime ();                 \
    i = s->reps;                        \
    do                                  \
      function (z, s->size);            \
    while (--i != 0);                   \
    t = speed_endtime ();               \
                                        \
    mpz_clear (z);                      \
    return t;                           \
  }  


/* Calculate 2^(m-1) mod m for random odd m of s->size limbs.  Having m odd
   allows redc to be used.  Actually the exponent (m-1) is cut down to at
   most 6 limbs so the calculation doesn't take too long.  */
#define SPEED_ROUTINE_MPZ_POWM(function)        \
  {                                             \
    mpz_t     r, b, e, m;                       \
    unsigned  i;                                \
    double    t;                                \
                                                \
    SPEED_RESTRICT_COND (s->size >= 1);         \
                                                \
    mpz_init (r);                               \
    mpz_init_set_ui (b, 2);                     \
                                                \
    /* force m to odd */                        \
    mpz_init (m);                               \
    mpz_set_n (m, s->xp, s->size);              \
    PTR(m)[0] |= 1;                             \
                                                \
    mpz_init_set (e, m);                        \
    mpz_sub_ui (e, e, 1);                       \
    SIZ(e) = MIN (SIZ(e), 6);                   \
                                                \
    speed_starttime ();                         \
    i = s->reps;                                \
    do                                          \
      function (r, b, e, m);                    \
    while (--i != 0);                           \
    t = speed_endtime ();                       \
                                                \
    mpz_clear (r);                              \
    mpz_clear (b);                              \
    mpz_clear (e);                              \
    mpz_clear (m);                              \
    return t;                                   \
  }  


#define SPEED_ROUTINE_MPN_ADDSUB_CALL(call)                               \
  {                                                                       \
    mp_ptr    wp, wp2, xp, yp;                                            \
    unsigned  i;                                                          \
    double    t;                                                          \
    TMP_DECL (marker);                                                    \
                                                                          \
    SPEED_RESTRICT_COND (s->size >= 0);                                   \
                                                                          \
    TMP_MARK (marker);                                                    \
    wp  = SPEED_TMP_ALLOC_LIMBS (s->size, s->align_wp);                   \
    wp2 = SPEED_TMP_ALLOC_LIMBS (s->size, s->align_wp2);                  \
    xp = s->xp;                                                           \
    yp = s->yp;                                                           \
                                                                          \
    switch (s->r) {                                                       \
    case 0:                           break;                              \
    case 1: xp =    wp;               break;                              \
    case 2:              yp =    wp2; break;                              \
    case 3: xp =    wp;  yp =    wp2; break;                              \
    case 4: xp =    wp2; yp =    wp;  break;                              \
    default:                                                              \
      fprintf (stderr, "Unrecognised r=%ld in addsub measuring\n", s->r); \
      abort ();                                                           \
    }                                                                     \
    if (xp != s->xp) MPN_COPY (xp, s->xp, s->size);                       \
    if (yp != s->yp) MPN_COPY (yp, s->yp, s->size);                       \
                                                                          \
    speed_operand_src (s, xp, s->size);                                   \
    speed_operand_src (s, yp, s->size);                                   \
    speed_operand_dst (s, wp, s->size);                                   \
    speed_operand_dst (s, wp2, s->size);                                  \
    speed_cache_fill (s);                                                 \
                                                                          \
    speed_starttime ();                                                   \
    i = s->reps;                                                          \
    do                                                                    \
      call;                                                               \
    while (--i != 0);                                                     \
    t = speed_endtime ();                                                 \
                                                                          \
    TMP_FREE (marker);                                                    \
    return t;                                                             \
  }

#define SPEED_ROUTINE_MPN_ADDSUB_N(function)    \
  SPEED_ROUTINE_MPN_ADDSUB_CALL                 \
    (function (wp, wp2, xp, yp, s->size));

#define SPEED_ROUTINE_MPN_ADDSUB_NC(function)   \
  SPEED_ROUTINE_MPN_ADDSUB_CALL                 \
    (function (wp, wp2, xp, yp, s->size, 0));


#define SPEED_ROUTINE_MPN_GCD_1xN(function)     \
  {                                             \
    unsigned  i;                                \
    double    t;                                \
    TMP_DECL (marker);                          \
                                                \
    SPEED_RESTRICT_COND (s->size >= 1);         \
    SPEED_RESTRICT_COND (s->r != 0);            \
                                                \
    TMP_MARK (marker);                          \
                                                \
    speed_operand_src (s, s->xp, s->size);      \
    speed_cache_fill (s);                       \
                                                \
    speed_starttime ();                         \
    i = s->reps;                                \
    do                                          \
      function (s->xp, s->size, s->r);          \
    while (--i != 0);                           \
    t = speed_endtime ();                       \
                                                \
    TMP_FREE (marker);                          \
    return t;                                   \
  }  


/* SPEED_BLOCK_SIZE many one GCDs of s->size bits each. */

#define SPEED_ROUTINE_MPN_GCD_1_CALL(setup, call)               \
  {                                                             \
    unsigned  i, j;                                             \
    mp_ptr    px, py;                                           \
    mp_limb_t x_mask, y_mask;                                   \
    double    t;                                                \
    TMP_DECL (marker);                                          \
                                                                \
    SPEED_RESTRICT_COND (s->size >= 1);                         \
    SPEED_RESTRICT_COND (s->size <= mp_bits_per_limb);          \
                                                                \
    TMP_MARK (marker);                                          \
    px = SPEED_TMP_ALLOC_LIMBS (SPEED_BLOCK_SIZE, s->align_xp); \
    py = SPEED_TMP_ALLOC_LIMBS (SPEED_BLOCK_SIZE, s->align_yp); \
    MPN_COPY (px, s->xp_block, SPEED_BLOCK_SIZE);               \
    MPN_COPY (py, s->yp_block, SPEED_BLOCK_SIZE);               \
                                                                \
    x_mask = MP_LIMB_T_LOWBITMASK (s->size);                    \
    y_mask = MP_LIMB_T_LOWBITMASK (s->r != 0 ? s->r : s->size); \
    for (i = 0; i < SPEED_BLOCK_SIZE; i++)                      \
      {                                                         \
        px[i] &= x_mask; px[i] += (px[i] == 0);                 \
        py[i] &= y_mask; py[i] += (py[i] == 0);                 \
        setup;                                                  \
      }                                                         \
                                                                \
    speed_operand_src (s, px, SPEED_BLOCK_SIZE);                \
    speed_operand_src (s, py, SPEED_BLOCK_SIZE);                \
    speed_cache_fill (s);                                       \
                                                                \
    speed_starttime ();                                         \
    i = s->reps;                                                \
    do                                                          \
      {                                                         \
        j = SPEED_BLOCK_SIZE;                                   \
        do                                                      \
          {                                                     \
            call;                                               \
          }                                                     \
        while (--j != 0);                                       \
      }                                                         \
    while (--i != 0);                                           \
    t = speed_endtime ();                                       \
                                                                \
    TMP_FREE (marker);                                          \
                                                                \
    s->time_divisor = SPEED_BLOCK_SIZE;                         \
    return t;                                                   \
  }  

#define SPEED_ROUTINE_MPN_GCD_1(function) \
  SPEED_ROUTINE_MPN_GCD_1_CALL( , function (&px[j-1], 1, py[j-1]))

#define SPEED_ROUTINE_MPN_JACBASE(function)     \
  SPEED_ROUTINE_MPN_GCD_1_CALL                  \
    ({                                          \
       px[i] %= py[i];                          \
       px[i] |= 1;                              \
       py[i] |= 1;                              \
       if (py[i]==1) py[i]=3;                   \
     },                                         \
     function (px[j-1], py[j-1], 0))


/* Run some GCDs of s->size limbs each.  The number of different data values
   is decreased as s->size**2, since GCD is a quadratic algorithm.
   SPEED_ROUTINE_MPN_GCD runs more times than SPEED_ROUTINE_MPN_GCDEXT
   though, because the plain gcd is about twice as fast as gcdext.  */

#define SPEED_ROUTINE_MPN_GCD_CALL(datafactor, call)            \
  {                                                             \
    unsigned  i;                                                \
    mp_size_t j, pieces, psize;                                 \
    mp_ptr    wp, wp2, xtmp, ytmp, px, py;                      \
    double    t;                                                \
    TMP_DECL (marker);                                          \
                                                                \
    SPEED_RESTRICT_COND (s->size >= 1);                         \
                                                                \
    TMP_MARK (marker);                                          \
    xtmp = SPEED_TMP_ALLOC_LIMBS (s->size+1, s->align_xp);      \
    ytmp = SPEED_TMP_ALLOC_LIMBS (s->size+1, s->align_yp);      \
    wp = SPEED_TMP_ALLOC_LIMBS (s->size, s->align_wp);          \
    wp2 = SPEED_TMP_ALLOC_LIMBS (s->size, s->align_wp2);        \
                                                                \
    pieces = SPEED_BLOCK_SIZE * datafactor / s->size / s->size; \
    pieces = MAX (pieces, 1);                                   \
    pieces = MIN (pieces, SPEED_BLOCK_SIZE / s->size);          \
                                                                \
    psize = pieces * s->size;                                   \
    px = TMP_ALLOC_LIMBS (psize);                               \
    py = TMP_ALLOC_LIMBS (psize);                               \
    MPN_COPY (px, pieces==1 ? s->xp : s->xp_block, psize);      \
    MPN_COPY (py, pieces==1 ? s->yp : s->yp_block, psize);      \
                                                                \
    /* y must be odd, x must have at least as many bits as y */ \
    for (j = 0; j < pieces; j++)                                \
      {                                                         \
        mp_ptr  x = px+j*s->size;                               \
        mp_ptr  y = py+j*s->size;                               \
        y[0] |= 1;                                              \
        if (x[s->size-1] == 0) x[s->size-1] = 1;                \
        if (y[s->size-1] == 0) y[s->size-1] = 1;                \
        x[s->size-1] = MAX (x[s->size-1], y[s->size-1]);        \
      }                                                         \
                                                                \
    speed_operand_src (s, px, psize);                           \
    speed_operand_src (s, py, psize);                           \
    speed_operand_dst (s, xtmp, s->size);                       \
    speed_operand_dst (s, ytmp, s->size);                       \
    speed_operand_dst (s, wp, s->size);                         \
    speed_cache_fill (s);                                       \
                                                                \
    speed_starttime ();                                         \
    i = s->reps;                                                \
    do                                                          \
      {                                                         \
        j = pieces;                                             \
        do                                                      \
          {                                                     \
            MPN_COPY (xtmp, px+(j-1)*s->size, s->size);         \
            MPN_COPY (ytmp, py+(j-1)*s->size, s->size);         \
            call;                                               \
          }                                                     \
        while (--j != 0);                                       \
      }                                                         \
    while (--i != 0);                                           \
    t = speed_endtime ();                                       \
                                                                \
    TMP_FREE (marker);                                          \
                                                                \
    s->time_divisor = pieces;                                   \
    return t;                                                   \
  }  

#define SPEED_ROUTINE_MPN_GCD(function) \
  SPEED_ROUTINE_MPN_GCD_CALL (8, function (wp, xtmp, s->size, ytmp, s->size))

#define SPEED_ROUTINE_MPN_GCDEXT(function)                               \
  SPEED_ROUTINE_MPN_GCD_CALL                                             \
    (4, { mp_size_t  wp2size;                                            \
          function (wp, wp2, &wp2size, xtmp, s->size, ytmp, s->size); })


#define SPEED_ROUTINE_MPN_DIVREM_2(function)            \
  {                                                     \
    mp_ptr    wp, xp;                                   \
    mp_limb_t yp[2];                                    \
    unsigned  i;                                        \
    double    t;                                        \
    TMP_DECL (marker);                                  \
                                                        \
    SPEED_RESTRICT_COND (s->size >= 1);                 \
                                                        \
    TMP_MARK (marker);                                  \
    xp = SPEED_TMP_ALLOC_LIMBS (s->size, s->align_xp);  \
    wp = SPEED_TMP_ALLOC_LIMBS (s->size, s->align_wp);  \
                                                        \
    /* source is destroyed */                           \
    MPN_COPY (xp, s->xp, s->size);                      \
                                                        \
    /* divisor must be normalized */                    \
    MPN_COPY (yp, s->yp_block, 2);                      \
    yp[1] |= MP_LIMB_T_HIGHBIT;                         \
                                                        \
    speed_operand_src (s, xp, s->size);                 \
    speed_operand_src (s, yp, 2);                       \
    speed_operand_dst (s, wp, s->size);                 \
    speed_cache_fill (s);                               \
                                                        \
    speed_starttime ();                                 \
    i = s->reps;                                        \
    do                                                  \
      function (wp, 0, xp, s->size, yp);                \
    while (--i != 0);                                   \
    t = speed_endtime ();                               \
                                                        \
    TMP_FREE (marker);                                  \
    return t;                                           \
  }  


#define SPEED_ROUTINE_MODLIMB_INVERT(function)                  \
  {                                                             \
    unsigned   i, j;                                            \
    mp_ptr     xp;                                              \
    mp_limb_t  n = 1;                                           \
    double     t;                                               \
                                                                \
    xp = s->xp_block-1;                                         \
                                                                \
    speed_operand_src (s, s->xp_block, SPEED_BLOCK_SIZE);       \
    speed_cache_fill (s);                                       \
                                                                \
    speed_starttime ();                                         \
    i = s->reps;                                                \
    do                                                          \
      {                                                         \
        j = SPEED_BLOCK_SIZE;                                   \
        do                                                      \
          {                                                     \
            /* randomized but successively dependent */         \
            n += (xp[j] << 1);                                  \
                                                                \
            function (n, n);                                    \
          }                                                     \
        while (--j != 0);                                       \
      }                                                         \
    while (--i != 0);                                           \
    t = speed_endtime ();                                       \
                                                                \
    /* make sure the compiler won't optimize away n */          \
    noop_1 (n);                                                 \
                                                                \
    s->time_divisor = SPEED_BLOCK_SIZE;                         \
    return t;                                                   \
  }  

#endif
