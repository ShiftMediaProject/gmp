/* Templates for defines setup by configure.

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

@TOP@

/* Define one (and only one) of these for the CPU host.
   Only hosts that are going to be tested for need to be in this list,
   not everything that can possibly be selected.
 */
#undef HAVE_HOST_CPU_alpha
#undef HAVE_HOST_CPU_alphaev5
#undef HAVE_HOST_CPU_alphaev6
#undef HAVE_HOST_CPU_alphaev67

#undef HAVE_HOST_CPU_m68k
#undef HAVE_HOST_CPU_m68000
#undef HAVE_HOST_CPU_m68010
#undef HAVE_HOST_CPU_m68020
#undef HAVE_HOST_CPU_m68030
#undef HAVE_HOST_CPU_m68040
#undef HAVE_HOST_CPU_m68060
#undef HAVE_HOST_CPU_m68302
#undef HAVE_HOST_CPU_m68360

#undef HAVE_HOST_CPU_powerpc604
#undef HAVE_HOST_CPU_powerpc604e
#undef HAVE_HOST_CPU_powerpc750
#undef HAVE_HOST_CPU_powerpc7400

#undef HAVE_HOST_CPU_sparc
#undef HAVE_HOST_CPU_sparcv8
#undef HAVE_HOST_CPU_supersparc
#undef HAVE_HOST_CPU_sparclite
#undef HAVE_HOST_CPU_microsparc
#undef HAVE_HOST_CPU_ultrasparc1
#undef HAVE_HOST_CPU_ultrasparc2
#undef HAVE_HOST_CPU_sparc64

#undef HAVE_HOST_CPU_hppa1_0
#undef HAVE_HOST_CPU_hppa1_1
#undef HAVE_HOST_CPU_hppa2_0n
#undef HAVE_HOST_CPU_hppa2_0w

#undef HAVE_HOST_CPU_i386
#undef HAVE_HOST_CPU_i486
#undef HAVE_HOST_CPU_i586
#undef HAVE_HOST_CPU_i686
#undef HAVE_HOST_CPU_pentium
#undef HAVE_HOST_CPU_pentiummmx
#undef HAVE_HOST_CPU_pentiumpro
#undef HAVE_HOST_CPU_pentium2
#undef HAVE_HOST_CPU_pentium3
#undef HAVE_HOST_CPU_k5
#undef HAVE_HOST_CPU_k6
#undef HAVE_HOST_CPU_k62
#undef HAVE_HOST_CPU_k63
#undef HAVE_HOST_CPU_athlon

/* a dummy to make autoheader happy */
#undef HAVE_HOST_CPU_


/* Define one (and only one) of these for the CPU host family.
   Only hosts that are going to be tested for need to be in this list,
   not everything that can possibly be selected.
 */
#undef HAVE_HOST_CPU_FAMILY_power
#undef HAVE_HOST_CPU_FAMILY_powerpc
#undef HAVE_HOST_CPU_FAMILY_x86


/* Define if we have native implementation of function. */
#undef HAVE_NATIVE_mpn_add
#undef HAVE_NATIVE_mpn_add_1
#undef HAVE_NATIVE_mpn_add_n
#undef HAVE_NATIVE_mpn_add_nc
#undef HAVE_NATIVE_mpn_addmul_1
#undef HAVE_NATIVE_mpn_addmul_1c
#undef HAVE_NATIVE_mpn_addmul_2
#undef HAVE_NATIVE_mpn_addmul_3
#undef HAVE_NATIVE_mpn_addmul_4
#undef HAVE_NATIVE_mpn_addsub_n
#undef HAVE_NATIVE_mpn_addsub_nc
#undef HAVE_NATIVE_mpn_and_n
#undef HAVE_NATIVE_mpn_andn_n
#undef HAVE_NATIVE_mpn_bdivmod
#undef HAVE_NATIVE_mpn_cmp
#undef HAVE_NATIVE_mpn_com_n
#undef HAVE_NATIVE_mpn_copyd
#undef HAVE_NATIVE_mpn_copyi
#undef HAVE_NATIVE_mpn_divexact_1
#undef HAVE_NATIVE_mpn_divexact_by3c
#undef HAVE_NATIVE_mpn_divrem
#undef HAVE_NATIVE_mpn_divrem_1
#undef HAVE_NATIVE_mpn_divrem_1c
#undef HAVE_NATIVE_mpn_divrem_2
#undef HAVE_NATIVE_mpn_divrem_newton
#undef HAVE_NATIVE_mpn_divrem_classic
#undef HAVE_NATIVE_mpn_dump
#undef HAVE_NATIVE_mpn_gcd
#undef HAVE_NATIVE_mpn_gcd_1
#undef HAVE_NATIVE_mpn_gcd_finda
#undef HAVE_NATIVE_mpn_gcdext
#undef HAVE_NATIVE_mpn_get_str
#undef HAVE_NATIVE_mpn_hamdist
#undef HAVE_NATIVE_mpn_invert_limb
#undef HAVE_NATIVE_mpn_ior_n
#undef HAVE_NATIVE_mpn_iorn_n
#undef HAVE_NATIVE_mpn_lshift
#undef HAVE_NATIVE_mpn_mod_1
#undef HAVE_NATIVE_mpn_mod_1c
#undef HAVE_NATIVE_mpn_modexact_1_odd
#undef HAVE_NATIVE_mpn_modexact_1c_odd
#undef HAVE_NATIVE_mpn_mul
#undef HAVE_NATIVE_mpn_mul_1
#undef HAVE_NATIVE_mpn_mul_1c
#undef HAVE_NATIVE_mpn_mul_2
#undef HAVE_NATIVE_mpn_mul_3
#undef HAVE_NATIVE_mpn_mul_4
#undef HAVE_NATIVE_mpn_mul_basecase
#undef HAVE_NATIVE_mpn_mul_n
#undef HAVE_NATIVE_mpn_nand_n
#undef HAVE_NATIVE_mpn_nior_n
#undef HAVE_NATIVE_mpn_perfect_square_p
#undef HAVE_NATIVE_mpn_popcount
#undef HAVE_NATIVE_mpn_preinv_mod_1
#undef HAVE_NATIVE_mpn_random2
#undef HAVE_NATIVE_mpn_random
#undef HAVE_NATIVE_mpn_rawrandom
#undef HAVE_NATIVE_mpn_rshift
#undef HAVE_NATIVE_mpn_scan0
#undef HAVE_NATIVE_mpn_scan1
#undef HAVE_NATIVE_mpn_set_str
#undef HAVE_NATIVE_mpn_sqrtrem
#undef HAVE_NATIVE_mpn_sqr_basecase
#undef HAVE_NATIVE_mpn_sqr_diagonal
#undef HAVE_NATIVE_mpn_sub
#undef HAVE_NATIVE_mpn_sub_1
#undef HAVE_NATIVE_mpn_sub_n
#undef HAVE_NATIVE_mpn_sub_nc
#undef HAVE_NATIVE_mpn_submul_1
#undef HAVE_NATIVE_mpn_submul_1c
#undef HAVE_NATIVE_mpn_udiv_w_sdiv
#undef HAVE_NATIVE_mpn_umul_ppmm
#undef HAVE_NATIVE_mpn_udiv_qrnnd
#undef HAVE_NATIVE_mpn_xor_n
#undef HAVE_NATIVE_mpn_xnor_n

/* a dummy to make autoheader happy */
#undef HAVE_NATIVE_
