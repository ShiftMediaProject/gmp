CC 		= gcc
GMPDIR 		= ../..
#GMPLIBDIR	= $(GMPDIR)
GMPLIBDIR	= /tmp/linus/gmp-current
GMPLIB		= $(GMPLIBDIR)/libgmp.a
GMPH		= $(GMPDIR)/gmp.h $(GMPLIBDIR)/mpn/gmp-mparam.h

DEP	= statlib.o $(GMPLIB) $(GMPH)

CFLAGS 	= -Wall -g -I$(GMPDIR) -I$(GMPLIBDIR)/mpn -L$(GMPLIBDIR) $(XCFLAGS)
LIBS	= -lgmp -lm

findcl: findcl.c $(DEP)
gen: gen.c $(DEP)
stat: stat.c $(DEP)
	$(CC) $(CFLAGS) -L. -o $@ $< statlib.o $(LIBS)
foo: foo.c zdiv_round.o $(DEP)
	$(CC) $(CFLAGS) -o $@ $< zdiv_round.o -lgmp -lm

statlib.o: statlib.c statlib.h $(GMPLIB)

spect: spect.c zdiv_round.o $(DEP)
	$(CC) $(CFLAGS) -o $@ $< zdiv_round.o -lgmp -lm

ks: ks.c $(DEP)
	$(CC) $(CFLAGS) -o $@ $< statlib.o $(LIBS)

test: gen stat
	@(echo -n '16i: '; ./gen -f mpz_urandomb -z 16 1000 | ./stat -i 0xffff | grep ^[0-9])
	@(echo -n '32i: '; ./gen -f mpz_urandomb -z 32 1000 | ./stat -i 0xffffffff  | grep ^[0-9])
	@(echo -n '64i: '; ./gen -f mpz_urandomb -z 64 1000 | ./stat -i 0xffffffffffffffff  | grep ^[0-9])
	@(echo -n '128i: '; ./gen -f mpz_urandomb -z 128 1000 | ./stat -i 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF | grep ^[0-9])

	@(echo -n '16f: '; ./gen -f mpf_urandomb -z 16 1000 | ./stat | grep ^[0-9])
	@(echo -n '32f: '; ./gen -f mpf_urandomb -z 32 1000 | ./stat | grep ^[0-9])
	@(echo -n '64f: '; ./gen -f mpf_urandomb -z 64 1000 | ./stat | grep ^[0-9])
	@(echo -n '128f: '; ./gen -f mpf_urandomb -z 128 1000 | ./stat | grep ^[0-9])

bigtest: gen stat
	@(echo '16i: '; ./gen -f mpz_urandomb -z 16 50000 | ./stat -2 1000 -i 0xffff | grep ^K[mp])
	@(echo '32i: '; ./gen -f mpz_urandomb -z 32 50000 | ./stat -2 1000 -i 0xffffffff | grep ^K[mp])
	@(echo '64i: '; ./gen -f mpz_urandomb -z 64 50000 | ./stat -2 1000 -i 0xffffffffffffffff  | grep ^K[mp])
	@(echo '128i: '; ./gen -f mpz_urandomb -z 128 50000 | ./stat -2 1000 -i 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF | grep ^K[mp])

	@(echo '16f: '; ./gen -f mpf_urandomb -z 16 50000 | ./stat -2 1000 | grep ^K[mp])
	@(echo '32f: '; ./gen -f mpf_urandomb -z 32 50000 | ./stat -2 1000 | grep ^K[mp])
	@(echo '64f: '; ./gen -f mpf_urandomb -z 64 50000 | ./stat -2 1000 | grep ^K[mp])
	@(echo '128f: '; ./gen -f mpf_urandomb -z 128 50000 | ./stat -2 1000 | grep ^K[mp])


%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<
% : %.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)
