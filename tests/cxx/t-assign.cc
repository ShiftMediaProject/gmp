/* Test mp*_class assignment operators.

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
          cout << "error on " #type "_class assignment: "      \
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

  mpz_class z(1);

  // operator=(const mpz_class &)
  val = z; CHECK_MPZ("val = z [mpz_class]", "1");

  // template <class T, class U> operator=(const __gmp_expr<T, U> &)
  // not tested here, see t-expr.cc

  // operator=(signed char)
  // operator=(unsigned char)

  // operator=(signed int)
  val = 0;    CHECK_MPZ("val = 0"   , "0"   );
  val = -123; CHECK_MPZ("val = -123", "-123");

  // operator=(unsigned int)
  // operator=(signed short int)
  // operator=(unsigned short int)
  // operator=(signed long int)

  // operator=(unsigned long int)
  val = 3456789012UL; CHECK_MPZ("val = 3456789012UL", "3456789012");

  // operator=(float)
  // operator=(double)

  // operator=(long double)
  // currently not implemented

  // operator=(const char *)
  val = "12345678901234567890";
  CHECK_MPZ("val = \"12345678901234567890\"", "12345678901234567890");

  // operator=(const std::string &)
  val = string("1234567890");
  CHECK_MPZ("val = string(\"1234567890\")", "1234567890");

  mpz_clear(ref);
}

void check_mpq(void)
{
  mpq_class val;
  mpq_t ref;
  mpq_init(ref);

  mpq_class q(1, 2);

  // operator=(const mpq_class &)
  val = q; CHECK_MPQ("val = q [mpq_class]", "1/2");

  // template <class T, class U> operator=(const __gmp_expr<T, U> &)
  // not tested here, see t-expr.cc

  // operator=(signed char)
  // operator=(unsigned char)

  // operator=(signed int)
  val = 0;    CHECK_MPQ("val = 0"   , "0"   );
  val = -123; CHECK_MPQ("val = -123", "-123");

  // operator=(unsigned int)
  // operator=(signed short int)
  // operator=(unsigned short int)
  // operator=(signed long int)

  // operator=(unsigned long int)
  val = 3456789012UL; CHECK_MPQ("val = 3456789012UL", "3456789012");

  // operator=(float)
  // operator=(double)
  // operator=(long double)

  // operator=(const char *)
  val = "12345678901234567890";
  CHECK_MPQ("val = \"12345678901234567890\"", "12345678901234567890");

  // operator=(const std::string &)
  val = string("1234567890");
  CHECK_MPQ("val = string(\"1234567890\")", "1234567890");

  mpq_clear(ref);
}

void check_mpf(void)
{
  mpf_class val;
  mpf_t ref;
  mpf_init(ref);

  mpf_class f("1.0");

  // operator=(const mpf_class &)
  val = f; CHECK_MPF("val = f [mpf_class]", "1.0");

  // template <class T, class U> operator=(const __gmp_expr<T, U> &)
  // operator=(signed char)
  // operator=(unsigned char)

  // operator=(signed int)
  val = 0;    CHECK_MPF("val = 0"   , "0.0"   );
  val = -123; CHECK_MPF("val = -123", "-123.0");

  // operator=(unsigned int)
  // operator=(signed short int)
  // operator=(unsigned short int)
  // operator=(signed long int)

  // operator=(unsigned long int)
  val = 3456789012UL; CHECK_MPF("val = 3456789012UL", "3456789012.0");

  // operator=(float)
  // operator=(double)
  // operator=(long double)

  // operator=(const char *)
  val = "1234567890"; CHECK_MPF("val = \"1234567890\"", "1234567890.0");

  // operator=(const std::string &)
  val = string("123456"); CHECK_MPF("val = string(\"123456\")", "123456");

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
