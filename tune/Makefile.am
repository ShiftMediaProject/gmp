## Process this file with automake to generate Makefile.in

# Copyright (C) 2000 Free Software Foundation, Inc.
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

# No dependency on libgmp, so can use libgmptune instead for tune program
EXTRA_LTLIBRARIES = libspeed.la
libspeed_la_SOURCES = common.c time.c noop.c
libspeed_la_DEPENDENCIES = $(SPEED_CYCLECOUNTER_OBJS)
libspeed_la_LIBADD = $(libspeed_la_DEPENDENCIES)

# Both libspeed and the programs are built static-only since static is
# fastest and will be preferred by users who care about maximizing speed.
#
# Actually libtool doesn't seem to pass -static through to gcc, it only
# chooses the ".a" versions of libtool libraries.  Rerun the gcc link
# invocation with -static if you want a static link of libc and libm.
#
LDFLAGS = -static
LDADD = libspeed.la $(top_builddir)/libgmp.la -lm

EXTRA_PROGRAMS = speed speed-ext tuneup

tuneup_SOURCES = tuneup.c
tuneup_DEPENDENCIES = libspeed.la libgmptune.a
tuneup_LDADD = $(tuneup_DEPENDENCIES) -lm

# Don't know how to get tuneup$U.lo to depend on sqr_basecase.h, so just do
# an explicit make.  A plain "make tuneup" will fail if used directly.
tune:
	$(MAKE) $(AM_MAKEFLAGS) sqr_basecase.h tuneup$(EXEEXT)
	./tuneup

allprogs: sqr_basecase.h $(EXTRA_PROGRAMS)


speed_SOURCES = speed.c

SQR_CLEANFILES = sqr_basecase.c sqr_basecase.h sqr_basecase.asm

CLEANFILES = $(EXTRA_PROGRAMS) $(EXTRA_LTLIBRARIES) \
	$(SQR_CLEANFILES) libgmptune.a libgmptmp.a \
	stg.gnuplot stg.data \
	mtg.gnuplot mtg.data \
	fibg.gnuplot fibg.data \
	graph.gnuplot graph.data


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
	echo "$(COMPILE_FLAGS)"
	$(M4) $(M4FLAGS) -DPIC -DOPERATION_$* $< >tmp-$*.s
	$(LIBTOOL) --mode=compile $(CCAS) $(COMPILE_FLAGS) tmp-$*.s -o $@
	$(M4) $(M4FLAGS) -DOPERATION_$* $< >tmp-$*.s
	$(CCAS) $(COMPILE_FLAGS) tmp-$*.s -o $*.o
	rm -f tmp-$*.s


# Recompiled versions of $(LIBGMPTUNE_OBJS) are always used.  sqr_basecase.o
# is replaced if it's mpn/generic/sqr_basecase.c (due to the fixed size
# temporary array), or sqr_basecase.asm with a KARATSUBA_SQR_THRESHOLD_MAX
# (because that says it normally works only up to KARATSUBA_SQR_THRESHOLD).

SQR_GENERIC_MAX = 200

LIBGMPTUNE_OBJS = \
  mul_n.o mul_fft.o mul.o \
  bz_divrem_n.o tdiv_qr.o fib_ui.o powm.o gcd.o gcdext.o

sqr_basecase.h: libgmptune.a


# "cp -f" is not portable (eg. it fails on SunOS)
# "ar r" is going to be a bit fragile because there's more than one mul.o in
# libgmp.a.

libgmptune.a: $(top_builddir)/libgmp.la $(LIBGMPTUNE_OBJS)
	rm -f libgmptmp.a
	eval `$(LIBTOOL) --config | grep '^objdir='` ; \
		cp $(top_builddir)/$$objdir/libgmp.a libgmptmp.a
	$(AR) r libgmptmp.a $(LIBGMPTUNE_OBJS)
	rm -f $(SQR_CLEANFILES)
	echo "" >sqr_basecase.h
	if test -f $(top_builddir)/mpn/sqr_basecase.c; then		\
	  $(MAKE) $(AM_MAKEFLAGS) sqr-basecase-generic;			\
        else								\
	  if grep KARATSUBA_SQR_THRESHOLD_MAX				\
	      $(top_builddir)/mpn/sqr_basecase.asm >/dev/null; then	\
	    $(MAKE) $(AM_MAKEFLAGS) sqr-basecase-asm;			\
	  fi								\
	fi
	$(RANLIB) libgmptmp.a
	rm -f libgmptune.a
	mv libgmptmp.a libgmptune.a

sqr-basecase-generic:
	cp $(top_builddir)/mpn/sqr_basecase.c .
	$(MAKE) $(AM_MAKEFLAGS) \
		CFLAGS='$(CFLAGS) -DKARATSUBA_SQR_THRESHOLD=$(SQR_GENERIC_MAX)' \
		sqr_basecase.o
	$(AR) r libgmptmp.a sqr_basecase.o
	echo '#define KARATSUBA_SQR_MAX  $(SQR_GENERIC_MAX)' >sqr_basecase.h

sqr-basecase-asm:
	cp $(top_builddir)/mpn/sqr_basecase.asm .
	$(MAKE) $(AM_MAKEFLAGS) \
		M4FLAGS='$(M4FLAGS) -DKARATSUBA_SQR_THRESHOLD_OVERRIDE=KARATSUBA_SQR_THRESHOLD_MAX' \
		sqr_basecase.o
	ar r libgmptmp.a sqr_basecase.o
	echo "#define KARATSUBA_SQR_MAX  `sed -n 's/def...(KARATSUBA_SQR_THRESHOLD_MAX, *\([0-9]*\))/\1/p' sqr_basecase.asm`" >sqr_basecase.h


mul_n.o: $(top_builddir)/mpn/mul_n.c
	$(COMPILE) -DTUNE_PROGRAM_BUILD=1 -c $(top_builddir)/mpn/mul_n.c

mul_fft.o: $(top_builddir)/mpn/mul_fft.c
	$(COMPILE) -DTUNE_PROGRAM_BUILD=1 -c $(top_builddir)/mpn/mul_fft.c

mul.o: $(top_builddir)/mpn/mul.c
	$(COMPILE) -DTUNE_PROGRAM_BUILD=1 -c $(top_builddir)/mpn/mul.c

bz_divrem_n.o: $(top_builddir)/mpn/bz_divrem_n.c
	$(COMPILE) -DTUNE_PROGRAM_BUILD=1 -c $(top_builddir)/mpn/bz_divrem_n.c

tdiv_qr.o: $(top_builddir)/mpn/tdiv_qr.c
	$(COMPILE) -DTUNE_PROGRAM_BUILD=1 -c $(top_builddir)/mpn/tdiv_qr.c

fib_ui.o: $(top_srcdir)/mpz/fib_ui.c
	$(COMPILE) -DTUNE_PROGRAM_BUILD=1 -c $(top_srcdir)/mpz/fib_ui.c

powm.o: $(top_srcdir)/mpz/powm.c
	$(COMPILE) -DTUNE_PROGRAM_BUILD=1 -c $(top_srcdir)/mpz/powm.c

gcd.o: $(top_builddir)/mpn/gcd.c
	$(COMPILE) -DTUNE_PROGRAM_BUILD=1 -c $(top_builddir)/mpn/gcd.c

gcdext.o: $(top_builddir)/mpn/gcdext.c
	$(COMPILE) -DTUNE_PROGRAM_BUILD=1 -c $(top_builddir)/mpn/gcdext.c




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

bz:
	./speed -s 5-40 -c mpn_bz_divrem_sb mpn_bz_divrem_n mpn_bz_tdiv_qr

fib:
	./speed -s 80-120 -c mpz_fib_ui
fibg:
	./speed -s 10-300 -P fibg mpz_fib_ui


gcd:
	./speed -s 1-20 -c mpn_gcd


graph:
	./speed -s 1-5000 -f 1.02 -P graph mpn_mul_n mpn_sqr
	gnuplot graph.gnuplot
