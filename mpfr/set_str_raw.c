/* mpfr_set_str_raw -- set a floating-point number from a binary string

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
#include <stdlib.h>
#ifdef HAS_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#include "mpfr.h"

/* Currently the number should be of the form +/- xxxx.xxxxxxEyy, with 
   decimal exponent. The mantissa of x is supposed to be large enough 
   to hold all the bits of str. */

void
#if __STDC__
mpfr_set_str_raw(mpfr_ptr x, char *str)
#else
mpfr_set_str_raw(x, str)
     mpfr_ptr x; 
     char *str; 
#endif
{
  char *str2, *str0, negative = 0; 
  unsigned long j, l, k = 0, xsize, cnt; mp_limb_t *xp; 
  long expn = 0, e; char *endstr2;

  xp = x -> _mp_d; 
  xsize = 1 + (PREC(x)-1)/BITS_PER_MP_LIMB;
  str0 = str2 = (char *) malloc((strlen(str)+1)*sizeof(char)); 

  if (*str == '-') { negative = 1; str++; }
  else if (*str == '+') str++;

  while (*str == '0') { str++; } 

  while (*str == '0' || *str == '1')
    { *(str2++) = *(str++); k++; }

  if (*str == '.') 
    {
      str++;
      while (*str == '0' || *str == '1')
	{ *(str2++) = *(str++); }

      if (*str == '[') { while (*str != ']') str++; }
    }
    
  if (*str == '[') { while (*str != ']') str++; }

  if (*str == 'e' || *str == 'E') 
    {
      e = atol(++str); /* signed exponent after 'e' or 'E' */
      expn = k + e; 
      if (expn < e)
	  fprintf(stderr, "Warning: overflow in exponent in Str -> mpfr\n"); 
    }
  else expn=k;

  endstr2 = str2;
  *str2 = (char) 0; /* end of string */
  l = (strlen(str0) - 1) / BITS_PER_MP_LIMB + 1; str2 = str0; 

  /* str2[0]..endstr2[-1] contains the mantissa */
  for (k = 1; k <= l; k++)
    {
      j = 0; 
      xp[xsize - k] = 0; 
      while (str2<endstr2 && j < BITS_PER_MP_LIMB)
	{
	  xp[xsize - k] = (xp[xsize - k] << 1) + (*str2 - '0'); 
	  str2++; j++; 
	}
      xp[xsize - k] <<= (BITS_PER_MP_LIMB - j); 
    }

  for (; k <= xsize; k++) { xp[xsize - k] = 0; }

  count_leading_zeros(cnt, xp[xsize - 1]); 
  if (cnt) mpn_lshift(xp, xp, xsize, cnt); 

  x -> _mp_exp = expn - cnt; 
  x -> _mp_size = xsize; if (negative) CHANGE_SIGN(x);

  free(str0); 
  
  /* May change to take into account : syntax errors, overflow in exponent, 
     string truncated because of size of x */
}
