## Process this file with automake to generate Makefile.in

# Copyright (C) 2000 Free Software Foundation, Inc.
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


AUTOMAKE_OPTIONS = gnu no-dependencies

INCLUDES = -I$(top_srcdir)
LDADD = $(top_builddir)/libgmp.la

EXTRA_PROGRAMS = \
  add_n addmul_1 addsub_n addsub_n_2 copy divmod_1 divrem lshift mul_1 rshift \
  sub_n submul_1 tst-addsub

EXTRA_HEADERS = ref.h 
EXTRA_DIST = ref.c

allprogs: $(EXTRA_PROGRAMS)
