## Process this file with automake to generate Makefile.in

# Copyright 1996, 1999, 2000, 2001 Free Software Foundation, Inc.
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
LDADD = $(top_builddir)/tests/libtests.la $(top_builddir)/libgmp.la

check_PROGRAMS = t-aors t-cmp t-cmp_ui t-cmp_si t-equal t-get_d t-get_str \
  t-inp_str t-md_2exp t-set_f t-set_str
TESTS = $(check_PROGRAMS)

# t-inp_str.tmp is used by t-inp_str.c and will be removed automatically if
# the tests pass, but have it cleaned here in case the tests fail.
#
CLEANFILES = t-inp_str.tmp

$(top_builddir)/tests/libtests.la:
	cd $(top_builddir)/tests; $(MAKE) $(AM_MAKEFLAGS) libtests.la
