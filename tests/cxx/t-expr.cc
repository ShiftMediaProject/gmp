/* Test mp*_class arithmetic expressions.

Copyright 2001 Free Software Foundation, Inc.

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

#include <iostream>
#include <strstream>
#include <string>
#include <cstdlib>
#include "gmp.h"
#include "gmp-impl.h"
#include "gmpxx.h"
#include "tests.h"

using namespace std;


#define CHECK_MPZ(expr, want)                                   \
  do {                                                          \
    mpz_set_str (ref, want, 0);                                 \
    if (mpz_cmp (z.get_mpz_t(), ref) != 0)                      \
      {                                                         \
        cout << "mpz_class expression wrong: " << expr << "\n"; \
        cout << "  want:  " << ref << "\n";                     \
        cout << "  got:   " << z.get_mpz_t() << "\n";           \
        abort ();                                               \
      }                                                         \
  } while (0)

#define CHECK_MPQ(expr, want)                                   \
  do {                                                          \
    mpq_set_str (ref, want, 0);                                 \
    if (mpq_cmp (q.get_mpq_t(), ref) != 0)                      \
      {                                                         \
        cout << "mpq_class expression wrong: " << expr << "\n"; \
        cout << "  want:  " << ref << "\n";                     \
        cout << "  got:   " << q.get_mpq_t() << "\n";           \
        abort ();                                               \
      }                                                         \
  } while (0)

#define CHECK_MPF(expr, want)                                    \
  do {                                                           \
    mpf_set_str (ref, want, 10);                                 \
    if (mpf_cmp (f.get_mpf_t(), ref) != 0)                       \
      {                                                          \
        cout << "mpf_class constructor wrong: " << expr << "\n"; \
        cout << "  want:  " << ref << "\n";                      \
        cout << "  got:   " << f.get_mpf_t() << "\n";            \
        abort ();                                                \
      }                                                          \
  } while (0)

void
check_mpz (void)
{
  mpz_class z, w (1), v (2), u(3);
  mpz_t ref;
  mpz_init(ref);

  // simple assignments

  // mpz_class
  z = w;
  CHECK_MPZ ("z = w", "1");

  // int
  z = -1;
  CHECK_MPZ ("z = -1", "-1");

  // unsigned long
  z = 3456789012ul;
  CHECK_MPZ ("z = 3456789012ul", "3456789012");

  // char *
  z = "12345678901234567890";
  CHECK_MPZ ("z = \"12345678901234567890\"", "12345678901234567890");

  // string
  z = string ("1234567890");
  CHECK_MPZ ("z = string (\"1234567890\")", "1234567890");

  // compound expressions

  // template<class Op>
  // __gmp_expr<__gmpz_value, __gmp_unary_expr<mpz_class, Op> >
  // [Op = __gmp_unary_minus]
  z = -w;
  CHECK_MPZ ("z = -w", "-1");

  // template<class Op>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr<mpz_class, mpz_class, Op> >
  // [Op = __gmp_binary_plus]
  z = w + v;
  CHECK_MPZ ("z = w + v", "3");

  // template<class T, class Op>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr<mpz_class, T, Op> >
  // [T = int, Op = __gmp_binary_minus]
  z = w - 2;
  CHECK_MPZ ("z = w - 2", "-1");

  // template<class T, class Op>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr<T, mpz_class, Op> >
  // [T = int, Op = __gmp_binary_divides]
  z = 3 / w;
  CHECK_MPZ ("z = 3 / w", "3");

  // template<class T, class U, class Op>
  // __gmp_expr
  // <__gmpz_value, __gmp_binary_expr<mpz_class, __gmp_expr<T, U>, Op>
  // [T = __gmpz_value, U = __gmp_unary_expr<mpz_class, __gmp_unary_minus>,
  // Op = __gmp_binary_multiplies]
  z = w * (-v);
  CHECK_MPZ ("z = w * (-v)", "-2");

  // template<class T, class U, class Op>
  // __gmp_expr<__gmpz_value,
  // __gmp_binary_expr<__gmp_expr<T, U>, mpz_class, Op>
  // [T = __gmpz_value,
  // U = __gmp_binary_expr<mpz_class, mpz_class, __gmp_binary_modulus>,
  // Op = __gmp_binary_plus]
  z = (w % v) + u;
  CHECK_MPZ ("z = (w % v) + u", "4");

  // template<class T, class U, class V, class Op>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr<__gmp_expr<T, U>, V, Op>
  // [T = __gmpz_value, U = __gmp_unary_expr<mpz_class, __gmp_unary_minus>,
  // V = int, Op = __gmp_binary_lshift]
  z = (-w) << 2;
  CHECK_MPZ ("z = (-w) << 2", "-4");

  // template<class T, class U, class V, class Op>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr<T, __gmp_expr<U, V>, Op>
  // [T = double, U = __gmpz_value,
  // V = __gmp_binary_expr<mpz_class, mpz_class, __gmp_binary_plus>,
  // Op = __gmp_binary_divides]
  z = 6.0 / (w + v);
  CHECK_MPZ ("z = 6.0 / (w + v)", "2");

  // template<class T, class U, class V, class W, class Op>
  // __gmp_expr
  // <__gmpz_value, __gmp_binary_expr<__gmp_expr<T, U>, __gmp_expr<V, W>, Op>
  // [T = __gmpz_value,
  // U = __gmp_binary_expr<mpz_class, mpz_class, __gmp_binary_minus>,
  // V = __gmpz_value, W = __gmp_unary_expr<mpz_class, __gmp_unary_minus>,
  // Op = __gmp_binary_multiplies]
  z = (w - v) * (-u);
  CHECK_MPZ ("z = (w - v) * (-u)", "3");

  mpz_clear(ref);
}

void
check_mpq (void)
{
  mpq_class q, r ("1/2"), s ("3/4"), t ("5/6");
  mpq_t ref;
  mpq_init(ref);

  // simple assignments

  // mpq_class
  q = r;
  CHECK_MPQ ("q = r", "1/2");

  // int
  q = -1;
  CHECK_MPQ ("q = -1", "-1");

  // unsigned long
  q = 3456789012ul;
  CHECK_MPQ ("q = 3456789012ul", "3456789012");

  // char *
  q = "12345678901234567890";
  CHECK_MPQ ("q = \"12345678901234567890\"", "12345678901234567890");

  // string
  q = string ("1234567890");
  CHECK_MPQ ("q = string (\"1234567890\")", "1234567890");

  // compound expressions

  // template<class Op>
  // __gmp_expr<__gmpq_value, __gmp_unary_expr<mpq_class, Op> >
  // [Op = __gmp_unary_minus]
  q = -r;
  CHECK_MPQ ("q = -r", "-1/2");

  // template<class Op>
  // __gmp_expr<__gmpq_value, __gmp_binary_expr<mpq_class, mpq_class, Op> >
  // [Op = __gmp_binary_plus]
  q = r + s;
  CHECK_MPQ ("q = r + s", "5/4");

  // template<class T, class Op>
  // __gmp_expr<__gmpq_value, __gmp_binary_expr<mpq_class, T, Op> >
  // [T = int, Op = __gmp_binary_minus]
  q = r - 2;
  CHECK_MPQ ("q = r - 2", "-3/2");

  // template<class T, class Op>
  // __gmp_expr<__gmpq_value, __gmp_binary_expr<T, mpq_class, Op> >
  // [T = int, Op = __gmp_binary_divides]
  q = 3 / r;
  CHECK_MPQ ("q = 3 / r", "6");

  // template<class T, class U, class Op>
  // __gmp_expr
  // <__gmpq_value, __gmp_binary_expr<mpq_class, __gmp_expr<T, U>, Op>
  // [T = __gmpq_value, U = __gmp_unary_expr<mpq_class, __gmp_unary_minus>,
  // Op = __gmp_binary_multiplies]
  q = r * (-s);
  CHECK_MPQ ("q = r * (-s)", "-3/8");

  // template<class T, class U, class Op>
  // __gmp_expr<__gmpq_value,
  // __gmp_binary_expr<__gmp_expr<T, U>, mpq_class, Op>
  // [T = __gmpq_value,
  // U = __gmp_binary_expr<mpq_class, mpq_class, __gmp_binary_divides>,
  // Op = __gmp_binary_plus]
  q = (r / s) + t;
  CHECK_MPQ ("q = (r / s) + t", "3/2");

  // template<class T, class U, class V, class Op>
  // __gmp_expr<__gmpq_value, __gmp_binary_expr<__gmp_expr<T, U>, V, Op>
  // [T = __gmpq_value, U = __gmp_unary_expr<mpq_class, __gmp_unary_minus>,
  // V = int, Op = __gmp_binary_lshift]
  q = (-r) << 2;
  CHECK_MPQ ("q = (-r) << 2", "-2");

  // template<class T, class U, class V, class Op>
  // __gmp_expr<__gmpq_value, __gmp_binary_expr<T, __gmp_expr<U, V>, Op>
  // [T = double, U = __gmpq_value,
  // V = __gmp_binary_expr<mpq_class, mpq_class, __gmp_binary_plus>,
  // Op = __gmp_binary_divides]
  q = 6.0 / (r + s);
  CHECK_MPQ ("q = 6.0 / (r + s)", "24/5");

  // template<class T, class U, class V, class W, class Op>
  // __gmp_expr
  // <__gmpq_value, __gmp_binary_expr<__gmp_expr<T, U>, __gmp_expr<V, W>, Op>
  // [T = __gmpq_value,
  // U = __gmp_binary_expr<mpq_class, mpq_class, __gmp_binary_minus>,
  // V = __gmpq_value, W = __gmp_unary_expr<mpq_class, __gmp_unary_minus>,
  // Op = __gmp_binary_multiplies]
  q = (r - s) * (-t);
  CHECK_MPQ ("q = (r - s) * (-t)", "5/24");

  mpq_clear(ref);
}

void
check_mpf (void)
{
  mpf_class f, g ("1.0"), h ("0.25"), i ("3e+2");
  mpf_t ref;
  mpf_init(ref);

  // simple assignments

  // mpf_class
  f = g;
  CHECK_MPF ("f = g", "1.0");

  // int
  f = -1;
  CHECK_MPF ("f = -1", "-1.0");

  // unsigned long
  f = 3456789012ul;
  CHECK_MPF ("f = 3456789012ul", "3456789012.0");

  // char *
  f = "1234567890";
  CHECK_MPF ("f = \"1234567890\"", "1234567890.0");

  // string
  f = string ("123456");
  CHECK_MPF ("f = string (\"123456\")", "123456");

  // compound expressions

  // template<class Op>
  // __gmp_expr<__gmpf_value, __gmp_unary_expr<mpf_class, Op> >
  // [Op = __gmp_unary_minus]
  f = -g;
  CHECK_MPF ("f = -g", "-1.0");

  // template<class Op>
  // __gmp_expr<__gmpf_value, __gmp_binary_expr<mpf_class, mpf_class, Op> >
  // [Op = __gmp_binary_plus]
  f = g + h;
  CHECK_MPF ("f = g + h", "1.25");

  // template<class T, class Op>
  // __gmp_expr<__gmpf_value, __gmp_binary_expr<mpf_class, T, Op> >
  // [T = int, Op = __gmp_binary_minus]
  f = g - 2;
  CHECK_MPF ("f = g - 2", "-1.0");

  // template<class T, class Op>
  // __gmp_expr<__gmpf_value, __gmp_binary_expr<T, mpf_class, Op> >
  // [T = int, Op = __gmp_binary_divides]
  f = 3 / g;
  CHECK_MPF ("f = 3 / g", "3.0");

  // template<class T, class U, class Op>
  // __gmp_expr
  // <__gmpf_value, __gmp_binary_expr<mpf_class, __gmp_expr<T, U>, Op>
  // [T = __gmpf_value, U = __gmp_unary_expr<mpf_class, __gmp_unary_minus>,
  // Op = __gmp_binary_multiplies]
  f = g * (-h);
  CHECK_MPF ("f = g * (-h)", "-0.25");

  // template<class T, class U, class Op>
  // __gmp_expr<__gmpf_value,
  // __gmp_binary_expr<__gmp_expr<T, U>, mpf_class, Op>
  // [T = __gmpf_value,
  // U = __gmp_binary_expr<mpf_class, mpf_class, __gmp_binary_divides>,
  // Op = __gmp_binary_plus]
  f = (g / h) + i;
  CHECK_MPF ("f = (g / h) + i", "304.0");

  // template<class T, class U, class V, class Op>
  // __gmp_expr<__gmpf_value, __gmp_binary_expr<__gmp_expr<T, U>, V, Op>
  // [T = __gmpf_value, U = __gmp_unary_expr<mpf_class, __gmp_unary_minus>,
  // V = int, Op = __gmp_binary_lshift]
  f = (-g) << 2;
  CHECK_MPF ("f = (-g) << 2", "-4.0");

  // template<class T, class U, class V, class Op>
  // __gmp_expr<__gmpqfvalue, __gmp_binary_expr<T, __gmp_expr<U, V>, Op>
  // [T = double, U = __gmpf_value,
  // V = __gmp_binary_expr<mpf_class, mpf_class, __gmp_binary_plus>,
  // Op = __gmp_binary_divides]
  f = 5.0 / (g + h);
  CHECK_MPF ("f = 5.0 / (g + h)", "4.0");

  // template<class T, class U, class V, class W, class Op>
  // __gmp_expr
  // <__gmpf_value, __gmp_binary_expr<__gmp_expr<T, U>, __gmp_expr<V, W>, Op>
  // [T = __gmpf_value,
  // U = __gmp_binary_expr<mpf_class, mpf_class, __gmp_binary_minus>,
  // V = __gmpf_value, W = __gmp_unary_expr<mpf_class, __gmp_unary_minus>,
  // Op = __gmp_binary_multiplies]
  f = (g - h) * (-i);
  CHECK_MPF ("f = (g - h) * (-i)", "-225.0");

  mpf_clear(ref);
}


int
main (int argc, char *argv[])
{
  tests_start ();

  check_mpz ();
  check_mpq ();
  check_mpf ();

  tests_end ();
  exit (0);
}
