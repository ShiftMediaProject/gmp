/* Tests support prototypes etc.

Copyright 2000, 2001 Free Software Foundation, Inc.

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


#ifndef __TESTS_H__
#define __TESTS_H__

#if defined (__cplusplus)
extern "C" {
#endif


#ifdef __cplusplus
#define ANYARGS  ...
#else
#define ANYARGS
#endif


void tests_start _PROTO ((void));
void tests_end _PROTO ((void));

void tests_memory_start _PROTO ((void));
void tests_memory_end _PROTO ((void));

void tests_rand_start _PROTO ((void));
void tests_rand_end _PROTO ((void));


#if HAVE_CALLING_CONVENTIONS
extern mp_limb_t (*calling_conventions_function) _PROTO ((ANYARGS));
mp_limb_t calling_conventions _PROTO ((ANYARGS));
int calling_conventions_check _PROTO ((void));
#define CALLING_CONVENTIONS(function) \
  (calling_conventions_function = (function), calling_conventions)
#define CALLING_CONVENTIONS_CHECK()    (calling_conventions_check())
#else
#define CALLING_CONVENTIONS(function)  (function)
#define CALLING_CONVENTIONS_CHECK()    1 /* always ok */
#endif


extern int mp_trace_base;
void mpn_trace _PROTO ((const char *name, mp_srcptr ptr, mp_size_t size));
void mpn_tracea _PROTO ((const char *name, const mp_ptr *a, int count,
                 mp_size_t size));
void mpn_tracen _PROTO ((const char *name, int num, mp_srcptr ptr,
                 mp_size_t size));
void mpn_trace_file _PROTO ((const char *filename,
                             mp_srcptr ptr, mp_size_t size));
void mpn_tracea_file _PROTO ((const char *filename,
                              const mp_ptr *a, int count, mp_size_t size));
void mpf_trace _PROTO ((const char *name, mpf_srcptr z));
void mpq_trace _PROTO ((const char *name, mpq_srcptr q));
void mpz_trace _PROTO ((const char *name, mpz_srcptr z));
void mpz_tracen _PROTO ((const char *name, int num, mpz_srcptr z));


void spinner _PROTO ((void));
extern unsigned long  spinner_count;
extern int  spinner_wanted;
extern int  spinner_tick;


void *align_pointer _PROTO ((void *p, size_t align));
void *__gmp_allocate_func_aligned _PROTO ((size_t bytes, size_t align));
void *__gmp_allocate_or_reallocate _PROTO ((void *ptr,
                                          size_t oldsize, size_t newsize));
char *__gmp_allocate_strdup _PROTO ((const char *s));
char *strtoupper _PROTO ((char *s_orig));
mp_limb_t urandom _PROTO ((void));


void mpf_set_str_or_abort _PROTO ((mpf_ptr f, const char *str, int base));


void mpq_set_str_or_abort _PROTO ((mpq_ptr q, const char *str, int base));


void mpz_erandomb _PROTO ((mpz_ptr rop, gmp_randstate_t rstate,
                           unsigned long nbits));
void mpz_erandomb_nonzero _PROTO ((mpz_ptr rop, gmp_randstate_t rstate,
                                   unsigned long nbits));
void mpz_errandomb _PROTO ((mpz_ptr rop, gmp_randstate_t rstate,
                            unsigned long nbits));
void mpz_errandomb_nonzero _PROTO ((mpz_ptr rop, gmp_randstate_t rstate,
                                    unsigned long nbits));
void mpz_flipbit _PROTO ((mpz_ptr r, unsigned long bit));
void mpz_init_set_n _PROTO ((mpz_ptr z, mp_srcptr p, mp_size_t size));
void mpz_negrandom _PROTO ((mpz_ptr rop, gmp_randstate_t rstate));
int mpz_pow2abs_p _PROTO ((mpz_srcptr z)) __GMP_ATTRIBUTE_PURE;
void mpz_set_n _PROTO ((mpz_ptr z, mp_srcptr p, mp_size_t size));
void mpz_set_str_or_abort _PROTO ((mpz_ptr z, const char *str, int base));

mp_size_t mpn_diff_highest _PROTO ((mp_srcptr p1, mp_srcptr p2, mp_size_t n)) __GMP_ATTRIBUTE_PURE;
mp_size_t mpn_diff_lowest _PROTO ((mp_srcptr p1, mp_srcptr p2, mp_size_t n)) __GMP_ATTRIBUTE_PURE;


void refmpf_add _PROTO ((mpf_ptr, mpf_srcptr, mpf_srcptr));
void refmpf_sub _PROTO ((mpf_ptr, mpf_srcptr, mpf_srcptr));


mp_limb_t refmpn_add _PROTO ((mp_ptr rp,
                              mp_srcptr s1p, mp_size_t s1size,
                              mp_srcptr s2p, mp_size_t s2size));
mp_limb_t refmpn_add_1 _PROTO ((mp_ptr rp, mp_srcptr sp, mp_size_t size,
                                mp_limb_t n));
mp_limb_t refmpn_add_n _PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                                mp_size_t size));
mp_limb_t refmpn_add_nc _PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                                 mp_size_t size, mp_limb_t carry));
mp_limb_t refmpn_addmul_1 _PROTO ((mp_ptr wp, mp_srcptr xp, mp_size_t size,
                                   mp_limb_t multiplier));
mp_limb_t refmpn_addmul_1c _PROTO ((mp_ptr wp, mp_srcptr xp, mp_size_t size,
                                    mp_limb_t multiplier, mp_limb_t carry));
mp_limb_t refmpn_addsub_n _PROTO ((mp_ptr r1p, mp_ptr r2p,
                                   mp_srcptr s1p, mp_srcptr s2p,
                                   mp_size_t size));
mp_limb_t refmpn_addsub_nc _PROTO ((mp_ptr r1p, mp_ptr r2p,
                                    mp_srcptr s1p, mp_srcptr s2p,
                                    mp_size_t size, mp_limb_t carry));

void refmpn_and_n  _PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                            mp_size_t size));
void refmpn_andn_n _PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                            mp_size_t size));

int refmpn_cmp _PROTO ((mp_srcptr s1p, mp_srcptr s2p, mp_size_t size));
int refmpn_cmp_twosizes _PROTO ((mp_srcptr xp, mp_size_t xsize,
                                 mp_srcptr yp, mp_size_t ysize));

void refmpn_com_n _PROTO ((mp_ptr rp, mp_srcptr sp, mp_size_t size));
void refmpn_copyi _PROTO ((mp_ptr rp, mp_srcptr sp, mp_size_t size));
void refmpn_copyd _PROTO ((mp_ptr rp, mp_srcptr sp, mp_size_t size));

unsigned refmpn_count_leading_zeros _PROTO ((mp_limb_t x));
unsigned refmpn_count_trailing_zeros _PROTO ((mp_limb_t x));

mp_limb_t refmpn_divexact_by3 _PROTO ((mp_ptr rp, mp_srcptr sp,
                                       mp_size_t size));
mp_limb_t refmpn_divexact_by3c _PROTO ((mp_ptr rp, mp_srcptr sp,
                                       mp_size_t size, mp_limb_t carry));

mp_limb_t refmpn_divmod_1 _PROTO ((mp_ptr rp, mp_srcptr sp, mp_size_t size,
                                   mp_limb_t divisor));
mp_limb_t refmpn_divmod_1c _PROTO ((mp_ptr rp, mp_srcptr sp, mp_size_t size,
                                    mp_limb_t divisor, mp_limb_t carry));
mp_limb_t refmpn_divrem_1 _PROTO ((mp_ptr rp, mp_size_t xsize,
                                   mp_srcptr sp, mp_size_t size,
                                   mp_limb_t divisor));
mp_limb_t refmpn_divrem_1c _PROTO ((mp_ptr rp, mp_size_t xsize,
                                    mp_srcptr sp, mp_size_t size,
                                    mp_limb_t divisor, mp_limb_t carry));

void refmpn_fill _PROTO ((mp_ptr p, mp_size_t s, mp_limb_t v));

mp_limb_t refmpn_gcd_1 _PROTO ((mp_srcptr xp, mp_size_t xsize, mp_limb_t y));
mp_limb_t refmpn_gcd _PROTO ((mp_ptr gp, mp_ptr xp, mp_size_t xsize,
                              mp_ptr yp, mp_size_t ysize));
mp_limb_t refmpn_gcd_finda _PROTO ((const mp_limb_t c[2]));

unsigned long refmpn_hamdist _PROTO ((mp_srcptr s1p, mp_srcptr s2p,
                                      mp_size_t size));

mp_limb_t refmpn_invert_limb _PROTO ((mp_limb_t d));
void refmpn_ior_n  _PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                            mp_size_t size));
void refmpn_iorn_n _PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                            mp_size_t size));

mp_limb_t refmpn_lshift _PROTO ((mp_ptr wp, mp_srcptr xp, mp_size_t size,
                                 unsigned shift));
mp_limb_t refmpn_lshift_or_copy _PROTO ((mp_ptr wp,
                                         mp_srcptr xp, mp_size_t size,
                                         unsigned shift));

mp_ptr refmpn_malloc_limbs _PROTO ((mp_size_t size));
mp_ptr refmpn_malloc_limbs_aligned _PROTO ((size_t n, size_t m));
mp_limb_t refmpn_msbone _PROTO ((mp_limb_t x));
mp_limb_t refmpn_msbone_mask _PROTO ((mp_limb_t x));
mp_ptr refmpn_memdup_limbs _PROTO ((mp_srcptr ptr, mp_size_t size));

mp_limb_t refmpn_mod_1 _PROTO ((mp_srcptr sp, mp_size_t size,
                                mp_limb_t divisor));
mp_limb_t refmpn_mod_1c _PROTO ((mp_srcptr sp, mp_size_t size,
                                 mp_limb_t divisor, mp_limb_t carry));

mp_limb_t refmpn_mul_1 _PROTO ((mp_ptr wp, mp_srcptr xp, mp_size_t size,
                                mp_limb_t multiplier));
mp_limb_t refmpn_mul_1c _PROTO ((mp_ptr wp, mp_srcptr xp, mp_size_t size,
                                 mp_limb_t multiplier, mp_limb_t carry));
mp_limb_t refmpn_mul_2 _PROTO ((mp_ptr dst, mp_srcptr src, mp_size_t size,
                                mp_limb_t low, mp_limb_t high));

void refmpn_mul_basecase _PROTO ((mp_ptr prodp,
                                  mp_srcptr up, mp_size_t usize,
                                  mp_srcptr vp, mp_size_t vsize));
void refmpn_mul_any _PROTO ((mp_ptr prodp,
                             mp_srcptr up, mp_size_t usize,
                             mp_srcptr vp, mp_size_t vsize));
void refmpn_mul_n _PROTO ((mp_ptr prodp, mp_srcptr up, mp_srcptr vp,
                           mp_size_t size));

void refmpn_nand_n _PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                            mp_size_t size));
void refmpn_nior_n _PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                            mp_size_t size));
mp_limb_t refmpn_neg_n _PROTO ((mp_ptr dst, mp_srcptr src, mp_size_t size));

unsigned long refmpn_popcount _PROTO ((mp_srcptr sp, mp_size_t size));
mp_limb_t refmpn_preinv_mod_1 _PROTO ((mp_srcptr sp, mp_size_t size,
                                       mp_limb_t divisor,
                                       mp_limb_t divisor_inverse));

mp_limb_t refmpn_rshift _PROTO ((mp_ptr wp, mp_srcptr xp, mp_size_t size,
                                 unsigned shift));
mp_limb_t refmpn_rshift_or_copy _PROTO ((mp_ptr wp,
                                         mp_srcptr xp, mp_size_t size,
                                         unsigned shift));

mp_limb_t refmpn_sb_divrem_mn _PROTO ((mp_ptr qp,
                                       mp_ptr np, mp_size_t nsize,
                                       mp_srcptr dp, mp_size_t dsize));
void refmpn_sqr _PROTO ((mp_ptr dst, mp_srcptr src, mp_size_t size));

mp_limb_t refmpn_sub _PROTO ((mp_ptr rp,
                              mp_srcptr s1p, mp_size_t s1size,
                              mp_srcptr s2p, mp_size_t s2size));
mp_limb_t refmpn_sub_1 _PROTO ((mp_ptr rp, mp_srcptr sp, mp_size_t size,
                                mp_limb_t n));
mp_limb_t refmpn_sub_n _PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                                mp_size_t size));
mp_limb_t refmpn_sub_nc _PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                                 mp_size_t size, mp_limb_t carry));
mp_limb_t refmpn_submul_1 _PROTO ((mp_ptr wp, mp_srcptr xp, mp_size_t size,
                                   mp_limb_t multiplier));
mp_limb_t refmpn_submul_1c _PROTO ((mp_ptr wp, mp_srcptr xp, mp_size_t size,
                                    mp_limb_t multiplier, mp_limb_t carry));

void refmpn_tdiv_qr _PROTO ((mp_ptr qp, mp_ptr rp, mp_size_t qxn,
                             mp_ptr np, mp_size_t nsize,
                             mp_srcptr dp, mp_size_t dsize));
int refmpn_tstbit _PROTO ((mp_srcptr ptr, mp_size_t size));

void refmpn_udiv_qrnnd _PROTO ((mp_limb_t *q, mp_limb_t *r,
                                mp_limb_t h, mp_limb_t l, mp_limb_t d));
mp_limb_t refmpn_umul_ppmm _PROTO ((mp_limb_t *lo, mp_limb_t x, mp_limb_t y));

void refmpn_xnor_n _PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                            mp_size_t size));
void refmpn_xor_n  _PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                            mp_size_t size));

void refmpn_zero _PROTO ((mp_ptr p, mp_size_t s));
int refmpn_zero_p _PROTO ((mp_srcptr ptr, mp_size_t size));


int refmpz_kronecker _PROTO ((mpz_srcptr a_orig, mpz_srcptr b_orig));
int refmpz_jacobi _PROTO ((mpz_srcptr a_orig, mpz_srcptr b_orig));
int refmpz_legendre _PROTO ((mpz_srcptr a_orig, mpz_srcptr b_orig));
int refmpz_kronecker_si _PROTO ((mpz_srcptr, long));
int refmpz_kronecker_ui _PROTO ((mpz_srcptr, unsigned long));
int refmpz_si_kronecker _PROTO ((long, mpz_srcptr));
int refmpz_ui_kronecker _PROTO ((unsigned long, mpz_srcptr));

void refmpz_pow_ui _PROTO ((mpz_ptr w, mpz_srcptr b, unsigned long e));


#if defined (__cplusplus)
}
#endif

#endif /* __TESTS_H__ */
