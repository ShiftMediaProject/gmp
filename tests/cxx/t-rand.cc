/* Test mp*_class functions.

Copyright 2002 Free Software Foundation, Inc.

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

#include "gmp.h"
#include "gmpxx.h"
#include "gmp-impl.h"
#include "tests.h"

using namespace std;


void
check_mpz (void)
{
  {
    mpz_class a(0);
    unsigned long c = 0, m2exp = 8;
    gmp_randclass r(gmp_randinit_lc_2exp, a, c, m2exp);
  }

  {
    gmp_randclass r(gmp_randinit_default);
    mpz_class a(123);
    unsigned int b = 256;
    mpz_class c;
    r.seed(a);
    c = r.get_z_bits(b);
  }
  {
    gmp_randclass r(gmp_randinit_default);
    mpz_class a(256);
    unsigned long b = 123;
    mpz_class c;
    r.seed(b);
    c = r.get_z_bits(a);
  }
  {
    gmp_randclass r(gmp_randinit_default);
    mpz_class a(123), b(256);
    mpz_class c;
    r.seed(a);
    c = r.get_z_range(b);
  }
}

void
check_mpf (void)
{
  {
    unsigned long m2exp = 64;
    gmp_randclass r(gmp_randinit_lc_2exp_size, m2exp);
  }

  {
    gmp_randclass r(gmp_randinit_default);
    mpz_class a(123);
    r.seed(a);
    mpf_class b;
    b = r.get_f();
  }
  {
    gmp_randclass r(gmp_randinit_default);
    int a = 123, b = 128;
    r.seed(a);
    mpf_class c;
    c = r.get_f(b);
  }
}


int
main (void)
{
  tests_start();

  check_mpz();
  check_mpf();

  tests_end();
  return 0;
}
