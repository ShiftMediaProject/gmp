## Process this file with automake to generate Makefile.in

#AUTOMAKE_OPTIONS = gnu no-dependencies
AUTOMAKE_OPTIONS = gnu

# $top_srcdir is needed for `#include "gmp-impl.h"'.  FIXME?
# FIXME: Need `-I$(srcdir)' and `-I.'?  Where to find config.h?
INCLUDES = -I$(top_srcdir)

EXTRA_DIST = tests

noinst_LTLIBRARIES = libmpq.la
libmpq_la_SOURCES = \
  add.c canonicalize.c clear.c cmp.c cmp_ui.c div.c get_d.c \
  get_den.c get_num.c init.c inv.c mul.c neg.c set.c set_den.c \
  set_num.c set_si.c set_ui.c sub.c equal.c set_z.c
