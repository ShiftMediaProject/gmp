## Process this file with automake to generate Makefile.in

# Copyright (C) 1996, 1999, 2000 Free Software Foundation, Inc.
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


INCLUDES = -DBERKELEY_MP -D__gmpz_realloc=_mp_realloc -I$(top_srcdir)

nodist_libmpbsd_la_SOURCES = \
	add.c cmp.c gcd.c mul.c pow_ui.c powm.c realloc.c sqrtrem.c sub.c \
	tdiv_qr.c
libmpbsd_la_SOURCES = \
	itom.c mfree.c min.c mout.c move.c mtox.c sdiv.c xtom.c

if WANT_MPBSD
noinst_LTLIBRARIES = libmpbsd.la
endif

CLEANFILES = $(nodist_libmpbsd_la_SOURCES)


add.c: $(top_srcdir)/mpz/add.c
	cp $(top_srcdir)/mpz/add.c add.c

cmp.c: $(top_srcdir)/mpz/cmp.c
	cp $(top_srcdir)/mpz/cmp.c cmp.c

gcd.c: $(top_srcdir)/mpz/gcd.c
	cp $(top_srcdir)/mpz/gcd.c gcd.c

mul.c: $(top_srcdir)/mpz/mul.c
	cp $(top_srcdir)/mpz/mul.c mul.c

pow_ui.c: $(top_srcdir)/mpz/pow_ui.c
	cp $(top_srcdir)/mpz/pow_ui.c pow_ui.c

powm.c: $(top_srcdir)/mpz/powm.c
	cp $(top_srcdir)/mpz/powm.c powm.c

realloc.c: $(top_srcdir)/mpz/realloc.c
	cp $(top_srcdir)/mpz/realloc.c realloc.c

sqrtrem.c: $(top_srcdir)/mpz/sqrtrem.c
	cp $(top_srcdir)/mpz/sqrtrem.c sqrtrem.c

sub.c: $(top_srcdir)/mpz/sub.c
	cp $(top_srcdir)/mpz/sub.c sub.c

tdiv_qr.c: $(top_srcdir)/mpz/tdiv_qr.c
	cp $(top_srcdir)/mpz/tdiv_qr.c tdiv_qr.c
