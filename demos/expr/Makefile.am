## Process this file with automake to generate Makefile.in

# Copyright 2001, 2002 Free Software Foundation, Inc.
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

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/tests -I$(top_srcdir)/mpfr

if WANT_MPFR
MPFR_SOURCES_OPTION = exprfr.c exprfra.c
MPFR_LIBADD_OPTION = $(top_builddir)/mpfr/libmpfr.a
endif

# ensure these are always distributed
EXTRA_DIST = exprfr.c exprfra.c

EXTRA_LIBRARIES = libexpr.a
libexpr_a_SOURCES = expr.h expr-impl.h \
  expr.c exprv.c exprz.c exprza.c exprq.c exprqa.c exprf.c exprfa.c \
  $(MPFR_SOURCES_OPTION)

# On SunOS 4, HUGE_VAL used by mpfr infinity handling needs -lm.
#
EXTRA_PROGRAMS = run-expr t-expr
LDADD = libexpr.a $(MPFR_LIBADD_OPTION) $(top_builddir)/libgmp.la $(LIBM)
t_expr_LDADD = $(top_builddir)/tests/libtests.la $(LDADD)

CLEANFILES = $(EXTRA_PROGRAMS) $(EXTRA_LIBRARIES)

allprogs: $(EXTRA_PROGRAMS)

$(top_builddir)/tests/libtests.la:
	cd $(top_builddir)/tests; $(MAKE) $(AM_MAKEFLAGS) libtests.la
