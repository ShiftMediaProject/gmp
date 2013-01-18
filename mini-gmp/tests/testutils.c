/*

Copyright 2013, Free Software Foundation, Inc.

This file is part of the GNU MP Library test suite.

The GNU MP Library test suite is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 3 of the License,
or (at your option) any later version.

The GNU MP Library test suite is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
Public License for more details.

You should have received a copy of the GNU General Public License along with
the GNU MP Library test suite.  If not, see http://www.gnu.org/licenses/.  */

#include "testutils.h"

/* Include it here, so we we could tweak, e.g., how MPZ_REALLOC
   works. */
#include "../mini-gmp.c"

int
main (int argc, char **argv)
{
  hex_random_init ();
  /* Currently, t-comb seems to be the only program accepting any
     arguments. It might make sense to parse common arguments here. */
  testmain (argc, argv);

  return 0;
}
