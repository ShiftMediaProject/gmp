## Process this file with automake to generate Makefile.in

# Copyright 2000, 2001, 2002, 2003 Free Software Foundation, Inc.
#
# This file is part of the GNU MP Library.
#
# The GNU MP Library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Library General Public License as published by
# the Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
#
# The GNU MP Library is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
# License for more details.
#
# You should have received a copy of the GNU Library General Public License
# along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
# MA 02111-1307, USA.


INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/mpfr
CFLAGS = @MPFR_CFLAGS@
LIBS = @MPFR_LIBS@

LDADD = libfrtests.a $(MPFR_LIBM) ../libmpfr.a $(top_builddir)/libgmp.la

libfrtests_a_SOURCES = memory.c rnd_mode.c tests.c
EXTRA_libfrtests_a_SOURCES = amd64.asm x86.asm
libfrtests_a_DEPENDENCIES = $(TESTS_ASM_OBJECTS)
libfrtests_a_LIBADD = $(libfrtests_a_DEPENDENCIES)

if WANT_MPFR
check_PROGRAMS = tinits tsgn tcheck tisnan texceptions tset tabs tset_d tset_f tset_q tset_si tset_str tset_z tset_ld tadd tsub tmul tdiv tcmp tcmp2 tcan_round tround_prec tswap reuse tsqrt tnext tcomparisons teq tadd_ui tsub_ui tcmp_ui tdiv_ui tmul_ui tsqrt_ui tui_div tui_sub tcmp_d tmul_2exp tfma tfrac tget_str tout_str tget_d tget_d_2exp tconst_log2 tconst_pi tconst_euler trandom ttrunc trint texp texp2 texpm1 tlog tlog2 tlog10 tlog1p tpow tui_pow tpow3 tcosh tsinh ttanh tacosh tasinh tatanh thyperbolic tasin tacos tcos tatan tsin ttan tsin_cos tagm thypot tfactorial tgamma terf tcbrt mpf_compat mpfr_compat tzeta tsub1sp
check_LIBRARIES = libfrtests.a
TESTS = $(check_PROGRAMS)
endif
EXTRA_DIST = tgeneric.c mpf_compat.h

../libmpfr.a:
	cd ..; make libmpfr.a  


# This $(srcdir) and $(CYGPATH_W) stuff follows automake 1.7.2
SUFFIXES = .asm
.asm.o:
	grep -v '^#' `test -f '$<' || echo '$(srcdir)/'`$< >tmp-$*.s
	$(COMPILE) -c tmp-$*.s -o $@
	rm -f tmp-$*.s
.asm.obj:
	grep -v '^#' `if test -f '$<'; then $(CYGPATH_W) '$<'; else $(CYGPATH_W) '$(srcdir)/$<' >tmp-$*.s
	$(COMPILE) -c tmp-$*.s -o $@
	rm -f tmp-$*.s
