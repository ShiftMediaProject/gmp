/* Exercise various mpbsd functions. */

/*
Copyright (C) 2000 Free Software Foundation, Inc.

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
MA 02111-1307, USA.
*/

#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mp.h"

#define SGN(x)       ((x) < 0 ? -1 : (x) == 0 ? 0 : 1)


void
check_itom (void)
{
  static const struct {
    short      m;
    mp_size_t  want_size;
    mp_limb_t  want_limb;
  } data[] = {

    {  0L,  0 },
    {  1L,  1, 1 },
    { -1L, -1, 1 },

    {  SHORT_MAX,  1, SHORT_MAX },
    { -SHORT_MAX, -1, SHORT_MAX },

    { SHORT_HIGHBIT, -1, USHORT_HIGHBIT },
  };

  MINT  *m;
  int   i;

  for (i = 0; i < numberof (data); i++)
    {
      m = itom (data[i].m);
      if (m->_mp_size != data[i].want_size
          || (m->_mp_size != 0 && m->_mp_d[0] != data[i].want_limb))
        {
          printf ("itom wrong on data[%d]\n", i); 
          abort();                                    
        }
      mfree (m);
    }
}


int
main (void)
{
  check_itom ();

  exit (0);
}
