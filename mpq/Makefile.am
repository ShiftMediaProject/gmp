## Process this file with automake to generate Makefile.in

# Copyright (C) 1996, 1998, 2000 Free Software Foundation, Inc.
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


AUTOMAKE_OPTIONS = gnu no-dependencies

SUBDIRS = tests

INCLUDES = -I$(top_srcdir)

noinst_LTLIBRARIES = libmpq.la
libmpq_la_SOURCES = \
  add.c canonicalize.c clear.c cmp.c cmp_ui.c div.c get_d.c \
  get_den.c get_num.c init.c inv.c mul.c neg.c out_str.c set.c set_den.c \
  set_num.c set_si.c set_ui.c sub.c equal.c set_z.c set_d.c swap.c
