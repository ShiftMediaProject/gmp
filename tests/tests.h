/* Tests support prototypes etc.

Copyright 2000, 2001, 2002 Free Software Foundation, Inc.

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


void tests_start __GMP_PROTO ((void));
void tests_end __GMP_PROTO ((void));

void tests_memory_start __GMP_PROTO ((void));
void tests_memory_end __GMP_PROTO ((void));
void *tests_allocate __GMP_PROTO ((size_t size));
void *tests_reallocate __GMP_PROTO ((void *ptr, size_t old_size, size_t new_size));
void tests_free __GMP_PROTO ((void *ptr, size_t size));
void tests_free_nosize __GMP_PROTO ((void *ptr));
int tests_memory_valid __GMP_PROTO ((void *ptr));

void tests_rand_start __GMP_PROTO ((void));
void tests_rand_end __GMP_PROTO ((void));


#if HAVE_CALLING_CONVENTIONS
extern mp_limb_t (*calling_conventions_function) __GMP_PROTO ((ANYARGS));
mp_limb_t calling_conventions __GMP_PROTO ((ANYARGS));
int calling_conventions_check __GMP_PROTO ((void));
#define CALLING_CONVENTIONS(function) \
  (calling_conventions_function = (function), calling_conventions)
#define CALLING_CONVENTIONS_CHECK()    (calling_conventions_check())
#else
#define CALLING_CONVENTIONS(function)  (function)
#define CALLING_CONVENTIONS_CHECK()    1 /* always ok */
#endif


extern int mp_trace_base;
void mpn_trace __GMP_PROTO ((const char *name, mp_srcptr ptr, mp_size_t size));
void mpn_tracea __GMP_PROTO ((const char *name, const mp_ptr *a, int count,
                 mp_size_t size));
void mpn_tracen __GMP_PROTO ((const char *name, int num, mp_srcptr ptr,
                 mp_size_t size));
void mpn_trace_file __GMP_PROTO ((const char *filename,
                             mp_srcptr ptr, mp_size_t size));
void mpn_tracea_file __GMP_PROTO ((const char *filename,
                              const mp_ptr *a, int count, mp_size_t size));
void mpf_trace __GMP_PROTO ((const char *name, mpf_srcptr z));
void mpq_trace __GMP_PROTO ((const char *name, mpq_srcptr q));
void mpz_trace __GMP_PROTO ((const char *name, mpz_srcptr z));
void mpz_tracen __GMP_PROTO ((const char *name, int num, mpz_srcptr z));
void byte_trace __GMP_PROTO ((const char *, const void *, mp_size_t));
void byte_tracen __GMP_PROTO ((const char *, int, const void *, mp_size_t));


void spinner __GMP_PROTO ((void));
extern unsigned long  spinner_count;
extern int  spinner_wanted;
extern int  spinner_tick;


void *align_pointer __GMP_PROTO ((void *p, size_t align));
void *__gmp_allocate_func_aligned __GMP_PROTO ((size_t bytes, size_t align));
void *__gmp_allocate_or_reallocate __GMP_PROTO ((void *ptr,
                                          size_t oldsize, size_t newsize));
char *__gmp_allocate_strdup __GMP_PROTO ((const char *s));
char *strtoupper __GMP_PROTO ((char *s_orig));
mp_limb_t urandom __GMP_PROTO ((void));


void mpf_set_str_or_abort __GMP_PROTO ((mpf_ptr f, const char *str, int base));


void mpq_set_str_or_abort __GMP_PROTO ((mpq_ptr q, const char *str, int base));


void mpz_erandomb __GMP_PROTO ((mpz_ptr rop, gmp_randstate_t rstate,
                           unsigned long nbits));
void mpz_erandomb_nonzero __GMP_PROTO ((mpz_ptr rop, gmp_randstate_t rstate,
                                   unsigned long nbits));
void mpz_errandomb __GMP_PROTO ((mpz_ptr rop, gmp_randstate_t rstate,
                            unsigned long nbits));
void mpz_errandomb_nonzero __GMP_PROTO ((mpz_ptr rop, gmp_randstate_t rstate,
                                    unsigned long nbits));
void mpz_flipbit __GMP_PROTO ((mpz_ptr r, unsigned long bit));
void mpz_init_set_n __GMP_PROTO ((mpz_ptr z, mp_srcptr p, mp_size_t size));
void mpz_negrandom __GMP_PROTO ((mpz_ptr rop, gmp_randstate_t rstate));
int mpz_pow2abs_p __GMP_PROTO ((mpz_srcptr z)) __GMP_ATTRIBUTE_PURE;
void mpz_set_n __GMP_PROTO ((mpz_ptr z, mp_srcptr p, mp_size_t size));
void mpz_set_str_or_abort __GMP_PROTO ((mpz_ptr z, const char *str, int base));

mp_size_t mpn_diff_highest __GMP_PROTO ((mp_srcptr p1, mp_srcptr p2, mp_size_t n)) __GMP_ATTRIBUTE_PURE;
mp_size_t mpn_diff_lowest __GMP_PROTO ((mp_srcptr p1, mp_srcptr p2, mp_size_t n)) __GMP_ATTRIBUTE_PURE;
mp_size_t byte_diff_highest __GMP_PROTO ((const void *p1, const void *p2, mp_size_t size)) __GMP_ATTRIBUTE_PURE;
mp_size_t byte_diff_lowest __GMP_PROTO ((const void *p1, const void *p2, mp_size_t size)) __GMP_ATTRIBUTE_PURE;


void refmpf_add __GMP_PROTO ((mpf_ptr, mpf_srcptr, mpf_srcptr));
void refmpf_sub __GMP_PROTO ((mpf_ptr, mpf_srcptr, mpf_srcptr));


mp_limb_t refmpn_add __GMP_PROTO ((mp_ptr rp,
                              mp_srcptr s1p, mp_size_t s1size,
                              mp_srcptr s2p, mp_size_t s2size));
mp_limb_t refmpn_add_1 __GMP_PROTO ((mp_ptr rp, mp_srcptr sp, mp_size_t size,
                                mp_limb_t n));
mp_limb_t refmpn_add_n __GMP_PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                                mp_size_t size));
mp_limb_t refmpn_add_nc __GMP_PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                                 mp_size_t size, mp_limb_t carry));
mp_limb_t refmpn_addmul_1 __GMP_PROTO ((mp_ptr wp, mp_srcptr xp, mp_size_t size,
                                   mp_limb_t multiplier));
mp_limb_t refmpn_addmul_1c __GMP_PROTO ((mp_ptr wp, mp_srcptr xp, mp_size_t size,
                                    mp_limb_t multiplier, mp_limb_t carry));
mp_limb_t refmpn_addsub_n __GMP_PROTO ((mp_ptr r1p, mp_ptr r2p,
                                   mp_srcptr s1p, mp_srcptr s2p,
                                   mp_size_t size));
mp_limb_t refmpn_addsub_nc __GMP_PROTO ((mp_ptr r1p, mp_ptr r2p,
                                    mp_srcptr s1p, mp_srcptr s2p,
                                    mp_size_t size, mp_limb_t carry));

void refmpn_and_n  __GMP_PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                            mp_size_t size));
void refmpn_andn_n __GMP_PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                            mp_size_t size));

mp_limb_t refmpn_bswap_limb __GMP_PROTO ((mp_limb_t src));

void refmpn_clrbit __GMP_PROTO ((mp_ptr, unsigned long));
int refmpn_cmp __GMP_PROTO ((mp_srcptr s1p, mp_srcptr s2p, mp_size_t size));
int refmpn_cmp_allowzero __GMP_PROTO ((mp_srcptr, mp_srcptr, mp_size_t));
int refmpn_cmp_twosizes __GMP_PROTO ((mp_srcptr xp, mp_size_t xsize,
                                 mp_srcptr yp, mp_size_t ysize));

void refmpn_com_n __GMP_PROTO ((mp_ptr rp, mp_srcptr sp, mp_size_t size));
void refmpn_copy  __GMP_PROTO ((mp_ptr rp, mp_srcptr sp, mp_size_t size));
void refmpn_copyi __GMP_PROTO ((mp_ptr rp, mp_srcptr sp, mp_size_t size));
void refmpn_copyd __GMP_PROTO ((mp_ptr rp, mp_srcptr sp, mp_size_t size));

unsigned refmpn_count_leading_zeros __GMP_PROTO ((mp_limb_t x));
unsigned refmpn_count_trailing_zeros __GMP_PROTO ((mp_limb_t x));

mp_limb_t refmpn_divexact_by3 __GMP_PROTO ((mp_ptr rp, mp_srcptr sp,
                                       mp_size_t size));
mp_limb_t refmpn_divexact_by3c __GMP_PROTO ((mp_ptr rp, mp_srcptr sp,
                                       mp_size_t size, mp_limb_t carry));

mp_limb_t refmpn_divmod_1 __GMP_PROTO ((mp_ptr rp, mp_srcptr sp, mp_size_t size,
                                   mp_limb_t divisor));
mp_limb_t refmpn_divmod_1c __GMP_PROTO ((mp_ptr rp, mp_srcptr sp, mp_size_t size,
                                    mp_limb_t divisor, mp_limb_t carry));
mp_limb_t refmpn_divrem_1 __GMP_PROTO ((mp_ptr rp, mp_size_t xsize,
                                   mp_srcptr sp, mp_size_t size,
                                   mp_limb_t divisor));
mp_limb_t refmpn_divrem_1c __GMP_PROTO ((mp_ptr rp, mp_size_t xsize,
                                    mp_srcptr sp, mp_size_t size,
                                    mp_limb_t divisor, mp_limb_t carry));

int refmpn_equal_anynail __GMP_PROTO ((mp_srcptr, mp_srcptr, mp_size_t));

void refmpn_fill __GMP_PROTO ((mp_ptr p, mp_size_t s, mp_limb_t v));

mp_limb_t refmpn_gcd_1 __GMP_PROTO ((mp_srcptr xp, mp_size_t xsize, mp_limb_t y));
mp_limb_t refmpn_gcd __GMP_PROTO ((mp_ptr gp, mp_ptr xp, mp_size_t xsize,
                              mp_ptr yp, mp_size_t ysize));
mp_limb_t refmpn_gcd_finda __GMP_PROTO ((const mp_limb_t c[2]));

size_t refmpn_get_str __GMP_PROTO ((unsigned char *, int, mp_ptr, mp_size_t));

unsigned long refmpn_hamdist __GMP_PROTO ((mp_srcptr s1p, mp_srcptr s2p,
                                      mp_size_t size));

mp_limb_t refmpn_invert_limb __GMP_PROTO ((mp_limb_t d));
void refmpn_ior_n  __GMP_PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                            mp_size_t size));
void refmpn_iorn_n __GMP_PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                            mp_size_t size));

mp_limb_t refmpn_lshift __GMP_PROTO ((mp_ptr wp, mp_srcptr xp, mp_size_t size,
                                 unsigned shift));
mp_limb_t refmpn_lshift_or_copy __GMP_PROTO ((mp_ptr wp,
                                         mp_srcptr xp, mp_size_t size,
                                         unsigned shift));

mp_ptr refmpn_malloc_limbs __GMP_PROTO ((mp_size_t size));
mp_ptr refmpn_malloc_limbs_aligned __GMP_PROTO ((size_t n, size_t m));
mp_limb_t refmpn_msbone __GMP_PROTO ((mp_limb_t x));
mp_limb_t refmpn_msbone_mask __GMP_PROTO ((mp_limb_t x));
mp_ptr refmpn_memdup_limbs __GMP_PROTO ((mp_srcptr ptr, mp_size_t size));

mp_limb_t refmpn_mod_1 __GMP_PROTO ((mp_srcptr sp, mp_size_t size,
                                mp_limb_t divisor));
mp_limb_t refmpn_mod_1c __GMP_PROTO ((mp_srcptr sp, mp_size_t size,
                                 mp_limb_t divisor, mp_limb_t carry));
mp_limb_t refmpn_mod_34lsub1 __GMP_PROTO ((mp_srcptr p, mp_size_t n));

mp_limb_t refmpn_mul_1 __GMP_PROTO ((mp_ptr wp, mp_srcptr xp, mp_size_t size,
                                mp_limb_t multiplier));
mp_limb_t refmpn_mul_1c __GMP_PROTO ((mp_ptr wp, mp_srcptr xp, mp_size_t size,
                                 mp_limb_t multiplier, mp_limb_t carry));
mp_limb_t refmpn_mul_2 __GMP_PROTO ((mp_ptr dst, mp_srcptr src, mp_size_t size,
                                mp_srcptr mult));

void refmpn_mul_basecase __GMP_PROTO ((mp_ptr prodp,
                                  mp_srcptr up, mp_size_t usize,
                                  mp_srcptr vp, mp_size_t vsize));
void refmpn_mul_any __GMP_PROTO ((mp_ptr prodp,
                             mp_srcptr up, mp_size_t usize,
                             mp_srcptr vp, mp_size_t vsize));
void refmpn_mul_n __GMP_PROTO ((mp_ptr prodp, mp_srcptr up, mp_srcptr vp,
                           mp_size_t size));

void refmpn_nand_n __GMP_PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                            mp_size_t size));
void refmpn_nior_n __GMP_PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                            mp_size_t size));
mp_limb_t refmpn_neg_n __GMP_PROTO ((mp_ptr dst, mp_srcptr src, mp_size_t size));

unsigned long refmpn_popcount __GMP_PROTO ((mp_srcptr sp, mp_size_t size));
mp_limb_t refmpn_preinv_divrem_1 __GMP_PROTO ((mp_ptr rp, mp_size_t xsize,
                                          mp_srcptr sp, mp_size_t size,
                                          mp_limb_t divisor,
                                          mp_limb_t inverse, unsigned shift));
mp_limb_t refmpn_preinv_mod_1 __GMP_PROTO ((mp_srcptr sp, mp_size_t size,
                                       mp_limb_t divisor,
                                       mp_limb_t divisor_inverse));

void refmpn_random __GMP_PROTO ((mp_ptr, mp_size_t));
void refmpn_random2 __GMP_PROTO ((mp_ptr, mp_size_t));
mp_limb_t refmpn_random_limb __GMP_PROTO ((void));

mp_limb_t refmpn_rshift __GMP_PROTO ((mp_ptr wp, mp_srcptr xp, mp_size_t size,
                                 unsigned shift));
mp_limb_t refmpn_rshift_or_copy __GMP_PROTO ((mp_ptr wp,
                                         mp_srcptr xp, mp_size_t size,
                                         unsigned shift));

mp_limb_t refmpn_sb_divrem_mn __GMP_PROTO ((mp_ptr qp,
                                       mp_ptr np, mp_size_t nsize,
                                       mp_srcptr dp, mp_size_t dsize));
unsigned long refmpn_scan0 __GMP_PROTO ((mp_srcptr, unsigned long));
unsigned long refmpn_scan1 __GMP_PROTO ((mp_srcptr, unsigned long));
void refmpn_setbit __GMP_PROTO ((mp_ptr, unsigned long));
void refmpn_sqr __GMP_PROTO ((mp_ptr dst, mp_srcptr src, mp_size_t size));

mp_limb_t refmpn_sub __GMP_PROTO ((mp_ptr rp,
                              mp_srcptr s1p, mp_size_t s1size,
                              mp_srcptr s2p, mp_size_t s2size));
mp_limb_t refmpn_sub_1 __GMP_PROTO ((mp_ptr rp, mp_srcptr sp, mp_size_t size,
                                mp_limb_t n));
mp_limb_t refmpn_sub_n __GMP_PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                                mp_size_t size));
mp_limb_t refmpn_sub_nc __GMP_PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                                 mp_size_t size, mp_limb_t carry));
mp_limb_t refmpn_submul_1 __GMP_PROTO ((mp_ptr wp, mp_srcptr xp, mp_size_t size,
                                   mp_limb_t multiplier));
mp_limb_t refmpn_submul_1c __GMP_PROTO ((mp_ptr wp, mp_srcptr xp, mp_size_t size,
                                    mp_limb_t multiplier, mp_limb_t carry));

void refmpn_tdiv_qr __GMP_PROTO ((mp_ptr qp, mp_ptr rp, mp_size_t qxn,
                             mp_ptr np, mp_size_t nsize,
                             mp_srcptr dp, mp_size_t dsize));
int refmpn_tstbit __GMP_PROTO ((mp_srcptr, unsigned long));

mp_limb_t refmpn_udiv_qrnnd __GMP_PROTO ((mp_limb_t *, mp_limb_t, mp_limb_t, mp_limb_t));
mp_limb_t refmpn_udiv_qrnnd_r __GMP_PROTO ((mp_limb_t, mp_limb_t, mp_limb_t, mp_limb_t *));
mp_limb_t refmpn_umul_ppmm __GMP_PROTO ((mp_limb_t *, mp_limb_t, mp_limb_t));
mp_limb_t refmpn_umul_ppmm_r __GMP_PROTO ((mp_limb_t, mp_limb_t, mp_limb_t *));

void refmpn_xnor_n __GMP_PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                            mp_size_t size));
void refmpn_xor_n  __GMP_PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                            mp_size_t size));

void refmpn_zero __GMP_PROTO ((mp_ptr p, mp_size_t s));
int refmpn_zero_p __GMP_PROTO ((mp_srcptr ptr, mp_size_t size));


void refmpq_add __GMP_PROTO ((mpq_ptr w, mpq_srcptr x, mpq_srcptr y));
void refmpq_sub __GMP_PROTO ((mpq_ptr w, mpq_srcptr x, mpq_srcptr y));


unsigned long refmpz_hamdist __GMP_PROTO ((mpz_srcptr x, mpz_srcptr y));
int refmpz_kronecker __GMP_PROTO ((mpz_srcptr a_orig, mpz_srcptr b_orig));
int refmpz_jacobi __GMP_PROTO ((mpz_srcptr a_orig, mpz_srcptr b_orig));
int refmpz_legendre __GMP_PROTO ((mpz_srcptr a_orig, mpz_srcptr b_orig));
int refmpz_kronecker_si __GMP_PROTO ((mpz_srcptr, long));
int refmpz_kronecker_ui __GMP_PROTO ((mpz_srcptr, unsigned long));
int refmpz_si_kronecker __GMP_PROTO ((long, mpz_srcptr));
int refmpz_ui_kronecker __GMP_PROTO ((unsigned long, mpz_srcptr));

void refmpz_pow_ui __GMP_PROTO ((mpz_ptr w, mpz_srcptr b, unsigned long e));


#if defined (__cplusplus)
}
#endif

#endif /* __TESTS_H__ */
