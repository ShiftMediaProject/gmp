/* srandom.h -- define srandom to initialize the seed of the urandom() 
   function. 

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

#if defined (__hpux) || defined (__svr4__) || defined (__SVR4)
/* HPUX lacks random().  */
static inline void
srandom (long int seed)
{
  srand48 (seed);
}
#define __SRANDOM
#endif

#if defined (__alpha) && !defined (__SRANDOM)
/* DEC OSF/1 1.2 random() returns a double.  */
long srand48 (long int);
static inline void
srandom (long int seed)
{
  srand48 (seed); 
}
#define __SRANDOM
#endif

#if !defined(__SRANDOM)
void srandom(unsigned int);
#define __SRANDOM
#endif
