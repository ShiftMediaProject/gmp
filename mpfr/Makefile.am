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

SUBDIRS = tests


INCLUDES = -I$(top_srcdir)

if WANT_MPFR
noinst_LTLIBRARIES = libmpfr.la
endif

# FIXME: Add rnd_mode.c when it's clean.
libmpfr_la_SOURCES = \
	add.c div_2exp.c neg.c set_dfl_prec.c set_str_raw.c agm.c get_str.c \
	print_raw.c set_dfl_rnd.c sqrt.c clear.c init.c set_f.c \
	sub.c cmp.c mul.c round.c set_prec.c cmp_ui.c mul_2exp.c set.c \
	set_si.c div.c mul_ui.c set_d.c pow.c out_str.c pi.c set_z.c \
	add_ulp.c log2.c random.c log.c exp.c div_ui.c zeta.c karadiv.c \
	karasqrt.c print_rnd_mode.c

EXTRA_DIST = mpfr.h mpfr-impl.h
