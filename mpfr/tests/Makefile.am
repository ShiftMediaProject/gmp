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

AUTOMAKE_OPTIONS = gnu no-dependencies

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/mpfr
LDADD = ../libmpfr.a $(top_builddir)/libgmp.la $(LIBM)

if WANT_MPFR
check_PROGRAMS= reuse tabs tadd tagm tcan_round tcmp tcmp2 tcmp_ui tdiv tdiv_ui tdump teq texp tget_str tlog tlog2 tmul tmul_2exp tmul_ui tout_str tpi tpow trandom tround tset_d tset_f tset_q tset_si tset_str tset_z tsqrt tsqrt_ui tui_div tui_sub tswap ttrunc tisnan tget_d tatan tcosh tsinh ttanh tacosh tasinh tatanh thyperbolic texp2 tfactorial tsub tasin teuler tcos tsin ttan tsub_ui tset tlog1p texpm1 tlog_base_2 tui_pow tpow3 tlog_base_10 tadd_ui texceptions tfma thypot
TESTS = $(check_PROGRAMS)
endif
EXTRA_DIST= tgeneric.c

../libmpfr.a:
	cd $(top_srcdir)/mpfr; make libmpfr.a  

