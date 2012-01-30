/* Test C++11 features

Copyright 2011 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.  */

#include "config.h"

#include "gmp.h"
#include "gmpxx.h"
#include "gmp-impl.h"
#include "tests.h"

#if __GMPXX_USE_CXX11

#include <utility>
#include <type_traits>

void check_noexcept ()
{
  mpz_class z1, z2;
  mpq_class q1, q2;
  mpf_class f1, f2;
  // gcc 4.6 is missing noexcept on std::move
  static_assert(noexcept(z1 = static_cast<mpz_class&&>(z2)), "sorry");
  static_assert(noexcept(q1 = static_cast<mpq_class&&>(q2)), "sorry");
  static_assert(noexcept(f1 = static_cast<mpf_class&&>(f2)), "sorry");
  static_assert(noexcept(q1 = static_cast<mpz_class&&>(z1)), "sorry");
}

void check_common_type ()
{
#define CHECK_COMMON_TYPE(T, U, Res) \
  static_assert(std::is_same<std::common_type<T, U>::type, Res>::value, "sorry")
  CHECK_COMMON_TYPE (mpz_class, mpq_class, mpq_class);
  CHECK_COMMON_TYPE (mpz_class, mpf_class, mpf_class);
  CHECK_COMMON_TYPE (mpf_class, mpq_class, mpf_class);

  mpz_class z; mpq_class q; mpf_class f;

  CHECK_COMMON_TYPE (decltype(-z), mpz_class, mpz_class);
  CHECK_COMMON_TYPE (decltype(-q), mpq_class, mpq_class);
  CHECK_COMMON_TYPE (decltype(-f), mpf_class, mpf_class);

  CHECK_COMMON_TYPE (decltype(-z), mpq_class, mpq_class);
  CHECK_COMMON_TYPE (decltype(-z), mpf_class, mpf_class);
  CHECK_COMMON_TYPE (decltype(-q), mpf_class, mpf_class);

  /* Not currently supported
  CHECK_COMMON_TYPE (decltype(-q), mpz_class, mpq_class);
  CHECK_COMMON_TYPE (decltype(-f), mpz_class, mpf_class);
  CHECK_COMMON_TYPE (decltype(-f), mpq_class, mpf_class);

  CHECK_COMMON_TYPE (decltype(-z), decltype(-q), mpq_class);
  CHECK_COMMON_TYPE (decltype(-z), decltype(-f), mpq_class);
  CHECK_COMMON_TYPE (decltype(-q), decltype(-f), mpq_class);
  */
}

template<class T, class U = T>
void check_move_init ()
{
  {
    // Delete moved-from x1
    T x1 = 3;
    U x2 = std::move(x1);
    ASSERT_ALWAYS (x2 == 3);
  }
  {
    // Assign to moved-from x1
    T x1 = 2;
    U x2 = std::move(x1);
    x1 = -7;
    ASSERT_ALWAYS (x1 == -7);
    ASSERT_ALWAYS (x2 == 2);
  }
}

template<class T, class U = T>
void check_move_assign ()
{
  {
    // Delete moved-from x1
    T x1 = 3; U x2;
    x2 = std::move(x1);
    ASSERT_ALWAYS (x2 == 3);
  }
  {
    // Assign to moved-from x1
    T x1 = 2; U x2;
    x2 = std::move(x1);
    x1 = -7;
    ASSERT_ALWAYS (x1 == -7);
    ASSERT_ALWAYS (x2 == 2);
  }
  {
    // Self move-assign (not necessary, but it happens to work...)
    T x = 4;
    x = std::move(x);
    ASSERT_ALWAYS (x == 4);
  }
}

int
main (void)
{
  tests_start();

  check_noexcept();
  check_common_type();
  check_move_init<mpz_class>();
  check_move_init<mpq_class>();
  check_move_init<mpf_class>();
  check_move_assign<mpz_class>();
  check_move_assign<mpq_class>();
  check_move_assign<mpf_class>();
  check_move_init<mpz_class,mpq_class>();
  check_move_assign<mpz_class,mpq_class>();

  tests_end();
  return 0;
}

#else
int main () { return 0; }
#endif
