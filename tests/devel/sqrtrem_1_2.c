/*
Copyright 2017 Free Software Foundation, Inc.

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
the GNU MP Library test suite.  If not, see https://www.gnu.org/licenses/.  */

/* Usage:

   ./sqrtrem_1_2 x

     Checks mpn_sqrtrem() exhaustively, starting from 0, incrementing
     the operand by a single unit, until all values handled by
     sqrtrem_{1,2} are tested. SLOW.

   ./sqrtrem_1_2 c

     Checks all corner cased for mpn_sqrtrem(). I.e. values of the form
     i*i and (i+1)*(i+1)-1, for each value of i, until all such values,
     handled by sqrtrem_{1,2}, are tested.
     Less slow than the exhaustive test.

   For larger values, use
   ./try mpn_sqrtrem

 */

#include <stdlib.h>
#include <stdio.h>
#include "gmp-impl.h"
#include "longlong.h"
#include "tests.h"
/* #define STOP(x) return (x) */
#define STOP(x) x
#define SPINNER(v)					\
  do {							\
    MPN_SIZEINBASE_2EXP (spinner_count, q, v, 1);	\
    --spinner_count;					\
    spinner();						\
  } while (0)

int something_wrong (mp_limb_t er, mp_limb_t ec, mp_limb_t es)
{
  fprintf (stderr, "root = %lu , rem = {%lu , %lu}\n", (long unsigned) es,(long unsigned) ec,(long unsigned) er);
  return -1;
}

int
check_all_values ()
{
  mp_limb_t es, mer, er, s[1], r[2], q[2];
  mp_size_t x;
  unsigned bits;

  es=1;
  er=0;
  mer=2;
  *q = 1;
  printf ("All values tested, up to bits:\n");
  do {
    x = mpn_sqrtrem (s, r, q, 1);
    if (UNLIKELY (x != (er != 0)) || UNLIKELY (*s != es)
	|| UNLIKELY ((x == 1) && (er != *r)))
      STOP (something_wrong (er, 0, es));

    if (UNLIKELY (er == mer)) {
      ++es;
      if (UNLIKELY ((es & 0xff) == 0))
	SPINNER(1);
      mer +=2; /* mer = es * 2 */
      er = 0;
    } else
      ++er;
    ++*q;
  } while (*q != 0);
  q[1] = 1;
  SPINNER(2);
  printf ("\nValues of a single limb, tested.\nAll values tested, up to bits:\n");
  do {
    x = mpn_sqrtrem (s, r, q, 2);
    if (UNLIKELY (x != (er != 0)) || UNLIKELY (*s != es)
	|| UNLIKELY ((x == 1) && (er != *r)))
      STOP (something_wrong (er, 0, es));

    if (UNLIKELY (er == mer)) {
      ++es;
      if (UNLIKELY ((es & 0x7f) == 0))
	SPINNER(2);
      mer +=2; /* mer = es * 2 */
      if (UNLIKELY (mer == 0))
	break;
      er = 0;
    } else
      ++er;
    q[1] += (++*q == 0);
  } while (1);
  MPN_SIZEINBASE_2EXP (bits, q, 2, 1);
  printf("\n%u\n", bits);
  printf ("\nValues with at most a limb for reminder, tested.\n");
  return 0;
}

int
check_corner_cases ()
{
  mp_limb_t es, er, s[1], r[2], q[2];
  mp_size_t x;
  unsigned bits;

  es = 1;
  er = 0;
  *q = 1;
  printf ("Corner cases tested, up to bits:\n");
  do {
    x = mpn_sqrtrem (s, r, q, 1);
    if (UNLIKELY (x != (er != 0)) || UNLIKELY (*s != es)
	|| UNLIKELY ((x == 1) && (er != *r)))
      STOP (something_wrong (er, 0, es));

    if (er != 0) {
      ++es;
      if (UNLIKELY ((es & 0xffff) == 0))
	SPINNER(1);
      er = 0;
      ++*q;
    } else {
      er = es * 2;
      *q += er;
    }
  } while (*q != 0);
  q[1] = 1;
  SPINNER(2);
  printf ("\nValues of a single limb, tested.\nCorner cases tested, up to bits:\n");
  do {
    x = mpn_sqrtrem (s, r, q, 2);
    if (UNLIKELY (x != (er != 0)) || UNLIKELY (*s != es)
	|| UNLIKELY ((x == 1) && (er != *r)))
      STOP (something_wrong (er, 0, es));

    if (er != 0) {
      ++es;
      if (UNLIKELY ((es & 0xff) == 0))
	SPINNER(2);
      er = 0;
      q[1] += (++*q == 0);
      if (UNLIKELY (es == GMP_NUMB_HIGHBIT))
	break;
    } else {
      er = es * 2;
      add_ssaaaa (q[1], *q, q[1], *q, 0, er);
    }
  } while (1);
  SPINNER(2);
  printf ("\nValues with at most a limb for reminder, tested.\nCorner cases tested, up to bits:\n");
  x = mpn_sqrtrem (s, r, q, 2);
  if ((*s != es) || (x != 0))
    STOP (something_wrong (0, 0, es));
  q[1] += 1;
  x = mpn_sqrtrem (s, r, q, 2);
  if ((*s != es) || (x != 2) || (*r != 0) || (r[1] != 1))
    STOP (something_wrong (0, 1, es));
  ++es;
  q[1] += (++*q == 0);
  do {
    x = mpn_sqrtrem (s, r, q, 2);
    if (UNLIKELY (x != (er != 0) * 2) || UNLIKELY (*s != es)
	|| UNLIKELY ((x == 2) && ((er != *r) || (r[1] != 1))))
      STOP (something_wrong (er, er != 0, es));

    if (er != 0) {
      ++es;
      if (UNLIKELY (es == 0))
	break;
      if (UNLIKELY ((es & 0xff) == 0))
	SPINNER(2);
      er = 0;
      q[1] += (++*q == 0);
    } else {
      er = es * 2;
      add_ssaaaa (q[1], *q, q[1], *q, 1, er);
    }
  } while (1);
  MPN_SIZEINBASE_2EXP (bits, q, 2, 1);
  printf ("\n%u\nValues of at most two limbs, tested.\n", bits);
  return 0;
}

int
main (int argc, char **argv)
{
  int mode = 0;

  if (argc > 1)
    {
      if (*argv[1] == 'x')
	mode = 0;
      else if (*argv[1] == 'c')
	mode = 1;
      else
	mode = -1;
      if (argc > 2 || mode == -1)
	{
	  fprintf (stderr, "usage: sqrtrem_1_2 [x|c]\n");
	  exit (1);
	}
    }

  if (mode == 0)
    return check_all_values ();
  else
    return check_corner_cases ();
}
