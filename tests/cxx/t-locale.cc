/* Test locale support in C++ functions.

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

#include <iostream>

#include "gmp.h"
#include "gmp-impl.h"
#include "tests.h"

using namespace std;


#if HAVE_STD__LOCALE

// Like std::numpunct, but with the decimal point character specified.
//
class my_numpunct : public numpunct<char> {
 private:
  char  point;
 public:
  explicit my_numpunct (size_t r, char c) : numpunct<char>(r) { point = c; }
 protected:
  char do_decimal_point() const { return point; }
};


void
check_input (void)
{
  static const struct {
    const char  *str1;
    const char  *str2;
    double      want;
  } data[] = {

    { "1","",   1.0 },
    { "1","0",  1.0 },
    { "1","00", 1.0 },

    { "","5",    0.5 },
    { "0","5",   0.5 },
    { "00","5",  0.5 },
    { "00","50", 0.5 },

    { "1","5",    1.5 },
    { "1","5e1", 15.0 },
  };

  static char point[] = {
    '.', ',', 'x', 0xFF
  };

  mpf_t  got;
  mpf_init (got);

  for (size_t i = 0; i < numberof (point); i++)
    {
      locale loc (locale::classic(), new my_numpunct (0, point[i]));

      for (int neg = 0; neg <= 1; neg++)
        {
          for (size_t j = 0; j < numberof (data); j++)
            {
              string str;
              if (neg)
                str += '-';
              str += data[j].str1;
              str += point[i];
              str += data[j].str2;

              istringstream is (str.c_str());
              is.imbue (loc);

              // dummy initial value
              mpf_set_d (got, 123.0);

              if (! (is >> got))
                {
                  cout << "istream mpf_t operator>> error\n";
                  cout << "  point " << point[i] << "\n";
                  cout << "  str   \"" << str << "\"\n";
                  abort ();
                }

              double want = data[j].want;
              if (neg)
                want = -want;
              if (mpf_cmp_d (got, want) != 0)
                {
                  cout << "istream mpf_t operator>> wrong\n";
                  cout << "  point " << point[i] << "\n";
                  cout << "  str   \"" << str << "\"\n";
                  cout << "  got   " << got << "\n";
                  cout << "  want  " << want << "\n";
                  abort ();
                }
            }
        }
    }

  mpf_clear (got);
}

void
check_output (void)
{
  static char point[] = {
    '.', ',', 'x', 0xFF
  };

  for (int i = 0; i < numberof (point); i++)
    {
      locale loc (locale::classic(), new my_numpunct (0, point[i]));
      ostringstream  got;
      got.imbue (loc);

      mpf_t  f;
      mpf_init (f);
      mpf_set_d (f, 1.5);
      got << f;
      mpf_clear (f);

      string  want;
      want += "1";
      want += point[i];
      want += "5";

      if (want.compare (got.str()) != 0)
        {
          cout << "ostream mpf_t operator<< doesn't respect locale\n";
          cout << "  point " << point[i] << "\n";
          cout << "  got   \"" << got.str() << "\"\n";
          cout << "  want  \"" << want      << "\"\n";
          abort ();
        }
    }
}

int
main (void)
{
  tests_start ();

  // not yet working
  // check_input ();

  check_output ();

  tests_end ();
  return 0;
}


#else

int
main (void)
{
  cout << "std::locale not supported, tests skipped\n";
  return 0;
}

#endif
