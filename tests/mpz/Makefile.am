## Process this file with automake to generate Makefile.in

# Copyright 1996, 1997, 1999, 2000, 2001, 2002 Free Software Foundation,
# Inc.
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

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/tests
LDADD = $(top_builddir)/tests/libtests.la $(top_builddir)/libgmp.la

check_PROGRAMS = t-cmp t-mul t-mul_i t-tdiv t-tdiv_ui t-fdiv t-fdiv_ui  \
  t-cdiv_ui t-gcd t-lcm dive dive_ui t-sqrtrem convert io t-inp_str     \
  logic bit t-powm t-powm_ui t-pow t-div_2exp reuse                     \
  t-root t-perfsqr t-jac t-bin t-get_d t-get_si t-set_si                \
  t-fac_ui t-fib_ui t-lucnum_ui t-scan t-fits                           \
  t-divis t-divis_2exp t-cong t-cong_2exp t-sizeinbase t-set_str        \
  t-aorsmul t-cmp_d t-cmp_si t-hamdist t-oddeven t-popcount t-set_f     \
  t-io_raw t-import t-export

TESTS = $(check_PROGRAMS)

# io.tmp, t-io_raw.tmp and t-inp_str.tmp are used by io.c, t-io_raw.c and
# t-inp_str.c respectively, all removed automatically if the tests pass, but
# arrange to clean them here if the tests fail.
#
CLEANFILES = io.tmp t-io_raw.tmp t-inp_str.tmp

$(top_builddir)/tests/libtests.la:
	cd $(top_builddir)/tests; $(MAKE) $(AM_MAKEFLAGS) libtests.la
