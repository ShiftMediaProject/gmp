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

#include <limits.h>
#include <string>

#include "gmp.h"
#include "gmp-impl.h"
#include "gmpxx.h"
#include "tests.h"

using namespace std;


void
check_mpz (void)
{
  // abs
  {
    mpz_class  a (-123);
    mpz_class  b = abs(a);
    ASSERT_ALWAYS (b == 123);
  }

  // cmp
  {
    mpz_class a(123), b(45);
    int  c;
    c = cmp (a, b); ASSERT_ALWAYS (c > 0);
    c = cmp (b, a); ASSERT_ALWAYS (c < 0);
  }
#if 0
  {
    mpz_class a(123);
    unsigned long b = 45;
    int  c;
    c = cmp (a, b); ASSERT_ALWAYS (c > 0);
    c = cmp (b, a); ASSERT_ALWAYS (c < 0);
  }
  {
    mpz_class a(123);
    long b = 45;
    int  c;
    c = cmp (a, b); ASSERT_ALWAYS (c > 0);
    c = cmp (b, a); ASSERT_ALWAYS (c < 0);
  }
  {
    mpz_class a(123);
    double b = 45;
    int  c;
    c = cmp (a, b); ASSERT_ALWAYS (c > 0);
    c = cmp (b, a); ASSERT_ALWAYS (c < 0);
  }
#endif

  // mpz_class::fits_sint_p
  {
    int        fits;
    mpz_class  z;
    z = INT_MIN; fits = z.fits_sint_p(); ASSERT_ALWAYS (fits);
    z--;         fits = z.fits_sint_p(); ASSERT_ALWAYS (! fits);
    z = INT_MAX; fits = z.fits_sint_p(); ASSERT_ALWAYS (fits);
    z++;         fits = z.fits_sint_p(); ASSERT_ALWAYS (! fits);
  }

  // mpz_class::fits_uint_p
  {
    int        fits;
    mpz_class  z;
    z = 0;        fits = z.fits_uint_p(); ASSERT_ALWAYS (fits);
    z--;          fits = z.fits_uint_p(); ASSERT_ALWAYS (! fits);
    z = UINT_MAX; fits = z.fits_uint_p(); ASSERT_ALWAYS (fits);
    z++;          fits = z.fits_uint_p(); ASSERT_ALWAYS (! fits);
  }

  // mpz_class::fits_slong_p
  {
    int        fits;
    mpz_class  z;
    z = LONG_MIN; fits = z.fits_slong_p(); ASSERT_ALWAYS (fits);
    z--;          fits = z.fits_slong_p(); ASSERT_ALWAYS (! fits);
    z = LONG_MAX; fits = z.fits_slong_p(); ASSERT_ALWAYS (fits);
    z++;          fits = z.fits_slong_p(); ASSERT_ALWAYS (! fits);
  }

  // mpz_class::fits_ulong_p
  {
    int        fits;
    mpz_class  z;
    z = 0;         fits = z.fits_ulong_p(); ASSERT_ALWAYS (fits);
    z--;           fits = z.fits_ulong_p(); ASSERT_ALWAYS (! fits);
    z = ULONG_MAX; fits = z.fits_ulong_p(); ASSERT_ALWAYS (fits);
    z++;           fits = z.fits_ulong_p(); ASSERT_ALWAYS (! fits);
  }

  // mpz_class::fits_sshort_p
  {
    int        fits;
    mpz_class  z;
    z = SHRT_MIN; fits = z.fits_sshort_p(); ASSERT_ALWAYS (fits);
    z--;          fits = z.fits_sshort_p(); ASSERT_ALWAYS (! fits);
    z = SHRT_MAX; fits = z.fits_sshort_p(); ASSERT_ALWAYS (fits);
    z++;          fits = z.fits_sshort_p(); ASSERT_ALWAYS (! fits);
  }

  // mpz_class::fits_ushort_p
  {
    int        fits;
    mpz_class  z;
    z = 0;         fits = z.fits_ushort_p(); ASSERT_ALWAYS (fits);
    z--;           fits = z.fits_ushort_p(); ASSERT_ALWAYS (! fits);
    z = USHRT_MAX; fits = z.fits_ushort_p(); ASSERT_ALWAYS (fits);
    z++;           fits = z.fits_ushort_p(); ASSERT_ALWAYS (! fits);
  }

  // mpz_class::get_mpz_t
  {
    mpz_class  z(0);
    mpz_ptr    p = z.get_mpz_t();
    ASSERT_ALWAYS (mpz_cmp_ui (p, 0) == 0);
  }
  {
    mpz_class  z(0);
    mpz_srcptr p = z.get_mpz_t();
    ASSERT_ALWAYS (mpz_cmp_ui (p, 0) == 0);
  }

  // mpz_class::get_d
  // mpz_class::get_si
  // mpz_class::get_ui
  {
    mpz_class  z(123);
    { double d = z.get_d();  ASSERT_ALWAYS (d == 123.0); }
    { long   l = z.get_si(); ASSERT_ALWAYS (l == 123L); }
    { long   u = z.get_ui(); ASSERT_ALWAYS (u == 123L); }
  }
  {
    mpz_class  z(-123);
    { double d = z.get_d();  ASSERT_ALWAYS (d == -123.0); }
    { long   l = z.get_si(); ASSERT_ALWAYS (l == -123L); }
  }

  // mpz_class::get_str
  {
    mpz_class  z(123);
    string     s;
    s = z.get_str(); ASSERT_ALWAYS (s == "123");
    s = z.get_str(16); ASSERT_ALWAYS (s == "7b");
    s = z.get_str(-16); ASSERT_ALWAYS (s == "7B");
  }

  // mpz_class::set_str
  {
    mpz_class  z;
    int        ret;
    ret = z.set_str ("123", 10);  ASSERT_ALWAYS (ret == 0 && z == 123);
    ret = z.set_str ("7b",  16);  ASSERT_ALWAYS (ret == 0 && z == 123);
    ret = z.set_str ("7B",  16);  ASSERT_ALWAYS (ret == 0 && z == 123);
    ret = z.set_str ("0x7B", 0);  ASSERT_ALWAYS (ret == 0 && z == 123);

    ret = z.set_str (string("123"), 10);  ASSERT_ALWAYS (ret == 0 && z == 123);
    ret = z.set_str (string("7b"),  16);  ASSERT_ALWAYS (ret == 0 && z == 123);
    ret = z.set_str (string("7B"),  16);  ASSERT_ALWAYS (ret == 0 && z == 123);
    ret = z.set_str (string("0x7B"), 0);  ASSERT_ALWAYS (ret == 0 && z == 123);
  }

  // sgn
  {
    mpz_class  z(123);
    int        s = sgn(z);
    ASSERT_ALWAYS (s == 1);
  }
  {
    mpz_class  z(0);
    int        s = sgn(z);
    ASSERT_ALWAYS (s == 0);
  }
  {
    mpz_class  z(-123);
    int        s = sgn(z);
    ASSERT_ALWAYS (s == -1);
  }

  // sqrt
  {
    mpz_class  z(25);
    ASSERT_ALWAYS (sqrt(z) == 5);
  }
}


void
check_mpq (void)
{
  // abs
  {
    mpq_class  a (-123);
    mpq_class  b = abs(a);
    ASSERT_ALWAYS (b == 123);
  }

  // mpq_class::canonicalize
  {
    mpq_class  q(12,9);
    q.canonicalize();
    ASSERT_ALWAYS (q.get_num() == 4);
    ASSERT_ALWAYS (q.get_den() == 3);
  }

  // cmp
  {
    mpq_class a(123), b(45);
    int  c;
    c = cmp (a, b); ASSERT_ALWAYS (c > 0);
    c = cmp (b, a); ASSERT_ALWAYS (c < 0);
  }
#if 0
  {
    mpq_class a(123);
    unsigned long b = 45;
    int  c;
    c = cmp (a, b); ASSERT_ALWAYS (c > 0);
    c = cmp (b, a); ASSERT_ALWAYS (c < 0);
  }
  {
    mpq_class a(123);
    long b = 45;
    int  c;
    c = cmp (a, b); ASSERT_ALWAYS (c > 0);
    c = cmp (b, a); ASSERT_ALWAYS (c < 0);
  }
  {
    mpq_class a(123);
    double b = 45;
    int  c;
    c = cmp (a, b); ASSERT_ALWAYS (c > 0);
    c = cmp (b, a); ASSERT_ALWAYS (c < 0);
  }
#endif

  // mpq_class::get_d
  {
    mpq_class  q(123);
    { double d = q.get_d();  ASSERT_ALWAYS (d == 123.0); }
  }
  {
    mpq_class  q(-123);
    { double d = q.get_d();  ASSERT_ALWAYS (d == -123.0); }
  }

  // mpq_class::get_mpq_t
  {
    mpq_class  q(0);
    mpq_ptr    p = q.get_mpq_t();
    ASSERT_ALWAYS (mpq_cmp_ui (p, 0, 0) == 0);
  }
  {
    mpq_class  q(0);
    mpq_srcptr p = q.get_mpq_t();
    ASSERT_ALWAYS (mpq_cmp_ui (p, 0, 0) == 0);
  }

  // mpq_class::get_num, mpq_class::get_den
  {
    mpq_class  q(4,5);
    mpz_class  z;
    z = q.get_num(); ASSERT_ALWAYS (z == 4);
    z = q.get_den(); ASSERT_ALWAYS (z == 5);
  }

  // mpq_class::get_num_mpz_t, mpq_class::get_den_mpz_t
  {
    mpq_class  q(4,5);
    mpz_ptr    p;
    p = q.get_num_mpz_t(); ASSERT_ALWAYS (mpz_cmp_ui (p, 4) == 0);
    p = q.get_den_mpz_t(); ASSERT_ALWAYS (mpz_cmp_ui (p, 5) == 0);
  }
  {
    mpq_class  q(4,5);
    mpz_srcptr p;
    p = q.get_num_mpz_t(); ASSERT_ALWAYS (mpz_cmp_ui (p, 4) == 0);
    p = q.get_den_mpz_t(); ASSERT_ALWAYS (mpz_cmp_ui (p, 5) == 0);
  }

  // mpq_class::get_str
  {
    mpq_class  q(17,11);
    string     s;
    s = q.get_str();    ASSERT_ALWAYS (s == "17/11");
    s = q.get_str(10);  ASSERT_ALWAYS (s == "17/11");
    s = q.get_str(16);  ASSERT_ALWAYS (s == "11/b");
    s = q.get_str(-16); ASSERT_ALWAYS (s == "11/B");
  }

  // mpq_class::set_str
  {
    mpq_class  q;
    int        ret;
    ret = q.set_str ("123", 10);     ASSERT_ALWAYS (ret == 0 && q == 123);
    ret = q.set_str ("4/5", 10);     ASSERT_ALWAYS (ret == 0 && q == mpq_class(4,5));
    ret = q.set_str ("7b",  16);     ASSERT_ALWAYS (ret == 0 && q == 123);
    ret = q.set_str ("7B",  16);     ASSERT_ALWAYS (ret == 0 && q == 123);
    ret = q.set_str ("0x7B", 0);     ASSERT_ALWAYS (ret == 0 && q == 123);
    ret = q.set_str ("0x10/17", 0);  ASSERT_ALWAYS (ret == 0 && q == mpq_class(16,17));

    ret = q.set_str (string("4/5"), 10);  ASSERT_ALWAYS (ret == 0 && q == mpq_class(4,5));
    ret = q.set_str (string("123"), 10);  ASSERT_ALWAYS (ret == 0 && q == 123);
    ret = q.set_str (string("7b"),  16);  ASSERT_ALWAYS (ret == 0 && q == 123);
    ret = q.set_str (string("7B"),  16);  ASSERT_ALWAYS (ret == 0 && q == 123);
    ret = q.set_str (string("0x7B"), 0);  ASSERT_ALWAYS (ret == 0 && q == 123);
    ret = q.set_str (string("0x10/17"), 0);  ASSERT_ALWAYS (ret == 0 && q == mpq_class(16,17));
  }

  // sgn
  {
    mpq_class  q(123);
    int        s = sgn(q);
    ASSERT_ALWAYS (s == 1);
  }
  {
    mpq_class  q(0);
    int        s = sgn(q);
    ASSERT_ALWAYS (s == 0);
  }
  {
    mpq_class  q(-123);
    int        s = sgn(q);
    ASSERT_ALWAYS (s == -1);
  }

}


void
check_mpf (void)
{
  // abs
  {
    mpf_class  a (-123);
    mpf_class  b = abs(a);
    ASSERT_ALWAYS (b == 123);
  }

  // ceil
  {
    mpf_class  a (1.5);
    mpf_class  b = ceil(a);
    ASSERT_ALWAYS (b == 2);
  }
  {
    mpf_class  a (-1.5);
    mpf_class  b = ceil(a);
    ASSERT_ALWAYS (b == -1);
  }

  // cmp
  {
    mpf_class a(123), b(45);
    int  c;
    c = cmp (a, b); ASSERT_ALWAYS (c > 0);
    c = cmp (b, a); ASSERT_ALWAYS (c < 0);
  }
#if 0
  {
    mpf_class a(123);
    unsigned long b = 45;
    int  c;
    c = cmp (a, b); ASSERT_ALWAYS (c > 0);
    c = cmp (b, a); ASSERT_ALWAYS (c < 0);
  }
  {
    mpf_class a(123);
    long b = 45;
    int  c;
    c = cmp (a, b); ASSERT_ALWAYS (c > 0);
    c = cmp (b, a); ASSERT_ALWAYS (c < 0);
  }
  {
    mpf_class a(123);
    double b = 45;
    int  c;
    c = cmp (a, b); ASSERT_ALWAYS (c > 0);
    c = cmp (b, a); ASSERT_ALWAYS (c < 0);
  }
#endif

  // mpf_class::fits_sint_p
  {
    int        fits;
    mpf_class  f (0, 2*8*sizeof(int));
    f = INT_MIN; fits = f.fits_sint_p(); ASSERT_ALWAYS (fits);
    f--;         fits = f.fits_sint_p(); ASSERT_ALWAYS (! fits);
    f = INT_MAX; fits = f.fits_sint_p(); ASSERT_ALWAYS (fits);
    f++;         fits = f.fits_sint_p(); ASSERT_ALWAYS (! fits);
  }

  // mpf_class::fits_uint_p
  {
    int        fits;
    mpf_class  f (0, 2*8*sizeof(int));
    f = 0;        fits = f.fits_uint_p(); ASSERT_ALWAYS (fits);
    f--;          fits = f.fits_uint_p(); ASSERT_ALWAYS (! fits);
    f = UINT_MAX; fits = f.fits_uint_p(); ASSERT_ALWAYS (fits);
    f++;          fits = f.fits_uint_p(); ASSERT_ALWAYS (! fits);
  }

  // mpf_class::fits_slong_p
  {
    int        fits;
    mpf_class  f (0, 2*8*sizeof(long));
    f = LONG_MIN; fits = f.fits_slong_p(); ASSERT_ALWAYS (fits);
    f--;          fits = f.fits_slong_p(); ASSERT_ALWAYS (! fits);
    f = LONG_MAX; fits = f.fits_slong_p(); ASSERT_ALWAYS (fits);
    f++;          fits = f.fits_slong_p(); ASSERT_ALWAYS (! fits);
  }

  // mpf_class::fits_ulong_p
  {
    int        fits;
    mpf_class  f (0, 2*8*sizeof(long));
    f = 0;         fits = f.fits_ulong_p(); ASSERT_ALWAYS (fits);
    f--;           fits = f.fits_ulong_p(); ASSERT_ALWAYS (! fits);
    f = ULONG_MAX; fits = f.fits_ulong_p(); ASSERT_ALWAYS (fits);
    f++;           fits = f.fits_ulong_p(); ASSERT_ALWAYS (! fits);
  }

  // mpf_class::fits_sshort_p
  {
    int        fits;
    mpf_class  f (0, 2*8*sizeof(short));
    f = SHRT_MIN; fits = f.fits_sshort_p(); ASSERT_ALWAYS (fits);
    f--;          fits = f.fits_sshort_p(); ASSERT_ALWAYS (! fits);
    f = SHRT_MAX; fits = f.fits_sshort_p(); ASSERT_ALWAYS (fits);
    f++;          fits = f.fits_sshort_p(); ASSERT_ALWAYS (! fits);
  }

  // mpf_class::fits_ushort_p
  {
    int        fits;
    mpf_class  f (0, 2*8*sizeof(short));
    f = 0;         fits = f.fits_ushort_p(); ASSERT_ALWAYS (fits);
    f--;           fits = f.fits_ushort_p(); ASSERT_ALWAYS (! fits);
    f = USHRT_MAX; fits = f.fits_ushort_p(); ASSERT_ALWAYS (fits);
    f++;           fits = f.fits_ushort_p(); ASSERT_ALWAYS (! fits);
  }

  // floor
  {
    mpf_class  a (1.5);
    mpf_class  b = floor(a);
    ASSERT_ALWAYS (b == 1);
  }
  {
    mpf_class  a (-1.5);
    mpf_class  b = floor(a);
    ASSERT_ALWAYS (b == -2);
  }

  // mpf_class::get_d
  // mpf_class::get_si
  // mpf_class::get_ui
  {
    mpf_class  f(123);
    { double d = f.get_d();  ASSERT_ALWAYS (d == 123.0); }
    { long   l = f.get_si(); ASSERT_ALWAYS (l == 123L); }
    { long   u = f.get_ui(); ASSERT_ALWAYS (u == 123L); }
  }
  {
    mpf_class  f(-123);
    { double d = f.get_d();  ASSERT_ALWAYS (d == -123.0); }
    { long   l = f.get_si(); ASSERT_ALWAYS (l == -123L); }
  }

  // mpf_class::get_prec
  {
    mpf_class  f;
    ASSERT_ALWAYS (f.get_prec() == mpf_get_default_prec());
  }

  // mpf_class::get_str
  {
    mpf_class  f(123);
    string     s;
    mp_exp_t   e;
    s = f.get_str(e);        ASSERT_ALWAYS (s == "123" && e == 3);
    s = f.get_str(e,  16);   ASSERT_ALWAYS (s == "7b"  && e == 2);
    s = f.get_str(e, -16);   ASSERT_ALWAYS (s == "7B"  && e == 2);
    s = f.get_str(e, 10, 2); ASSERT_ALWAYS (s == "12"  && e == 3);
    s = f.get_str(e, 10, 1); ASSERT_ALWAYS (s == "1"   && e == 3);
  }

  // hypot
  {
    mpf_class  a(3), b(4), c;
    c = hypot (a, b);
    ASSERT_ALWAYS (c == 5);
  }

  // mpf_class::set_str
  {
    mpf_class  f;
    int        ret;
    ret = f.set_str ("123",     10);  ASSERT_ALWAYS (ret == 0 && f == 123);
    ret = f.set_str ("123e1",   10);  ASSERT_ALWAYS (ret == 0 && f == 1230);
    ret = f.set_str ("1230e-1", 10);  ASSERT_ALWAYS (ret == 0 && f == 123);
    ret = f.set_str ("7b",      16);  ASSERT_ALWAYS (ret == 0 && f == 123);
    ret = f.set_str ("7B",      16);  ASSERT_ALWAYS (ret == 0 && f == 123);
    ret = f.set_str ("7B@1",    16);  ASSERT_ALWAYS (ret == 0 && f == 1968);
    ret = f.set_str ("7B0@-1",  16);  ASSERT_ALWAYS (ret == 0 && f == 123);

    ret = f.set_str (string("123"),     10);  ASSERT_ALWAYS (ret == 0 && f == 123);
    ret = f.set_str (string("123e1"),   10);  ASSERT_ALWAYS (ret == 0 && f == 1230);
    ret = f.set_str (string("1230e-1"), 10);  ASSERT_ALWAYS (ret == 0 && f == 123);
    ret = f.set_str (string("7b"),      16);  ASSERT_ALWAYS (ret == 0 && f == 123);
    ret = f.set_str (string("7B"),      16);  ASSERT_ALWAYS (ret == 0 && f == 123);
    ret = f.set_str (string("7B@1"),    16);  ASSERT_ALWAYS (ret == 0 && f == 1968);
    ret = f.set_str (string("7B0@-1"),  16);  ASSERT_ALWAYS (ret == 0 && f == 123);
  }

  // mpf_class::set_prec
  {
    mpf_class  f;
    f.set_prec (256);
    ASSERT_ALWAYS (f.get_prec () >= 256);
  }

  // mpf_class::set_prec_raw
  {
    mpf_class  f (0, 100 * GMP_NUMB_BITS);
    f.set_prec_raw (5 * GMP_NUMB_BITS);
    ASSERT_ALWAYS (f.get_prec () >= 5 * GMP_NUMB_BITS);
    ASSERT_ALWAYS (f.get_prec () < 100 * GMP_NUMB_BITS);
    f.set_prec_raw (100 * GMP_NUMB_BITS);
  }

  // sgn
  {
    mpf_class  f(123);
    int        s = sgn(f);
    ASSERT_ALWAYS (s == 1);
  }
  {
    mpf_class  f(0);
    int        s = sgn(f);
    ASSERT_ALWAYS (s == 0);
  }
  {
    mpf_class  f(-123);
    int        s = sgn(f);
    ASSERT_ALWAYS (s == -1);
  }

  // sqrt
  {
    mpf_class  f(25);
    ASSERT_ALWAYS (sqrt(f) == 5);
  }

  // trunc
  {
    mpf_class  a (1.5);
    mpf_class  b = trunc(a);
    ASSERT_ALWAYS (b == 1);
  }
  {
    mpf_class  a (-1.5);
    mpf_class  b = trunc(a);
    ASSERT_ALWAYS (b == -1);
  }

}


void
check_rand (void)
{
  {
    mpz_class      a (0);
    unsigned long  c = 0;
    unsigned long  m2exp = 8;
    gmp_randclass  r (gmp_randinit_lc_2exp, a, c, m2exp);
  }

  {
    unsigned long  m2exp = 64;
    gmp_randclass  r (gmp_randinit_lc_2exp_size, m2exp);
  }

  {
    gmp_randclass  r (gmp_randinit_default);
    r.seed (mpz_class(123));
    r.seed (123L);

    { mpz_class z = r.get_z_bits(256); }
    { mpz_class z = r.get_z_bits(mpz_class(256)); }
    { mpf_class z = r.get_z_range(mpz_class(123)); }

    { mpf_class f = r.get_f (); }
    { mpf_class f = r.get_f (256); }
  }
}


int
main (int argc, char *argv[])
{
  tests_start ();

  check_mpz ();
  check_mpq ();
  check_mpf ();
  check_rand ();

  tests_end ();
  exit (0);
}
