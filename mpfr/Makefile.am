## Process this file with automake to generate Makefile.in

# Copyright 2000, 2001 Free Software Foundation, Inc.
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


# Only a libmpfr.a is built, since upward binary compatibility is not yet
# guaranteed.


AUTOMAKE_OPTIONS = gnu no-dependencies $(top_builddir)/ansi2knr
SUBDIRS = tests

INCLUDES = -I$(top_srcdir) -DOPERATION_`echo $* | sed 's/_$$//'`

if WANT_MPFR
lib_LIBRARIES = libmpfr.a
include_HEADERS = mpfr.h
endif

TEXINFO_TEX = ../texinfo.tex
info_TEXINFOS = mpfr.texi

# FIXME: As of Jan 2001, automake doesn't like conditionally defined
# info_TEXINFOS.  The fudge here ensures we distribute mpfr.info
# unconditionally, but only install it if the user wants mpfr.  This is
# highly dependent on automake internals.
#
if MAINTAINER_MODE
INFO_DEPS = mpfr.info
else
if WANT_MPFR
INFO_DEPS = mpfr.info
else
INFO_DEPS =
endif
endif


libmpfr_a_SOURCES = cputime.h mpfr.h mpfr-impl.h sin_cos.c extract.c	\
  add.c add_ui.c add_ulp.c agm.c clear.c cmp.c cmp_ui.c div_2exp.c	\
  div.c div_ui.c dump.c eq.c exp2.c exp3.c exp.c get_str.c init.c	\
  inp_str.c log2.c log.c mul_2exp.c mul.c mul_ui.c neg.c out_str.c	\
  pi.c pow.c print_raw.c print_rnd_mode.c random2.c random.c reldiff.c	\
  rnd_mode.c round.c set.c set_d.c set_dfl_prec.c set_rnd.c set_f.c	\
  set_prc_raw.c set_prec.c set_q.c set_si.c set_str.c set_str_raw.c	\
  set_z.c sqrt.c sqrt_ui.c sub.c sub_ui.c trunc.c ui_div.c ui_sub.c	\
  urandomb.c sqrtrem.c mpz_set_fr.c swap.c srandom.h

nodist_libmpfr_a_SOURCES = floor.c ceil.c

EXTRA_DIST = generic.c mpfr-test.h mpf2mpfr.h

CLEANFILES = $(nodist_libmpfr_a_SOURCES)

floor.c: $(srcdir)/trunc.c
	cp $(srcdir)/trunc.c floor.c
ceil.c: $(srcdir)/trunc.c
	cp $(srcdir)/trunc.c ceil.c
