## Process this file with automake to generate Makefile.in

# Copyright 2001, 2002 Free Software Foundation, Inc.
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

INCLUDES = -D__GMP_WITHIN_GMP -I$(top_srcdir)

noinst_LTLIBRARIES = libprintf.la
EXTRA_LTLIBRARIES = libdummy.la

libprintf_la_SOURCES =							 \
  asprintf.c asprntffuns.c doprnt.c doprntf.c doprnti.c			 \
  fprintf.c obprintf.c obvprintf.c obprntffuns.c			 \
  printf.c printffuns.c snprintf.c snprntffuns.c sprintf.c sprintffuns.c \
  vasprintf.c vfprintf.c vprintf.c vsnprintf.c vsprintf.c
libprintf_la_LIBADD = @REPL_VSNPRINTF_OBJ@
libprintf_la_DEPENDENCIES = $(libprintf_la_LIBADD)

# Ensure there's an ansi2knr rule for this, if it's needed.
libdummy_la_SOURCES = repl-vsnprintf.c
