/* urandom.h -- define urandom returning a full unsigned long random value.

Copyright 1995, 1996, 1997, 2000, 2001 Free Software Foundation, Inc.

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

/* Use mrand48 on systems that either lacks the function `random',
   or are known to have non-standard `random'.  */
#if defined (__hpux) || defined (__svr4__) || defined (__SVR4) \
 || defined (__osf__)

#if defined (__cplusplus)
extern "C" {
#endif
long mrand48 ();
#if defined (__cplusplus)
}
#endif
static inline mp_limb_t
urandom ()
{
#if BITS_PER_MP_LIMB <= 32
  return mrand48 ();
#else
  return mrand48 () ^ ((mp_limb_t) mrand48 () << 32);
#endif
}

#else

#if defined (__cplusplus)
extern "C" {
#endif
#if ! ((defined (__GLIBC__) && defined (_STDLIB_H)) || \
       (defined (__CYGWIN32__) && defined (_STDLIB_H_)))
long random ();
#endif
#if defined (__cplusplus)
}
#endif
static inline mp_limb_t
urandom ()
{
  /* random() returns 31 bits.  */
#if BITS_PER_MP_LIMB <= 31
  return random ();
#else
#if BITS_PER_MP_LIMB <= 62
  return random () ^ ((mp_limb_t) random () << 31);
#else
  return random () ^ ((mp_limb_t) random () << 31) ^ ((mp_limb_t) random () << 62);
#endif
#endif
}

#endif
