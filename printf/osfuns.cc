/* Support for mpz_out_ostream etc.

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

#include <iostream.h>
#include <stdarg.h>    /* for va_list and hence doprnt_funs_t */
#include <string.h>

#include "gmp.h"
#include "gmp-impl.h"


int
__gmp_ostream_reps (ostream &o, int c, int reps)
{
  char  buf[256];
  int   i, piece;
  ASSERT (reps >= 0);

  memset (buf, c, MIN ((size_t) reps, sizeof (buf)));
  for (i = reps; i > 0; i -= piece)
    {
      piece = MIN ((size_t) reps, sizeof (buf));
      o.write (buf, piece);
      if (! o)
        return -1;
    }
  return reps;
}

static int
ostream_memory (ostream &o, const char *str, size_t len)
{
  o.write (str, len);
  return (o ? (int) len : -1);
}

/* "format" not needed for just __gmp_doprnt_integer and
   __gmp_doprnt_float.  */

const struct doprnt_funs_t  __gmp_ostream_funs = {
  NULL,
  (doprnt_memory_t) ostream_memory,
  (doprnt_reps_t)   __gmp_ostream_reps,
};


void
__gmp_doprnt_params_from_ios (struct doprnt_params_t *p, ios &o)
{
  ios::fmtflags  flags = o.flags();

  switch (flags & ios::basefield) {
  case ios::hex:
    p->base   = (flags & ios::uppercase ? -16 : 16);
    p->expfmt = (flags & ios::uppercase ? "@%c%02X" : "@%c%02x");
    break;
  case ios::oct:
    p->base = 8;
    p->expfmt = (flags & ios::uppercase ? "E%c%02o" : "e%c%02o");
    break;
  default:
    p->base = 10;
    p->expfmt = (flags & ios::uppercase ? "E%c%02d" : "e%c%02d");
    break;
  }

  /* "general" if none or more than one bit set */
  switch (flags & ios::floatfield) {
  case ios::fixed:      p->conv = DOPRNT_CONV_FIXED;      break;
  case ios::scientific: p->conv = DOPRNT_CONV_SCIENTIFIC; break;
  default:              p->conv = DOPRNT_CONV_GENERAL;    break;
  }

  p->exptimes4 = 0;

  p->fill = o.fill();

  /* "right" if more than one bit set */
  switch (flags & ios::adjustfield) {
  case ios::left:     p->justify = DOPRNT_JUSTIFY_LEFT;     break;
  case ios::internal: p->justify = DOPRNT_JUSTIFY_INTERNAL; break;
  default:            p->justify = DOPRNT_JUSTIFY_RIGHT;    break;
  }

  /* ios::fixed allows prec==0, others take 0 as the default 6.
     Don't allow negatives (they do bad things to __gmp_doprnt_float_cxx).  */
  p->prec = MAX (0, o.precision());
  if (p->prec == 0 && p->conv != DOPRNT_CONV_FIXED)
    p->prec = 6;

  /* for hex showbase is always, for octal only non-zero */
  if (flags & ios::showbase)
    p->showbase = ((flags & ios::basefield) == ios::hex
                   ? DOPRNT_SHOWBASE_YES : DOPRNT_SHOWBASE_NONZERO);
  else
    p->showbase = DOPRNT_SHOWBASE_NO;

  p->showpoint = ((flags & ios::showpoint) != 0);

  /* in fixed and scientific always show trailing zeros, in general format
     show them if showpoint is set (or so it seems) */
  switch (flags & ios::floatfield) {
  case ios::fixed:
  case ios::scientific: p->showtrailing = 1;            break;
  default:              p->showtrailing = p->showpoint; break;
  }

  p->sign = (flags & ios::showpos ? '+' : '\0');

  p->width = o.width();

  /* reset on each output */
  o.width (0);
}
