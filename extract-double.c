#include <stdlib.h>

#include "gmp.h"
#include "gmp-impl.h"

#ifdef XDEBUG
#undef _GMP_IEEE_FLOATS
#endif

#ifndef _GMP_IEEE_FLOATS
#define _GMP_IEEE_FLOATS 0
#endif


#define MP_BASE_AS_DOUBLE (2.0 * ((mp_limb_t) 1 << (BITS_PER_MP_LIMB - 1)))

/* Extract a non-negative double in d.  */

int
__gmp_extract_double (mp_ptr rp, double d)
{
  long exp;
  unsigned sc;
  mp_limb_t manh, manl;

  /* BUGS

     1. Should handle Inf and NaN in IEEE specific code.
     2. Handle Inf and NaN also in default code, to avoid hangs.
     3. Generalize to handle all BITS_PER_MP_LIMB >= 32.
     4. This lits is incomplete and misspelled.
   */

  if (d == 0.0)
    {
      rp[0] = 0;
      rp[1] = 0;
#if BITS_PER_MP_LIMB == 32
      rp[2] = 0;
#endif
      return 0;
    }

  if (_GMP_IEEE_FLOATS)
    {
      union ieee_double_extract x;
      x.d = d;

      exp = x.s.exp;
      sc = (unsigned) (exp + 2) % BITS_PER_MP_LIMB;
#if BITS_PER_MP_LIMB == 64
      manl = (((mp_limb_t) 1 << 63)
	      | ((mp_limb_t) x.s.manh << 43) | ((mp_limb_t) x.s.manl << 11));
#else
      manh = ((mp_limb_t) 1 << 31) | (x.s.manh << 11) | (x.s.manl >> 21);
      manl = x.s.manl << 11;
#endif
    }
  else
    {
      /* Unknown (or known to be non-IEEE) double format.  */
      exp = 0;
      if (d >= 1.0)
	{
	  if (d * 0.5 == d)
	    abort ();

	  while (d >= 32768.0)
	    {
	      d *= (1.0 / 65536.0);
	      exp += 16;
	    }
	  while (d >= 1.0)
	    {
	      d *= 0.5;
	      exp += 1;
	    }
	}
      else if (d < 0.5)
	{
	  while (d < (1.0 / 65536.0))
	    {
	      d *=  65536.0;
	      exp -= 16;
	    }
	  while (d < 0.5)
	    {
	      d *= 2.0;
	      exp -= 1;
	    }
	}

      sc = (unsigned) exp % BITS_PER_MP_LIMB;

      d *= MP_BASE_AS_DOUBLE;
#if BITS_PER_MP_LIMB == 64
      manl = d;
#else
      manh = d;
      manl = (d - manh) * MP_BASE_AS_DOUBLE;
#endif

      exp += 1022;
    }

  exp = (unsigned) (exp + 1) / BITS_PER_MP_LIMB - 1024 / BITS_PER_MP_LIMB + 1;

#if BITS_PER_MP_LIMB == 64
  if (sc != 0)
    {
      rp[1] = manl >> (BITS_PER_MP_LIMB - sc);
      rp[0] = manl << sc;
    }
  else
    {
      rp[1] = manl;
      rp[0] = 0;
    }
#else
  if (sc != 0)
    {
      rp[2] = manh >> (BITS_PER_MP_LIMB - sc);
      rp[1] = (manl >> (BITS_PER_MP_LIMB - sc)) | (manh << sc);
      rp[0] = manl << sc;
    }
  else
    {
      rp[2] = manh;
      rp[1] = manl;
      rp[0] = 0;
    }
#endif

  return exp;
}


#ifdef MAIN
main (argc, argv)
     char **argv;
{
  double d = strtod (argv[1], 0);
  int exp;
  mp_limb_t rp[3];
  int i;

  MPN_ZERO (rp, 3);

#ifdef TIMES
  {
    int i;
    long t0 = cputime ();
    for (i = 0; i < TIMES; i++)
      exp = __gmp_extract_double (rp, d);
    printf ("[%lf microsec]\n", (double) (cputime () - t0) / TIMES * 1000);
  }
#endif

  exp = __gmp_extract_double (rp, d);
  for (i = 2; i >= 0; i--)
    printf ("%0*lX ", 2 * sizeof (mp_limb_t), rp[i]);
  printf ("\n");
  printf ("%d\n", exp);
}
#endif
