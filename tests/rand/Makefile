CC 		= gcc
GMPDIR 		= ../..
GMPLIBDIR	= $(GMPDIR)
GMPLIB		= $(GMPLIBDIR)/libgmp.a
GMPINC		= $(GMPDIR)/gmp.h

DEP	= statlib.o $(GMPLIB) $(GMPINC)

CFLAGS 	= -Wall -g -I$(GMPDIR) -L$(GMPLIBDIR) $(XCFLAGS)
LIBS	= -lgmp -lm

findcl: findcl.c $(DEP)
gen: gen.c $(DEP)
stat: stat.c $(DEP)
	$(CC) $(CFLAGS) -L$(GMPLIBDIR) -L. -o $@ $< statlib.o $(LIBS)
foo: foo.c $(DEP)
	$(CC) $(CFLAGS) -L$(GMPLIBDIR) -o $@ $< -lgmp

statlib.o: statlib.c statlib.h $(GMPLIB)

ks: ks.c $(DEP)
	$(CC) $(CFLAGS) -o $@ $< statlib.o $(LIBS)

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<
% : %.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)
