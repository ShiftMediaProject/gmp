/* mpn_and_n, mpn_andn_n, mpn_nand_n, mpn_ior_n, mpn_iorn_n, mpn_nior_n,
   mpn_xor_n, mpn_xnor_n -- mpn bitwise logical operations.  */

/*
Copyright (C) 1999-2000 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "gmp.h"
#include "gmp-impl.h"


#if defined (OPERATION_AND)
#define FUNCTION mpn_and_n
#define OP(a,b)  ((a) & (b))

#elif defined (OPERATION_ANDN)
#define FUNCTION mpn_andn_n
#define OP(a,b)  ((a) & ~(b))

#elif defined (OPERATION_NAND)
#define FUNCTION mpn_nand_n
#define OP(a,b)  (~((a) & (b)))

#elif defined (OPERATION_IOR)
#define FUNCTION mpn_ior_n
#define OP(a,b)  ((a) | (b))

#elif defined (OPERATION_IORN)
#define FUNCTION mpn_iorn_n
#define OP(a,b)  ((a) | ~(b))

#elif defined (OPERATION_NIOR)
#define FUNCTION mpn_nior_n
#define OP(a,b)  (~((a) | (b)))

#elif defined (OPERATION_XOR)
#define FUNCTION mpn_xor_n
#define OP(a,b)  ((a) ^ (b))

#elif defined (OPERATION_XNOR)
#define FUNCTION mpn_xnor_n
#define OP(a,b)  (~(a) ^ (b))

#else
Error, need an OPERATION selected.
#endif


void
#if __STDC__
FUNCTION (mp_ptr dst, mp_srcptr src1, mp_srcptr src2, mp_size_t size)
#else
FUNCTION (dst, src1, src2, size)
     mp_ptr     dst;
     mp_srcptr  src1;
     mp_srcptr  src2;
     mp_size_t  size;
#endif
{
  do
    dst[size-1] = OP (src1[size-1], src2[size-1]);
  while (--size);
}
