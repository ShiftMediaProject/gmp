## Process this file with automake to generate Makefile.in

AUTOMAKE_OPTIONS = gnu

INCLUDES = -I$(top_srcdir) -I..
LDADD = libstat.la ../../libgmp.la -lm

noinst_PROGRAMS = findlc gen gen.static spect stat
gen_static_SOURCES = gen.c
gen_static_LDFLAGS = -static

noinst_LTLIBRARIES = libstat.la
libstat_la_SOURCES = statlib.c zdiv_round.c

test: gen stat
	@(echo -n '16i: '; ./gen -f mpz_urandomb -z 16 1000 | ./stat -i 0xffff | grep ^[0-9])
	@(echo -n '32i: '; ./gen -f mpz_urandomb -z 32 1000 | ./stat -i 0xffffffff  | grep ^[0-9])
	@(echo -n '33i: '; ./gen -f mpz_urandomb -z 33 1000 | ./stat -i 0x1ffffffff  | grep ^[0-9])
	@(echo -n '64i: '; ./gen -f mpz_urandomb -z 64 1000 | ./stat -i 0xffffffffffffffff  | grep ^[0-9])
	@(echo -n '128i: '; ./gen -f mpz_urandomb -z 128 1000 | ./stat -i 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF | grep ^[0-9])

	@(echo -n '16f: '; ./gen -f mpf_urandomb -z 16 1000 | ./stat | grep ^[0-9])
	@(echo -n '32f: '; ./gen -f mpf_urandomb -z 32 1000 | ./stat | grep ^[0-9])
	@(echo -n '33f: '; ./gen -f mpf_urandomb -z 33 1000 | ./stat | grep ^[0-9])
	@(echo -n '64f: '; ./gen -f mpf_urandomb -z 64 1000 | ./stat | grep ^[0-9])
	@(echo -n '128f: '; ./gen -f mpf_urandomb -z 128 1000 | ./stat | grep ^[0-9])

bigtest: gen stat
	@(echo '16i: '; ./gen -f mpz_urandomb -z 16 50000 | ./stat -2 1000 -i 0xffff | grep ^K[mp])
	@(echo '32i: '; ./gen -f mpz_urandomb -z 32 50000 | ./stat -2 1000 -i 0xffffffff | grep ^K[mp])
	@(echo '33i: '; ./gen -f mpz_urandomb -z 33 50000 | ./stat -2 1000 -i 0x1ffffffff | grep ^K[mp])
	@(echo '64i: '; ./gen -f mpz_urandomb -z 64 50000 | ./stat -2 1000 -i 0xffffffffffffffff  | grep ^K[mp])
	@(echo '128i: '; ./gen -f mpz_urandomb -z 128 50000 | ./stat -2 1000 -i 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF | grep ^K[mp])

	@(echo '16f: '; ./gen -f mpf_urandomb -z 16 50000 | ./stat -2 1000 | grep ^K[mp])
	@(echo '32f: '; ./gen -f mpf_urandomb -z 32 50000 | ./stat -2 1000 | grep ^K[mp])
	@(echo '33f: '; ./gen -f mpf_urandomb -z 33 50000 | ./stat -2 1000 | grep ^K[mp])
	@(echo '64f: '; ./gen -f mpf_urandomb -z 64 50000 | ./stat -2 1000 | grep ^K[mp])
	@(echo '128f: '; ./gen -f mpf_urandomb -z 128 50000 | ./stat -2 1000 | grep ^K[mp])
