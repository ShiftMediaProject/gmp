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

#ifndef __TRY_H__
#define __TRY_H__

#if defined (__cplusplus)
extern "C" {
#endif


#ifdef __cplusplus
#define ANYARGS  ...
#else
#define ANYARGS
#endif

typedef mp_limb_t (*tryfun_t) _PROTO ((ANYARGS));

#if HAVE_CALLING_CONVENTIONS
extern tryfun_t calling_conventions_function;
mp_limb_t calling_conventions _PROTO ((ANYARGS));
int calling_conventions_check _PROTO ((void));
#define CALLING_CONVENTIONS(function) \
  (calling_conventions_function = (function), calling_conventions)
#define CALLING_CONVENTIONS_CHECK()    (calling_conventions_check())
#else
#define CALLING_CONVENTIONS(function)  (function)
#define CALLING_CONVENTIONS_CHECK()    1 /* always ok */
#endif


extern int mp_trace_base;
void mpn_trace _PROTO ((const char *name, mp_srcptr ptr, mp_size_t size));
void mpn_tracea _PROTO ((const char *name, const mp_ptr *a, int count,
                 mp_size_t size));
void mpn_tracen _PROTO ((const char *name, int num, mp_srcptr ptr,
                 mp_size_t size));
void mpn_trace_file _PROTO ((const char *filename,
                             mp_srcptr ptr, mp_size_t size));
void mpn_tracea_file _PROTO ((const char *filename,
                              const mp_ptr *a, int count, mp_size_t size));
void mpq_trace _PROTO ((const char *name, mpq_srcptr q));
void mpz_trace _PROTO ((const char *name, mpz_srcptr z));
void mpz_tracen _PROTO ((const char *name, int num, mpz_srcptr z));

void spinner _PROTO ((void));
extern unsigned long  spinner_count;


#if defined (__cplusplus)
}
#endif

#endif
