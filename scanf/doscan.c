/* __gmp_doscan -- formatted input internals.

   THE FUNCTIONS IN THIS FILE ARE FOR INTERNAL USE ONLY.  THEY'RE ALMOST
   CERTAIN TO BE SUBJECT TO INCOMPATIBLE CHANGES OR DISAPPEAR COMPLETELY IN
   FUTURE GNU MP RELEASES.

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

#include "config.h"

#if HAVE_STDARG
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#include <ctype.h>
#include <stddef.h>    /* for ptrdiff_t */
#include <stdio.h>
#include <string.h>

#if HAVE_LOCALE_H
#include <locale.h>    /* for localeconv */
#endif

#if HAVE_STDINT_H
#include <stdint.h>    /* for intmax_t */
#endif

#if HAVE_SYS_TYPES_H
#include <sys/types.h> /* for quad_t */
#endif

#include "gmp.h"
#include "gmp-impl.h"


/* Change this to "#define TRACE(x) x" for some traces. */
#define TRACE(x)


/* It's necessary to parse up the string to recognise the GMP extra types F,
   Q and Z.  Other types and conversions are passed across to the standard
   sscanf or fscanf via funs->scan, for ease of implemenation.  This is
   essential in the case of something like glibc %p where the pointer format
   isn't actually documented.

   Because funs->scan doesn't get the whole input it can't put the right
   values in for %n, so that's handled in __gmp_doscan.  Neither sscanf nor
   fscanf directly indicate how many characters were read, so an extra %n is
   appended to each run for that.  For fscanf this merely supports our %n
   output, but for sscanf it lets funs->step move us along the input string.

   For standard % conversions, funs->scan is called once for each conversion
   and any preceding fixed match text.  %% is considered a fixed match for
   this purpose.  "next" in __gmp_doscan is where a new fixed match plus %
   conversion is sought.  "next_again" extends the current this_fmt.

   For GMP % conversions, funs->scan is called for any fixed preceding text
   to match, plus a space to read leading whitespace.  If there's no fixed
   text then that space might be all that's passed.  This probably isn't a
   particularly speedy way to skip whitespace, but it fits easily into the
   scheme and saves some complexity in gmpscan.

   vfscanf and vsscanf are only C99 additions, and so might not be available
   on old systems.  If they were available then multiple standard
   conversions could be passed to funs->scan with a va_list for the
   parameters.  But plain fscanf and sscanf work fine, and parsing one field
   at a time shouldn't be too much of a slowdown.

   gmpscan reads a gmp type.  It's only used once, but is a separate
   subroutine to avoid a big chunk of complicated code in the middle of
   __gmp_doscan.  With a couple of loopbacks it's possible to share code for
   parsing integers, rationals and floats.

   In gmpscan normally one char of lookahead is maintained, but when width
   is reached that stops, on the principle that an fgetc/ungetc of a char
   past where we're told to stop would be undesirable.  "chars" is how many
   characters have been read so far, including the current c.  When
   chars==width and another character is desired then a jump is done to the
   "convert" stage.  c is invalid and mustn't be unget'ed in this case;
   chars is set to width+1 to indicate that.

   gmpscan normally returns the number of characters read.  -1 means an
   invalid field, like a "-" or "+" alone.  -2 means EOF reached before any
   matching characters were read.

   Consideration was given to using a separate gmpscan routine for
   gmp_fscanf and for gmp_sscanf.  The sscanf case could zip across a string
   rather than making a function call fun->get per character, and the fscanf
   could use getc rather than fgetc too, which might help those systems
   where getc is a macro or otherwise inlined.  But none of this scanning
   and converting will be particularly fast, so it seems better to have just
   a common gmpscan to test and maintain.

   Enhancements:

   A way to read the GLIBC printf %a format that we support in gmp_printf
   would be good.  That would probably be good for plain GLIBC scanf too, so
   perhaps we can simply follow its lead if it gets such a feature in the
   future.  */



#define GET(c)                  \
  do {                          \
    ASSERT (chars <= width);    \
    chars++;                    \
    if (chars > width)          \
      goto convert;             \
    (c) = (*funs->get) (data);  \
  } while (0)

/* store into "s", extending if necessary */
#define STORE(c)                                                \
  do {                                                          \
    ASSERT (s_upto <= s_alloc);                                 \
    if (s_upto >= s_alloc)                                      \
      {                                                         \
        size_t  s_alloc_new = s_alloc + S_ALLOC_STEP;           \
        s = (*__gmp_reallocate_func) (s, s_alloc, s_alloc_new); \
        s_alloc = s_alloc_new;                                  \
      }                                                         \
    s[s_upto++] = c;                                            \
  } while (0)

#define S_ALLOC_STEP  512


static int
gmpscan (const struct gmp_doscan_funs_t *funs, void *data,
         const struct gmp_doscan_params_t *p, void *dst)
{
  int     chars, c, base, first, width, seen_point, seen_digit;
  size_t  s_upto, s_alloc;
  char    *s;
  int     invalid = 0;

  TRACE (printf ("gmpscan\n"));

  ASSERT (p->type == 'F' || p->type == 'Q' || p->type == 'Z');

  c = (*funs->get) (data);
  if (c == EOF)
    return -2;

  chars = 1;
  first = 1;
  seen_point = 0;
  seen_digit = 0;
  width = (p->width == 0 ? INT_MAX-1 : p->width);
  base = p->base;
  s_alloc = S_ALLOC_STEP;
  s = (*__gmp_allocate_func) (s_alloc);
  s_upto = 0;

 another:
  if (c == '-')
    {
      STORE (c);
      goto get_for_sign;
    }
  else if (c == '+')
    {
      /* don't store '+', it's not accepted by mpz_set_str etc */
    get_for_sign:
      GET (c);
    }

  if (base == 0)
    {
      base = 10;
      if (c == '0')
        {
          seen_digit = 1;
          base = 8;
          STORE (c);
          GET (c);
          if (c == 'x' || c == 'X')
            {
              base = 16;
            store_get_digits:
              STORE (c);
              GET (c);
            }
        }
    }

 digits:
  for (;;)
    {
      if (base == 16)
        {
          if (! (isascii (c) && isxdigit (c)))
            break;
        }
      else
        {
          if (! (isascii (c) && isdigit (c)))
            break;
          if (base == 8 && (c == '8' || c == '9'))
            break;
        }

      seen_digit = 1;
      STORE (c);
      GET (c);
    }

  if (first)
    {
      /* decimal point */
      if (p->type == 'F' && ! seen_point)
        {
#if HAVE_LOCALECONV
          /* For a multi-character decimal point, if the first character is
             present then all of it must be, otherwise the input is
             considered invalid.  */
          const char  *point;
          int         pc;
          point = localeconv()->decimal_point;
          pc = *point++;
          if (c == pc)
            {
              for (;;)
                {
                  STORE (c);
                  GET (c);
                  pc = *point++;
                  if (pc == '\0')
                    break;
                  if (c != pc)
                    goto invalid;
                }
              seen_point = 1;
              goto digits;
            }
#else
          if (c == '.')
            {
              seen_point = 1;
              goto store_get_digits;
            }
#endif
        }

      /* exponent */
      if (p->type == 'F' && (c == 'e' || c == 'E'))
        {
          /* must have at least one digit in the mantissa, just an exponent
             is not good enough */
          if (! seen_digit)
            goto invalid;

        exponent:
          first = 0;
          STORE (c);
          GET (c);
          goto another;
        }

      /* denominator */
      if (p->type == 'Q' && c == '/')
        {
          /* must have at least one digit in the numerator */
          if (! seen_digit)
            goto invalid;

          /* now look for at least one digit in the denominator */
          seen_digit = 0;

          /* allow the base to be redetermined for "%i" */
          base = p->base;
          goto exponent;
        }
    }

 convert:
  if (! seen_digit)
    {
    invalid:
      invalid = 1;
      goto done;
    }

  if (! p->ignore)
    {
      STORE ('\0');
      TRACE (printf ("  convert \"%s\"\n", s));

      /* We ought to have parsed out a valid string above, so just test
         mpz_set_str etc with an ASSERT.  */
      switch (p->type) {
      case 'F':
        ASSERT (p->base == 10);
        ASSERT_NOCARRY (mpf_set_str (dst, s, 10));
        break;
      case 'Q':
        ASSERT_NOCARRY (mpq_set_str (dst, s, p->base));
        break;
      case 'Z':
        ASSERT_NOCARRY (mpz_set_str (dst, s, p->base));
        break;
      default:
        ASSERT (0);
        /*FALLTHRU*/
        break;
      }
    }

 done:
  ASSERT (chars <= width+1);
  if (chars != width+1)
    {
      (*funs->unget) (c, data);
      TRACE (printf ("  ungetc %d, to give %d chars\n", c, chars-1));
    }
  chars--;

  (*__gmp_free_func) (s, s_alloc);

  if (invalid)
    {
      TRACE (printf ("  invalid\n"));
      return -1;
    }

  TRACE (printf ("  return %d chars (cf width %d)\n", chars, width));
  return chars;
}


int
__gmp_doscan (const struct gmp_doscan_funs_t *funs, void *data,
              const char *orig_fmt, va_list orig_ap)
{
  struct gmp_doscan_params_t  param;
  va_list     ap;
  char        *alloc_fmt;
  const char  *fmt, *this_fmt, *percent, *end_fmt;
  size_t      orig_fmt_len, alloc_fmt_size, len;
  int         new_fields, new_chars, gmptype;
  char        fchar;
  int         fields = 0;
  int         chars = 0;

  TRACE (printf ("__gmp_doscan \"%s\"\n", orig_fmt));

  /* Don't modify orig_ap, if va_list is actually an array and hence call by
     reference.  It could be argued that it'd be more efficient to leave
     callers to make a copy if they care, but doing so here is going to be a
     very small part of the total work, and we may as well keep applications
     out of trouble.  */
  va_copy (ap, orig_ap);

  /* Parts of the format string are going to be copied so that a " %n" can
     be appended.  alloc_fmt is some space for that.  orig_fmt_len+4 will be
     needed if fmt consists of a single "%" specifier, but otherwise is an
     overestimate.  We're not going to be very fast here, so use
     __gmp_allocate_func rather than TMP_ALLOC.  */
  orig_fmt_len = strlen (orig_fmt);
  alloc_fmt_size = orig_fmt_len + 4;
  alloc_fmt = (*__gmp_allocate_func) (alloc_fmt_size);

  fmt = orig_fmt;
  end_fmt = orig_fmt + orig_fmt_len;

  for (;;)
    {
    next:
      this_fmt = fmt;
      if (fmt == end_fmt)
        break;
      TRACE (printf ("  this_fmt \"%s\"\n", this_fmt));

    next_again:
      param.type = '\0';
      param.base = 10;
      param.ignore = 0;
      param.width = 0;

      percent = memchr (fmt, '%', end_fmt - fmt);
      if (percent == NULL)
        {
          TRACE (printf ("  no more fields\n"));
          fmt = end_fmt;
          if (fmt != this_fmt)
            {
            doscan_ignore:
              param.ignore = 1;
              goto doscan;
            }
          goto done;
        }
      TRACE (printf ("  percent \"%s\"\n", percent));
      fmt = percent + 1;

      for (;;)
        {
          ASSERT (fmt <= end_fmt);
          fchar = *fmt++;
          switch (fchar) {

          case '\0':  /* unterminated % sequence */
            ASSERT (0);
            goto done;

          case '%':   /* literal % */
            goto next_again;

          case '[':   /* character range */
            fchar = *fmt++;
            if (fchar == '^')
              fchar = *fmt++;
            /* literal ']' allowed as the first char (possibly after '^') */
            if (fchar == ']')
              fchar = *fmt++;
            for (;;)
              {
                ASSERT (fmt <= end_fmt);
                if (fchar == '\0')
                  {
                    /* unterminated % sequence */
                    ASSERT (0);
                    goto done;
                  }
                if (fchar == ']')
                  break;
                fchar = *fmt++;
              }
            /*FALLTHRU*/
          case 'c':   /* characters */
          case 'd':   /* decimal */
          case 'e':   /* float */
          case 'E':   /* float */
          case 'f':   /* float */
          case 'g':   /* float */
          case 'G':   /* float */
          case 'p':   /* pointer */
          case 's':   /* string of non-whitespace */
          case 'u':   /* decimal */
          doscan:
            gmptype = (param.type == 'F'
                       || param.type == 'Q'
                       || param.type == 'Z');
            len = (gmptype ? percent : fmt) - this_fmt;
            memcpy (alloc_fmt, this_fmt, len);

            /* let funs->scan skip leading whitespace for gmp types */
            if (gmptype)
              alloc_fmt[len++] = ' ';

            /* Append a %n to give us a count of characters read.  This %n
               doesn't count towards the return value from funs->scan.  */
            alloc_fmt[len++] = '%';
            alloc_fmt[len++] = 'n';
            alloc_fmt[len] = '\0';
            ASSERT (len < alloc_fmt_size);

            TRACE (printf ("doscan \"%s\"\n", alloc_fmt);
                   if (funs->scan == (gmp_doscan_scan_t) sscanf)
                     printf ("  s=\"%s\"\n", (const char *) data));

            new_chars = -1;
            if (param.ignore || gmptype)
              {
                new_fields = (*funs->scan) (data, alloc_fmt, &new_chars);
                ASSERT (-1 <= new_fields && new_fields <= 0);
              }
            else
              {
                new_fields = (*funs->scan) (data, alloc_fmt,
                                            va_arg (ap, void *), &new_chars);
                ASSERT (-1 <= new_fields && new_fields <= 1);
                if (new_fields == 0)
                  goto done;
              }
            TRACE (printf ("  new_chars %d\n", new_chars));
            TRACE (printf ("  new_fields %d\n", new_fields));

            if (new_fields == -1)
              {
              no_chars_matched:
                if (fields == 0)
                  fields = -1;
                goto done;
              }

            /* Under param.ignore we don't know if new_fields==0 means a
               match (suppressed), or no match.  new_chars will only be
               written for a successful match.  */
            if (new_chars == -1)
              goto done;
            chars += new_chars;
            (*funs->step) (data, new_chars);

            if (gmptype)
              {
                new_chars = gmpscan (funs, data, &param,
                                     param.ignore ? NULL : va_arg (ap, void*));
                if (new_chars == -2)
                  goto no_chars_matched;
                if (new_chars == -1)
                  goto done;
                ASSERT (new_chars >= 0);
                chars += new_chars;
              }

            if (! param.ignore)
              fields++;
            goto next;

          case 'a':   /* glibc allocate string */
          case '\'':  /* glibc digit groupings */
            break;

          case 'F':   /* mpf_t */
          case 'j':   /* intmax_t */
          case 'L':   /* long long */
          case 'q':   /* quad_t */
          case 'Q':   /* mpq_t */
          case 't':   /* ptrdiff_t */
          case 'z':   /* size_t */
          case 'Z':   /* mpz_t */
          set_type:
            param.type = fchar;
            break;

          case 'h':   /* short or char */
            if (param.type != 'h')
              goto set_type;
            param.type = 'H';   /* internal code for "hh" */
            break;

          case 'i':
            param.base = 0;
            goto doscan;

          case 'l':   /* long, long long, double or long double */
            if (param.type != 'l')
              goto set_type;
            param.type = 'L';   /* "ll" means "L" */
            break;

          case 'n':
            if (this_fmt != percent)
              {
                /* match fixed text before %n */
                param.type = '\0';
                fmt = percent;
                TRACE (printf ("  %%n back to \"%s\"\n", fmt));
                goto doscan_ignore;
              }

            if (! param.ignore)
              {
                void  *p;
                p = va_arg (ap, void *);
                TRACE (printf ("  store to %p, ap %p\n", p, ap));
                switch (param.type) {
                case '\0': * (int       *) p = chars; break;
                case 'F':  mpf_set_si ((mpf_ptr) p, (long) chars); break;
                case 'H':  * (char      *) p = chars; break;
                case 'h':  * (short     *) p = chars; break;
#if HAVE_INTMAX_T
                case 'j':  * (intmax_t  *) p = chars; break;
#else
                case 'j':  ASSERT_FAIL (intmax_t not available); break;
#endif
                case 'l':  * (long      *) p = chars; break;
#if HAVE_QUAD_T && HAVE_LONG_LONG
                case 'q':
                  ASSERT_ALWAYS (sizeof (quad_t) == sizeof (long long));
                  /*FALLTHRU*/
#else
                case 'q':  ASSERT_FAIL (quad_t not available); break;
#endif
#if HAVE_LONG_LONG
                case 'L':  * (long long *) p = chars; break;
#else
                case 'L':  ASSERT_FAIL (long long not available); break;
#endif
                case 'Q':  mpq_set_si ((mpq_ptr) p, (long) chars, 1L); break;
#if HAVE_PTRDIFF_T
                case 't':  * (ptrdiff_t *) p = chars; break;
#else
                case 't':  ASSERT_FAIL (ptrdiff_t not available); break;
#endif
#if HAVE_SIZE_T
                case 'z':  * (size_t    *) p = chars; break;
#else
                case 'z':  ASSERT_FAIL (size_t not available); break;
#endif
                case 'Z':  mpz_set_si ((mpz_ptr) p, (long) chars); break;
                default: ASSERT (0); break;
                }
              }
            goto next;

          case 'o':
            param.base = 8;
            goto doscan;

          case 'x':
          case 'X':
            param.base = 16;
            goto doscan;

          case '0': case '1': case '2': case '3': case '4':
          case '5': case '6': case '7': case '8': case '9':
            param.width = 0;
            do {
              param.width = param.width * 10 + (fchar-'0');
              fchar = *fmt++;
            } while (isascii (fchar) && isdigit (fchar));
            fmt--; /* unget the non-digit */
            break;

          case '*':
            param.ignore = 1;
            break;

          default:
            /* something invalid in a % sequence */
            ASSERT (0);
            goto next;
          }
        }
    }

 done:
  (*__gmp_free_func) (alloc_fmt, alloc_fmt_size);
  return fields;
}
