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

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/tests
LDADD = $(top_builddir)/tests/libtests.la \
  $(top_builddir)/libgmpxx.la $(LIBMPFR) $(top_builddir)/libgmp.la
if WANT_MPFR
LIBMPFR = $(top_builddir)/mpfr/libmpfr.a
endif

if WANT_CXX
check_PROGRAMS = t-assign t-binary t-constr t-headers t-locale t-misc t-ops \
  t-ostream t-prec t-rand t-ternary t-unary
TESTS = $(check_PROGRAMS)
endif

t_assign_SOURCES  = t-assign.cc
t_binary_SOURCES  = t-binary.cc
t_constr_SOURCES  = t-constr.cc
t_headers_SOURCES = t-headers.cc
t_istream_SOURCES = t-istream.cc
t_locale_SOURCES  = t-locale.cc
t_misc_SOURCES    = t-misc.cc
t_ops_SOURCES     = t-ops.cc
t_ostream_SOURCES = t-ostream.cc
t_prec_SOURCES    = t-prec.cc
t_rand_SOURCES    = t-rand.cc
t_ternary_SOURCES = t-ternary.cc
t_unary_SOURCES   = t-unary.cc

$(top_builddir)/tests/libtests.la:
	cd $(top_builddir)/tests; $(MAKE) $(AM_MAKEFLAGS) libtests.la
