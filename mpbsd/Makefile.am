## Process this file with automake to generate Makefile.in

# Copyright 1996, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
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

# -I$(top_srcdir)/mpz is for #includes done by mpz .c files.  Perhaps most
# compilers are smart enough to look in the same directory as the .c file
# already, but lets make absolutely sure.
#
INCLUDES = -DBERKELEY_MP -D__GMP_WITHIN_GMP -D__gmpz_realloc=_mp_realloc \
	-I$(top_srcdir) -I$(top_srcdir)/mpz

# The mpz sources here all know to look for -DBERKELEY_MP to compile to in
# mpbsd form.
#
# FIXME: Would have just put ../mpz/add.c etc in libmpbsd_la_SOURCES, but as
# of automake 1.5 that doesn't work in combination with ansi2knr.
#
libmpbsd_la_SOURCES = itom.c mfree.c min.c mout.c mtox.c rpow.c sdiv.c xtom.c
nodist_libmpbsd_la_SOURCES = \
  add.c cmp.c gcd.c mul.c powm.c realloc.c set.c sqrtrem.c sub.c tdiv_qr.c

$(nodist_libmpbsd_la_SOURCES):
	for i in $(nodist_libmpbsd_la_SOURCES); do \
	  cp $(top_srcdir)/mpz/$$i .; \
        done

CLEANFILES = $(nodist_libmpbsd_la_SOURCES)

if WANT_MPBSD
noinst_LTLIBRARIES = libmpbsd.la
endif
