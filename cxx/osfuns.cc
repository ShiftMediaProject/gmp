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


/* Don't need "format" for mpz_out_ostream, mpq_out_ostream and
   mpf_out_ostream, just "memory" and "reps".  Omitting gmp_asprintf_format
   lets us avoid dragging vsnprintf into the link.  __gmp_asprintf_final
   will be called directly and doesn't need to be in the struct.  */

const struct doprnt_funs_t  __gmp_asprintf_funs_noformat = {
  NULL,
  (doprnt_memory_t) __gmp_asprintf_memory,
  (doprnt_reps_t)   __gmp_asprintf_reps,
  NULL
};


void
__gmp_doprnt_params_from_ios (struct doprnt_params_t *p, ios &o)
{
  switch (o.flags() & ios::basefield) {
  case ios::hex:
    p->base = (o.flags() & ios::uppercase ? -16 : 16);
    switch (o.flags() & (ios::uppercase | ios::showbase)) {
    default:                              p->expfmt = "@%c%02x"; break;
    case ios::uppercase:                  p->expfmt = "@%c%02X"; break;
    case ios::showbase:                   p->expfmt = "@%c%#04x"; break;
    case ios::showbase | ios::uppercase:  p->expfmt = "@%c%#04X"; break;
    }
    break;
  case ios::oct:
    p->base = 8;
    switch (o.flags() & (ios::uppercase | ios::showbase)) {
    default:                              p->expfmt = "e%c%02o"; break;
    case ios::uppercase:                  p->expfmt = "E%c%02o"; break;
    case ios::showbase:                   p->expfmt = "e%c%#02o"; break;
    case ios::showbase | ios::uppercase:  p->expfmt = "E%c%#02o"; break;
    }
    break;
  default:
    p->base = 10;
    switch (o.flags() & ios::uppercase) {
    default:             p->expfmt = "e%c%02d"; break;
    case ios::uppercase: p->expfmt = "E%c%02d"; break;
    }
    break;
  }

  /* "general" if none or more than one bit set */
  switch (o.flags() & ios::floatfield) {
  case ios::fixed:      p->conv = DOPRNT_CONV_FIXED;      break;
  case ios::scientific: p->conv = DOPRNT_CONV_SCIENTIFIC; break;
  default:              p->conv = DOPRNT_CONV_GENERAL;    break;
  }

  p->exptimes4 = 0;

  p->fill = o.fill();

  /* "right" if more than one bit set */
  switch (o.flags() & ios::adjustfield) {
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
  if (o.flags() & ios::showbase)
    p->showbase = ((o.flags() & ios::basefield) == ios::hex
                   ? DOPRNT_SHOWBASE_YES : DOPRNT_SHOWBASE_NONZERO);
  else
    p->showbase = DOPRNT_SHOWBASE_NO;

  p->showpoint = ((o.flags() & ios::showpoint) != 0);

  /* in fixed and scientific always show trailing zeros, in general format
     show them if showpoint is set (or so it seems) */
  switch (o.flags() & ios::floatfield) {
  case ios::fixed:
  case ios::scientific: p->showtrailing = 1;            break;
  default:              p->showtrailing = p->showpoint; break;
  }

  p->sign = (o.flags() & ios::showpos ? '+' : '\0');

  p->width = o.width();

  /* reset on each output */
  o.width (0);
}
