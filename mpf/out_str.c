/* mpf_out_str (stream, base, n_digits, op) -- Print N_DIGITS digits from
   the float OP to STREAM in base BASE.  Return the number of characters
   written, or 0 if an error occurred.

Copyright 1996, 1997, 2001, 2002 Free Software Foundation, Inc.

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

#include "config.h"

#include <stdio.h>
#include <string.h>

#if HAVE_LOCALE_H
#include <locale.h>    /* for localeconv */
#endif

#include "gmp.h"
#include "gmp-impl.h"

size_t
mpf_out_str (FILE *stream, int base, size_t n_digits, mpf_srcptr op)
{
  char *str;
  mp_exp_t exp;
  size_t written;
  TMP_DECL (marker);

  TMP_MARK (marker);

  if (base == 0)
    base = 10;
  if (n_digits == 0)
    MPF_SIGNIFICANT_DIGITS (n_digits, base, op->_mp_prec);

  if (stream == 0)
    stream = stdout;

  str = (char *) TMP_ALLOC (n_digits + 2); /* extra for minus sign and \0 */

  mpf_get_str (str, &exp, base, n_digits, op);
  n_digits = strlen (str);

  written = 0;

  /* Write sign */
  if (str[0] == '-')
    {
      str++;
      fputc ('-', stream);
      written = 1;
      n_digits--;
    }

#if HAVE_LOCALECONV
  {
    const char  *point = localeconv()->decimal_point;
    size_t      pointlen = strlen (point);
    putc ('0', stream);
    fwrite (point, 1, pointlen, stream);
    written += pointlen + 1;
  }
#else
  fwrite ("0.", 1, 2, stream);
  written += 2;
#endif

  /* Write mantissa */
  {
    size_t fwret;
    fwret = fwrite (str, 1, n_digits, stream);
    written += fwret;
  }

  /* Write exponent */
  {
    int fpret;
    fpret = fprintf (stream, (base <= 10 ? "e%ld" : "@%ld"), exp);
    written += fpret;
  }

  TMP_FREE (marker);
  return ferror (stream) ? 0 : written;
}
