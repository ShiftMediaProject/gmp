## Process this file with automake to generate Makefile.in

# Copyright (C) 2000 Free Software Foundation, Inc.
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

INCLUDES = -I$(top_srcdir)
LDADD = $(top_builddir)/libgmp.la

check_PROGRAMS = t-rand
TESTS = $(check_PROGRAMS)

EXTRA_PROGRAMS = findlc gen gen.static spect stat
gen_static_SOURCES = gen.c
gen_static_LDFLAGS = -static
findlc_LDADD = libstat.la
spect_LDADD = libstat.la
stat_LDADD = libstat.la

EXTRA_LTLIBRARIES = libstat.la
libstat_la_SOURCES = statlib.c zdiv_round.c
libstat_la_LIBADD = $(top_builddir)/libgmp.la -lm

EXTRA_DIST = gmpstat.h

CLEANFILES = $(EXTRA_PROGRAMS) $(EXTRA_LTLIBRARIES)

allprogs: $(EXTRA_PROGRAMS)

manual-test: gen$(EXEEXT) stat$(EXEEXT)
	@(echo -n '16i: '; ./gen -f mpz_urandomb -z 16 1000 \
		| ./stat -i 0xffff | grep '^[0-9]')
	@(echo -n '32i: '; ./gen -f mpz_urandomb -z 32 1000 \
		| ./stat -i 0xffffffff  | grep '^[0-9]')
	@(echo -n '33i: '; ./gen -f mpz_urandomb -z 33 1000 \
		| ./stat -i 0x1ffffffff  | grep '^[0-9]')
	@(echo -n '64i: '; ./gen -f mpz_urandomb -z 64 1000 \
		| ./stat -i 0xffffffffffffffff  | grep '^[0-9]')
	@(echo -n '128i: '; ./gen -f mpz_urandomb -z 128 1000 \
		| ./stat -i 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF | grep '^[0-9]')

	@(echo -n '16f: '; ./gen -f mpf_urandomb -z 16 1000 \
		| ./stat | grep '^[0-9]')
	@(echo -n '32f: '; ./gen -f mpf_urandomb -z 32 1000 \
		| ./stat | grep '^[0-9]')
	@(echo -n '33f: '; ./gen -f mpf_urandomb -z 33 1000 \
		| ./stat | grep '^[0-9]')
	@(echo -n '64f: '; ./gen -f mpf_urandomb -z 64 1000 \
		| ./stat | grep '^[0-9]')
	@(echo -n '128f: '; ./gen -f mpf_urandomb -z 128 1000 \
		| ./stat | grep '^[0-9]')

manual-bigtest: gen$(EXEEXT) stat$(EXEEXT)
	@(echo '16i: '; ./gen -f mpz_urandomb -z 16 50000 \
		| ./stat -2 1000 -i 0xffff | grep '^K[mp]')
	@(echo '32i: '; ./gen -f mpz_urandomb -z 32 50000 \
		| ./stat -2 1000 -i 0xffffffff | grep '^K[mp]')
	@(echo '33i: '; ./gen -f mpz_urandomb -z 33 50000 \
		| ./stat -2 1000 -i 0x1ffffffff | grep '^K[mp]')
	@(echo '64i: '; ./gen -f mpz_urandomb -z 64 50000 \
		| ./stat -2 1000 -i 0xffffffffffffffff  | grep '^K[mp]')
	@(echo '128i: '; ./gen -f mpz_urandomb -z 128 50000 \
		| ./stat -2 1000 -i 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF | grep '^K[mp]')

	@(echo '16f: '; ./gen -f mpf_urandomb -z 16 50000 \
		| ./stat -2 1000 | grep '^K[mp]')
	@(echo '32f: '; ./gen -f mpf_urandomb -z 32 50000 \
		| ./stat -2 1000 | grep '^K[mp]')
	@(echo '33f: '; ./gen -f mpf_urandomb -z 33 50000 \
		| ./stat -2 1000 | grep '^K[mp]')
	@(echo '64f: '; ./gen -f mpf_urandomb -z 64 50000 \
		| ./stat -2 1000 | grep '^K[mp]')
	@(echo '128f: '; ./gen -f mpf_urandomb -z 128 50000 \
		| ./stat -2 1000 | grep '^K[mp]')
