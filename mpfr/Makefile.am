## Process this file with automake to generate Makefile.in

# Copyright 2000, 2001, 2002 Free Software Foundation, Inc.
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

INCLUDES = -I$(top_srcdir)
CFLAGS = @MPFR_CFLAGS@
LIBS = @MPFR_LIBS@

if WANT_MPFR
lib_LIBRARIES = libmpfr.a
include_HEADERS = mpfr.h mpf2mpfr.h
endif

# FIXME: Automake (version 1.5) doesn't allow conditionalized info_TEXINFOS,
# see top level Makefile.am for the workaround.
info_TEXINFOS = mpfr.texi
TEXINFO_TEX = ../texinfo.tex

# Override automake default AR=ar with the value from GMP_PROG_AR.
AR = @AR@

libmpfr_a_SOURCES = mpfr.h mpf2mpfr.h mpfr-impl.h mpfr-math.h mpfr-test.h exceptions.c save_expo.c extract.c uceil_exp2.c uceil_log2.c ufloor_log2.c add.c add1.c add_one_ulp.c add_ui.c agm.c clear.c cmp.c cmp_abs.c cmp_ui.c div_2exp.c div_2si.c div_2ui.c div.c div_ui.c dump.c eq.c exp2.c exp3.c exp.c get_d.c get_str.c init.c inp_str.c isinteger.c isinf.c isnan.c isnum.c const_log2.c log.c mul_2exp.c mul_2si.c mul_2ui.c mul.c mul_ui.c neg.c out_str.c const_pi.c pow.c pow_si.c pow_ui.c print_raw.c print_rnd_mode.c random2.c random.c reldiff.c rnd_mode.c round_prec.c set.c set_d.c set_dfl_prec.c set_rnd.c set_f.c set_prc_raw.c set_prec.c set_q.c set_si.c set_str.c set_str_raw.c set_ui.c set_z.c sqrt.c sqrt_ui.c sub.c sub1.c sub_one_ulp.c sub_ui.c rint.c ui_div.c ui_sub.c urandomb.c get_z_exp.c swap.c  factorial.c cosh.c sinh.c tanh.c acosh.c asinh.c atanh.c atan.c cmp2.c exp_2.c asin.c const_euler.c cos.c sin.c tan.c fma.c hypot.c log1p.c expm1.c log2.c log10.c ui_pow.c ui_pow_ui.c minmax.c dim.c copysign.c gmp_op.c init2.c acos.c sin_cos.c set_nan.c set_inf.c

libmpfr_a_LIBADD = @LIBOBJS@

EXTRA_DIST = BUGS generic.c
