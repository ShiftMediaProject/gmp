/* mpfr_cmp -- compare two floating-point numbers

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
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#include "mpfr.h"

/* returns 0 iff b = c
            a positive value iff b > c
            a negative value iff b < c

More precisely, in case b and c are of same sign, the absolute value 
of the result is one plus the absolute difference between the exponents 
of b and c, i.e. one plus the number of bits shifts to align b and c
(this value is useful in mpfr_sub).

*/

/* #define DEBUG */

/* compares b and sign(s)*c */
int 
#if __STDC__
mpfr_cmp3 ( mpfr_srcptr b, mpfr_srcptr c, long int s)
#else
mpfr_cmp3(b, c, s)
     mpfr_srcptr b;
     mpfr_srcptr c; 
     long int s;
#endif
{
   long int diff_exp;
   unsigned long bn, cn;
   mp_limb_t *bp, *cp;

   if (!NOTZERO(b) && !NOTZERO(c)) { return 0; }
   s = s*SIGN(b)*SIGN(c);
   if (s<0) return(SIGN(b));

   /* now signs are equal */

   diff_exp = EXP(b)-EXP(c);
   s = (SIGN(b)>0) ? 1 : -1;

   if (diff_exp>0) return(s*(1+diff_exp));
   else if (diff_exp<0) return(s*(-1+diff_exp));
   /* both signs and exponents are equal */

   bn = (PREC(b)-1)/mp_bits_per_limb+1;
   cn = (PREC(c)-1)/mp_bits_per_limb+1;
   bp = MANT(b); cp = MANT(c);

   while (bn && cn) {
     if (bp[--bn] != cp[--cn])
       return((bp[bn]>cp[cn]) ? s : -s);
   }

   if (bn) { while (bn) if (bp[--bn]) return(s); }
   else if (cn) while (cn) if (cp[--cn]) return(-s);

   return 0;
}

/* returns the number of cancelled bits when one subtracts abs(c) from abs(b). 
   Assumes b>=c, which implies EXP(b)>=EXP(c).
   if b=c, returns prec(b).
*/
int 
#if __STDC__
mpfr_cmp2 ( mpfr_srcptr b, mpfr_srcptr c )
#else
mpfr_cmp2(b, c)
     mpfr_srcptr b; 
     mpfr_srcptr c; 
#endif
{
  long int d, bn, cn, k, z;
  mp_limb_t *bp, *cp, t, u=0, cc=0;

#ifdef DEBUG
  printf("b="); mpfr_print_raw(b); putchar('\n');
  printf("c="); mpfr_print_raw(c); putchar('\n');
#endif  
  if (NOTZERO(c)==0) return (NOTZERO(b)) ? 0 : PREC(b);
  d = EXP(b)-EXP(c);
  k = 0; /* result can be d or d+1 if d>1, or >= d otherwise */
  /* k is the number of identical bits in the high part,
     then z is the number of possibly cancelled bits */
#ifdef DEBUG
  if (d<0) { printf("assumption EXP(b)<EXP(c) violated\n"); exit(1); }
#endif
  bn = (PREC(b)-1)/mp_bits_per_limb;
  cn = (PREC(c)-1)/mp_bits_per_limb;
  bp = MANT(b); cp = MANT(c);
  /* subtract c from b from most significant to less significant limbs,
     and first determines first non zero limb difference */
  if (d)
    {
      cc = bp[bn--];
      if (d<mp_bits_per_limb)
	cc -= cp[cn]>>d; 
    }
  else { /* d=0 */
    while (bn>=0 && cn>=0 && (cc=(bp[bn--]-cp[cn--]))==0) {
      k+=mp_bits_per_limb;
    }

    if (cc==0) { /* either bn<0 or cn<0 */
      while (bn>=0 && (cc=bp[bn--])==0) k+=mp_bits_per_limb;
    }
    /* now bn<0 or cc<>0 */
    if (cc==0 && bn<0) return(PREC(b));
  }

  /* the first non-zero limb difference is cc, and the number
     of cancelled bits in the upper limbs is k */

  count_leading_zeros(u, cc);
  k += u; 

  if (cc != (1<<(mp_bits_per_limb-u-1))) return k;

  /* now cc is an exact power of two */
  if (cc != 1) 
    /* We just need to compare the following limbs */
    /* until two of them differ. The result is either k or k+1. */
    {
      /* First flush all the unmatched limbs of b ; they all have to
	 be 0 in order for the process to go on */
      while (bn >= 0)
	{
	  if (cn < 0) { return k; }
	  t = bp[bn--]; 
	  if (d < mp_bits_per_limb)
	    {
	      if (d) 
		{ 
		  u = cp[cn--] << (mp_bits_per_limb - d); 
		  if (cn >= 0) u+=(cp[cn]>>d); 
		}
	      else u = cp[cn--]; 
	      
	      if (t > u || (t == u && cn < 0)) return k; 
	      if (t < u) return k+1; 
	    }
	  else
	    if (t) return k; else d -= mp_bits_per_limb; 
	}
	  
      /* bn < 0; if some limb of c is nonzero, return k+1, otherwise return k*/

      if (cn>=0 && (cp[cn--] << (mp_bits_per_limb - d))) { return k+1; }

      while (cn >= 0) 
	if (cp[cn--]) return k+1; 
      return k; 
    }

  /* cc = 1. Too bad. */
  z = 0; /* number of possibly cancelled bits - 1 */
  /* thus result is either k if low(b) >= low(c)
                        or k+z+1 if low(b) < low(c) */
  if (d > mp_bits_per_limb) { return k; }

  while (bn >= 0)
    { 
      if (cn < 0) { return k; }

      if (d) 
	 { 
	   u = cp[cn--] << (mp_bits_per_limb - d); 
	   if (cn >= 0) u+=(cp[cn]>>d);
	 }
       else u = cp[cn--]; 

      /* bp[bn--] > cp[cn--] : no borrow possible, k unchanged
	 bp[bn--] = cp[cn--] : need to consider next limbs
         bp[bn--] < cp[cn--] : borrow
      */
       if ((cc = bp[bn--]) != u) {
	 if (cc>u) return k;
	 else { count_leading_zeros(u, cc-u); return k + 1 + z + u; }
       }
       else z += mp_bits_per_limb; 
    }

  if (cn >= 0)
    count_leading_zeros(cc, ~(cp[cn--] << (mp_bits_per_limb - d))); 
  else { cc = 0; }

  k += cc; 
  if (cc < d) return k;
  
  while (cn >= 0 && !~cp[cn--]) { z += mp_bits_per_limb; } 
  if (cn >= 0) { count_leading_zeros(cc, ~cp[cn--]); return (k + z + cc); }

  return k; /* We **need** that the nonsignificant limbs of c are set
	       to zero there */	       
}
