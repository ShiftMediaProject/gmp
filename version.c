/* gmp_version -- version number compiled into the library */

/*
Copyright (C) 1996, 1999, 2000  Free Software Foundation, Inc.

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
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA.  */

#if STDC_HEADERS
# include <stdlib.h>
# include <string.h>
#endif

#include "gmp.h"
#include "gmp-impl.h"

static struct
{
  int major, minor, patchlevel;
} __gmp_version_struct = {0, 0, 0};
  
int
#if __STDC__
gmp_version (gmp_version_enum v)
#else     
gmp_version (v)
     gmp_version_enum v;
#endif
{
  if (__gmp_version_struct.major == 0
      && __gmp_version_struct.minor == 0
      && __gmp_version_struct.patchlevel == 0)
    {
      /* Initialize struct. */
      char *ver_str;
      char c;
      char *cp, *start;
      TMP_DECL (mark);

      TMP_MARK (mark);
      ver_str = TMP_ALLOC (strlen (VERSION) + 1);
      strcpy (ver_str, VERSION);

      start = cp = ver_str;
      while (*cp != '.' && *cp != '\0')
	cp++;
      c = *cp; *cp = '\0';
      __gmp_version_struct.major = atoi (start);

      if (c == '.')
	{
	  start = ++cp;
	  while (*cp != '.' && *cp != '\0')
	    cp++;
	  c = *cp; *cp = '\0';
	  __gmp_version_struct.minor = atoi (start);
	  
	  if (c == '.')
	    {
	      start = ++cp;
	      while (*cp != '.' && *cp != '\0')
		cp++;
	      *cp = '\0';
	      __gmp_version_struct.patchlevel = atoi (start);
	    }
	}
      TMP_FREE (mark);
    }

  switch (v)
    {
    case GMP_GET_VERSION_MAJOR:
      return __gmp_version_struct.major;
    case GMP_GET_VERSION_MINOR:
      return __gmp_version_struct.minor;
    case GMP_GET_VERSION_PATCHLEVEL:
      return __gmp_version_struct.patchlevel;
    default:
      gmp_errno |= GMP_ERROR_INVALID_ARGUMENT;
      return 0;
    }
}
      
  
