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
LDADD = $(top_builddir)/libgmp.la

EXTRA_DIST = calc.y calclex.l

calc_SOURCES = calc.c calclex.c calc.h
qcn_LDADD = $(LDADD) -lm

# None of these programs are built by default, but "make <whatever>" will
# build them once libgmp.la is built.
#
EXTRA_PROGRAMS = calc factorize isprime pexpr primes qcn

CLEANFILES = $(EXTRA_PROGRAMS)

allprogs: $(EXTRA_PROGRAMS)


# The distribution includes calc.c, calc.h and calclex.c, generated with
# bison and flex, so the calc program will build even if yacc and lex aren't
# available.  The rules below regenerate those files if you modify calc.y or
# calclex.l.
#
# Using "cd"s instead of a $(srcdir) path ensures $(srcdir) doesn't get into
# the "#line"s in the generated files.
#
# FIXME: Automake yacc handling isn't quite right when building in a
# separate object directory, hence the explicit rules used for now.
#
# FIXME: With ansi2knr and no-dependencies, there doesn't seem any easy way
# to manually express that calclex$U.lo depends on calc.h.  Making calclex.c
# depend on calc.h gets the right result, but the extra lex runs are
# unnecessary.

$(srcdir)/calclex.c: calclex.l calc.h
	cd $(srcdir); lex -ocalclex.c calclex.l
$(srcdir)/calc.c $(srcdir)/calc.h: calc.y
	cd $(srcdir); yacc -d -ocalc.c calc.y
