/* Test mp*_class expression templates.

Copyright 2001, 2002 Free Software Foundation, Inc.

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

#include <cstdlib>
#include <iostream>
#include <string>
#include "gmp.h"
#include "gmpxx.h"
#include "gmp-impl.h"
#include "tests.h"

using namespace std;


#define CHECK_GMP(type, message, want)                         \
  do                                                           \
    {                                                          \
      type##_set_str(ref, want, 0);                            \
      if (type##_cmp(val.get_##type##_t(), ref) != 0)          \
        {                                                      \
          cout << "error on " #type "_class expression: "      \
	       << message << "\n";                             \
          cout << "  want:  " << ref << "\n";                  \
          cout << "  got:   " << val.get_##type##_t() << "\n"; \
          abort();                                             \
        }                                                      \
    }                                                          \
  while (0)

#define CHECK_MPZ(expr, want) CHECK_GMP(mpz, expr, want)
#define CHECK_MPQ(expr, want) CHECK_GMP(mpq, expr, want)
#define CHECK_MPF(expr, want) CHECK_GMP(mpf, expr, want)


void check_mpz(void)
{
  mpz_class val;
  mpz_t ref;
  mpz_init(ref);

  mpz_class z(1), w(2), v(3);

  // unary expressions

  // template<class Op>
  // __gmp_expr<__gmpz_value, __gmp_unary_expr<mpz_class, Op> >
  val = +z; CHECK_MPZ("val = +z", "1");
  val = -w; CHECK_MPZ("val = -w", "-2");
  val = ~v; CHECK_MPZ("val = ~v", "-4");

  // template <class T, class U, class Op>
  // __gmp_expr<__gmpz_value, __gmp_unary_expr<__gmp_expr<T, U>, Op> >
  val = -(-z); CHECK_MPZ("val = -(-z)", "1");

  // binary expressions

  // template<class Op>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr<mpz_class, mpz_class, Op> >
  val = z + w; CHECK_MPZ("val = z + w", "3");
  val = z * v; CHECK_MPZ("val = z * v", "3");
  val = v % w; CHECK_MPZ("val = v * w", "1");

  // template<class T, class Op>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr<mpz_class, T, Op> >
  val = z - 3;   CHECK_MPZ("val = z - 3"  , "-2");
  val = w / 2u;  CHECK_MPZ("val = w / 2u" , "1" );
  val = v + 4.0; CHECK_MPZ("val = v + 4.0", "7" );

  // template<class T, class Op>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr<T, mpz_class, Op> >
  val = 3 / z; CHECK_MPZ("val = 3 / z", "3");

  // template<class T, class U, class Op>
  // __gmp_expr
  // <__gmpz_value, __gmp_binary_expr<mpz_class, __gmp_expr<T, U>, Op>
  val = z * (-w); CHECK_MPZ("val = z * (-w)", "-2");

  // template<class T, class U, class Op>
  // __gmp_expr
  // <__gmpz_value, __gmp_binary_expr<__gmp_expr<T, U>, mpz_class, Op>
  val = (z % w) + v; CHECK_MPZ("val = (z % w) + v", "4");

  // template<class T, class U, class V, class Op>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr<__gmp_expr<T, U>, V, Op>
  val = (-z) << 2; CHECK_MPZ("val = (-z) << 2", "-4");

  // template<class T, class U, class V, class Op>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr<T, __gmp_expr<U, V>, Op>
  val = 6.0 / (z + w); CHECK_MPZ("val = 6.0 / (z + w)", "2");

  // template<class T, class U, class V, class W, class Op>
  // __gmp_expr
  // <__gmpz_value, __gmp_binary_expr<__gmp_expr<T, U>, __gmp_expr<V, W>, Op>
  val = (z - w) * (-v); CHECK_MPZ("val = (z - w) * (-v)", "3");

  // ternary expressions

  // template<class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr<mpz_class, __gmp_expr
  // <__gmpz_value, __gmp_binary_expr<mpz_class, mpz_class, Op1> >, Op2> >
  val = z + w * v; CHECK_MPZ("val = z + w * v", "7" );
  val = z - w * v; CHECK_MPZ("val = z - w * v", "-5");

  // template <class T, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr<mpz_class, __gmp_expr
  // <__gmpz_value, __gmp_binary_expr<mpz_class, T, Op1> >, Op2> >
  val = z + w * 3; CHECK_MPZ("val = z + w * 3", "7" );
  val = z - w * 3; CHECK_MPZ("val = z - w * 3", "-5");

  // template <class T, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr<mpz_class, __gmp_expr
  // <__gmpz_value, __gmp_binary_expr<T, mpz_class, Op1> >, Op2> >
  val = z + 2 * v; CHECK_MPZ("val = z + 2 * v", "7" );
  val = z - 2 * v; CHECK_MPZ("val = z - 2 * v", "-5");

  // template <class T, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr
  // <mpz_class, __gmp_expr<__gmpz_value, __gmp_binary_expr
  // <mpz_class, __gmp_expr<__gmpz_value, T>, Op1> >, Op2> >
  val = z + w * (v + 4); CHECK_MPZ("val = z + w * (v + 4)", "15" );
  val = z - w * (v + 4); CHECK_MPZ("val = z - w * (v + 4)", "-13");

  // template <class T, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr
  // <mpz_class, __gmp_expr<__gmpz_value, __gmp_binary_expr
  // <__gmp_expr<__gmpz_value, T>, mpz_class, Op1> >, Op2> >
  val = z + (w - 5) * v; CHECK_MPZ("val = z + (w - 5) * v", "-8");
  val = z - (w - 5) * v; CHECK_MPZ("val = z - (w - 5) * v", "10");

  // template <class T, class U, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr
  // <mpz_class, __gmp_expr<__gmpz_value, __gmp_binary_expr
  // <__gmp_expr<__gmpz_value, T>, U, Op1> >, Op2> >
  val = z + (w + 1) * 2; CHECK_MPZ("val = z + (w + 1) * 2", "7" );
  val = z - (w + 1) * 2; CHECK_MPZ("val = z - (w + 1) * 2", "-5");

  // template <class T, class U, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr
  // <mpz_class, __gmp_expr<__gmpz_value, __gmp_binary_expr
  // <T, __gmp_expr<__gmpz_value, U>, Op1> >, Op2> >
  val = z + 2 * (v + 3); CHECK_MPZ("val = z + 2 * (v + 3)", "13" );
  val = z - 2 * (v + 3); CHECK_MPZ("val = z - 2 * (v + 3)", "-11");

  // template <class T, class U, class Op1, class Op2>
  // class __gmp_expr<__gmpz_value, __gmp_binary_expr
  // <mpz_class, __gmp_expr<__gmpz_value, __gmp_binary_expr
  // <__gmp_expr<__gmpz_value, T>, __gmp_expr<__gmpz_value, U>, Op1> >, Op2> >
  val = z + (w - 1) * (v + 2); CHECK_MPZ("val = z + (w - 1) * (v + 2)", "6" );
  val = z - (w - 1) * (v + 2); CHECK_MPZ("val = z - (w - 1) * (v + 2)", "-4");

  // template <class T, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr
  // <__gmp_expr<__gmpz_value, T>, __gmp_expr
  // <__gmpz_value, __gmp_binary_expr<mpz_class, mpz_class, Op1> >, Op2> >
  val = (z + 3) + w * v; CHECK_MPZ("val = (z + 3) + w * v", "10");
  val = (z + 3) - w * v; CHECK_MPZ("val = (z + 3) - w * v", "-2");

  // template <class T, class U, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr
  // <__gmp_expr<__gmpz_value, T>, __gmp_expr
  // <__gmpz_value, __gmp_binary_expr<mpz_class, U, Op1> >, Op2> >
  val = (z - 2) + w * 4; CHECK_MPZ("val = (z - 2) + w * 4", "7" );
  val = (z - 2) - w * 4; CHECK_MPZ("val = (z - 2) - w * 4", "-9");

  // template <class T, class U, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr
  // <__gmp_expr<__gmpz_value, T>, __gmp_expr
  // <__gmpz_value, __gmp_binary_expr<U, mpz_class, Op1> >, Op2> >
  val = (z + 1) + 2 * v; CHECK_MPZ("val = (z + 1) + 2 * v", "8" );
  val = (z + 1) - 2 * v; CHECK_MPZ("val = (z + 1) - 2 * v", "-4");

  // template <class T, class U, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr<__gmp_expr<__gmpz_value, T>,
  // __gmp_expr<__gmpz_value, __gmp_binary_expr
  // <mpz_class, __gmp_expr<__gmpz_value, U>, Op1> >, Op2> >
  val = (z + 2) + w * (v - 2); CHECK_MPZ("val = (z + 2) + w * (v - 2)", "5");
  val = (z + 2) - w * (v - 2); CHECK_MPZ("val = (z + 2) - w * (v - 2)", "1");

  // template <class T, class U, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr<__gmp_expr<__gmpz_value, T>,
  // __gmp_expr<__gmpz_value, __gmp_binary_expr
  // <__gmp_expr<__gmpz_value, U>, mpz_class, Op1> >, Op2> >
  val = (z - 3) + (w + 3) * v;
  CHECK_MPZ("val = (z - 3) + (w + 3) * v", "13" );
  val = (z - 3) - (w + 3) * v;
  CHECK_MPZ("val = (z - 3) - (w + 3) * v", "-17");

  // template <class T, class U, class V, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr<__gmp_expr<__gmpz_value, T>,
  // __gmp_expr<__gmpz_value, __gmp_binary_expr
  // <__gmp_expr<__gmpz_value, U>, V, Op1> >, Op2> >
  val = (z + 1) + (w + 2) * 3;
  CHECK_MPZ("val = (z + 1) + (w + 2) * 3", "14" );
  val = (z + 1) - (w + 2) * 3;
  CHECK_MPZ("val = (z + 1) - (w + 2) * 3", "-10");

  // template <class T, class U, class V, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr<__gmp_expr<__gmpz_value, T>,
  // __gmp_expr<__gmpz_value, __gmp_binary_expr
  // <U, __gmp_expr<__gmpz_value, V>, Op1> >, Op2> >
  val = (z - 2) + 3 * (v - 4); CHECK_MPZ("val = (z - 2) + 3 * (v - 4)", "-4");
  val = (z - 2) - 3 * (v - 4); CHECK_MPZ("val = (z - 2) - 3 * (v - 4)", "2" );

  // template <class T, class U, class V, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr<__gmp_expr<__gmpz_value, T>,
  // __gmp_expr<__gmpz_value, __gmp_binary_expr
  // <__gmp_expr<__gmpz_value, U>, __gmp_expr<__gmpz_value, V>, Op1> >, Op2> >
  val = (z + 2) + (w - 3) * (v + 4);
  CHECK_MPZ("val = (z + 2) + (w - 3) * (v + 4)", "-4");
  val = (z + 2) - (w - 3) * (v + 4);
  CHECK_MPZ("val = (z + 2) - (w - 3) * (v + 4)", "10");

  // template <class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr<__gmp_expr<__gmpz_value,
  // __gmp_binary_expr<mpz_class, mpz_class, Op1> >, mpz_class, Op2> >
  val = z * w + v; CHECK_MPZ("val = z * w + v", "5" );
  val = z * w - v; CHECK_MPZ("val = z * w - v", "-1");

  // template <class T, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr<__gmp_expr<__gmpz_value,
  // __gmp_binary_expr<mpz_class, T, Op1> >, mpz_class, Op2> >
  val = z * 2 + v; CHECK_MPZ("val = z * 2 + v", "5" );
  val = z * 2 - v; CHECK_MPZ("val = z * 2 - v", "-1");

  // template <class T, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr<__gmp_expr<__gmpz_value,
  // __gmp_binary_expr<T, mpz_class, Op1> >, mpz_class, Op2> >
  val = z * w + 3; CHECK_MPZ("val = z * w + 3", "5" );
  val = z * w - 3; CHECK_MPZ("val = z * w - 3", "-1");

  // template <class T, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr
  // <__gmp_expr<__gmpz_value, __gmp_binary_expr
  // <mpz_class, __gmp_expr<__gmpz_value, T>, Op1> >, mpz_class, Op2> >
  val = z * (w + 4) + v; CHECK_MPZ("val = z * (w + 4) + v", "9");
  val = z * (w + 4) - v; CHECK_MPZ("val = z * (w + 4) - v", "3");

  // template <class T, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr
  // <__gmp_expr<__gmpz_value, __gmp_binary_expr
  // <__gmp_expr<__gmpz_value, T>, mpz_class, Op1> >, mpz_class, Op2> >
  val = (z - 5) * w + v; CHECK_MPZ("val = (z - 5) * w + v", "-5" );
  val = (z - 5) * w - v; CHECK_MPZ("val = (z - 5) * w - v", "-11");

  // template <class T, class U, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr
  // <__gmp_expr<__gmpz_value, __gmp_binary_expr
  // <__gmp_expr<__gmpz_value, T>, U, Op1> >, mpz_class, Op2> >
  val = (z + 1) * 2 + v; CHECK_MPZ("val = (z + 1) * 2 + v", "7");
  val = (z + 1) * 2 - v; CHECK_MPZ("val = (z + 1) * 2 - v", "1");

  // template <class T, class U, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr
  // <__gmp_expr<__gmpz_value, __gmp_binary_expr
  // <T, __gmp_expr<__gmpz_value, U>, Op1> >, mpz_class, Op2> >
  val = 3 * (w - 4) + v; CHECK_MPZ("val = 3 * (w - 4) + v", "-3");
  val = 3 * (w - 4) - v; CHECK_MPZ("val = 3 * (w - 4) - v", "-9");

  // template <class T, class U, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr
  // <__gmp_expr<__gmpz_value, __gmp_binary_expr<__gmp_expr<__gmpz_value, T>,
  // __gmp_expr<__gmpz_value, U>, Op1> >, mpz_class, Op2> >
  val = (z - 2) * (w + 2) + v; CHECK_MPZ("val = (z - 2) * (w + 2) + v", "-1");
  val = (z - 2) * (w + 2) - v; CHECK_MPZ("val = (z - 2) * (w + 2) - v", "-7");

  // template <class T, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr
  // <__gmp_expr<__gmpz_value, __gmp_binary_expr
  // <mpz_class, mpz_class, Op1> >, __gmp_expr<__gmpz_value, T>, Op2> >
  val = z * w + (v - 4); CHECK_MPZ("val = z * w + (v - 4)", "1");
  val = z * w - (v - 4); CHECK_MPZ("val = z * w - (v - 4)", "3");

  // template <class T, class U, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr
  // <__gmp_expr<__gmpz_value, __gmp_binary_expr
  // <mpz_class, T, Op1> >, __gmp_expr<__gmpz_value, U>, Op2> >
  val = z * 2 + (v + 3); CHECK_MPZ("val = z * 2 + (v + 3)", "8" );
  val = z * 2 - (v + 3); CHECK_MPZ("val = z * 2 - (v + 3)", "-4");

  // template <class T, class U, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr
  // <__gmp_expr<__gmpz_value, __gmp_binary_expr
  // <T, mpz_class, Op1> >, __gmp_expr<__gmpz_value, U>, Op2> >
  val = 4 * w + (v - 5); CHECK_MPZ("val = 4 * w + (v - 5)", "6" );
  val = 4 * w - (v - 5); CHECK_MPZ("val = 4 * w - (v - 5)", "10");

  // template <class T, class U, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr<__gmp_expr<__gmpz_value,
  // __gmp_binary_expr<mpz_class, __gmp_expr<__gmpz_value, T>, Op1> >,
  // __gmp_expr<__gmpz_value, U>, Op2> >
  val = z * (w - 1) + (v + 1); CHECK_MPZ("val = z * (w - 1) + (v + 1)", "5" );
  val = z * (w - 1) - (v + 1); CHECK_MPZ("val = z * (w - 1) - (v + 1)", "-3");

  // template <class T, class U, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr<__gmp_expr<__gmpz_value,
  // __gmp_binary_expr<__gmp_expr<__gmpz_value, T>, mpz_class, Op1> >,
  // __gmp_expr<__gmpz_value, U>, Op2> >
  val = (z + 2) * w + (v - 2); CHECK_MPZ("val = (z + 2) * w + (v - 2)", "7");
  val = (z + 2) * w - (v - 2); CHECK_MPZ("val = (z + 2) * w - (v - 2)", "5");

  // template <class T, class U, class V, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr<__gmp_expr<__gmpz_value,
  // __gmp_binary_expr<__gmp_expr<__gmpz_value, T>, U, Op1> >,
  // __gmp_expr<__gmpz_value, V>, Op2> >
  val = (z + 1) * 2 + (v + 3); CHECK_MPZ("val = (z + 1) * 2 + (v + 3)", "10");
  val = (z + 1) * 2 - (v + 3); CHECK_MPZ("val = (z + 1) * 2 - (v + 3)", "-2");

  // template <class T, class U, class V, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr<__gmp_expr<__gmpz_value,
  // __gmp_binary_expr<T, __gmp_expr<__gmpz_value, U>, Op1> >,
  // __gmp_expr<__gmpz_value, V>, Op2> >
  val = 3 * (w + 2) + (v - 1); CHECK_MPZ("val = 3 * (w + 2) + (v - 1)", "14");
  val = 3 * (w + 2) - (v - 1); CHECK_MPZ("val = 3 * (w + 2) - (v - 1)", "10");

  // template <class T, class U, class V, class Op1, class Op2>
  // __gmp_expr<__gmpz_value, __gmp_binary_expr<__gmp_expr<__gmpz_value,
  // __gmp_binary_expr<__gmp_expr<__gmpz_value, T>,
  // __gmp_expr<__gmpz_value, U>, Op1> >, __gmp_expr<__gmpz_value, V>, Op2> >
  val = (z + 2) * (w - 3) + (v + 4);
  CHECK_MPZ("val = (z + 2) * (w - 3) + (v + 4)", "4"  );
  val = (z + 2) * (w - 3) - (v + 4);
  CHECK_MPZ("val = (z + 2) * (w - 3) - (v + 4)", "-10");

  mpz_clear(ref);
}

void check_mpq(void)
{
  mpq_class val;
  mpq_t ref;
  mpq_init(ref);

  mpq_class q(1, 2), r(3, 4), s(5, 6);

  // unary expressions

  // template<class Op>
  // __gmp_expr<__gmpq_value, __gmp_unary_expr<mpq_class, Op> >
  val = -q; CHECK_MPQ("val = -q", "-1/2");

  // template <class T, class U, class Op>
  // __gmp_expr<__gmpq_value, __gmp_unary_expr<__gmp_expr<T, U>, Op> >
  val = -(-q); CHECK_MPQ("val = -(-q)", "1/2");

  // binary expressions

  // template<class Op>
  // __gmp_expr<__gmpq_value, __gmp_binary_expr<mpq_class, mpq_class, Op> >
  val = q + r; CHECK_MPQ("val = q + r", "5/4");

  // template<class T, class Op>
  // __gmp_expr<__gmpq_value, __gmp_binary_expr<mpq_class, T, Op> >
  val = q - 2; CHECK_MPQ("val = q - 2", "-3/2");

  // template<class T, class Op>
  // __gmp_expr<__gmpq_value, __gmp_binary_expr<T, mpq_class, Op> >
  val = 3 / q; CHECK_MPQ("val = 3 / q", "6");

  // template<class T, class U, class Op>
  // __gmp_expr
  // <__gmpq_value, __gmp_binary_expr<mpq_class, __gmp_expr<T, U>, Op>
  val = q * (-r); CHECK_MPQ("val = q * (-r)", "-3/8");

  // template<class T, class U, class Op>
  // __gmp_expr<__gmpq_value,
  // __gmp_binary_expr<__gmp_expr<T, U>, mpq_class, Op>
  val = (q / r) + s; CHECK_MPQ("val = (q / r) + s", "3/2");

  // template<class T, class U, class V, class Op>
  // __gmp_expr<__gmpq_value, __gmp_binary_expr<__gmp_expr<T, U>, V, Op>
  val = (-q) << 2; CHECK_MPQ("val = (-q) << 2", "-2");

  // template<class T, class U, class V, class Op>
  // __gmp_expr<__gmpq_value, __gmp_binary_expr<T, __gmp_expr<U, V>, Op>
  val = 6.0 / (q + r); CHECK_MPQ("val = 6.0 / (q + r)", "24/5");

  // template<class T, class U, class V, class W, class Op>
  // __gmp_expr
  // <__gmpq_value, __gmp_binary_expr<__gmp_expr<T, U>, __gmp_expr<V, W>, Op>
  val = (q - r) * (-s); CHECK_MPQ("val = (q - r) * (-s)", "5/24");

  mpq_clear(ref);
}

void check_mpf(void)
{
  mpf_class val;
  mpf_t ref;
  mpf_init(ref);

  mpf_class f(1.0), g(0.25), h(3e+2);

  // unary expressions

  // template<class Op>
  // __gmp_expr<__gmpf_value, __gmp_unary_expr<mpf_class, Op> >
  val = -f; CHECK_MPF("val = -f", "-1.0");

  // template <class T, class U, class Op>
  // __gmp_expr<__gmpf_value, __gmp_unary_expr<__gmp_expr<T, U>, Op> >
  val = -(-f); CHECK_MPF("val = -(-f)", "1.0");

  // binary expressions

  // template<class Op>
  // __gmp_expr<__gmpf_value, __gmp_binary_expr<mpf_class, mpf_class, Op> >
  val = f + g; CHECK_MPF("val = f + g", "1.25");

  // template<class T, class Op>
  // __gmp_expr<__gmpf_value, __gmp_binary_expr<mpf_class, T, Op> >
  val = f - 2; CHECK_MPF("val = f - 2", "-1.0");

  // template<class T, class Op>
  // __gmp_expr<__gmpf_value, __gmp_binary_expr<T, mpf_class, Op> >
  val = 3 / f; CHECK_MPF("val = 3 / f", "3.0");

  // template<class T, class U, class Op>
  // __gmp_expr
  // <__gmpf_value, __gmp_binary_expr<mpf_class, __gmp_expr<T, U>, Op>
  val = f * (-g); CHECK_MPF("val = f * (-g)", "-0.25");

  // template<class T, class U, class Op>
  // __gmp_expr<__gmpf_value,
  // __gmp_binary_expr<__gmp_expr<T, U>, mpf_class, Op>
  val = (f / g) + h; CHECK_MPF("val = (f / g) + h", "304.0");

  // template<class T, class U, class V, class Op>
  // __gmp_expr<__gmpf_value, __gmp_binary_expr<__gmp_expr<T, U>, V, Op>
  val = (-f) << 2; CHECK_MPF("val = (-f) << 2", "-4.0");

  // template<class T, class U, class V, class Op>
  // __gmp_expr<__gmpqfvalue, __gmp_binary_expr<T, __gmp_expr<U, V>, Op>
  val = 5.0 / (f + g); CHECK_MPF("val = 5.0 / (f + g)", "4.0");

  // template<class T, class U, class V, class W, class Op>
  // __gmp_expr
  // <__gmpf_value, __gmp_binary_expr<__gmp_expr<T, U>, __gmp_expr<V, W>, Op>
  val = (f - g) * (-h); CHECK_MPF("val = (f - g) * (-h)", "-225.0");

  mpf_clear(ref);
}


int main()
{
  tests_start();

  check_mpz();
  check_mpq();
  check_mpf();

  tests_end();
  return 0;
}
