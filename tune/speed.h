/* Header for speed and threshold things. */

#ifndef __SPEED_H__
#define __SPEED_H__

/* size of temporary space required by karatsuba functions */
#define MPN_KARA_MUL_N_TSIZE(n)  (2*((n)+BITS_PER_MP_LIMB))
#define MPN_KARA_SQR_N_TSIZE(n)  (2*((n)+BITS_PER_MP_LIMB))


void pentium_wbinvd(void);

/* Pad ptr,oldsize with zero limbs (at the most significant end) to make it
   newsize long.  Requires newsize >= oldsize. */
#define MPN_ZERO_EXTEND(ptr, oldsize, newsize)          \
  do {                                                  \
    ASSERT ((newsize) >= (oldsize));                    \
    MPN_ZERO ((ptr)+(oldsize), (newsize)-(oldsize));    \
  } while (0)


/* align must be a power of 2 here, usually CACHE_LINE_SIZE is a good choice */

#define TMP_ALLOC_ALIGNED(bytes, align)                                 \
  ((char *) (((unsigned) (TMP_ALLOC ((bytes) + (align)-1) + (align)-1)) \
             & ~((align)-1)))
#define TMP_ALLOC_LIMBS_ALIGNED(limbs, align)                           \
  ((mp_ptr) TMP_ALLOC_ALIGNED ((limbs)*sizeof(mp_limb_t), align))

/* Note that memory allocated with this macro can never be freed, because
   the start address of the block allocated is discarded. */
#define _mp_allocate_func_aligned(bytes, align)                         \
  ((void *) (((unsigned) ((*_mp_allocate_func) ((bytes) + (align)-1)    \
                          + (align)-1)) & ~((align)-1)))

/* 32 is right for pentium family, need to configure this for other CPUs */
#define CACHE_LINE_SIZE    64 /* bytes */

#define SPEED_TMP_ALLOC_ADJUST_MASK  (CACHE_LINE_SIZE/BYTES_PER_MP_LIMB - 1)

#define SPEED_TMP_ALLOC(limbs, align) \
  (speed_tmp_alloc_adjust \
    (TMP_ALLOC_LIMBS((limbs) + SPEED_TMP_ALLOC_ADJUST_MASK), (align)))


extern double  speed_unittime;
extern double  speed_cycletime;
extern int     speed_precision;
extern const char *speed_time_string;
void speed_time_init (void);
void speed_starttime (void);
double speed_endtime (void);

struct speed_params {
  unsigned   reps;  /* how many times to run the routine */
  mp_ptr     xp;    /* first argument */
  mp_ptr     yp;    /* second argument */
  mp_size_t  size;  /* size of both arguments */
  long       r;     /* user supplied parameter */
  mp_size_t  align_xp;  /* alignment of xp */
  mp_size_t  align_yp;  /* alignment of yp */
  mp_size_t  align_wp;  /* intended alignment of wp */
  mp_size_t  align_wp2; /* intended alignment of wp2 */

  int        cache;
  unsigned   src_num, dst_num;
  struct {
    mp_ptr    ptr;
    mp_size_t size;
  } src[2], dst[2];
};

typedef double (*speed_function_t) (struct speed_params *s);

double speed_measure (double (*fun)(struct speed_params *s),
                      struct speed_params *s);
double speed_memcpy (struct speed_params *s);
double speed_mpn_add_n (struct speed_params *s);
double speed_mpn_add_n_self (struct speed_params *s);
double speed_mpn_add_n_inplace (struct speed_params *s);
double speed_mpn_addmul_1 (struct speed_params *s);
double speed_mpn_bz_divrem_n (struct speed_params *s);
double speed_mpn_bz_divrem_sb (struct speed_params *s);
double speed_mpn_bz_tdiv_qr (struct speed_params *s);
double speed_MPN_COPY (struct speed_params *s);
double speed_MPN_COPY_DECR (struct speed_params *s);
double speed_MPN_COPY_INCR (struct speed_params *s);
double speed_mpn_divexact_by3 (struct speed_params *s);
double speed_mpn_divmod_1 (struct speed_params *s);
double speed_mpn_divrem_1 (struct speed_params *s);
double speed_mpn_mod_1 (struct speed_params *s);
double speed_mpn_popcount (struct speed_params *s);
double speed_mpn_sqr_recurse (struct speed_params *s);
double speed_mpn_sqr_toom (struct speed_params *s);
double speed_mpn_sqr_toom3 (struct speed_params *s);
double speed_mpn_sub_n (struct speed_params *s);
double speed_mpn_lshift (struct speed_params *s);
double speed_mpn_kara_mul_n (struct speed_params *s);
double speed_mpn_kara_sqr_n (struct speed_params *s);
double speed_mpn_toom3_mul_n (struct speed_params *s);
double speed_mpn_toom3_sqr_n (struct speed_params *s);
double speed_mpn_mul_1 (struct speed_params *s);
double speed_mpn_mul_basecase (struct speed_params *s);
double speed_mpn_mul_n (struct speed_params *s);
double speed_mpn_mul_n_recurse (struct speed_params *s);
double speed_mpn_mul_n_sqr (struct speed_params *s);
double speed_mpn_mul_n_toom (struct speed_params *s);
double speed_mpn_mul_n_toom3 (struct speed_params *s);
double speed_mpn_rshift (struct speed_params *s);
double speed_mpn_sqr_n (struct speed_params *s);
double speed_mpn_sqr_basecase (struct speed_params *s);
double speed_mpn_submul_1 (struct speed_params *s);
double speed_mpz_fac_ui (struct speed_params *s);
double speed_mpz_fib_ui (struct speed_params *s);
double speed_mpn_jacobi_base (struct speed_params *s);
double speed_mpn_jacobi_base_division (struct speed_params *s);
double speed_noop (struct speed_params *s);
double speed_noop_wxs (struct speed_params *s);
double speed_noop_wxys (struct speed_params *s);
double speed_toom3Mul (struct speed_params *s);
void noop (void);
void noop_wxs (mp_ptr wp, mp_srcptr xp, mp_size_t size);
void noop_wxys (mp_ptr wp, mp_srcptr xp, mp_srcptr yp, mp_size_t size);
void mpn_cache_fill (mp_srcptr ptr, mp_size_t size);
void mpn_cache_fill_dummy (mp_limb_t n);
mp_ptr speed_tmp_alloc_adjust (void *ptr, mp_size_t align);
void *_mp_allocate_or_reallocate (void *ptr, size_t oldsize, size_t newsize);
void mpn_memcpy (mp_ptr dst, mp_ptr src, mp_size_t size);
void speed_cache_fill (struct speed_params *s);


#if defined(__GNUC__)
#define pentium_rdtsc(_p)                                       \
  __asm__ ("cpuid; rdtsc" : "=a" ((_p)[0]), "=d" ((_p)[1])      \
    : : "ebx", "ecx" )
#else
void pentium_rdtsc (unsigned p[2]);
#endif

void x86_cpuid (unsigned eax, unsigned p[4]);
int x86_cpuid_available (void);
int x86_mmx_available (void);
int x86_cmov_available (void);
int x86_3dnow_available (void);


#define SPEED_OPERAND_SRC(s,p,sz)       \
  do {                                  \
    (s)->src[(s)->src_num].ptr = (p);   \
    (s)->src[(s)->src_num].size = (sz); \
    (s)->src_num++;                     \
  } while (0)

#define SPEED_OPERAND_DST(s,p,sz)       \
  do {                                  \
    (s)->dst[(s)->dst_num].ptr = (p);   \
    (s)->dst[(s)->dst_num].size = (sz); \
    (s)->dst_num++;                     \
  } while (0)


#define SPEED_RESTRICT_COND(cond)   if (!(cond)) return -1.0;

/* For mpn_copy or similar. */
#define SPEED_ROUTINE_MPN_COPY_CALL(speed_function, restriction, call)  \
  double                                                                \
  speed_function (struct speed_params *s)                               \
  {                                                                     \
    mp_ptr    wp;                                                       \
    unsigned  i;                                                        \
    double    t;                                                        \
    TMP_DECL (marker);                                                  \
                                                                        \
    SPEED_RESTRICT_COND (s->size >= 1);                                 \
    SPEED_RESTRICT_COND (restriction);                                  \
                                                                        \
    TMP_MARK (marker);                                                  \
    wp = SPEED_TMP_ALLOC (s->size, s->align_wp);                        \
                                                                        \
    SPEED_OPERAND_SRC (s, s->xp, s->size);                              \
    SPEED_OPERAND_DST (s, wp, s->size);                                 \
    speed_cache_fill (s);                                               \
                                                                        \
    speed_starttime ();                                                 \
    i = s->reps;                                                        \
    do                                                                  \
      call;                                                             \
    while (--i != 0);                                                   \
    t = speed_endtime ();                                               \
                                                                        \
    TMP_FREE (marker);                                                  \
    return t;                                                           \
  }  

#define SPEED_ROUTINE_MPN_COPY(speed_function, function, restriction)   \
  SPEED_ROUTINE_MPN_COPY_CALL(speed_function, restriction,              \
    function (wp, s->xp, s->size))

/* For mpn_add_n, mpn_sub_n, or similar. */
#define SPEED_ROUTINE_MPN_BINARY_N_CALL(speed_function, restriction, call) \
  double                                                                 \
  speed_function (struct speed_params *s)                                \
  {                                                                      \
    mp_ptr    wp;                                                        \
    unsigned  i;                                                         \
    double    t;                                                         \
    TMP_DECL (marker);                                                   \
                                                                         \
    SPEED_RESTRICT_COND (s->size >= 1);                                  \
    SPEED_RESTRICT_COND (restriction);                                   \
                                                                         \
    TMP_MARK (marker);                                                   \
    wp = SPEED_TMP_ALLOC (s->size, s->align_wp);                         \
                                                                         \
    SPEED_OPERAND_SRC (s, s->xp, s->size);                               \
    SPEED_OPERAND_SRC (s, s->yp, s->size);                               \
    SPEED_OPERAND_DST (s, wp, s->size);                                  \
    speed_cache_fill (s);                                                \
                                                                         \
    speed_starttime ();                                                  \
    i = s->reps;                                                         \
    do                                                                   \
      call;                                                              \
    while (--i != 0);                                                    \
    t = speed_endtime ();                                                \
                                                                         \
    TMP_FREE (marker);                                                   \
    return t;                                                            \
  }  

#define SPEED_ROUTINE_MPN_BINARY_N(speed_function, function, restriction) \
   SPEED_ROUTINE_MPN_BINARY_N_CALL (speed_function, restriction,            \
     (*function) (wp, s->xp, s->yp, s->size))

#define SPEED_ROUTINE_MPN_BINARY_NC(speed_function, function, restriction) \
   SPEED_ROUTINE_MPN_BINARY_N_CALL (speed_function, restriction,             \
     (*function) (wp, s->xp, s->yp, s->size, 0))

#define SPEED_ROUTINE_MPN_BINARY_N_SELF(speed_function, function,       \
                                        restriction)                    \
   SPEED_ROUTINE_MPN_BINARY_N_CALL (speed_function, restriction,          \
     (*function) (wp, s->xp, s->xp, s->size))

#define SPEED_ROUTINE_MPN_BINARY_N_INPLACE(speed_function, function,    \
                                        restriction)                    \
   SPEED_ROUTINE_MPN_BINARY_N_CALL (speed_function, restriction,          \
     (*function) (wp, wp, s->xp, s->size))


/* For mpn_lshift, mpn_rshift, mpn_mul_1, with r, or similar. */
#define SPEED_ROUTINE_MPN_UNARY_1_CALL(speed_function, restriction, call) \
  double                                                                \
  speed_function (struct speed_params *s)                               \
  {                                                                     \
    mp_ptr    wp;                                                       \
    unsigned  i;                                                        \
    double    t;                                                        \
    TMP_DECL (marker);                                                  \
                                                                        \
    SPEED_RESTRICT_COND (s->size >= 1);                                 \
    SPEED_RESTRICT_COND (restriction);                                  \
                                                                        \
    TMP_MARK (marker);                                                  \
    wp = SPEED_TMP_ALLOC (s->size, s->align_wp);                        \
                                                                        \
    SPEED_OPERAND_SRC (s, s->xp, s->size);                              \
    SPEED_OPERAND_DST (s, wp, s->size);                                 \
    speed_cache_fill (s);                                               \
                                                                        \
    speed_starttime ();                                                 \
    i = s->reps;                                                        \
    do                                                                  \
      call;                                                             \
    while (--i != 0);                                                   \
    t = speed_endtime ();                                               \
                                                                        \
    TMP_FREE (marker);                                                  \
    return t;                                                           \
  }  

#define SPEED_ROUTINE_MPN_UNARY_1(speed_function, function, restriction) \
   SPEED_ROUTINE_MPN_UNARY_1_CALL (speed_function, restriction,          \
     (*function) (wp, s->xp, s->size, s->r))

#define SPEED_ROUTINE_MPN_UNARY_1C(speed_function, function, restriction) \
   SPEED_ROUTINE_MPN_UNARY_1_CALL (speed_function, restriction,           \
     (*function) (wp, s->xp, s->size, s->r, 0))

#define SPEED_ROUTINE_MPN_DIVREM_1(speed_function, function, restriction) \
   SPEED_ROUTINE_MPN_UNARY_1_CALL (speed_function, restriction,           \
     (*function) (wp, 0, s->xp, s->size, s->r))

#define SPEED_ROUTINE_MPN_DIVREM_1C(speed_function, function, restriction) \
   SPEED_ROUTINE_MPN_UNARY_1_CALL (speed_function, restriction,            \
     (*function) (wp, 0, s->xp, s->size, s->r, 0))

#define SPEED_ROUTINE_MPN_DIVREM_1F(speed_function, function, restriction) \
   SPEED_ROUTINE_MPN_UNARY_1_CALL (speed_function, restriction,           \
     (*function) (wp, s->size, s->xp, 0, s->r))

#define SPEED_ROUTINE_MPN_DIVREM_1CF(speed_function, function, restriction) \
   SPEED_ROUTINE_MPN_UNARY_1_CALL (speed_function, restriction,            \
     (*function) (wp, s->size, s->xp, 0, s->r, 0))


/* For mpn_mul_basecase, xsize==r ysize==s->size. */
#define SPEED_ROUTINE_MPN_MUL_BASECASE(speed_function, function, restriction) \
  double                                                                      \
  speed_function (struct speed_params *s)                                     \
  {                                                                           \
    mp_ptr    wp;                                                             \
    mp_size_t  size1;                                                         \
    unsigned  i;                                                              \
    double    t;                                                              \
    TMP_DECL (marker);                                                        \
                                                                              \
    SPEED_RESTRICT_COND (s->size >= 1);                                       \
    SPEED_RESTRICT_COND (s->r == 0 || s->r >= s->size);                       \
    SPEED_RESTRICT_COND (restriction);                                        \
                                                                              \
    size1 = (s->r == 0 ? s->size : s->r);                                     \
                                                                              \
    TMP_MARK (marker);                                                        \
    wp = SPEED_TMP_ALLOC (size1 + s->size, s->align_wp);                      \
                                                                              \
    SPEED_OPERAND_SRC (s, s->xp, size1);                                      \
    SPEED_OPERAND_SRC (s, s->yp, s->size);                                    \
    SPEED_OPERAND_DST (s, wp, size1 + s->size);                               \
    speed_cache_fill (s);                                                     \
                                                                              \
    speed_starttime ();                                                       \
    i = s->reps;                                                              \
    do                                                                        \
      function (wp, s->xp, size1, s->yp, s->size);                            \
    while (--i != 0);                                                         \
    t = speed_endtime ();                                                     \
                                                                              \
    TMP_FREE (marker);                                                        \
    return t;                                                                 \
  }  


#define SPEED_ROUTINE_MPN_MUL_N_CALL(speed_function, restriction, call, tsize) \
  double                                                \
  speed_function (struct speed_params *s)               \
  {                                                     \
    mp_ptr    wp, tspace;                               \
    unsigned  i;                                        \
    double    t;                                        \
    TMP_DECL (marker);                                  \
                                                        \
    SPEED_RESTRICT_COND (s->size >= 1);                 \
    SPEED_RESTRICT_COND (restriction);                  \
                                                        \
    TMP_MARK (marker);                                  \
    wp = SPEED_TMP_ALLOC (2*s->size, s->align_wp);      \
    tspace = SPEED_TMP_ALLOC (tsize, s->align_wp2);     \
                                                        \
    SPEED_OPERAND_SRC (s, s->xp, s->size);              \
    SPEED_OPERAND_SRC (s, s->yp, s->size);              \
    SPEED_OPERAND_DST (s, wp, 2*s->size);               \
    SPEED_OPERAND_DST (s, tspace, tsize);               \
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

#define SPEED_ROUTINE_MPN_MUL_N(speed_function, function, restriction)   \
  SPEED_ROUTINE_MPN_MUL_N_CALL (speed_function, restriction,             \
                                function (wp, s->xp, s->yp, s->size), 1)

#define SPEED_ROUTINE_GMPN_TOOM3_MUL_N(speed_function, function, restriction) \
  SPEED_ROUTINE_MPN_MUL_N_CALL (speed_function, restriction,                  \
                                function (wp, s->xp, s->yp, s->size, tspace), \
                                __GMPN_TOOM3_MUL_N_TSIZE (s->size))


#define SPEED_ROUTINE_MPN_SQR_CALL(speed_function, restriction, call, tsize) \
  double                                                \
  speed_function (struct speed_params *s)               \
  {                                                     \
    mp_ptr    wp, tspace;                               \
    unsigned  i;                                        \
    double    t;                                        \
    TMP_DECL (marker);                                  \
                                                        \
    SPEED_RESTRICT_COND (s->size >= 1);                 \
    SPEED_RESTRICT_COND (restriction);                  \
                                                        \
    TMP_MARK (marker);                                  \
    wp = SPEED_TMP_ALLOC (2*s->size, s->align_wp);      \
    tspace = SPEED_TMP_ALLOC (tsize, s->align_wp2);     \
                                                        \
    SPEED_OPERAND_SRC (s, s->xp, s->size);              \
    SPEED_OPERAND_DST (s, wp, 2*s->size);               \
    SPEED_OPERAND_DST (s, tspace, tsize);               \
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

#define SPEED_ROUTINE_MPN_SQR(speed_function, function, restriction)    \
  SPEED_ROUTINE_MPN_SQR_CALL (speed_function, restriction,              \
                              function (wp, s->xp, s->size), 1)

#define SPEED_ROUTINE_GMPN_TOOM3_SQR_N(speed_function, function, restriction) \
  SPEED_ROUTINE_MPN_SQR_CALL (speed_function, restriction,                    \
                              function (wp, s->xp, s->size, tspace),          \
                              __GMPN_TOOM3_SQR_N_TSIZE (s->size))


#define SPEED_ROUTINE_MPN_MOD_CALL(speed_function, restriction, call)   \
  double                                                                \
  speed_function (struct speed_params *s)                               \
  {                                                                     \
    unsigned  i;                                                        \
                                                                        \
    SPEED_RESTRICT_COND (s->size >= 1);                                 \
    SPEED_RESTRICT_COND (restriction);                                  \
                                                                        \
    SPEED_OPERAND_SRC (s, s->xp, s->size);                              \
    speed_cache_fill (s);                                               \
                                                                        \
    speed_starttime ();                                                 \
    i = s->reps;                                                        \
    do                                                                  \
      call;                                                             \
    while (--i != 0);                                                   \
    return speed_endtime ();                                            \
  }  

#define SPEED_ROUTINE_MPN_MOD_1(speed_function, function, restriction)  \
   SPEED_ROUTINE_MPN_MOD_CALL (speed_function, restriction,             \
     (*function) (s->xp, s->size, s->r))

#define SPEED_ROUTINE_MPN_MOD_1C(speed_function, function, restriction) \
   SPEED_ROUTINE_MPN_MOD_CALL (speed_function, restriction,             \
     (*function) (s->xp, s->size, s->r, 0))


#define SPEED_ROUTINE_MPN_BZ_DIVREM_CALL(speed_function, restriction, call) \
  double                                                                    \
  speed_function (struct speed_params *s)                                   \
  {                                                                         \
    unsigned   i;                                                           \
    mp_ptr     a, q, r;                                                     \
                                                                            \
    SPEED_RESTRICT_COND (s->size >= 1);                                     \
    SPEED_RESTRICT_COND (restriction);                                      \
                                                                            \
    s->yp[s->size-1] |= 0x80000000;                                         \
    s->xp[s->size-1] = s->yp[s->size-1]-1;                                  \
                                                                            \
    TMP_MARK (marker);                                                      \
    a = SPEED_TMP_ALLOC (2*s->size, s->align_wp);                           \
    q = SPEED_TMP_ALLOC (s->size+1, s->align_wp);                           \
    r = SPEED_TMP_ALLOC (s->size, s->align_wp2);                            \
                                                                            \
                                                                            \
    MPN_COPY (a, s->xp, s->size);                                           \
    MPN_COPY (a+s->size, s->xp, s->size);                                   \
                                                                            \
    SPEED_OPERAND_SRC (s, a, 2*s->size);                                    \
    SPEED_OPERAND_SRC (s, s->yp, s->size);                                  \
    SPEED_OPERAND_DST (s, q, s->size+1);                                    \
    SPEED_OPERAND_DST (s, r, s->size);                                      \
    speed_cache_fill (s);                                                   \
                                                                            \
    speed_starttime ();                                                     \
    i = s->reps;                                                            \
    do                                                                      \
      call;                                                                 \
    while (--i != 0);                                                       \
    return speed_endtime ();                                                \
  }  

#define SPEED_ROUTINE_MPN_BZ_DIVREM_N(speed_function, function, restriction) \
  SPEED_ROUTINE_MPN_BZ_DIVREM_CALL(speed_function, restriction,              \
                    (*function) (q, a, s->yp, s->size))

#define SPEED_ROUTINE_MPN_BZ_DIVREM_SB(speed_function, function, restriction) \
  SPEED_ROUTINE_MPN_BZ_DIVREM_CALL(speed_function, restriction,               \
                    (*function) (q, a, 2*s->size, s->yp, s->size))

#define SPEED_ROUTINE_MPN_BZ_TDIV_QR(speed_function, function, restriction)  \
  SPEED_ROUTINE_MPN_BZ_DIVREM_CALL(speed_function, restriction,              \
                    (*function) (q, r, 0, a, 2*s->size, s->yp, s->size))


#define SPEED_ROUTINE_MPN_POPCOUNT(speed_function, function, restriction) \
  double                                                                  \
  speed_function (struct speed_params *s)                                 \
  {                                                                       \
    unsigned  i;                                                          \
                                                                          \
    SPEED_RESTRICT_COND (s->size >= 1);                                   \
    SPEED_RESTRICT_COND (restriction);                                    \
                                                                          \
    SPEED_OPERAND_SRC (s, s->xp, s->size);                                \
    speed_cache_fill (s);                                                 \
                                                                          \
    speed_starttime ();                                                   \
    i = s->reps;                                                          \
    do                                                                    \
      function (s->xp, s->size);                                          \
    while (--i != 0);                                                     \
    return speed_endtime ();                                              \
  }  

#define SPEED_ROUTINE_MPZ_UI(speed_function, function)  \
  double                                                \
  speed_function (struct speed_params *s)               \
  {                                                     \
    mpz_t     z;                                        \
    unsigned  i;                                        \
    double    t;                                        \
                                                        \
    SPEED_RESTRICT_COND (s->size >= 0);                 \
                                                        \
    mpz_init (z);                                       \
                                                        \
    speed_starttime ();                                 \
    i = s->reps;                                        \
    do                                                  \
      function (z, s->size);                            \
    while (--i != 0);                                   \
    t = speed_endtime ();                               \
                                                        \
    mpz_clear (z);                                      \
    return t;                                           \
  }  

/* Currently testing full overlap case only */
#define SPEED_ROUTINE_MPN_ADDSUB_N(speed_function, function, restriction) \
  double                                                                  \
  speed_function (struct speed_params *s)                                 \
  {                                                                       \
    mp_ptr    wp1, wp2;                                                   \
    unsigned  i;                                                          \
    double    t;                                                          \
    TMP_DECL (marker);                                                    \
                                                                          \
    SPEED_RESTRICT_COND (s->size >= 0);                                   \
    SPEED_RESTRICT_COND (restriction);                                    \
                                                                          \
    TMP_MARK (marker);                                                    \
    wp1 = TMP_ALLOC_LIMBS_ALIGNED (s->size, CACHE_LINE_SIZE);             \
    wp2 = TMP_ALLOC_LIMBS_ALIGNED (s->size, CACHE_LINE_SIZE);             \
                                                                          \
    MPN_COPY (wp1, s->xp, s->size);                                       \
    MPN_COPY (wp2, s->yp, s->size);                                       \
                                                                          \
    SPEED_OPERAND_SRC (s, s->xp, s->size);                                \
    SPEED_OPERAND_SRC (s, s->yp, s->size);                                \
    SPEED_OPERAND_DST (s, wp1, s->size);                                  \
    SPEED_OPERAND_DST (s, wp2, s->size);                                  \
    speed_cache_fill (s);                                                 \
                                                                          \
    speed_starttime ();                                                   \
    i = s->reps;                                                          \
    do                                                                    \
      function (wp1, wp2, wp2, wp1, s->size); /*reverse*/                 \
    while (--i != 0);                                                     \
    t = speed_endtime ();                                                 \
                                                                          \
    TMP_FREE (marker);                                                    \
    return t;                                                             \
  }

/* function (wp1, wp2, wp1, wp2, s->size); */ /*full*/          

#endif
