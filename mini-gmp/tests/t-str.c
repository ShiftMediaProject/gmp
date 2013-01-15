/*

Copyright 2012, Free Software Foundation, Inc.

This file is part of the GNU MP Library test suite.

The GNU MP Library test suite is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 3 of the License,
or (at your option) any later version.

The GNU MP Library test suite is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
Public License for more details.

You should have received a copy of the GNU General Public License along with
the GNU MP Library test suite.  If not, see http://www.gnu.org/licenses/.  */

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mini-random.h"

#define MAXBITS 400
#define COUNT 2000

#define GMP_LIMB_BITS (sizeof(mp_limb_t) * CHAR_BIT)
#define MAXLIMBS ((MAXBITS + GMP_LIMB_BITS - 1) / GMP_LIMB_BITS)

static void
dump (const char *label, const mpz_t x)
{
  char *buf = mpz_get_str (NULL, 16, x);
  fprintf (stderr, "%s: %s\n", label, buf);
  free (buf);
}

int
main (int argc, char **argv)
{
  unsigned i;
  char *ap;
  char *bp;
  char *rp;
  size_t bn, rn, arn;

  mpz_t a, b;

  hex_random_init ();

  mpz_init (a);
  mpz_init (b);

  for (i = 0; i < COUNT; i++)
    {
      int base;
      for (base = 2; base <= 36; base++)
	{
	  hex_random_str_op (MAXBITS, base, &ap, &rp);
	  if (mpz_set_str (a, ap, 16) != 0)
	    {
	      fprintf (stderr, "mpz_set_str failed on input %s\n", ap);
	      abort ();
	    }

	  rn = strlen (rp);
	  arn = rn - (rp[0] == '-');

	  bn = mpz_sizeinbase (a, base);
	  if (bn < arn || bn > (arn + 1))
	    {
	      fprintf (stderr, "mpz_sizeinbase failed:\n");
	      dump ("a", a);
	      fprintf (stderr, "r = %s\n", rp);
	      fprintf (stderr, "  base %d, correct size %u, got %u\n",
		       base, (unsigned) arn, (unsigned)bn);
	      abort ();
	    }
	  bp = mpz_get_str (NULL, base, a);
	  if (strcmp (bp, rp))
	    {
	      fprintf (stderr, "mpz_get_str failed:\n");
	      dump ("a", a);
	      fprintf (stderr, "b = %s\n", bp);
	      fprintf (stderr, "  base = %d\n", base);
	      fprintf (stderr, "r = %s\n", rp);
	      abort ();
	    }

	  mpz_set_str (b, rp, base);

	  if (mpz_cmp (a, b))
	    {
	      fprintf (stderr, "mpz_set_str failed:\n");
	      fprintf (stderr, "r = %s\n", rp);
	      fprintf (stderr, "  base = %d\n", base);
	      fprintf (stderr, "r = %s\n", ap);
	      fprintf (stderr, "  base = 16\n");
	      dump ("b", b);
	      dump ("r", a);
	      abort ();
	    }
	  
	  /* Test mpn interface */
	  if (mpz_sgn (a))
	    {
	      size_t i;
	      const char *absr;
	      mp_limb_t t[MAXLIMBS];
	      mp_size_t tn = mpz_size (a);

	      assert (tn <= MAXLIMBS);
	      mpn_copyi (t, a->_mp_d, tn);
	      
	      bn = mpn_get_str (bp, base, t, tn);
	      if (bn != arn)
		{
		  fprintf (stderr, "mpn_get_str failed:\n");
		  fprintf (stderr, "returned length: %d (bad)\n", bn);
		  fprintf (stderr, "expected: %d\n", arn);
		  fprintf (stderr, "  base = %d\n", base);
		  fprintf (stderr, "r = %s\n", ap);
		  fprintf (stderr, "  base = 16\n");
		  dump ("b", b);
		  dump ("r", a);
		  abort ();
		}
	      absr = rp + (rp[0] == '-');

	      for (i = 0; i < bn; i++)
		{
		  unsigned char digit = absr[i];
		  unsigned value;
		  if (digit >= '0' && digit <= '9')
		    value = digit - '0';
		  else if (digit >= 'a' && digit <= 'z')
		    value = digit - 'a' + 10;
		  else if (digit >= 'A' && digit <= 'Z')
		    value = digit - 'A' + 10;
		  else
		    {
		      fprintf (stderr, "Internal error in test.\n");
		      abort();
		    }
		  if (bp[i] != value)
		    {
		      fprintf (stderr, "mpn_get_str failed:\n");
		      fprintf (stderr, "digit %d: %d (bad)\n", i, bp[i]);
		      fprintf (stderr, "expected: %d\n", value);
		      fprintf (stderr, "  base = %d\n", base);
		      fprintf (stderr, "r = %s\n", ap);
		      fprintf (stderr, "  base = 16\n");
		      dump ("b", b);
		      dump ("r", a);
		      abort ();
		    }
		}
	      tn = mpn_set_str (t, bp, bn, base);
	      if (tn != mpz_size (a) || mpn_cmp (t, a->_mp_d, tn))
		{
		  fprintf (stderr, "mpn_set_str failed:\n");
		  fprintf (stderr, "r = %s\n", rp);
		  fprintf (stderr, "  base = %d\n", base);
		  fprintf (stderr, "r = %s\n", ap);
		  fprintf (stderr, "  base = 16\n");
		  dump ("r", a);
		  abort ();
		}
	    }
	  free (ap);
	  free (bp);
	}
    }
  mpz_clear (a);
  mpz_clear (b);

  return 0;
}
