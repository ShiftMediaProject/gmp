## Process this file with automake to generate Makefile.in

AUTOMAKE_OPTIONS = gnu

# $top_srcdir is needed for `#include "gmp-impl.h"'.
# .. is needed for config.h and gmp-mparam.h.
INCLUDES = -I$(top_srcdir) -I..

noinst_LTLIBRARIES = libmpf.la
libmpf_la_SOURCES = \
  init.c init2.c set.c set_ui.c set_si.c set_str.c set_d.c set_z.c \
  set_q.c iset.c iset_ui.c iset_si.c iset_str.c iset_d.c clear.c get_str.c \
  dump.c size.c eq.c reldiff.c sqrt.c random2.c inp_str.c out_str.c \
  add.c add_ui.c sub.c sub_ui.c ui_sub.c mul.c mul_ui.c div.c div_ui.c \
  cmp.c cmp_ui.c cmp_si.c mul_2exp.c div_2exp.c abs.c neg.c get_d.c \
  set_dfl_prec.c set_prc.c set_prc_raw.c get_prc.c ui_div.c sqrt_ui.c \
  pow_ui.c urandom.c
nodist_libmpf_la_SOURCES = floor.c ceil.c trunc.c

H = $(top_srcdir)/gmp.h $(top_srcdir)/gmp-impl.h ../gmp-mparam.h

EXTRA_DIST = integer.c tests

floor.c: $(srcdir)/integer.c
	cp $(srcdir)/integer.c floor.c
floor.o: floor.c $(H)
	$(COMPILE) -c -DMPF_FLOOR floor.c
floor.lo: floor.c $(H)
	$(LTCOMPILE) -c -DMPF_FLOOR floor.c

ceil.c: $(srcdir)/integer.c
	cp $(srcdir)/integer.c ceil.c
ceil.o: ceil.c $(H)
	$(COMPILE) -c -DMPF_CEIL ceil.c
ceil.lo: ceil.c $(H)
	$(LTCOMPILE) -c -DMPF_CEIL ceil.c

trunc.c: $(srcdir)/integer.c
	cp $(srcdir)/integer.c trunc.c
trunc.o: trunc.c $(H)
	$(COMPILE) -c -DMPF_TRUNC trunc.c
trunc.lo: trunc.c $(H)
	$(LTCOMPILE) -c -DMPF_TRUNC trunc.c
