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


AUTOMAKE_OPTIONS = gnu $(top_builddir)/ansi2knr no-dependencies

INCLUDES = -I$(top_srcdir)
LDADD = $(top_builddir)/libgmp.la

EXTRA_PROGRAMS = \
  add_n addmul_1 copy divmod_1 divrem lshift mul_1 rshift \
  sub_n submul_1 tst-addsub try

# addsub_n addsub_n_2 not yet built since mpn_addsub_n doesn't yet exist

EXTRA_HEADERS = ref.h try.h
EXTRA_DIST = x86call.asm x86check.c

try_SOURCES = try.c ref.c trace.c spinner.c
try_LDADD = @CALLING_CONVENTIONS_OBJS@ $(top_builddir)/libgmp.la
try_DEPENDENCIES = $(try_LDADD)

allprogs: $(EXTRA_PROGRAMS)

CLEANFILES = $(EXTRA_PROGRAMS)


SUFFIXES = .asm

# Relative paths don't work out right when running m4 from a second level
# subdirectory, so copy the necessary files up to build them.
.asm.o:
	cp $< ..; cd ..; $(MAKE) $(AM_MAKEFLAGS) $@; rm $*.asm; mv $*.* tests;
.asm.obj:
	cp $< ..; cd ..; $(MAKE) $(AM_MAKEFLAGS) $@; rm $*.asm; mv $*.* tests;
.asm.lo:
	cp $< ..; cd ..; $(MAKE) $(AM_MAKEFLAGS) $@; rm $*.asm; mv $*.* tests;
