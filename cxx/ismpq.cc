/* operator>> -- C++-style input of mpq_t.

Copyright 2001, 2003 Free Software Foundation, Inc.

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

#include <cctype>
#include <iostream>
#include <string>
#include "gmp.h"
#include "gmp-impl.h"

using namespace std;


istream &
operator>> (istream &i, mpq_ptr q)
{
  int base;
  char c = 0;
  string s;
  bool ok = false, zero, showbase;

  i.get(c); // start reading

  if (i.flags() & ios::skipws) // skip initial whitespace
    {
#if HAVE_STD__LOCALE
      const ctype<char>& ct = use_facet< ctype<char> >(i.getloc());
#define cxx_isspace(c)  (ct.is(ctype_base::space,(c)))
#else
#define cxx_isspace(c)  isspace(c)
#endif

      while (cxx_isspace(c) && i.get(c))
        ;
    }
  if (c == '-' || c == '+') // sign
    {
      if (c == '-')
	s = "-";
      i.get(c);
    }

  base = __gmp_istream_set_base(i, c, zero, showbase); // select the base
  __gmp_istream_set_digits(s, i, c, ok, base);         // read the numerator

  if (! ok && zero) // the only digit read was "0"
    {
      base = 10;
      s += '0';
      ok = true;
    }

  if (c == '/') // there's a denominator
    {
      bool zero2 = false;
      int base2 = base;

      s += '/';
      ok = false; // denominator is mandatory
      i.get(c);

      if (showbase) // check base of denominator
	base2 = __gmp_istream_set_base(i, c, zero2, showbase);

      if (base2 == base || base2 == 10) // read the denominator
	__gmp_istream_set_digits(s, i, c, ok, base);

      if (! ok && zero2) // the only digit read was "0"
	{                // denominator is 0, but that's your business
	  s += '0';
	  ok = true;
	}
    }

  if (i.good()) // last character read was non-numeric
    i.putback(c);
  else if (i.eof() && ok) // stopped just before eof
    i.clear();

  if (ok)
    mpq_set_str(q, s.c_str(), base); // extract the number
  else
    i.setstate(ios::failbit); // read failed

  return i;
}
