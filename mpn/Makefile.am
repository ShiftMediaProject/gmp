## Process this file with automake to generate Makefile.in

# Copyright 1996, 1998, 1999, 2000, 2001, 2002 Free Software Foundation,
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

INCLUDES = -D__GMP_WITHIN_GMP -I$(top_srcdir) \
  -DOPERATION_`echo $* | sed 's/_$$//'`

OFILES = @mpn_objects@


# All possible mpn normal and optional function files are listed here as C
# sources for a dummy library, to get automake to generate ansi2knr rules
# for each.  Such rules will be ignored for any that are instead implemented
# with a .asm (or whatever) for a particular target.
#
EXTRA_LTLIBRARIES = libdummy.la
nodist_libdummy_la_SOURCES =						    \
  add.c add_1.c add_n.c addmul_1.c and_n.c andn_n.c bdivmod.c		    \
  cmp.c com_n.c copyd.c copyi.c						    \
  dc_divrem_n.c dive_1.c diveby3.c divis.c divrem.c divrem_1.c divrem_2.c   \
  dump.c fib2_ui.c gcd.c						    \
  gcd_finda.c gcd_1.c gcdext.c get_str.c hamdist.c invert_limb.c	    \
  ior_n.c iorn_n.c jacbase.c lshift.c mod_1.c mod_34lsub1.c mode1o.c	    \
  mul.c mul_1.c mul_2.c mul_fft.c mul_n.c mul_basecase.c		    \
  nand_n.c nior_n.c perfsqr.c popcount.c				    \
  pre_divrem_1.c pre_mod_1.c pow_1.c random.c random2.c rshift.c	    \
  rootrem.c sb_divrem_mn.c scan0.c scan1.c set_str.c			    \
  sqr_basecase.c sqr_diagonal.c						    \
  sqrtrem.c sub.c sub_1.c sub_n.c submul_1.c				    \
  tdiv_qr.c udiv_qrnnd.c udiv_w_sdiv.c xor_n.c xnor_n.c


noinst_LTLIBRARIES = libmpn.la
libmpn_la_SOURCES = mp_bases.c
libmpn_la_LIBADD = $(OFILES)
libmpn_la_DEPENDENCIES = $(OFILES)

TARG_DIST = a29k alpha arm clipper cray generic i960 ia64 lisp m68k m88k \
  mips32 mips64 ns32k pa32 pa64 power powerpc32 powerpc64 pyr s390 sh \
  sparc32 sparc64 thumb vax x86 z8000 z8000x

EXTRA_DIST = Makeasm.am asm-defs.m4 cpp-ccas m4-ccas $(TARG_DIST)


include Makeasm.am
