## Process this file with automake to generate Makefile.in


# Copyright 1991, 1993, 1994, 1996, 1997, 1999, 2000 Free Software
# Foundation, Inc.
#
# This file is part of the GNU MP Library.
#
# The GNU MP Library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 2.1 of the License, or (at your
# option) any later version.
#
# The GNU MP Library is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
# License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
# MA 02111-1307, USA.


# make check
#
#     It'd be good if "make check" first did a "make all" or whatever to
#     build libgmp.la, but it's not clear how best to do this.  Putting a
#     "check:" target is overridden by automake, and a "check-local:" runs
#     too late (due to depth-first subdirectory processing).  For now it's
#     necessary to do "make && make check".


# Libtool -version-info for libgmp.la and libmp.la.  See (libtool)Versioning
#
# 1. No interfaces changed, only implementations (good): Increment REVISION.
#
# 2. Interfaces added, none removed (good): Increment CURRENT, increment
#    AGE, set REVISION to 0.
#
# 3. Interfaces removed (BAD, breaks upward compatibility): Increment
#    CURRENT, set AGE and REVISION to 0.
#
# Do this separately for libgmp and libmp, only do it just before a release.
#
#	  GMP	   -version-info
#	release   libgmp  libmp
#	 2.0.x      -       -
#	 3.0      3:0:0   3:0:0
#        3.0.1    3:1:0   3:0:0
#        3.1      4:0:1   4:0:1
#        3.1.1    4:1:1   4:1:1
#
#
# Starting at 3:0:0 is a slight abuse of the versioning system, but it
# ensures we're past soname libgmp.so.2, which was used on Debian GNU/Linux
# packages of gmp 2.  Pretend gmp 2 was 2:0:0, so the interface changes for
# gmp 3 mean 3:0:0 is right.

LIBGMP_LT_CURRENT =  4
LIBGMP_LT_REVISION = 0
LIBGMP_LT_AGE =      1

LIBMP_LT_CURRENT =  4
LIBMP_LT_REVISION = 0
LIBMP_LT_AGE =      1


AUTOMAKE_OPTIONS = gnu check-news no-dependencies ansi2knr

SUBDIRS = mpn mpz mpq mpf mpbsd mpfr tests demos tune

include_HEADERS = gmp.h $(MPBSD_HEADERS_OPTION) $(MPFR_HEADERS_OPTION)
EXTRA_HEADERS = mp.h

lib_LTLIBRARIES = libgmp.la $(MPBSD_LTLIBRARIES_OPTION)

EXTRA_DIST = doc macos configfsf.guess configfsf.sub \
 .gdbinit INSTALL.autoconf gmp-impl.h longlong.h stack-alloc.h urandom.h

DISTCLEANFILES = asm-syntax.h config.m4 @gmp_srclinks@


# Libtool needs all the .lo files passed to it if it's going to build both a
# static and shared library.  If a convenience library like mpf/libmpf.la is
# passed then the resulting libgmp.a gets the PIC .lo objects rather than
# the non-PIC .o's.
#
# Unfortunately this leads to the big lists of objects below.  Something
# like mpz/*.lo would probably work, but might risk missing something out or
# getting something extra.  The source files for each .lo are listed in the
# Makefile.am's in the subdirectories.

MPF_OBJECTS = mpf/init$U.lo mpf/init2$U.lo mpf/set$U.lo mpf/set_ui$U.lo	    \
  mpf/set_si$U.lo mpf/set_str$U.lo mpf/set_d$U.lo mpf/set_z$U.lo	    \
  mpf/iset$U.lo mpf/iset_ui$U.lo mpf/iset_si$U.lo mpf/iset_str$U.lo	    \
  mpf/iset_d$U.lo mpf/clear$U.lo mpf/get_str$U.lo mpf/dump$U.lo		    \
  mpf/size$U.lo mpf/eq$U.lo mpf/reldiff$U.lo mpf/sqrt$U.lo mpf/random2$U.lo \
  mpf/inp_str$U.lo mpf/out_str$U.lo mpf/add$U.lo mpf/add_ui$U.lo	    \
  mpf/sub$U.lo mpf/sub_ui$U.lo mpf/ui_sub$U.lo mpf/mul$U.lo mpf/mul_ui$U.lo \
  mpf/div$U.lo mpf/div_ui$U.lo mpf/cmp$U.lo mpf/cmp_ui$U.lo mpf/cmp_si$U.lo \
  mpf/mul_2exp$U.lo mpf/div_2exp$U.lo mpf/abs$U.lo mpf/neg$U.lo		    \
  mpf/set_q$U.lo mpf/get_d$U.lo mpf/set_dfl_prec$U.lo mpf/set_prc$U.lo	    \
  mpf/set_prc_raw$U.lo mpf/get_prc$U.lo mpf/ui_div$U.lo mpf/sqrt_ui$U.lo    \
  mpf/floor$U.lo mpf/ceil$U.lo mpf/trunc$U.lo mpf/pow_ui$U.lo		    \
  mpf/urandomb$U.lo mpf/swap$U.lo

MPZ_OBJECTS = mpz/abs$U.lo mpz/add$U.lo mpz/add_ui$U.lo			\
  mpz/addmul_ui$U.lo mpz/and$U.lo mpz/array_init$U.lo mpz/bin_ui$U.lo	\
  mpz/bin_uiui$U.lo mpz/cdiv_q$U.lo mpz/cdiv_q_ui$U.lo			\
  mpz/cdiv_qr$U.lo mpz/cdiv_qr_ui$U.lo mpz/cdiv_r$U.lo			\
  mpz/cdiv_r_ui$U.lo mpz/cdiv_ui$U.lo mpz/clear$U.lo mpz/clrbit$U.lo	\
  mpz/cmp$U.lo mpz/cmp_si$U.lo mpz/cmp_ui$U.lo mpz/cmpabs$U.lo		\
  mpz/cmpabs_ui$U.lo mpz/com$U.lo					\
  mpz/divexact$U.lo mpz/divegcd$U.lo mpz/dump$U.lo			\
  mpz/fac_ui$U.lo mpz/fdiv_q$U.lo mpz/fdiv_q_2exp$U.lo			\
  mpz/fdiv_q_ui$U.lo mpz/fdiv_qr$U.lo mpz/fdiv_qr_ui$U.lo		\
  mpz/fdiv_r$U.lo mpz/fdiv_r_2exp$U.lo mpz/fdiv_r_ui$U.lo		\
  mpz/fdiv_ui$U.lo mpz/fib_ui$U.lo mpz/fits_sint$U.lo			\
  mpz/fits_slong$U.lo mpz/fits_sshort$U.lo mpz/fits_uint$U.lo		\
  mpz/fits_ulong$U.lo mpz/fits_ushort$U.lo mpz/gcd$U.lo			\
  mpz/gcd_ui$U.lo mpz/gcdext$U.lo mpz/get_d$U.lo mpz/get_si$U.lo	\
  mpz/get_str$U.lo mpz/get_ui$U.lo mpz/getlimbn$U.lo mpz/hamdist$U.lo	\
  mpz/init$U.lo mpz/inp_raw$U.lo mpz/inp_str$U.lo mpz/invert$U.lo	\
  mpz/ior$U.lo mpz/iset$U.lo mpz/iset_d$U.lo mpz/iset_si$U.lo		\
  mpz/iset_str$U.lo mpz/iset_ui$U.lo mpz/jacobi$U.lo mpz/kronsz$U.lo	\
  mpz/kronuz$U.lo mpz/kronzs$U.lo mpz/kronzu$U.lo mpz/lcm$U.lo		\
  mpz/legendre$U.lo mpz/mod$U.lo mpz/mul$U.lo mpz/mul_2exp$U.lo		\
  mpz/mul_si$U.lo mpz/mul_ui$U.lo mpz/neg$U.lo mpz/nextprime$U.lo	\
  mpz/out_raw$U.lo mpz/out_str$U.lo mpz/perfpow$U.lo mpz/perfsqr$U.lo	\
  mpz/popcount$U.lo mpz/pow_ui$U.lo mpz/powm$U.lo mpz/powm_ui$U.lo	\
  mpz/pprime_p$U.lo mpz/random$U.lo mpz/random2$U.lo mpz/realloc$U.lo	\
  mpz/remove$U.lo mpz/root$U.lo mpz/rrandomb$U.lo mpz/scan0$U.lo	\
  mpz/scan1$U.lo mpz/set$U.lo mpz/set_d$U.lo mpz/set_f$U.lo		\
  mpz/set_q$U.lo mpz/set_si$U.lo mpz/set_str$U.lo mpz/set_ui$U.lo	\
  mpz/setbit$U.lo mpz/size$U.lo mpz/sizeinbase$U.lo mpz/sqrt$U.lo	\
  mpz/sqrtrem$U.lo mpz/sub$U.lo mpz/sub_ui$U.lo mpz/swap$U.lo		\
  mpz/tdiv_ui$U.lo mpz/tdiv_q$U.lo mpz/tdiv_q_2exp$U.lo			\
  mpz/tdiv_q_ui$U.lo mpz/tdiv_qr$U.lo mpz/tdiv_qr_ui$U.lo		\
  mpz/tdiv_r$U.lo mpz/tdiv_r_2exp$U.lo mpz/tdiv_r_ui$U.lo		\
  mpz/tstbit$U.lo mpz/ui_pow_ui$U.lo mpz/urandomb$U.lo			\
  mpz/urandomm$U.lo mpz/xor$U.lo

MPQ_OBJECTS = mpq/abs$U.lo mpq/add$U.lo				\
  mpq/canonicalize$U.lo mpq/clear$U.lo				\
  mpq/cmp$U.lo mpq/cmp_ui$U.lo mpq/div$U.lo mpq/get_d$U.lo	\
  mpq/get_den$U.lo mpq/get_num$U.lo mpq/init$U.lo mpq/inv$U.lo	\
  mpq/md_2exp$U.lo mpq/mul$U.lo mpq/neg$U.lo mpq/out_str$U.lo	\
  mpq/set$U.lo mpq/set_den$U.lo mpq/set_num$U.lo 		\
  mpq/set_si$U.lo mpq/set_ui$U.lo				\
  mpq/sub$U.lo mpq/equal$U.lo mpq/set_z$U.lo mpq/set_d$U.lo	\
  mpq/set_f$U.lo mpq/swap$U.lo

MPN_OBJECTS = @mpn_objs_in_libgmp@

MPBSD_OBJECTS = mpbsd/add$U.lo mpbsd/tdiv_qr$U.lo mpbsd/set$U.lo	\
  mpbsd/powm$U.lo mpbsd/sub$U.lo mpbsd/cmp$U.lo mpbsd/mfree$U.lo	\
  mpbsd/mtox$U.lo mpbsd/realloc$U.lo mpbsd/gcd$U.lo mpbsd/itom$U.lo	\
  mpbsd/min$U.lo mpbsd/mul$U.lo mpbsd/mout$U.lo mpbsd/pow_ui$U.lo	\
  mpbsd/sdiv$U.lo mpbsd/sqrtrem$U.lo mpbsd/xtom$U.lo

# FIXME: Add mpfr/rnd_mode$U.lo when it's clean.
MPFR_OBJECTS = mpfr/add$U.lo mpfr/div_2exp$U.lo mpfr/neg$U.lo		\
  mpfr/set_dfl_prec$U.lo mpfr/set_str_raw$U.lo mpfr/agm$U.lo		\
  mpfr/get_str$U.lo mpfr/print_raw$U.lo mpfr/set_dfl_rnd$U.lo		\
  mpfr/sqrt$U.lo mpfr/clear$U.lo mpfr/init$U.lo mpfr/set_f$U.lo		\
  mpfr/sub$U.lo mpfr/cmp$U.lo mpfr/mul$U.lo mpfr/round$U.lo		\
  mpfr/set_prec$U.lo mpfr/cmp_ui$U.lo mpfr/mul_2exp$U.lo mpfr/set$U.lo	\
  mpfr/set_si$U.lo mpfr/div$U.lo mpfr/mul_ui$U.lo mpfr/set_d$U.lo	\
  mpfr/pow$U.lo mpfr/out_str$U.lo mpfr/pi$U.lo mpfr/set_z$U.lo		\
  mpfr/add_ulp$U.lo mpfr/log2$U.lo mpfr/random$U.lo mpfr/log$U.lo	\
  mpfr/exp$U.lo mpfr/div_ui$U.lo mpfr/zeta$U.lo mpfr/karadiv$U.lo	\
  mpfr/karasqrt$U.lo mpfr/print_rnd_mode$U.lo


if WANT_MPFR
MPFR_HEADERS_OPTION = mpfr/mpfr.h
MPFR_OBJECTS_OPTION = $(MPFR_OBJECTS)
MPFR_LIBADD_OPTION = $(LIBM)
endif
libgmp_la_SOURCES = assert.c compat.c errno.c memory.c mp_set_fns.c	 \
  mp_clz_tab.c mp_minv_tab.c						 \
  rand.c randclr.c randlc.c randlc2x.c randraw.c randsd.c		 \
  randsdui.c version.c stack-alloc.c mp_bpl.c extract-dbl.c insert-dbl.c
libgmp_la_DEPENDENCIES = \
  $(MPF_OBJECTS) $(MPZ_OBJECTS) $(MPN_OBJECTS) $(MPQ_OBJECTS) \
  $(MPFR_OBJECTS_OPTION)
libgmp_la_LIBADD = $(libgmp_la_DEPENDENCIES) $(MPFR_LIBADD_OPTION)
libgmp_la_LDFLAGS = \
  -version-info $(LIBGMP_LT_CURRENT):$(LIBGMP_LT_REVISION):$(LIBGMP_LT_AGE)


if WANT_MPBSD
MPBSD_HEADERS_OPTION = mp.h
MPBSD_LTLIBRARIES_OPTION = libmp.la
endif
libmp_la_SOURCES = assert.c errno.c memory.c mp_bpl.c mp_clz_tab.c	\
  mp_minv_tab.c mp_set_fns.c stack-alloc.c
libmp_la_DEPENDENCIES = $(MPBSD_OBJECTS) $(MPN_OBJECTS) mpz/add$U.lo	\
  mpz/clear$U.lo mpz/cmp$U.lo mpz/init$U.lo mpz/mod$U.lo mpz/mul$U.lo	\
  mpz/mul_2exp$U.lo mpz/realloc$U.lo mpz/set$U.lo mpz/set_ui$U.lo	\
  mpz/tdiv_r$U.lo mpz/sub$U.lo
libmp_la_LIBADD = $(libmp_la_DEPENDENCIES)
libmp_la_LDFLAGS = \
  -version-info $(LIBMP_LT_CURRENT):$(LIBMP_LT_REVISION):$(LIBMP_LT_AGE)


info_TEXINFOS = gmp.texi


# Don't ship CVS directories or emacs backups.
dist-hook:
	-find $(distdir) \( -name CVS -type d \) -o -name "*.~*" \
		| xargs rm -rf
