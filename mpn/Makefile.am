## Process this file with automake to generate Makefile.in

AUTOMAKE_OPTIONS = gnu

## Some variables are inherited from Autoconf; these are `CC',
## `CFLAGS', `CPPFLAGS', `DEFS', `LDFLAGS', and `LIBS'.
## Defined by Automake: `INCLUDES', `COMPILE', `LINK'.

CPP	= @CPP@

# $top_srcdir is needed for `#include "gmp-impl.h"'.  FIXME?
# FIXME: Need `-I$(srcdir)' and `-I.'?  Where to find config.h?
INCLUDES = -I$(top_srcdir)

GENERIC_SOURCES = mp_bases.c
OFILES = @mpn_objects@

noinst_LTLIBRARIES = libmpn.la
libmpn_la_SOURCES = $(GENERIC_SOURCES)
libmpn_la_LIBADD = $(OFILES)
libmpn_la_DEPENDENCIES = $(OFILES)

## FIXME: --version-info is on the form `current[:revision[:age]]' (see libtool doc)
## Hint: See binutils/bfd/Makefile.am
## libmpn_la_LDFLAGS = -version-info @VERSION@

TARG_DIST = a29k alpha arm clipper cray generic hppa i960 m68k mips2 mips3 \
  ns32k pa64 power powerpc32 powerpc64 pyr sh sparc32 sparc64 thumb vax x86 \
  z8000 z8000x

EXTRA_DIST = tests underscore.h $(TARG_DIST)

## FIXME: Add `.asm'.
SUFFIXES = .s .S

.s.o:
	$(COMPILE) -c $<
.s.obj:
	$(COMPILE) -c `cygpath -w $<`
.s.lo:
	$(LTCOMPILE) -c $<

# COMPILE minus CC:
COMPILE_FLAGS =	\
	$(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)

.S.o:
	$(CPP) $(COMPILE_FLAGS) $< | grep -v '^#' >tmp-$*.s
	$(COMPILE) -c tmp-$*.s -o $@
	rm -f tmp-$*.s
.S.obj:
	$(CPP) $(COMPILE_FLAGS) `cygpath -w $<` | grep -v '^#' >tmp-$*.s
	$(COMPILE) -c tmp-$*.s -o $@
	rm -f tmp-$*.s

# We have to rebuild the static object file without -DPIC to CPP.
# FIXME: Teach libtool how to assemble with a preprocessor pass (CPP
# or m4).
.S.lo:
	$(CPP) $(COMPILE_FLAGS) -DPIC $< | grep -v '^#' >tmp-$*.s
	$(LTCOMPILE) -c tmp-$*.s -o $@
	$(CPP) $(COMPILE_FLAGS) $< | grep -v '^#' >tmp-$*.s
	$(COMPILE) -c tmp-$*.s -o $*.o
	rm -f tmp-$*.s
