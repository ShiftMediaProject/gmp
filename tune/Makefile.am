## Process this file with automake to generate Makefile.in

# Copyright 2000 Free Software Foundation, Inc.
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


AUTOMAKE_OPTIONS = gnu no-dependencies $(top_builddir)/ansi2knr

INCLUDES = -I$(top_srcdir)

EXTRA_DIST = alpha.asm pentium.asm sparcv9.asm hppa.asm hppa2.asm
noinst_HEADERS = speed.h


# The library code is faster static than shared on some systems, so do all
# tuning and measuring with static, since users who care about maximizing
# speed will be using that.
#
# Libtool doesn't seem to pass -static through to gcc, it only uses it to
# choose the static versions of libtool libraries.  Rerun the link with
# -static to get a static link of libc and libm.  This is probably only of
# interest when measuring libc routines like malloc/free.

LDFLAGS = -static
DEPENDENCIES = libspeed.la
LDADD = $(DEPENDENCIES)


EXTRA_LTLIBRARIES = libspeed.la libdummy.la

libspeed_la_SOURCES = \
  common.c freq.c modlinv.c mul_n_mpn.c mul_n_open.c noop.c time.c
libspeed_la_DEPENDENCIES = $(SPEED_CYCLECOUNTER_OBJ)
libspeed_la_LIBADD = $(libspeed_la_DEPENDENCIES) $(top_builddir)/libgmp.la -lm

# create an ansi2knr rule for sqr_basecase.c in case it's recompiled
nodist_libdummy_la_SOURCES = sqr_basecase.c


EXTRA_PROGRAMS = speed speed-ext tuneup

speed_SOURCES = speed.c
speed_ext_SOURCES = speed.c

tuneup_SOURCES = tuneup.c
nodist_tuneup_SOURCES = $(TUNE_MPN_SRCS) $(TUNE_MPZ_SRCS)
tuneup_DEPENDENCIES = $(TUNE_SQR_OBJ) libspeed.la
tuneup_LDADD = $(tuneup_DEPENDENCIES)


tune:
	$(MAKE) $(AM_MAKEFLAGS) tuneup$(EXEEXT)
	./tuneup

allprogs: $(EXTRA_PROGRAMS)


CLEANFILES = $(EXTRA_PROGRAMS) $(EXTRA_LTLIBRARIES) \
	$(nodist_tuneup_SOURCES) sqr_basecase.c sqr_asm.asm \
	stg.gnuplot stg.data \
	mtg.gnuplot mtg.data \
	fibg.gnuplot fibg.data \
	graph.gnuplot graph.data


# Generating these little files at build time seems better than including
# them in the distribution, since the list can be changed easily.
#
# FIXME: Would like for example mul_n.c to depend on $(top_builddir)/mul_n.c
# so the recompiled object will be rebuilt if that file changes.

TUNE_MPN_SRCS = mul_n.c mul_fft.c mul.c dc_divrem_n.c tdiv_qr.c gcd.c gcdext.c
TUNE_MPZ_SRCS = fib_ui.c powm.c

$(TUNE_MPN_SRCS):
	for i in $(TUNE_MPN_SRCS); do \
	  echo "#define TUNE_PROGRAM_BUILD 1" >$$i; \
	  echo "#include \"mpn/$$i\"" >>$$i; \
	done

$(TUNE_MPZ_SRCS):
	for i in $(TUNE_MPZ_SRCS); do \
	  echo "#define TUNE_PROGRAM_BUILD 1" >$$i; \
	  echo "#include \"mpz/$$i\"" >>$$i; \
	done

sqr_basecase.c: $(top_builddir)/mpn/sqr_basecase.c
	echo '#define TUNE_PROGRAM_BUILD 1' >sqr_basecase.c
	echo '#define TUNE_PROGRAM_BUILD_SQR 1' >>sqr_basecase.c
	echo '#include "mpn/sqr_basecase.c"' >>sqr_basecase.c

sqr_asm.asm: $(top_builddir)/mpn/sqr_basecase.asm
	echo 'define(KARATSUBA_SQR_THRESHOLD_OVERRIDE,KARATSUBA_SQR_THRESHOLD_MAX)' >sqr_asm.asm
	echo 'include(../mpn/sqr_basecase.asm)' >>sqr_asm.asm



# This bit like mpn/Makefile.am

# COMPILE minus CC.  FIXME: Really pass *_CFLAGS to CPP?
COMPILE_FLAGS =	\
	$(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)

SUFFIXES = .asm
.asm.o:
	$(M4) $(M4FLAGS) -DOPERATION_$* $< >tmp-$*.s
	$(CCAS) $(COMPILE_FLAGS) tmp-$*.s -o $@
	rm -f tmp-$*.s
.asm.obj:
	$(M4) $(M4FLAGS) -DOPERATION_$* `cygpath -w $<` >tmp-$*.s
	$(CCAS) $(COMPILE_FLAGS) tmp-$*.s -o $@
	rm -f tmp-$*.s
.asm.lo:
	$(M4) $(M4FLAGS) -DPIC -DOPERATION_$* $< >tmp-$*.s
	$(LIBTOOL) --mode=compile $(CCAS) $(COMPILE_FLAGS) tmp-$*.s -o $@
	$(M4) $(M4FLAGS) -DOPERATION_$* $< >tmp-$*.s
	$(CCAS) $(COMPILE_FLAGS) tmp-$*.s -o $*.o
	rm -f tmp-$*.s



# "mk" is multiplication in the karatsuba range
# "st" is squaring in the toom-cook range, etc
# "g" forms produce graphs

mk:
	./speed -s 5-40 -c mpn_mul_basecase mpn_kara_mul_n

MTS = -s 50-150 -c
mt:
	./speed $(MTS) mpn_kara_mul_n mpn_toom3_mul_n | less
mtg:
	./speed $(MTS) -P mtg mpn_kara_mul_n mpn_toom3_mul_n

sk:
	./speed -s 5-40 -c mpn_sqr_basecase mpn_kara_sqr_n

STS = -s 50-150 -c
st:
	./speed $(STS) mpn_kara_sqr_n mpn_toom3_sqr_n | less
stg:
	./speed $(STS) -P stg mpn_kara_sqr_n mpn_toom3_sqr_n

dc:
	./speed -s 5-40 -c mpn_dc_divrem_sb mpn_dc_divrem_n mpn_dc_tdiv_qr

fib:
	./speed -s 80-120 -c mpz_fib_ui
fibg:
	./speed -s 10-300 -P fibg mpz_fib_ui


gcd:
	./speed -s 1-20 -c mpn_gcd


graph:
	./speed -s 1-5000 -f 1.02 -P graph mpn_mul_n mpn_sqr
	gnuplot graph.gnuplot
