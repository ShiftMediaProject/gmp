/* Test mp*_class constructors.

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
          cout << "error on " #type "_class constructor: "     \
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
  mpz_t ref;
  mpz_init(ref);

  signed short int ss = -1;
  unsigned short int us = 2;

  // mpz_class()
  { mpz_class val; CHECK_MPZ("val()", "0"); }

  // mpz_class(const mpz_class &)
  {
    mpz_class z;      // just tested, should work
    mpz_class val(z); CHECK_MPZ("val(z) [mpz_class]", "0");
  }

  // template <class T, class U> mpz_class(const __gmp_expr<T, U> &)
  // not tested here, see t-expr.cc

  // mpz_class(signed char)

  // mpz_class(unsigned char)

  // mpz_class(signed int)
  { mpz_class val(0);    CHECK_MPZ("val(0)"   , "0"   ); }
  { mpz_class val(-123); CHECK_MPZ("val(-123)", "-123"); }
  { mpz_class val(4567); CHECK_MPZ("val(4567)", "4567"); }

  // mpz_class(unsigned int)

  // mpz_class(signed short int)
  { mpz_class val(ss); CHECK_MPZ("val(ss) [signed short int]", "-1"); }

  // mpz_class(unsigned short int)
  { mpz_class val(us); CHECK_MPZ("val(us) [unsigned short int]", "2"); }

  // mpz_class(signed long int)
  {
    mpz_class val(-1234567890L);
    CHECK_MPZ("val(-1234567890L)", "-1234567890");
  }

  // mpz_class(unsigned long int)
  { mpz_class val(1UL << 30); CHECK_MPZ("val(1UL << 30)", "1073741824"); }

  // mpz_class(float)

  // mpz_class(double)
  {
    mpz_class val(3.141592653589793238);
    CHECK_MPZ("val(3.141592653589793238)", "3");
  }

  // mpz_class(long double)
  // currently not implemented

  // mpz_class(const char *)
  {
    mpz_class val("12345678901234567890");
    CHECK_MPZ("val(\"12345678901234567890\")", "12345678901234567890");
  }

  // mpz_class(const char *, int)
  { mpz_class val("FFFF", 16); CHECK_MPZ("val(\"FFFF\", 16)", "65535"); }

  // mpz_class(const std::string &)
  {
    mpz_class val(string("1234567890"));
    CHECK_MPZ("val(string(\"1234567890\"))", "1234567890");
  }

  // mpz_class(const std::string &, int)
  {
    mpz_class val(string("7777"), 8);
    CHECK_MPZ("val(string(\"7777\", 8))", "4095");
  }

  // mpz_class(mpz_srcptr)
  // ref has the value from the last test
  { mpz_class val(ref); CHECK_MPZ("val(ref) [mpz_t]", "4095"); }

  // no constructor for bool, but it gets casted to int
  { mpz_class val(true);  CHECK_MPZ("val(true)" , "1"); }
  { mpz_class val(false); CHECK_MPZ("val(false)", "0"); }

  mpz_clear(ref);
}

void check_mpq(void)
{
  mpq_t ref;
  mpq_init(ref);

  signed short int ss = -1;
  unsigned short int us = 2;
  mpz_class z(-3), w(4);

  // mpq_class()
  { mpq_class val; CHECK_MPQ("val()", "0"); }

  // mpq_class(const mpq_class &)
  {
    mpq_class q;      // just tested, should work
    mpq_class val(q); CHECK_MPQ("val(q) [mpq_class]", "0");
  }

  // template <class T, class U> mpq_class(const __gmp_expr<T, U> &)
  // not tested here, see t-expr.cc

  // mpq_class(signed char)

  // mpq_class(unsigned char)

  // mpq_class(signed int)
  { mpq_class val(0);    CHECK_MPQ("val(0)"   , "0"   ); }
  { mpq_class val(-123); CHECK_MPQ("val(-123)", "-123"); }
  { mpq_class val(4567); CHECK_MPQ("val(4567)", "4567"); }

  // mpq_class(unsigned int)

  // mpq_class(signed short int)
  { mpq_class val(ss); CHECK_MPQ("val(us) [signed short int]", "-1"); }

  // mpq_class(unsigned short int)
  { mpq_class val(us); CHECK_MPQ("val(us) [unsigned short int]", "2"); }

  // mpq_class(signed long int)
  {
    mpq_class val(-1234567890L);
    CHECK_MPQ("val(-1234567890L)", "-1234567890");
  }

  // mpq_class(unsigned long int)
  { mpq_class val(1UL << 30); CHECK_MPQ("val(1UL << 30)", "1073741824"); }

  // mpq_class(float)

  // mpq_class(double)
  { mpq_class val(1.25); CHECK_MPQ("val(1.25)", "5/4"); }

  // mpq_class(long double)
  // currently not implemented

  // mpq_class(const char *)
  {
    mpq_class val("12345678901234567890");
    CHECK_MPQ("val(\"12345678901234567890\")", "12345678901234567890");
  }

  // mpq_class(const char *, int)
  { mpq_class val("FFFF", 16); CHECK_MPQ("val(\"FFFF\", 16)", "65535"); }

  // mpq_class(const std::string &)
  {
    mpq_class val(string("1234567890"));
    CHECK_MPQ("val(string(\"1234567890\"))", "1234567890");
  }

  // mpq_class(const std::string &s, int)
  {
    mpq_class val(string("7777"), 8);
    CHECK_MPQ("val(string(\"7777\"), 8)", "4095");
  }

  // mpq_class(mpq_srcptr)
  { mpq_class val(ref); CHECK_MPQ("val(ref) [mpq_t]", "4095"); }

  // mpq_class(const mpz_class &, const mpz_class &)
  { mpq_class val(z, w); CHECK_MPQ("val(z, w) [z, w = mpz_class]" , "-3/4"); }
  { mpq_class val(1, 2); CHECK_MPQ("val(1, 2)"                    , "1/2" ); }
  { mpq_class val(5, w); CHECK_MPQ("val(5, w) [w = mpz_class]"    , "5/4" ); }

  // template <class T, class U> mpq_class(const __gmp_expr<__gmpz_value, T> &, const __gmp_expr<__gmpz_value, U> &);

  // no constructor for bool, but it gets casted to int
  { mpq_class val(true);  CHECK_MPQ("val(true)" , "1"); }
  { mpq_class val(false); CHECK_MPQ("val(false)", "0"); }

  mpq_clear(ref);
}

void check_mpf(void)
{
  mpf_t ref;
  mpf_init(ref);

  signed short int ss = -1;
  unsigned short int us = 2;

  // mpf_class()
  { mpf_class val; CHECK_MPF("val()", "0.0"); }

  // mpf_class(const mpf_class &)
  {
    mpf_class f;      // just tested, should work
    mpf_class val(f); CHECK_MPF("val(f) [mpf_class]", "0.0");
  }

  // mpf_class(const mpf_class &, unsigned long int)
  {
    mpf_class f;           // just tested, should work
    mpf_class val(f, 128); CHECK_MPF("val(f, 128) [f = mpf_class]", "0.0");
  }

  // template <class T, class U> mpf_class(const __gmp_expr<T, U> &)
  // template <class T, class U> mpf_class(const __gmp_expr<T, U> &,
  //                                       unsigned long int)
  // not tested here, see t-expr.cc

  // mpf_class(signed char)
  // mpf_class(signed char, unsigned long int)
  // mpf_class(unsigned char)
  // mpf_class(unsigned char, unsigned long int)

  // mpf_class(signed int)
  { mpf_class val(0);    CHECK_MPF("val(0)"   , "0.0"   ); }
  { mpf_class val(-123); CHECK_MPF("val(-123)", "-123.0"); }
  { mpf_class val(4567); CHECK_MPF("val(4567)", "4567.0"); }

  // mpf_class(signed int, unsigned long int)
  { mpf_class val(-1, 192); CHECK_MPF("val(-1, 192)", "-1.0"); }

  // mpf_class(unsigned int)
  // mpf_class(unsigned int, unsigned long int)

  // mpf_class(signed short int)
  { mpf_class val(ss); CHECK_MPF("val(ss) [signed short int]", "-1.0"); }

  // mpf_class(signed short int, unsigned long int)
  {
    mpf_class val(ss, 64);
    CHECK_MPF("val(ss [signed short int], 64)", "-1.0");
  }

  // mpf_class(unsigned short int)
  { mpf_class val(us); CHECK_MPF("val(us) [unsigned short int]", "2.0"); }

  // mpf_class(unsigned short int, unsigned long int)
  {
    mpf_class val(us, 96);
    CHECK_MPF("val(us [unsigned short int], 96)", "2.0");
  }

  // mpf_class(signed long int)
  {
    mpf_class val(-1234567890L);
    CHECK_MPF("val(-1234567890L)", "-1234567890.0");
  }

  // mpf_class(signed long int, unsigned long int)
  {
    mpf_class val(-1234567890L, 64u);
    CHECK_MPF("val(-1234567890L, 64u)", "-1234567890.0");
  }

  // mpf_class(unsigned long int)
  // mpf_class(unsigned long int, unsigned long int)
  // mpf_class(float)
  // mpf_class(float, unsigned long int)

  // mpf_class(double)
  { mpf_class val(12345.0);   CHECK_MPF("val(12345.0)"  , "12345.0"); }
  { mpf_class val(1.2345e+4); CHECK_MPF("val(1.2345e+4)", "12345.0"); }
  { mpf_class val(312.5e-2);  CHECK_MPF("val(312.5e-2)" , "3.125"  ); }

  // mpf_class(double, unsigned long int)
  {
    mpf_class val(3.125e+4, 128u);
    CHECK_MPF("val(3.125e+4, 128u)", "31250.0");
  }

  // mpf_class(long double)
  // mpf_class(long double, unsigned long int)
  // currently not implemented

  // mpf_class(const char *)
  {
    mpf_class val("12345678901234567890");
    CHECK_MPF("val(\"12345678901234567890\")", "12345678901234567890.0");
  }

  // mpf_class(const char *, unsigned long int, int = 0)
  {
    mpf_class val("123456789012345", 192u);
    CHECK_MPF("val(\"123456789012345\", 192u)", "123456789012345.0");
  }

  // mpf_class(const std::string &)
  {
    mpf_class val(string("1234567890"));
    CHECK_MPF("val(string(\"1234567890\"))", "1234567890.0");
  }

  // mpf_class(const std::string &, unsigned long int, int = 0)
  {
    mpf_class val(string("1234567890", 256u));
    CHECK_MPF("val(string(\"1234567890\", 256u))", "1234567890.0");
  }

  // mpf_class(mpf_srcptr)
  { mpf_class val(ref); CHECK_MPF("val(ref) [mpf_t]", "1234567890.0"); }

  // mpf_class(mpf_srcptr, unsigned long int)
  {
    mpf_class val(ref, 64L);
    CHECK_MPF("val(ref, 64L) [ref = mpf_t]", "1234567890.0");
  }

  // no constructors for bool, but it gets casted to int
  { mpf_class val(true);      CHECK_MPF("val(true)"     , "1.0"); }
  { mpf_class val(false);     CHECK_MPF("val(false)"    , "0.0"); }
  { mpf_class val(true, 128); CHECK_MPF("val(true, 128)", "1.0"); }
  { mpf_class val(false, 64); CHECK_MPF("val(false, 64)", "0.0"); }

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
