/* Demo program to run expression evaluation.

Copyright 2000, 2001, 2002 Free Software Foundation, Inc.

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


/* Usage: ./run-expr [-z] [-q] [-f] [-r] [-p prec] [-b base] expression...

   Evaluate each argument as a simple expression.  By default this is in mpz
   integers, but -q selects mpq, -f selects mpf or -r selects mpfr (if
   available).  For mpf the float precision can be set with -p.  In all
   cases the input base can be set with -b, or the default is "0" meaning
   decimal with "0x" allowed.

   This is a pretty trivial program, it's just an easy way to experiment
   with the evaluation functions.  */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "gmp.h"

/* only to get HAVE_MPFR, not necessary for normal use */
#include "expr-impl.h"

#include "expr.h"

#if ! HAVE_DECL_OPTARG
extern char *optarg;
extern int optind, opterr;
#endif


#define TRY(exprfun, outfun, str)                       \
  {                                                     \
    ret = exprfun (res, base, str, foo, bar, NULL);     \
    printf ("\"%s\" base %d: ", str, base);             \
    if (ret == MPEXPR_RESULT_OK)                        \
      {                                                 \
        printf ("result ");                             \
        outfun;                                         \
        printf ("\n");                                  \
      }                                                 \
    else                                                \
      printf ("invalid (return code %d)\n", ret);       \
  }

  
int
main (int argc, char *argv[])
{
  int        type = 'z';
  int        base = 0;
  mp_size_t  prec = 64;
  int        obase, opt, i, ret;

  while ((opt = getopt (argc, argv, "b:fp:qrz")) != EOF)
    {
      switch (opt) {
      case 'f':
      case 'q':
      case 'r':
      case 'z':
        type = opt;
        break;
      case 'b':
        base = atoi (optarg);
        break;
      case 'p':
        prec = atoi (optarg);
        break;
      case '?':
      default:
        abort ();
      }
    }

  obase = (base == 0 ? 10 : base);

  if (optind >= argc)
    {
      printf ("Usage: %s [-z] [-q] [-f] [-r] [-p prec] [-b base] expression...\n", argv[0]);
      exit (1);
    }

  switch (type) {
  case 'z':
  default:
    {
      mpz_t  res, foo, bar;

      mpz_init (res);
      mpz_init_set_ui (foo, 55L);
      mpz_init_set_ui (bar, 99L);

      for (i = optind; i < argc; i++)
        TRY (mpz_expr, mpz_out_str (stdout, obase, res), argv[i]);

      mpz_clear (res);
      mpz_clear (foo);
      mpz_clear (bar);
    }
    break;

  case 'q':
    {
      mpq_t  res, foo, bar;

      mpq_init (res);
      mpq_init (foo);
      mpq_init (bar);

      mpq_set_ui (foo, 55L, 1);
      mpq_set_ui (bar, 99L, 1);

      for (i = optind; i < argc; i++)
        TRY (mpq_expr, mpq_out_str (stdout, obase, res), argv[i]);

      mpq_clear (res);
      mpq_clear (foo);
      mpq_clear (bar);
    }
    break;

  case 'f':
    {
      mpf_t  res, foo, bar;

      mpf_init2 (res, prec);
      mpf_init_set_ui (foo, 55L);
      mpf_init_set_ui (bar, 99L);

      for (i = optind; i < argc; i++)
        TRY (mpf_expr, mpf_out_str (stdout, obase, 0, res), argv[i]);

      mpf_clear (res);
      mpf_clear (foo);
      mpf_clear (bar);
    }
    break;

  case 'r':
#if HAVE_MPFR
    {
      mpfr_t  res, foo, bar;

      mpfr_init2 (res, prec);
      mpfr_init_set_ui (foo, 55L, GMP_RNDZ);
      mpfr_init_set_ui (bar, 99L, GMP_RNDZ);

      for (i = optind; i < argc; i++)
        TRY (mpfr_expr,
             mpfr_out_str (stdout, obase, 0, res, GMP_RNDZ),
             argv[i]);

      mpfr_clear (res);
      mpfr_clear (foo);
      mpfr_clear (bar);
    }
#else
    printf ("mpfr not compiled in\n");
    exit (1);
#endif
    break;
  }

  return 0;
}
