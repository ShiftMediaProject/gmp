## Process this file with automake to generate Makefile.in

AUTOMAKE_OPTIONS = gnu

CPP	= @CPP@

# $top_srcdir is needed for `#include "gmp-impl.h"'.
# .. is needed for config.h and gmp-mparam.h.
# Tell multi-function files which function to produce.  (This should be
# put in DEFS, but then we lose some `-I':s.)
INCLUDES = -I$(top_srcdir) -I.. -DOPERATION_$*

GENERIC_SOURCES = mp_bases.c
OFILES = @mpn_objects@

noinst_LTLIBRARIES = libmpn.la
libmpn_la_SOURCES = $(GENERIC_SOURCES)
libmpn_la_LIBADD = $(OFILES)
libmpn_la_DEPENDENCIES = $(OFILES)

TARG_DIST = a29k alpha arm clipper cray generic hppa i960 m68k mips2 mips3 \
  ns32k pa64 power powerpc32 powerpc64 pyr sh sparc32 sparc64 thumb vax x86 \
  z8000 z8000x

EXTRA_DIST = tests underscore.h asm-defs.m4 $(TARG_DIST)

# COMPILE minus CC.  FIXME: Really pass *_CFLAGS to CPP?
COMPILE_FLAGS =	\
	$(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)

SUFFIXES = .s .S .asm

# FIXME: Use `$(CCAS) $(COMPILE_FLAGS)' instead of $(COMPILE).

# *.s are not preprocessed at all.
.s.o:
	$(CCAS) $(COMPILE_FLAGS) $<
.s.obj:
	$(CCAS) $(COMPILE_FLAGS) `cygpath -w $<`
.s.lo:
	$(LIBTOOL) --mode=compile $(CCAS) $(COMPILE_FLAGS) $<

# *.S are preprocessed with CPP.
.S.o:
	$(CPP) $(COMPILE_FLAGS) $< | grep -v '^#' >tmp-$*.s
	$(CCAS) $(COMPILE_FLAGS) tmp-$*.s -o $@
	rm -f tmp-$*.s
.S.obj:
	$(CPP) $(COMPILE_FLAGS) `cygpath -w $<` | grep -v '^#' >tmp-$*.s
	$(CCAS) $(COMPILE_FLAGS) tmp-$*.s -o $@
	rm -f tmp-$*.s

# We have to rebuild the static object file without passing -DPIC to
# preprocessor.  The overhead cost is one extra assemblation.  FIXME:
# Teach libtool how to assemble with a preprocessor pass (CPP or m4).

.S.lo:
	$(CPP) $(COMPILE_FLAGS) -DPIC $< | grep -v '^#' >tmp-$*.s
	$(LIBTOOL) --mode=compile $(CCAS) $(COMPILE_FLAGS) tmp-$*.s -o $@
	$(CPP) $(COMPILE_FLAGS) $< | grep -v '^#' >tmp-$*.s
	$(CCAS) $(COMPILE_FLAGS) tmp-$*.s -o $*.o
	rm -f tmp-$*.s

# *.m4 are preprocessed with m4.
.asm.o:
	$(M4) -DOPERATION_$* $< >tmp-$*.s
	$(CCAS) $(COMPILE_FLAGS) tmp-$*.s -o $@
	rm -f tmp-$*.s
.asm.obj:
	$(M4) -DOPERATION_$* `cygpath -w $<` >tmp-$*.s
	$(CCAS) $(COMPILE_FLAGS) tmp-$*.s -o $@
	rm -f tmp-$*.s
.asm.lo:
	$(M4) -DPIC -DOPERATION_$* $< >tmp-$*.s
	$(LIBTOOL) --mode=compile $(CCAS) $(COMPILE_FLAGS) tmp-$*.s -o $@
	$(M4) -DOPERATION_$* $< >tmp-$*.s
	$(CCAS) $(COMPILE_FLAGS) tmp-$*.s -o $*.o
	rm -f tmp-$*.s
