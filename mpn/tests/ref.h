/* Prototypes for refmpn functions. */

/*
Copyright (C) 2000 Free Software Foundation, Inc.

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
MA 02111-1307, USA.
*/


mp_limb_t refmpn_add_n _PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                                mp_size_t size));
mp_limb_t refmpn_add_nc _PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                                 mp_size_t size, mp_limb_t carry));
mp_limb_t refmpn_addmul_1 _PROTO ((mp_ptr wp, mp_srcptr xp, mp_size_t size,
                                   mp_limb_t multiplier));
mp_limb_t refmpn_addmul_1c _PROTO ((mp_ptr wp, mp_srcptr xp, mp_size_t size,
                                    mp_limb_t multiplier, mp_limb_t carry));
mp_limb_t refmpn_addsub_n _PROTO ((mp_ptr r1p, mp_ptr r2p,
                                   mp_srcptr s1p, mp_srcptr s2p, mp_size_t n));

void refmpn_and_n  _PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                            mp_size_t size));
void refmpn_andn_n _PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                            mp_size_t size));

mp_limb_t refmpn_cmp _PROTO ((mp_srcptr s1p, mp_srcptr s2p, mp_size_t size));

void refmpn_com_n _PROTO ((mp_ptr rp, mp_srcptr sp, mp_size_t size));
void refmpn_copyi _PROTO ((mp_ptr rp, mp_srcptr sp, mp_size_t size));
void refmpn_copyd _PROTO ((mp_ptr rp, mp_srcptr sp, mp_size_t size));

mp_limb_t refmpn_divexact_by3 _PROTO ((mp_ptr rp, mp_srcptr sp,
                                       mp_size_t size));

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

void refmpn_ior_n  _PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                            mp_size_t size));
void refmpn_iorn_n _PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                            mp_size_t size));

mp_limb_t refmpn_lshift _PROTO ((mp_ptr wp, mp_srcptr xp, mp_size_t size,
                                 unsigned shift));

mp_limb_t refmpn_mod_1 _PROTO ((mp_srcptr sp, mp_size_t size,
                                mp_limb_t divisor));
mp_limb_t refmpn_mod_1c _PROTO ((mp_srcptr sp, mp_size_t size,
                                 mp_limb_t divisor, mp_limb_t carry));

mp_limb_t refmpn_mul_1 _PROTO ((mp_ptr wp, mp_srcptr xp, mp_size_t size,
                                mp_limb_t multiplier));
mp_limb_t refmpn_mul_1c _PROTO ((mp_ptr wp, mp_srcptr xp, mp_size_t size,
                                 mp_limb_t multiplier, mp_limb_t carry));

void refmpn_mul_basecase _PROTO ((mp_ptr prodp,
                                  mp_srcptr up, mp_size_t usize,
                                  mp_srcptr vp, mp_size_t vsize));
void refmpn_mul_n _PROTO ((mp_ptr prodp, mp_srcptr up, mp_srcptr vp,
                           mp_size_t size));

void refmpn_nand_n _PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                            mp_size_t size));
void refmpn_nior_n _PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                            mp_size_t size));

mp_limb_t refmpn_rshift _PROTO ((mp_ptr wp, mp_srcptr xp, mp_size_t size,
                                 unsigned shift));

void refmpn_sqr _PROTO ((mp_ptr dst, mp_srcptr src, mp_size_t size));

mp_limb_t refmpn_sub_n _PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                                mp_size_t size));
mp_limb_t refmpn_sub_nc _PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                                 mp_size_t size, mp_limb_t carry));
mp_limb_t refmpn_submul_1 _PROTO ((mp_ptr wp, mp_srcptr xp, mp_size_t size,
                                   mp_limb_t multiplier));
mp_limb_t refmpn_submul_1c _PROTO ((mp_ptr wp, mp_srcptr xp, mp_size_t size,
                                    mp_limb_t multiplier, mp_limb_t carry));

void refmpn_xnor_n _PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                            mp_size_t size));
void refmpn_xor_n  _PROTO ((mp_ptr wp, mp_srcptr xp, mp_srcptr yp,
                            mp_size_t size));

mp_ptr refmpn_memdup_limbs _PROTO ((mp_srcptr ptr, mp_size_t size));
mp_ptr refmpn_malloc_limbs _PROTO ((mp_size_t size));
