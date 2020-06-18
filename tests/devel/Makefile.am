## Process this file with automake to generate Makefile.in

# Copyright 2000-2002, 2018 Free Software Foundation, Inc.
#
# This file is part of the GNU MP Library test suite.
#
# The GNU MP Library test suite is free software; you can redistribute it
# and/or modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 3 of the License,
# or (at your option) any later version.
#
# The GNU MP Library test suite is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
# Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# the GNU MP Library test suite.  If not, see https://www.gnu.org/licenses/.


AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/tests
AM_LDFLAGS = -no-install
LDADD = $(top_builddir)/tests/libtests.la $(top_builddir)/libgmp.la

EXTRA_PROGRAMS = \
  sqrtrem_1_2 primes try

BUILT_SOURCES =
DISTCLEANFILES = $(BUILT_SOURCES)
EXTRA_DIST =

nodist_SOURCES = test-add_ssaaaa.c test-sub_ddmmss.c

allprogs: $(EXTRA_PROGRAMS) test-add_ssaaaa test-sub_ddmmss

CLEANFILES = $(EXTRA_PROGRAMS)

$(top_builddir)/tests/libtests.la:
	cd $(top_builddir)/tests; $(MAKE) $(AM_MAKEFLAGS) libtests.la

test-add_ssaaaa.c: gen-test-longlong_h$(EXEEXT_FOR_BUILD)
	./gen-test-longlong_h add >test-add_ssaaaa.c || (rm -f test-add_ssaaaa.c; exit 1)
#BUILT_SOURCES += test-add_ssaaaa.c

test-sub_ddmmss.c: gen-test-longlong_h$(EXEEXT_FOR_BUILD)
	./gen-test-longlong_h sub >test-sub_ddmmss.c || (rm -f test-sub_ddmmss.c; exit 1)
#BUILT_SOURCES += test-sub_ddmmss.c

gen-test-longlong_h$(EXEEXT_FOR_BUILD): gen-test-longlong_h.c
	$(CC_FOR_BUILD) `test -f 'gen-test-longlong_h.c' || echo '$(srcdir)/'`gen-test-longlong_h.c -o gen-test-longlong_h$(EXEEXT_FOR_BUILD)
DISTCLEANFILES += gen-test-longlong_h$(EXEEXT_FOR_BUILD)
EXTRA_DIST += gen-test-longlong_h.c
