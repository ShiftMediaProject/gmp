/* trace.c -- Support for diagnostic traces. */

/*
Copyright (C) 1999, 2000 Free Software Foundation, Inc.

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
#include "try.h"


/* Number base for the various trace printing routines.
   Set this in main() or with the debugger.
   If hexadecimal is going to be fed into GNU bc, remember to use -16
   because bc requires upper case.  */

int  mp_trace_base = 10;


/* Print "name=value\n" to stdout for an mpq_t value.  */
void
mpq_trace (const char *name, mpq_srcptr q)
{
  if (name != NULL && name[0] != '\0')
    printf ("%s=", name);

  switch (ABS (mp_trace_base)) {
  case  8: printf ("oct");                          break;
  case 10:                                          break;
  case 16: printf ("0x");                           break;
  default: printf ("base%d:", ABS (mp_trace_base)); break;
  }

  mpq_out_str (stdout, mp_trace_base, q);

  /* It's not very interesting to know when numbers are unnormalized.
     mpz's should always be and ought to be checked with ASSERT() if in doubt.
     mpn's can often be unnormalized without affecting anything.  */
  /* if (!mpq_normalized_p (z))  printf (" (unnorm)"); */

  printf ("\n");
}


/* Print "name=value\n" to stdout for an mpz_t value.  */
void
mpz_trace (const char *name, mpz_srcptr z)
{
  mpq_t      q;
  mpz_t      one;
  mp_limb_t  one_limb;

  PTR(one) = &one_limb;
  SIZ(one) = 1;
  one_limb = 1;

  q->_mp_num = *z;
  q->_mp_den = *one;

  mpq_trace(name, q);
}


/* Print "namenum=value\n" to stdout for an mpz_t value.
   "name" should have a "%d" to get the number. */
void
mpz_tracen (const char *name, int num, mpz_srcptr z)
{
  if (name != NULL && name[0] != '\0')
    {
      printf (name, num);
      putchar ('=');
    }
  mpz_trace (NULL, z);
}


/* Print "name=value\n" to stdout for an mpn style ptr,size. */
void
mpn_trace (const char *name, mp_srcptr ptr, mp_size_t size)
{
  mpz_t  z;
  MPN_NORMALIZE (ptr, size);
  PTR(z) = (mp_ptr) ptr;
  SIZ(z) = (int) size;
  mpz_trace (name, z);
}


/* Print "namenum=value\n" to stdout for an mpn style ptr,size.
   "name" should have a "%d" to get the number.  */
void
mpn_tracen (const char *name, int num, mp_srcptr ptr, mp_size_t size)
{
  if (name != NULL && name[0] != '\0')
    {
      printf (name, num);
      putchar ('=');
    }
  mpn_trace (NULL, ptr, size);
}


/* Print "namenum=value\n" to stdout for an array of mpn style ptr,size.

   "a" is an array of pointers, each a[i] is a pointer to "size" many limbs.
   The formal parameter isn't mp_srcptr because that causes compiler
   warnings, but the values aren't modified.

   "name" should have a printf style "%d" to get the array index.  */

void
mpn_tracea (const char *name, const mp_ptr *a, int count, mp_size_t size)
{
  int i;
  for (i = 0; i < count; i++)
    mpn_tracen (name, i, a[i], size);
}


/* Print "value\n" to a file for an mpz_t value.  Any previous contents of
   the file are overwritten, so you need different file names each time this
   is called.

   Overwriting the file is a feature, it means you get old data replaced
   when you run a test program repeatedly.  */

void
mpn_trace_file (const char *filename, mp_srcptr ptr, mp_size_t size)
{
  FILE   *fp;
  mpz_t  z;

  fp = fopen (filename, "w");
  if (fp == NULL)
    {
      perror ("fopen");
      abort();
    }

  MPN_NORMALIZE (ptr, size);
  PTR(z) = (mp_ptr) ptr;
  SIZ(z) = (int) size;

  mpz_out_str (fp, mp_trace_base, z);
  fprintf (fp, "\n");

  if (ferror (fp) || fclose (fp) != 0)
    {
      printf ("error writing %s\n", filename);
      abort();
    }
}


/* Print "value\n" to a set of files, one file for each element of the given
   array of mpn style ptr,size.  Any previous contents of the files are
   overwritten, so you need different file names each time this is called.
   Each file is "filenameN" where N is 0 to count-1.

   "a" is an array of pointers, each a[i] is a pointer to "size" many limbs.
   The formal parameter isn't mp_srcptr because that causes compiler
   warnings, but the values aren't modified.

   Overwriting the files is a feature, it means you get old data replaced
   when you run a test program repeatedly.  The output style isn't
   particularly pretty, but at least it gets something out, and you can cat
   the files into bc, or whatever. */

void
mpn_tracea_file (const char *filename,
  const mp_ptr *a, int count, mp_size_t size)
{
  char  *s;
  int   i;
  TMP_DECL (marker);

  TMP_MARK (marker);
  s = (char *) TMP_ALLOC (strlen (filename) + 50);

  for (i = 0; i < count; i++)
    {
      sprintf (s, "%s%d", filename, i);
      mpn_trace_file (s, a[i], size);
    }

  TMP_FREE (marker);
}
