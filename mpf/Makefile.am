## Process this file with automake to generate Makefile.in

# Copyright (C) 1996, 1998, 1999, 2000 Free Software Foundation, Inc.
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

INCLUDES = -I$(top_srcdir) -DOPERATION_$*

noinst_LTLIBRARIES = libmpf.la
libmpf_la_SOURCES = \
  init.c init2.c set.c set_ui.c set_si.c set_str.c set_d.c set_z.c \
  set_q.c iset.c iset_ui.c iset_si.c iset_str.c iset_d.c clear.c get_str.c \
  dump.c size.c eq.c reldiff.c sqrt.c random2.c inp_str.c out_str.c \
  add.c add_ui.c sub.c sub_ui.c ui_sub.c mul.c mul_ui.c div.c div_ui.c \
  cmp.c cmp_ui.c cmp_si.c mul_2exp.c div_2exp.c abs.c neg.c get_d.c \
  set_dfl_prec.c set_prc.c set_prc_raw.c get_prc.c ui_div.c sqrt_ui.c \
  pow_ui.c urandomb.c swap.c

nodist_libmpf_la_SOURCES = floor.c ceil.c trunc.c

EXTRA_DIST = integer.c
CLEANFILES = $(nodist_libmpf_la_SOURCES)

floor.c: $(srcdir)/integer.c
	cp $(srcdir)/integer.c floor.c
ceil.c: $(srcdir)/integer.c
	cp $(srcdir)/integer.c ceil.c
trunc.c: $(srcdir)/integer.c
	cp $(srcdir)/integer.c trunc.c
