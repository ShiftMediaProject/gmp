/* Test gmp_printf and related functions.

Copyright 2001 Free Software Foundation, Inc.

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


/* Usage: t-printf [-s]

   -s  Check the data against the system printf, where possible.  This is
       only an option since we don't want to fail if the system printf is
       faulty or strange.  */


#include "config.h"

#if HAVE_STDARG
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if HAVE_OBSTACK_VPRINTF
#define obstack_chunk_alloc tests_allocate
#define obstack_chunk_free  tests_free_nosize
#include <obstack.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>  /* for unlink */
#endif

#include "gmp.h"
#include "gmp-impl.h"
#include "tests.h"


int   option_check_printf = 0;


#define CHECK_VFPRINTF_FILENAME  "t-printf.tmp"
FILE  *check_vfprintf_fp;


/* From any of the tests run here. */
#define MAX_OUTPUT  1024


void
#if HAVE_STDARG
check_plain (const char *want, const char *fmt_orig, ...)
#else
check_plain (va_alist)
     va_dcl
#endif
{
  char        got[MAX_OUTPUT];
  int         got_len, want_len;
  size_t      fmtsize;
  char        *fmt, *q;
  const char  *p;
  va_list     ap;
#if HAVE_STDARG
  va_start (ap, fmt_orig);
#else
  const char  *want;
  const char  *fmt_orig;
  va_start (ap);
  want = va_arg (ap, const char *);
  fmt_orig = va_arg (ap, const char *);
#endif

  if (! option_check_printf)
    return;

  fmtsize = strlen (fmt_orig) + 1;
  fmt = (*__gmp_allocate_func) (fmtsize);

  for (p = fmt_orig, q = fmt; *p != '\0'; p++)
    {
      switch (*p) {
      case 'a':
      case 'A':
        /* The exact value of the exponent isn't guaranteed in glibc, and it
           and gmp_printf do slightly different things, so don't compare
           directly. */
        goto done;
      case 'F':
        /* discard */
        break;
      case 'Z':
        /* transmute */
        *q++ = 'l';
        break;
      default:
        *q++ = *p;
        break;
      }
    }
  *q = '\0';

  want_len = strlen (want);
  ASSERT_ALWAYS (want_len < sizeof(got));

  got_len = vsprintf (got, fmt, ap);

  if (got_len != want_len || strcmp (got, want) != 0)
    {
      printf ("wanted data doesn't match plain vsprintf\n");
      printf ("  fmt      |%s|\n", fmt);
      printf ("  got      |%s|\n", got);
      printf ("  want     |%s|\n", want);
      printf ("  got_len  %d\n", got_len);
      printf ("  want_len %d\n", want_len);
      abort ();
    }

 done:
  (*__gmp_free_func) (fmt, fmtsize);
}

void
check_vsprintf (const char *want, const char *fmt, va_list ap)
{
  char  got[MAX_OUTPUT];
  int   got_len, want_len;
  
  want_len = strlen (want);
  got_len = gmp_vsprintf (got, fmt, ap);

  if (got_len != want_len || strcmp (got, want) != 0)
    {
      printf ("gmp_vsprintf wrong\n");
      printf ("  fmt      |%s|\n", fmt);
      printf ("  got      |%s|\n", got);
      printf ("  want     |%s|\n", want);
      printf ("  got_len  %d\n", got_len);
      printf ("  want_len %d\n", want_len);
      abort ();
    }
}

void
check_vfprintf (const char *want, const char *fmt, va_list ap)
{
  char  got[MAX_OUTPUT];
  int   got_len, want_len, fread_len;
  long  ftell_len;

  want_len = strlen (want);

  rewind (check_vfprintf_fp);
  got_len = gmp_vfprintf (check_vfprintf_fp, fmt, ap);
  ASSERT_ALWAYS (got_len != -1);
  ASSERT_ALWAYS (fflush (check_vfprintf_fp) == 0);

  ftell_len = ftell (check_vfprintf_fp);
  ASSERT_ALWAYS (ftell_len != -1);

  rewind (check_vfprintf_fp);
  ASSERT_ALWAYS (ftell_len <= sizeof(got));
  fread_len = fread (got, 1, ftell_len, check_vfprintf_fp);

  if (got_len != want_len
      || ftell_len != want_len
      || fread_len != want_len
      || memcmp (got, want, want_len) != 0)
    {
      printf ("gmp_vfprintf wrong\n");
      printf ("  fmt       |%s|\n", fmt);
      printf ("  got       |%.*s|\n", fread_len, got);
      printf ("  want      |%s|\n", want);
      printf ("  got_len   %d\n", got_len);
      printf ("  ftell_len %ld\n", ftell_len);
      printf ("  fread_len %d\n", fread_len);
      printf ("  want_len  %d\n", want_len);
      abort ();
    }
}

void
check_vsnprintf (const char *want, const char *fmt, va_list ap)
{
#if HAVE_VSNPRINTF
  char    got[MAX_OUTPUT+1];
  int     ret, got_len, want_len;
  size_t  bufsize;
  
  want_len = strlen (want);

  bufsize = -1;
  for (;;)
    {
      /* do 0 to 5, then want-5 to want+5 */
      bufsize++;
      if (bufsize > 5 && bufsize < want_len-5)
        bufsize = want_len-5;
      if (bufsize > want_len + 5)
        break;
      ASSERT_ALWAYS (bufsize+1 <= sizeof (got));

      got[bufsize] = '!';
      ret = gmp_vsnprintf (got, bufsize, fmt, ap);

      got_len = MIN (MAX (0, bufsize-1), want_len);

      if (got[bufsize] != '!')
        {
          printf ("gmp_vsnprintf overwrote bufsize sentinel\n");
          goto error;
        }

      if (ret != want_len)
        {
          printf ("gmp_vsnprintf return value wrong\n");
          goto error;
        }

      if (bufsize > 0)
        {
          if (memcmp (got, want, got_len) != 0 || got[got_len] != '\0')
            {
              printf ("gmp_vsnprintf wrong result string\n");
            error:
              printf ("  fmt       |%s|\n", fmt);
              printf ("  bufsize   %u\n", bufsize);
              printf ("  got       |%s|\n", got);
              printf ("  want      |%.*s|\n", got_len, want);
              printf ("  want full |%s|\n", want);
              printf ("  ret       %d\n", ret);
              printf ("  want_len  %d\n", want_len);
              abort ();
            }
        }
    }
#endif /* HAVE_VSNPRINTF */
}

void
check_vasprintf (const char *want, const char *fmt, va_list ap)
{
#if HAVE_VSNPRINTF
  char  *got;
  int   got_len, want_len;
  
  want_len = strlen (want);
  got_len = gmp_vasprintf (&got, fmt, ap);

  if (got_len != want_len || strcmp (got, want) != 0)
    {
      printf ("gmp_vasprintf wrong\n");
      printf ("  fmt      |%s|\n", fmt);
      printf ("  got      |%s|\n", got);
      printf ("  want     |%s|\n", want);
      printf ("  got_len  %d\n", got_len);
      printf ("  want_len %d\n", want_len);
      abort ();
    }
  (*__gmp_free_func) (got, strlen(got)+1);
#endif /* HAVE_VSNPRINTF */
}

void
check_obstack_vprintf (const char *want, const char *fmt, va_list ap)
{
#if HAVE_OBSTACK_VPRINTF
  struct obstack  ob;
  int   got_len, want_len, ob_len;
  char  *got;

  want_len = strlen (want);

  obstack_init (&ob);
  got_len = gmp_obstack_vprintf (&ob, fmt, ap);
  got = obstack_base (&ob);
  ob_len = obstack_object_size (&ob);

  if (got_len != want_len
      || ob_len != want_len
      || memcmp (got, want, want_len) != 0)
    {
      printf ("gmp_obstack_vprintf wrong\n");
      printf ("  fmt      |%s|\n", fmt);
      printf ("  got      |%s|\n", got);
      printf ("  want     |%s|\n", want);
      printf ("  got_len  %d\n", got_len);
      printf ("  ob_len   %d\n", ob_len);
      printf ("  want_len %d\n", want_len);
      abort ();
    }
  obstack_free (&ob, NULL);
#endif
}


void
#if HAVE_STDARG
check_one (const char *want, const char *fmt, ...)
#else
check_one (va_alist)
     va_dcl
#endif
{
  va_list ap;
#if HAVE_STDARG
  va_start (ap, fmt);
#else
  const char  *want;
  const char  *fmt;
  va_start (ap);
  want = va_arg (ap, const char *);
  fmt = va_arg (ap, const char *);
#endif

  /* simplest first */
  check_vsprintf (want, fmt, ap);
  check_vfprintf (want, fmt, ap);
  check_vsnprintf (want, fmt, ap);
  check_vasprintf (want, fmt, ap);
  check_obstack_vprintf (want, fmt, ap);
}


#define hex_or_octal_p(fmt)             \
  (strchr (fmt, 'x') != NULL            \
   || strchr (fmt, 'X') != NULL         \
   || strchr (fmt, 'o') != NULL)

void
check_z (void)
{
  static const struct {
    const char  *fmt;
    const char  *z;
    const char  *want;
  } data[] = {
    { "%Zd", "0",    "0" },
    { "%Zd", "1",    "1" },
    { "%Zd", "123",  "123" },
    { "%Zd", "-1",   "-1" },
    { "%Zd", "-123", "-123" },

    { "%+Zd", "0",      "+0" },
    { "%+Zd", "123",  "+123" },
    { "%+Zd", "-123", "-123" },

    { "%Zx",  "123",   "7b" },
    { "%ZX",  "123",   "7B" },
    { "%Zx", "-123",  "-7b" },
    { "%ZX", "-123",  "-7B" },
    { "%Zo",  "123",  "173" },
    { "%Zo", "-123", "-173" },

    { "%#Zx",    "0",     "0" },
    { "%#ZX",    "0",     "0" },
    { "%#Zx",  "123",  "0x7b" },
    { "%#ZX",  "123",  "0X7B" },
    { "%#Zx", "-123", "-0x7b" },
    { "%#ZX", "-123", "-0X7B" },

    { "%#Zo",    "0",     "0" },
    { "%#Zo",  "123",  "0173" },
    { "%#Zo", "-123", "-0173" },

    { "%10Zd",      "0", "         0" },
    { "%10Zd",    "123", "       123" },
    { "%10Zd",   "-123", "      -123" },

    { "%-10Zd",     "0", "0         " },
    { "%-10Zd",   "123", "123       " },
    { "%-10Zd",  "-123", "-123      " },

    { "%+10Zd",   "123", "      +123" },
    { "%+-10Zd",  "123", "+123      " },
    { "%+10Zd",  "-123", "      -123" },
    { "%+-10Zd", "-123", "-123      " },

    { "%08Zd",    "0", "00000000" },
    { "%08Zd",  "123", "00000123" },
    { "%08Zd", "-123", "-0000123" },

    { "%+08Zd",    "0", "+0000000" },
    { "%+08Zd",  "123", "+0000123" },
    { "%+08Zd", "-123", "-0000123" },

    { "%#08Zx",    "0", "00000000" },
    { "%#08Zx",  "123", "0x00007b" },
    { "%#08Zx", "-123", "-0x0007b" },

    { "%+#08Zx",    "0", "+0000000" },
    { "%+#08Zx",  "123", "+0x0007b" },
    { "%+#08Zx", "-123", "-0x0007b" },

  };

  int    i;
  mpz_t  z;

  mpz_init (z);
  
  for (i = 0; i < numberof (data); i++)
    {
      mpz_set_str_or_abort (z, data[i].z, 0);

      /* don't try negatives or forced sign in hex or octal */
      if (mpz_fits_slong_p (z)
          && ! (hex_or_octal_p (data[i].fmt)
                && (strchr (data[i].fmt, '+') != NULL || mpz_sgn(z) < 0)))
        {
          check_plain (data[i].want, data[i].fmt, mpz_get_si (z));
        }
          
      check_one (data[i].want, data[i].fmt, z);
    }
      
  mpz_clear (z);
}

void
check_q (void)
{
  static const struct {
    const char  *fmt;
    const char  *q;
    const char  *want;
  } data[] = {
    { "%Qd",    "0",    "0" },
    { "%Qd",    "1",    "1" },
    { "%Qd",  "123",  "123" },
    { "%Qd",   "-1",   "-1" },
    { "%Qd", "-123", "-123" },
    { "%Qd",  "3/2",  "3/2" },
    { "%Qd", "-3/2", "-3/2" },

    { "%+Qd", "0",      "+0" },
    { "%+Qd", "123",  "+123" },
    { "%+Qd", "-123", "-123" },
    { "%+Qd", "5/8",  "+5/8" },
    { "%+Qd", "-5/8", "-5/8" },

    { "%Qx",  "123",   "7b" },
    { "%QX",  "123",   "7B" },
    { "%Qx",  "15/16", "f/10" },
    { "%QX",  "15/16", "F/10" },
    { "%Qx", "-123",  "-7b" },
    { "%QX", "-123",  "-7B" },
    { "%Qx", "-15/16", "-f/10" },
    { "%QX", "-15/16", "-F/10" },
    { "%Qo",  "123",  "173" },
    { "%Qo", "-123", "-173" },
    { "%Qo",  "16/17",  "20/21" },
    { "%Qo", "-16/17", "-20/21" },

    { "%#Qx",    "0",     "0" },
    { "%#QX",    "0",     "0" },
    { "%#Qx",  "123",  "0x7b" },
    { "%#QX",  "123",  "0X7B" },
    { "%#Qx",  "5/8",  "0x5/0x8" },
    { "%#QX",  "5/8",  "0X5/0X8" },
    { "%#Qx", "-123", "-0x7b" },
    { "%#QX", "-123", "-0X7B" },
    { "%#Qx", "-5/8", "-0x5/0x8" },
    { "%#QX", "-5/8", "-0X5/0X8" },
    { "%#Qo",    "0",     "0" },
    { "%#Qo",  "123",  "0173" },
    { "%#Qo", "-123", "-0173" },
    { "%#Qo",  "5/7",  "05/07" },
    { "%#Qo", "-5/7", "-05/07" },

    { "%10Qd",      "0", "         0" },
    { "%-10Qd",     "0", "0         " },
    { "%10Qd",    "123", "       123" },
    { "%-10Qd",   "123", "123       " },
    { "%10Qd",   "-123", "      -123" },
    { "%-10Qd",  "-123", "-123      " },

    { "%+10Qd",   "123", "      +123" },
    { "%+-10Qd",  "123", "+123      " },
    { "%+10Qd",  "-123", "      -123" },
    { "%+-10Qd", "-123", "-123      " },

    { "%08Qd",    "0", "00000000" },
    { "%08Qd",  "123", "00000123" },
    { "%08Qd", "-123", "-0000123" },

    { "%+08Qd",    "0", "+0000000" },
    { "%+08Qd",  "123", "+0000123" },
    { "%+08Qd", "-123", "-0000123" },

    { "%#08Qx",    "0", "00000000" },
    { "%#08Qx",  "123", "0x00007b" },
    { "%#08Qx", "-123", "-0x0007b" },

    { "%+#08Qx",    "0", "+0000000" },
    { "%+#08Qx",  "123", "+0x0007b" },
    { "%+#08Qx", "-123", "-0x0007b" },

  };

  int    i;
  mpq_t  q;

  mpq_init (q);
  
  for (i = 0; i < numberof (data); i++)
    {
      mpq_set_str_or_abort (q, data[i].q, 0);
      check_one (data[i].want, data[i].fmt, q);
    }
      
  mpq_clear (q);
}

void
check_f (void)
{
  static const struct {
    const char  *fmt;
    const char  *f;
    const char  *want;

  } data[] = {

    { "%Ff",    "0",    "0.000000" },
    { "%Ff",  "123",  "123.000000" },
    { "%Ff", "-123", "-123.000000" },

    { "%+Ff",    "0",   "+0.000000" },
    { "%+Ff",  "123", "+123.000000" },
    { "%+Ff", "-123", "-123.000000" },

    { "%.0Ff",    "0",    "0" },
    { "%.0Ff",  "123",  "123" },
    { "%.0Ff", "-123", "-123" },

    { "%8.0Ff",    "0", "       0" },
    { "%8.0Ff",  "123", "     123" },
    { "%8.0Ff", "-123", "    -123" },

    { "%08.0Ff",    "0", "00000000" },
    { "%08.0Ff",  "123", "00000123" },
    { "%08.0Ff", "-123", "-0000123" },

    { "%10.2Ff",       "0", "      0.00" },
    { "%10.2Ff",    "0.25", "      0.25" },
    { "%10.2Ff",  "123.25", "    123.25" },
    { "%10.2Ff", "-123.25", "   -123.25" },

    { "%-10.2Ff",       "0", "0.00      " },
    { "%-10.2Ff",    "0.25", "0.25      " },
    { "%-10.2Ff",  "123.25", "123.25    " },
    { "%-10.2Ff", "-123.25", "-123.25   " },

    { "%.2Ff", "0.00000000000001", "0.00" },
    { "%.2Ff", "0.002",            "0.00" },
    { "%.2Ff", "0.008",            "0.01" },

    { "%.0Ff", "123.00000000000001", "123" },
    { "%.0Ff", "123.2",              "123" },
    { "%.0Ff", "123.8",              "124" },

    { "%.0Ff",  "999999.9", "1000000" },
    { "%.0Ff", "3999999.9", "4000000" },

    { "%Fe",    "0",  "0.000000e+00" },
    { "%Fe",    "1",  "1.000000e+00" },
    { "%Fe",  "123",  "1.230000e+02" },

    { "%FE",    "0",  "0.000000E+00" },
    { "%FE",    "1",  "1.000000E+00" },
    { "%FE",  "123",  "1.230000E+02" },

    { "%Fe",    "0",  "0.000000e+00" },
    { "%Fe",    "1",  "1.000000e+00" },

    { "%.0Fe",     "10000000000",    "1e+10" },
    { "%.0Fe",    "-10000000000",   "-1e+10" },

    { "%.2Fe",     "10000000000",  "1.00e+10" },
    { "%.2Fe",    "-10000000000", "-1.00e+10" },

    { "%8.0Fe",    "10000000000", "   1e+10" },
    { "%8.0Fe",   "-10000000000", "  -1e+10" },

    { "%-8.0Fe",   "10000000000", "1e+10   " },
    { "%-8.0Fe",  "-10000000000", "-1e+10  " },

    { "%12.2Fe",   "10000000000", "    1.00e+10" },
    { "%12.2Fe",  "-10000000000", "   -1.00e+10" },

    { "%012.2Fe",  "10000000000", "00001.00e+10" },
    { "%012.2Fe", "-10000000000", "-0001.00e+10" },

    { "%Fg",   "0", "0" },
    { "%Fg",   "1", "1" },
    { "%Fg",   "-1", "-1" },

    { "%.0Fg", "0", "0" },
    { "%.0Fg", "1", "1" },
    { "%.0Fg", "-1", "-1" },

    { "%.1Fg", "100", "1e+02" },
    { "%.2Fg", "100", "1e+02" },
    { "%.3Fg", "100", "100" },
    { "%.4Fg", "100", "100" },

    { "%Fg", "0.001",    "0.001" },
    { "%Fg", "0.0001",   "0.0001" },
    { "%Fg", "0.00001",  "1e-05" },
    { "%Fg", "0.000001", "1e-06" },

    { "%.4Fg", "1.00000000000001", "1" },
    { "%.4Fg", "100000000000001",  "1e+14" },

    { "%.4Fg", "12345678", "1.235e+07" },

    { "%Fa", "0","0x0p+0" },
    { "%FA", "0","0X0P+0" },

    { "%Fa", "1","0x1p+0" },
    { "%Fa", "65535","0xf.fffp+12" },

  };

  int     i;
  mpf_t   f;
  double  d;

  mpf_init2 (f, 256L);

  for (i = 0; i < numberof (data); i++)
    {
      if (data[i].f[0] == '0' && data[i].f[1] == 'x')
        mpf_set_str_or_abort (f, data[i].f, 16);
      else
        mpf_set_str_or_abort (f, data[i].f, 10);
        
      /* if mpf->double doesn't truncate, then expect same result */
      d = mpf_get_d (f);
      if (mpf_cmp_d (f, d) == 0)
        check_plain (data[i].want, data[i].fmt, d);

      check_one (data[i].want, data[i].fmt, f);
    }

  mpf_clear (f);
}


void
check_n (void)
{
  int  n;

  check_one ("blah", "%nblah", &n);
  ASSERT_ALWAYS (n == 0);

  check_one ("hello ", "hello %n", &n);
  ASSERT_ALWAYS (n == 6);

  check_one ("hello  world", "hello %n world", &n);
  ASSERT_ALWAYS (n == 6);
}


void
check_misc (void)
{
  check_one ("!", "%c", '!');
  check_one ("hello world", "hello %s", "world");

  {
    static char  xs[801];
    memset (xs, 'x', sizeof(xs)-1);
    check_one (xs, "%s", xs);
  }
}


int
main (int argc, char *argv[])
{
  if (argc > 1 && strcmp (argv[1], "-s") == 0)
    option_check_printf = 1;

  tests_start ();
  check_vfprintf_fp = fopen (CHECK_VFPRINTF_FILENAME, "w+");
  ASSERT_ALWAYS (check_vfprintf_fp != NULL);
  
  check_z ();
  check_q ();
  check_f ();
  check_n ();
  check_misc ();

  ASSERT_ALWAYS (fclose (check_vfprintf_fp) == 0);
  unlink (CHECK_VFPRINTF_FILENAME);
  tests_end ();
  exit (0);
}
