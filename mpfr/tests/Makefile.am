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

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/mpfr
LDADD = $(top_builddir)/libgmp.la
dummy_LDADD = 

if WANT_MPFR
# FIXME: The following tests need mpfr_set_machine_rnd_mode() which is not 
# built right now:
# tadd tget_str tset_f tsqrt tagm tmul tdiv tlog texp tdiv_ui tout_str
MPBSD_check_OPTION = tcmp2 tmul_ui tcmp_ui tround tset_si tcmp tmul_2exp \
  tset_d tset_str tpi tset_z tlog2 tcan_round tzeta
endif

check_PROGRAMS = dummy $(MPBSD_check_OPTION)
TESTS = $(check_PROGRAMS)
