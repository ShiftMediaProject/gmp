/* mpfr_out_str -- output a floating-point number to a stream

Copyright (C) 1999 PolKA project, Inria Lorraine and Loria

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
#include <math.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"

size_t 
#if __STDC__
mpfr_out_str (FILE *stream, int base, size_t n_digits, mpfr_srcptr op,
	      unsigned char rnd_mode)
#else
mpfr_out_str (stream, base, n_digits, op, rnd_mode)
     FILE *stream; 
     int base;
     size_t n_digits; 
     mpfr_srcptr op; 
     unsigned char rnd_mode; 
#endif
{
  char *s,*s0; size_t l; mp_exp_t e;

  if (FLAG_NAN(op)) { fprintf(stream, "NaN"); return 3; }
  if (!NOTZERO(op)) { fprintf(stream, "0"); return 1; }

  s = mpfr_get_str(NULL, &e, base, n_digits, op, rnd_mode);
  s0 = s;
  /* for op=3.1416 we have s = "31416" and e = 1 */
  
  l = strlen(s)+1;
  if (*s == '-') fputc(*s++, stream);

  fputc(*s++, stream); e--; /* writes leading digit */
  fputc('.', stream);       /* decimal point */
  fputs(s, stream);         /* rest of mantissa */
  if (e) {
    fputc((base>10) ? '@' : 'e', stream); l++;
    sprintf(s, "%ld", e);
    l += strlen(s);
    fprintf(stream, "%s", s);
  }

  (*_mp_free_func)(s0, l); 
  return l;
}
