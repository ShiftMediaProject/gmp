/* Test mp*_class constructors.

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


#define CHECK_MPZ(args, want)                                          \
  do {                                                                 \
    mpz_set_str (ref, want, 0);                                        \
    if (mpz_cmp (z.get_mpz_t(), ref) != 0)                             \
      {                                                                \
        cout << "mpz_class constructor wrong, args: " << args << "\n"; \
        cout << "  want:  " << ref << "\n";                            \
        cout << "  got:   " << z.get_mpz_t() << "\n";                  \
        abort ();                                                      \
      }                                                                \
  } while (0)

#define CHECK_MPQ(args, want)                                          \
  do {                                                                 \
    mpq_set_str (ref, want, 0);                                        \
    if (mpq_cmp (q.get_mpq_t(), ref) != 0)                             \
      {                                                                \
        cout << "mpq_class constructor wrong, args: " << args << "\n"; \
        cout << "  want:  " << ref << "\n";                            \
        cout << "  got:   " << q.get_mpq_t() << "\n";                  \
        abort ();                                                      \
      }                                                                \
  } while (0)

#define CHECK_MPF(args, want)                                          \
  do {                                                                 \
    mpf_set_str (ref, want, 10);                                       \
    if (mpf_cmp (f.get_mpf_t(), ref) != 0)                             \
      {                                                                \
        cout << "mpf_class constructor wrong, args: " << args << "\n"; \
        cout << "  want:  " << ref << "\n";                            \
        cout << "  got:   " << f.get_mpf_t() << "\n";                  \
        abort ();                                                      \
      }                                                                \
  } while (0)

void
check_mpz (void)
{
  mpz_t ref;
  mpz_init (ref);

  { // no arguments
    mpz_class z;
    CHECK_MPZ ("none", "0");
  }

  { // argument: mpz_class
    mpz_class w;
    mpz_class z (w);
    CHECK_MPZ ("w [mpz_class]", "0");
  }

  { // argument: int
    mpz_class z (0);
    CHECK_MPZ ("0", "0");
  }

  { // argument: bool
    mpz_class z (true);
    CHECK_MPZ ("true", "1");
  }

  { // argument: unsigned short
    mpz_class z ((unsigned short) 1);
    CHECK_MPZ ("(unsigned short) 1", "1");
  }

  { // argument: long
    mpz_class z (-1234567890L);
    CHECK_MPZ ("-1234567890L", "-1234567890");
  }

  { // argument: double
    mpz_class z (3.141592653589793238);
    CHECK_MPZ ("3.141592653589793238", "3");
  }

  { // argument: char *
    mpz_class z ("12345678901234567890");
    CHECK_MPZ ("\"12345678901234567890\"", "12345678901234567890");
  }

  { // arguments: char *, int
    mpz_class z ("FFFF", 16);
    CHECK_MPZ ("\"FFFF\", 16", "65535");
  }

  { // argument: string
    mpz_class z (string ("1234567890"));
    CHECK_MPZ ("string (\"1234567890\")", "1234567890");
  }

  { // arguments: string, int
    mpz_class z (string ("7777"), 8);
    CHECK_MPZ ("string (\"7777\", 8)", "4095");
  }

  { // argument: mpz_t
    mpz_class z (ref);
    CHECK_MPZ ("ref [mpz_t]", "4095");
  }

  mpz_clear (ref);
}

void
check_mpq (void)
{
  mpq_t ref;
  mpq_init (ref);

  { // no arguments
    mpq_class q;
    CHECK_MPQ ("none", "0");
  }

  { // argument: mpq_class
    mpq_class r;
    mpq_class q (r);
    CHECK_MPQ ("r [mpq_class]", "0");
  }

  { // argument: int
    mpq_class q (0);
    CHECK_MPQ ("0", "0");
  }

  { // argument: bool
    mpq_class q (true);
    CHECK_MPQ ("true", "1");
  }

  { // argument: unsigned short
    mpq_class q ((unsigned short) 1);
    CHECK_MPQ ("(unsigned short) 1", "1");
  }

  { // argument: long
    mpq_class q (-1234567890L);
    CHECK_MPQ ("-1234567890L", "-1234567890");
  }

  { // argument: double
    mpq_class q (1.25);
    CHECK_MPQ ("1.25", "5/4");
  }

  { // argument: char *
    mpq_class q ("12345678901234567890");
    CHECK_MPQ ("\"12345678901234567890\"", "12345678901234567890");
  }

  { // arguments: char *, int
    mpq_class q ("FFFF", 16);
    CHECK_MPQ ("\"FFFF\", 16", "65535");
  }

  { // argument: string
    mpq_class q (string ("1234567890"));
    CHECK_MPQ ("string (\"1234567890\")", "1234567890");
  }

  { // arguments: string, int
    mpq_class q (string ("7777"), 8);
    CHECK_MPQ ("string (\"7777\", 8)", "4095");
  }

  { // argument: mpz_t
    mpq_class q (ref);
    CHECK_MPQ ("ref [mpq_t]", "4095");
  }

  { // arguments: int, int
    mpq_class q (1, 2);
    CHECK_MPQ ("1, 2", "1/2");
  }

  { // arguments: mpz_class, mpz_class
    mpz_class z (3), w (4);
    mpq_class q (z, w);
    CHECK_MPQ ("z, w [mpz_class, mpz_class]", "3/4");
  }

  { // arguments: int, mpz_class
    mpz_class z (5);
    mpq_class q (6, z);
    CHECK_MPQ ("11, z [mpz_class]", "6/5");
  }

  mpq_clear (ref);
}

void
check_mpf (void)
{
  mpf_t ref;
  mpf_init (ref);

  { // no arguments
    mpf_class f;
    CHECK_MPF ("none", "0.0");
  }

  { // argument: mpf_class
    mpf_class g;
    mpf_class f (g);
    CHECK_MPF ("g [mpf_class]", "0.0");
  }

  { // argument: int
    mpf_class f (0);
    CHECK_MPF ("0", "0.0");
  }

  { // argument: bool
    mpf_class f (true);
    CHECK_MPF ("true", "1.0");
  }

  { // argument: unsigned short
    mpf_class f ((unsigned short) 1);
    CHECK_MPF ("(unsigned short) 1", "1.0");
  }

  { // argument: long
    mpf_class f (-1234567890L);
    CHECK_MPF ("-1234567890L", "-1234567890.0");
  }

  { // argument: double
    mpf_class f (3.125e+4);
    CHECK_MPF ("3.125e+4", "31250.0");
  }

  { // argument: char *
    mpf_class f ("12345678901234567890");
    CHECK_MPF ("\"12345678901234567890\"", "12345678901234567890.0");
  }

  { // argument: string
    mpf_class f (string ("1234567890"));
    CHECK_MPF ("string (\"1234567890\")", "1234567890.0");
  }

  { // argument: mpf_t
    mpf_class f (ref);
    CHECK_MPF ("ref [mpf_t]", "1234567890.0");
  }

  { // arguments: mpf_class, int
    mpf_class g;
    mpf_class f (g, 64);
    CHECK_MPF ("g [mpf_class], 64", "0.0");
  }

  { // arguments: int, int
    mpf_class f (0, 128);
    CHECK_MPF ("0, 128", "0.0");
  }

  { // arguments: bool, int
    mpf_class f (true, 192);
    CHECK_MPF ("true, 192", "1.0");
  }

  { // arguments: unsigned short, int
    mpf_class f ((unsigned short) 1, 256);
    CHECK_MPF ("(unsigned short) 1, 256", "1.0");
  }

  { // arguments: long, unsigned
    mpf_class f (-1234567890L, 64u);
    CHECK_MPF ("-1234567890L, 64u", "-1234567890.0");
  }

  { // argument: double, unsigned
    mpf_class f (3.125e+4, 128u);
    CHECK_MPF ("3.125e+4, 128u", "31250.0");
  }

  { // argument: char *, unsigned
    mpf_class f ("12345678901234567890", 192u);
    CHECK_MPF ("\"12345678901234567890\", 192u", "12345678901234567890.0");
  }

  { // argument: string, unsigned
    mpf_class f (string ("1234567890", 256u));
    CHECK_MPF ("string (\"1234567890\", 256u)", "1234567890.0");
  }

  { // argument: mpf_t, long
    mpf_class f (ref, 64L);
    CHECK_MPF ("ref [mpf_t], 64L", "1234567890.0");
  }

  mpf_clear (ref);
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
