/* Test precision of mpf_class expressions.

Copyright 2001, 2002, 2003 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include "config.h"

#include <iostream>

#include "gmp.h"
#ifdef WANT_MPFR
#  include "mpfr.h"
#endif
#include "gmpxx.h"
#include "gmp-impl.h"
#include "tests.h"

using namespace std;


const int
small_prec = 64, medium_prec = 128, large_prec = 192, very_large_prec = 256;

#define ASSERT_ALWAYS_PREC(a, s, prec) \
{                                      \
  mpf_srcptr _a = a.get_mpf_t();       \
  mpf_class _b(s, prec);               \
  mpf_srcptr _c = _b.get_mpf_t();      \
  ASSERT_ALWAYS(mpf_eq(_a, _c, prec)); \
}

#define ASSERT_ALWAYS_PREC2(a, s, prec) \
{                                       \
  mpfr_srcptr _a = a.get_mpfr_t();      \
  mpfr_class _b(strip(s), prec);        \
  mpfr_srcptr _c = _b.get_mpfr_t();     \
  ASSERT_ALWAYS(mpfr_eq(_a, _c, prec)); \
}


// mpfr_get_str doesn't accept whitespace, strip it
string
strip (const char *ptr)
{
  string s(ptr);
  size_t pos;
  while ((pos = s.find(" ")) != string::npos)
    s.replace(pos, 1, "");
  return s;
}


void
check_mpf (void)
{
  mpf_set_default_prec(medium_prec);

  // simple expressions
  {
    mpf_class f(3.0, small_prec);
    mpf_class g(1 / f, very_large_prec);
    ASSERT_ALWAYS_PREC
      (g, "0.33333 33333 33333 33333 33333 33333 33333 33333 33333 33333"
       "     33333 33333 33333 33333 33333 333", very_large_prec);
  }
  {
    mpf_class f(9.0, medium_prec);
    mpf_class g(0.0, very_large_prec);
    g = 1 / f;
    ASSERT_ALWAYS_PREC
      (g, "0.11111 11111 11111 11111 11111 11111 11111 11111 11111 11111"
       "     11111 11111 11111 11111 11111 11", very_large_prec);
  }
  {
    mpf_class f(15.0, large_prec);
    mpf_class g(0.0, very_large_prec);
    g = 1 / f;
    ASSERT_ALWAYS_PREC
      (g, "0.06666 66666 66666 66666 66666 66666 66666 66666 66666 66666"
       "     66666 66666 66666 66666 66666 67", very_large_prec);
  }

  // compound expressions
  {
    mpf_class f(3.0, small_prec);
    mpf_class g(-(-(-1 / f)), very_large_prec);
    ASSERT_ALWAYS_PREC
      (g, "-0.33333 33333 33333 33333 33333 33333 33333 33333 33333 33333"
       "      33333 33333 33333 33333 33333 333", very_large_prec);
  }
  {
    mpf_class f(3.0, small_prec), g(9.0, medium_prec);
    mpf_class h(0.0, very_large_prec);
    h = 1/f + 1/g;
    ASSERT_ALWAYS_PREC
      (h, "0.44444 44444 44444 44444 44444 44444 44444 44444 44444 44444"
       "     44444 44444 44444 44444 44444 444", very_large_prec);
  }
  {
    mpf_class f(3.0, small_prec), g(9.0, medium_prec), h(15.0, large_prec);
    mpf_class i(0.0, very_large_prec);
    i = f / g + h;
    ASSERT_ALWAYS_PREC
      (i, "15.33333 33333 33333 33333 33333 33333 33333 33333 33333 33333"
       "      33333 33333 33333 333", very_large_prec);
  }
  {
    mpf_class f(3.0, small_prec);
    mpf_class g(-(1 + f) / 3, very_large_prec);
    ASSERT_ALWAYS_PREC
      (g, "-1.33333 33333 33333 33333 33333 33333 33333 33333 33333 33333"
       "      33333 33333 33333 333", very_large_prec);
  }
  {
    mpf_class f(9.0, medium_prec);
    mpf_class g(0.0, very_large_prec);
    g = sqrt(1 / f);
    ASSERT_ALWAYS_PREC
      (g, "0.33333 33333 33333 33333 33333 33333 33333 33333 33333 33333"
       "     33333 33333 33333 33333 33333 333", very_large_prec);
  }
  {
    mpf_class f(15.0, large_prec);
    mpf_class g(0.0, very_large_prec);
    g = hypot(1 + 5 / f, 1.0);
    ASSERT_ALWAYS_PREC
      (g, "1.66666 66666 66666 66666 66666 66666 66666 66666 66666 66666"
       "     66666 66666 66666 667", very_large_prec);
  }

  // compound assignments
  {
    mpf_class f(3.0, small_prec), g(9.0, medium_prec);
    mpf_class h(1.0, very_large_prec);
    h -= f / g;
    ASSERT_ALWAYS_PREC
      (h, "0.66666 66666 66666 66666 66666 66666 66666 66666 66666 66666"
       "     66666 66666 66666 66666 66666 667", very_large_prec);
  }

  // construction from expressions
  {
    mpf_class f(3.0, small_prec);
    mpf_class g(0.0, very_large_prec);
    g = mpf_class(1 / f);
    ASSERT_ALWAYS_PREC(g, "0.33333 33333 33333 33333", small_prec);
  }
  {
    mpf_class f(9.0, medium_prec);
    mpf_class g(0.0, very_large_prec);
    g = mpf_class(1 / f);
    ASSERT_ALWAYS_PREC
      (g, "0.11111 11111 11111 11111 11111 11111 11111 111", medium_prec);
  }
  {
    mpf_class f(15.0, large_prec);
    mpf_class g(0.0, very_large_prec);
    g = mpf_class(1 / f);
    ASSERT_ALWAYS_PREC
      (g, "0.06666 66666 66666 66666 66666 66666 66666 66666 66666 66666"
       "     66666 667", large_prec);
  }

  {
    mpf_class f(3.0, small_prec), g(9.0, medium_prec);
    mpf_class h(0.0, very_large_prec);
    h = mpf_class(f / g + 1, large_prec);
    ASSERT_ALWAYS_PREC
      (h, "1.33333 33333 33333 33333 33333 33333 33333 33333 33333 3333",
       large_prec);
  }

  // mixed mpf/mpq expressions
  {
    mpf_class f(3.0, small_prec);
    mpq_class q(1, 3);
    mpf_class g(0.0, very_large_prec);
    g = f - q;
    ASSERT_ALWAYS_PREC
      (g, "2.66666 66666 66666 66666 66666 66666 66666 66666 66666 66666"
       "     66666 66666 66666 667", very_large_prec);
  }

  {
    mpf_class f(3.0, small_prec);
    mpq_class q(1, 3);
    mpf_class g(0.0, very_large_prec);
    g = mpf_class(f - q, large_prec);
    ASSERT_ALWAYS_PREC
      (g, "2.66666 66666 66666 66666 66666 66666 66666 66666 66666 6667",
       large_prec);
  }
  {
    mpf_class f(3.0, small_prec);
    mpq_class q(1, 3);
    mpf_class g(0.0, very_large_prec);
    g = mpf_class(f - q);
    ASSERT_ALWAYS_PREC
      (g, "2.66666 66666 66666 66666 66666 6667", medium_prec);
  }
  {
    mpf_class f(15.0, large_prec);
    mpq_class q(1, 3);
    mpf_class g(0.0, very_large_prec);
    g = mpf_class(f + q);
    ASSERT_ALWAYS_PREC
      (g, "15.33333 33333 33333 33333 33333 33333 33333 33333 33333 3333",
       large_prec);
  }
}

void
check_mpfr (void)
{
#ifdef WANT_MPFR

  mpfr_set_default_prec(medium_prec);

  // simple expressions
  {
    mpfr_class f(3.0, small_prec);
    mpfr_class g(1 / f, very_large_prec);
    ASSERT_ALWAYS_PREC2
      (g, "0.33333 33333 33333 33333 33333 33333 33333 33333 33333 33333"
       "     33333 33333 33333 33333 33333 333", very_large_prec);
  }
  {
    mpfr_class f(9.0, medium_prec);
    mpfr_class g(0.0, very_large_prec);
    g = 1 / f;
    ASSERT_ALWAYS_PREC2
      (g, "0.11111 11111 11111 11111 11111 11111 11111 11111 11111 11111"
       "     11111 11111 11111 11111 11111 111", very_large_prec);
  }
  {
    mpfr_class f(15.0, large_prec);
    mpfr_class g(0.0, very_large_prec);
    g = 1 / f;
    ASSERT_ALWAYS_PREC2
      (g, "0.06666 66666 66666 66666 66666 66666 66666 66666 66666 66666"
       "     66666 66666 66666 66666 66666 667", very_large_prec);
  }

  // compound expressions
  {
    mpfr_class f(3.0, small_prec);
    mpfr_class g(-(-(-1 / f)), very_large_prec);
    ASSERT_ALWAYS_PREC2
      (g, "-0.33333 33333 33333 33333 33333 33333 33333 33333 33333 33333"
       "      33333 33333 33333 33333 33333 333", very_large_prec);
  }
  {
    mpfr_class f(3.0, small_prec), g(9.0, medium_prec);
    mpfr_class h(0.0, very_large_prec);
    h = 1/f + 1/g;
    ASSERT_ALWAYS_PREC2
      (h, "0.44444 44444 44444 44444 44444 44444 44444 44444 44444 44444"
       "     44444 44444 44444 44444 44444 444", very_large_prec);
  }
  {
    mpfr_class f(3.0, small_prec), g(9.0, medium_prec), h(15.0, large_prec);
    mpfr_class i(0.0, very_large_prec);
    i = f / g + h;
    ASSERT_ALWAYS_PREC2
      (i, "15.33333 33333 33333 33333 33333 33333 33333 33333 33333 33333"
       "      33333 33333 33333 33333 33333 3", very_large_prec);
  }
  {
    mpfr_class f(3.0, small_prec);
    mpfr_class g(-(1 + f) / 3, very_large_prec);
    ASSERT_ALWAYS_PREC2
      (g, "-1.33333 33333 33333 33333 33333 33333 33333 33333 33333 33333"
       "      33333 33333 33333 33333 33333 333", very_large_prec);
  }
  {
    mpfr_class f(9.0, medium_prec);
    mpfr_class g(0.0, very_large_prec);
    g = sqrt(1 / f);
    ASSERT_ALWAYS_PREC2
      (g, "0.33333 33333 33333 33333 33333 33333 33333 33333 33333 33333"
       "     33333 33333 33333 33333 33333 333", very_large_prec);
  }
  {
    mpfr_class f(15.0, large_prec);
    mpfr_class g(0.0, very_large_prec);
    g = hypot(1 + 5 / f, 1.0);
    ASSERT_ALWAYS_PREC2
      (g, "1.66666 66666 66666 66666 66666 66666 66666 66666 66666 66666"
       "     66666 66666 66666 66666 66666 67", very_large_prec);
  }

  // compound assignments
  {
    mpfr_class f(3.0, small_prec), g(9.0, medium_prec);
    mpfr_class h(1.0, very_large_prec);
    h -= f / g;
    ASSERT_ALWAYS_PREC2
      (h, "0.66666 66666 66666 66666 66666 66666 66666 66666 66666 66666"
       "     66666 66666 66666 66666 66666 66", very_large_prec);
  }

  // construction from expressions
  {
    mpfr_class f(3.0, small_prec);
    mpfr_class g(0.0, very_large_prec);
    g = mpfr_class(1 / f);
    ASSERT_ALWAYS_PREC2(g, "0.33333 33333 33333 33333", small_prec);
  }
  {
    mpfr_class f(9.0, medium_prec);
    mpfr_class g(0.0, very_large_prec);
    g = mpfr_class(1 / f);
    ASSERT_ALWAYS_PREC2
      (g, "0.11111 11111 11111 11111 11111 11111 11111 11111", medium_prec);
  }
  {
    mpfr_class f(15.0, large_prec);
    mpfr_class g(0.0, very_large_prec);
    g = mpfr_class(1 / f);
    ASSERT_ALWAYS_PREC2
      (g, "0.06666 66666 66666 66666 66666 66666 66666 66666 66666 66666"
       "     66666 6667", large_prec);
  }

  {
    mpfr_class f(3.0, small_prec), g(9.0, medium_prec);
    mpfr_class h(0.0, very_large_prec);
    h = mpfr_class(f / g + 1, large_prec);
    ASSERT_ALWAYS_PREC2
      (h, "1.33333 33333 33333 33333 33333 33333 33333 33333 33333 33333"
       "     33333 333", large_prec);
  }

  // mixed mpfr/mpq expressions
  {
    mpfr_class f(3.0, small_prec);
    mpq_class q(1, 3);
    mpfr_class g(0.0, very_large_prec);
    g = f - q;
    ASSERT_ALWAYS_PREC2
      (g, "2.66666 66666 66666 66666 66666 66666 66666 66666 66666 66666"
       "     66666 66666 66666 66666 66666 67", very_large_prec);
  }

  {
    mpfr_class f(3.0, small_prec);
    mpq_class q(1, 3);
    mpfr_class g(0.0, very_large_prec);
    g = mpfr_class(f - q, large_prec);
    ASSERT_ALWAYS_PREC2
      (g, "2.66666 66666 66666 66666 66666 66666 66666 66666 66666 66666"
       "     66666 67", large_prec);
  }
  {
    mpfr_class f(3.0, small_prec);
    mpq_class q(1, 3);
    mpfr_class g(0.0, very_large_prec);
    g = mpfr_class(f - q);
    ASSERT_ALWAYS_PREC2
      (g, "2.66666 66666 66666 66666 66666 66666 66666 667", medium_prec);
  }
  {
    mpfr_class f(15.0, large_prec);
    mpq_class q(1, 3);
    mpfr_class g(0.0, very_large_prec);
    g = mpfr_class(f + q);
    ASSERT_ALWAYS_PREC2
      (g, "15.33333 33333 33333 33333 33333 33333 33333 33333 33333 33333"
       "      33333 33", large_prec);
  }

#endif /* WANT_MPFR */
}


int
main (void)
{
  tests_start();

  check_mpf();
  check_mpfr();

  tests_end();
  return 0;
}
