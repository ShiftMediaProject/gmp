/* mpfr_out_str -- output a floating-point number to a stream

Copyright (C) 1999 Free Software Foundation.

This file is part of the MPFR Library.

The MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <stdio.h>
#include <string.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

size_t 
#if __STDC__
mpfr_out_str (FILE *stream, int base, size_t n_digits, mpfr_srcptr op,
	      mp_rnd_t rnd_mode)
#else
mpfr_out_str (stream, base, n_digits, op, rnd_mode)
     FILE *stream;
     int base;
     size_t n_digits;
     mpfr_srcptr op;
     mp_rnd_t rnd_mode;
#endif
{
  char *s, *s0;
  size_t slen, l;
  mp_exp_t e;

  if (MPFR_IS_NAN(op)) { fprintf(stream, "NaN"); return 3; }
  if (!MPFR_NOTZERO(op)) { fprintf(stream, "0"); return 1; }
  if (MPFR_IS_INF(op)) 
    { 
      if (MPFR_SIGN(op) == 1) { fprintf(stream, "Inf"); return 3; }
      else { fprintf(stream, "-Inf"); return 4; }
    }

  s = mpfr_get_str (NULL, &e, base, n_digits, op, rnd_mode);
  /*	  TODO: maintenir le code pour les infinis dans get_str ?  */
  s0 = s;
  /* for op=3.1416 we have s = "31416" and e = 1 */
  
  l = strlen(s)+1;
  slen = l;
  if (*s == '-') fputc(*s++, stream);

  fputc(*s++, stream); e--; /* writes leading digit */
  fputc('.', stream);       /* decimal point */
  fputs(s, stream);         /* rest of mantissa */
  if (e) {
    l += fprintf (stream, (base <= 10 ? "e%ld" : "@%ld"), e);
  }

  (*__gmp_free_func)(s0, slen); 
  return l;
}
