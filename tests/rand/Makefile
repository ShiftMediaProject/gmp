CC 		= gcc
GMPDIR 		= ../..
GMPLIBDIR	= $(GMPDIR)
#GMPLIBDIR	= /tmp/linus/gmp-current
GMPLIB		= $(GMPLIBDIR)/libgmp.a
GMPINC		= $(GMPDIR)/gmp.h

DEP	= statlib.o $(GMPLIB) $(GMPINC)

CFLAGS 	= -Wall -g -I$(GMPDIR) -L$(GMPLIBDIR) $(XCFLAGS)
LIBS	= -lgmp -lm

findcl: findcl.c $(DEP)
gen: gen.c $(DEP)
stat: stat.c $(DEP)
	$(CC) $(CFLAGS) -L. -o $@ $< statlib.o $(LIBS)
foo: foo.c $(GMPLIB) $(GMPINC)
	$(CC) $(CFLAGS) -o $@ $< -lgmp -lm

statlib.o: statlib.c statlib.h $(GMPLIB)

ks: ks.c $(DEP)
	$(CC) $(CFLAGS) -o $@ $< statlib.o $(LIBS)

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<
% : %.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)
