/* Prototypes for refmpn functions. */

mp_limb_t refmpn_add_n (mp_ptr wp, mp_srcptr xp, mp_srcptr yp, mp_size_t size);
mp_limb_t refmpn_add_nc (mp_ptr wp, mp_srcptr xp, mp_srcptr yp, mp_size_t size,
                         mp_limb_t carry);
mp_limb_t refmpn_addmul_1 (mp_ptr wp, mp_srcptr xp, mp_size_t size,
                           mp_limb_t multiplier);
mp_limb_t refmpn_addmul_1c (mp_ptr wp, mp_srcptr xp, mp_size_t size,
                            mp_limb_t multiplier, mp_limb_t carry);
mp_limb_t refmpn_addsub_n (mp_ptr r1p, mp_ptr r2p,
                           mp_srcptr s1p, mp_srcptr s2p, mp_size_t n);

void refmpn_and_n  (mp_ptr wp, mp_srcptr xp, mp_srcptr yp, mp_size_t size);
void refmpn_andn_n (mp_ptr wp, mp_srcptr xp, mp_srcptr yp, mp_size_t size);

mp_limb_t refmpn_cmp (mp_srcptr s1p, mp_srcptr s2p, mp_size_t size);

void refmpn_com_n (mp_ptr rp, mp_srcptr sp, mp_size_t size);
void refmpn_copyi (mp_ptr rp, mp_srcptr sp, mp_size_t size);
void refmpn_copyd (mp_ptr rp, mp_srcptr sp, mp_size_t size);

mp_limb_t refmpn_divexact_by3 (mp_ptr rp, mp_srcptr sp, mp_size_t size);

mp_limb_t refmpn_divmod_1 (mp_ptr rp, mp_srcptr sp, mp_size_t size,
                           mp_limb_t divisor);
mp_limb_t refmpn_divmod_1c (mp_ptr rp, mp_srcptr sp, mp_size_t size,
                            mp_limb_t divisor, mp_limb_t carry);
mp_limb_t refmpn_divrem_1 (mp_ptr rp, mp_size_t xsize,
                           mp_srcptr sp, mp_size_t size, mp_limb_t divisor);
mp_limb_t refmpn_divrem_1c (mp_ptr rp, mp_size_t xsize,
                            mp_srcptr sp, mp_size_t size, mp_limb_t divisor,
                            mp_limb_t carry);

void refmpn_fill (mp_ptr p, mp_size_t s, mp_limb_t v);

void refmpn_ior_n  (mp_ptr wp, mp_srcptr xp, mp_srcptr yp, mp_size_t size);
void refmpn_iorn_n (mp_ptr wp, mp_srcptr xp, mp_srcptr yp, mp_size_t size);

mp_limb_t refmpn_lshift (mp_ptr wp, mp_srcptr xp, mp_size_t size,
                         unsigned shift);

mp_limb_t refmpn_mod_1 (mp_srcptr sp, mp_size_t size, mp_limb_t divisor);
mp_limb_t refmpn_mod_1c (mp_srcptr sp, mp_size_t size, mp_limb_t divisor,
                         mp_limb_t carry);

mp_limb_t refmpn_mul_1 (mp_ptr wp, mp_srcptr xp, mp_size_t size,
                        mp_limb_t multiplier);
mp_limb_t refmpn_mul_1c (mp_ptr wp, mp_srcptr xp, mp_size_t size,
                         mp_limb_t multiplier, mp_limb_t carry);

void refmpn_mul_basecase (mp_ptr prodp,
                          mp_srcptr up, mp_size_t usize,
                          mp_srcptr vp, mp_size_t vsize);
void refmpn_mul_n (mp_ptr prodp, mp_srcptr up, mp_srcptr vp, mp_size_t size);

void refmpn_nand_n (mp_ptr wp, mp_srcptr xp, mp_srcptr yp, mp_size_t size);
void refmpn_nior_n (mp_ptr wp, mp_srcptr xp, mp_srcptr yp, mp_size_t size);

mp_limb_t refmpn_rshift (mp_ptr wp, mp_srcptr xp, mp_size_t size,
                         unsigned shift);

void refmpn_sqr (mp_ptr dst, mp_srcptr src, mp_size_t size);

mp_limb_t refmpn_sub_n (mp_ptr wp, mp_srcptr xp, mp_srcptr yp, mp_size_t size);
mp_limb_t refmpn_sub_nc (mp_ptr wp, mp_srcptr xp, mp_srcptr yp, mp_size_t size,
                         mp_limb_t carry);
mp_limb_t refmpn_submul_1 (mp_ptr wp, mp_srcptr xp, mp_size_t size,
                           mp_limb_t multiplier);
mp_limb_t refmpn_submul_1c (mp_ptr wp, mp_srcptr xp, mp_size_t size,
                            mp_limb_t multiplier, mp_limb_t carry);

void refmpn_xnor_n (mp_ptr wp, mp_srcptr xp, mp_srcptr yp, mp_size_t size);
void refmpn_xor_n  (mp_ptr wp, mp_srcptr xp, mp_srcptr yp, mp_size_t size);

mp_ptr refmpn_memdup_limbs (mp_srcptr ptr, mp_size_t size);
mp_ptr refmpn_malloc_limbs (mp_size_t size);
