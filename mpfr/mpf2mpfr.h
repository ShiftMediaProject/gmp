/* types */
#define mpf_t mpfr_t


#undef mpf_ceil
#undef mpf_clear
#undef mpf_cmp
#undef mpf_cmp_si
#undef mpf_cmp_ui
#undef mpf_eq
#undef mpf_floor
#undef mpf_get_d
#undef mpf_get_prec
#undef mpf_init
#undef mpf_init2
#undef mpf_random2
#undef mpf_set_default_prec
#undef mpf_set_prec
#undef mpf_set_prec_raw
#undef mpf_trunc
#undef mpf_sgn
#undef mpf_swap

/* functions which take as argument the rounding mode */
#undef mpf_abs
#undef mpf_add
#undef mpf_add_ui
#undef mpf_div
#undef mpf_div_ui
#undef mpf_div_2exp
#undef mpf_dump
#undef mpf_get_str
#undef mpf_inp_str
#undef mpf_set_str
#undef mpf_init_set
#undef mpf_init_set_d
#undef mpf_init_set_si
#undef mpf_init_set_str
#undef mpf_init_set_ui
#undef mpf_mul
#undef mpf_mul_2exp
#undef mpf_mul_ui
#undef mpf_neg
#undef mpf_out_str
#undef mpf_pow_ui
#undef mpf_reldiff
#undef mpf_set
#undef mpf_set_d
#undef mpf_set_q
#undef mpf_set_si
#undef mpf_set_ui
#undef mpf_set_z
#undef mpf_sqrt
#undef mpf_sqrt_ui
#undef mpf_sub
#undef mpf_sub_ui
#undef mpf_ui_div
#undef mpf_ui_sub
#undef mpf_urandomb



/* functions which don't take as argument the rounding mode */
#define mpf_ceil mpfr_ceil
#define mpf_clear mpfr_clear
#define mpf_cmp mpfr_cmp
#define mpf_cmp_si mpfr_cmp_si
#define mpf_cmp_ui mpfr_cmp_ui
#define mpf_eq mpfr_eq
#define mpf_floor mpfr_floor
#define mpf_get_d mpfr_get_d
#define mpf_get_prec mpfr_get_prec
#define mpf_init mpfr_init
#define mpf_init2 mpfr_init2
#define mpf_random2 mpfr_random2
#define mpf_set_default_prec mpfr_set_default_prec
#define mpf_set_prec(x,p) mpfr_round(x,__gmp_default_rounding_mode,p)
#define mpf_set_prec_raw mpfr_set_prec_raw
#define mpf_trunc mpfr_trunc
#define mpf_sgn mpfr_sgn
#define mpf_swap mpfr_swap

/* functions which take as argument the rounding mode */
#define mpf_abs(x,y) mpfr_abs(x,y,__gmp_default_rounding_mode)
#define mpf_add(x,y,z) mpfr_add(x,y,z,__gmp_default_rounding_mode) 
#define mpf_add_ui(x,y,z) \
             mpfr_add_ui(x,y,z,__gmp_default_rounding_mode)
#define mpf_div(x,y,z) mpfr_div(x,y,z,__gmp_default_rounding_mode) 
#define mpf_div_ui(x,y,z) \
                          mpfr_div_ui(x,y,z,__gmp_default_rounding_mode) 
#define mpf_div_2exp(x,y,z) \
                         mpfr_div_2exp(x,y,z,__gmp_default_rounding_mode) 
#define mpf_dump(x,y,z) \
                mpfr_dump(x,y,z,__gmp_default_rounding_mode)
#define mpf_get_str(x,y,z,t,u) \
               mpfr_get_str(x,y,z,t,u,__gmp_default_rounding_mode) 
#define mpf_inp_str(x,y,z) mpfr_inp_str(x,y,z,__gmp_default_rounding_mode)
#define mpf_set_str(x,y,z) mpfr_set_str(x,y,z,__gmp_default_rounding_mode)
#define mpf_init_set(x,y) mpfr_init_set(x,y,__gmp_default_rounding_mode) 
#define mpf_init_set_d(x,y) mpfr_init_set_d(x,y,__gmp_default_rounding_mode) 
#define mpf_init_set_si(x,y) mpfr_init_set_si(x,y,__gmp_default_rounding_mode) 
#define mpf_init_set_str(x,y,z) mpfr_init_set_str(x,y,z,__gmp_default_rounding_mode) 
#define mpf_init_set_ui(x,y) mpfr_init_set_ui(x,y,__gmp_default_rounding_mode) 
#define mpf_mul(x,y,z) mpfr_mul(x,y,z,__gmp_default_rounding_mode) 
#define mpf_mul_2exp(x,y,z) mpfr_mul_2exp(x,y,z,__gmp_default_rounding_mode) 
#define mpf_mul_ui(x,y,z) mpfr_mul_ui(x,y,z,__gmp_default_rounding_mode) 
#define mpf_neg(x,y) mpfr_neg(x,y,__gmp_default_rounding_mode) 
#define mpf_out_str(x,y,z,t) mpfr_out_str(x,y,z,t,__gmp_default_rounding_mode) 
#define mpf_pow_ui(x,y,z) mpfr_pow_ui(x,y,z,__gmp_default_rounding_mode) 
#define mpf_reldiff(x,y,z) mpfr_reldiff(x,y,z,__gmp_default_rounding_mode)
#define mpf_set(x,y) mpfr_set(x,y,__gmp_default_rounding_mode) 
#define mpf_set_d(x,y) mpfr_set_d(x,y,__gmp_default_rounding_mode) 
#define mpf_set_q(x,y) mpfr_set_q(x,y,__gmp_default_rounding_mode) 
#define mpf_set_si(x,y) mpfr_set_si(x,y,__gmp_default_rounding_mode) 
#define mpf_set_ui(x,y) mpfr_set_ui(x,y,__gmp_default_rounding_mode) 
#define mpf_set_z(x,y) mpfr_set_z(x,y,__gmp_default_rounding_mode) 
#define mpf_sqrt(x,y) mpfr_sqrt(x,y,__gmp_default_rounding_mode) 
#define mpf_sqrt_ui(x,y) mpfr_sqrt_ui(x,y,__gmp_default_rounding_mode)
#define mpf_sub(x,y,z) mpfr_sub(x,y,z,__gmp_default_rounding_mode) 
#define mpf_sub_ui(x,y,z) mpfr_sub_ui(x,y,z,__gmp_default_rounding_mode) 
#define mpf_ui_div(x,y,z) mpfr_ui_div(x,y,z,__gmp_default_rounding_mode)
#define mpf_ui_sub(x,y,z) mpfr_ui_sub(x,y,z,__gmp_default_rounding_mode)
#define mpf_urandomb(x,y,n) mpfr_urandomb(x,y)
