/* A test program doing nothing really, just linking to all the BSD MP
   functions that're supposed to exist. */

/*
Copyright (C) 2000 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA.
*/


#include "mp.h"

int
main (argc, argv)
int argc;
char *argv[];
{
  MINT *a, *b, *c, *d;
  short  h;

  a = itom (123);
  b = xtom ("DEADBEEF");
  c = itom (0);
  d = itom (0);
  move (a, b);
  madd (a, b, c);
  msub (a, b, c);
  mult (a, b, c);
  mdiv (b, a, c, d);
  sdiv (b, 2, c, &h);
  msqrt (a, c, d);
  pow (b, a, a, c);
  rpow (a, 3, c);
  gcd (a, b, c);
  mcmp (a, b);
  if (argc > 1)
    min (c);
  mout (a);
  mtox (b);

  mfree(a);

  return 0;
}
