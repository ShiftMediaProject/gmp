/* Test mpz_gcd, mpz_gcdext, mpz_mul, mpz_tdiv_r, mpz_add, mpz_cmp,
   mpz_cmp_ui, mpz_init_set, mpz_set, mpz_clear.

Copyright 1991, 1993, 1994, 1996, 1997, 2000, 2001, 2002 Free Software
Foundation, Inc.

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

#include <stdio.h>
#include <stdlib.h>

#include "gmp.h"
#include "gmp-impl.h"
#include "tests.h"

void dump_abort _PROTO ((int, mpz_t, mpz_t));
void debug_mp _PROTO ((mpz_t, int));

/* Some values that have triggered bugs earlier */
struct test_value {
  const char *g;
  const char *a;
  const char *b;
};

static const struct test_value
gcd_values[] = {
  { "1196", "52303341519632765276", "49433716644" },
  { "4740082048705311538943345362292037678602347311085000707982891522106058"
    "2735763065078988616865606438193460103099042971323306078899664687619951"
    "0636202858506107680258478263756774379689643340947562894108660163354162"
    "374936043828126582328304370681608415",
    "8015326967851188851400293405078493364836916788262508632981042285204035"
    "5004615997127375101624993764238484599679469711647654938657985688930763"
    "7287424827169807289924434877622983992042888129128664489696730194326385"
    "6216391774389540863044605071044379894789674724286157720257605528646713"
    "2847002885102977402320357016075382192479739083401081052515370727048442"
    "3864952170123773854472156128206896173541365243433212241042259067713522"
    "7139214297041190334369882785408312687204541354886134856600715855936646"
    "70012143505055",
    "1064135473460906820987605566282348585048825587555590853309997377997553"
    "2153285969844110945542314457290466401676423923608835657703154250653436"
    "2697286178081669028728243744799767581489930105123394611992402008612146"
    "6315135137233526215819225723644477055479414450428533365154622432782101"
    "2963496044066970313007322547033017615226253884335888150937868475644198"
    "6815046003786634564130674128257615435686074614583459188437821503026771"
    "8607377395872495589370924963118031308777475509749809825552073442922966"
    "622165" },
  { "7429346139342862110654280467601899372609235135422491568134986074126326"
    "7001113896911862980149985840437220402126244505210711847099277120075448"
    "9276466409680247579999500806044547072585736002794426033200059956118884"
    "95178112011641119249678138689458366456903012217376632",
    "1166619509302372427119886202041900960295906253140250506966473400972870"
    "2868415272595956270245640347038981965569437521384521043132169633613249"
    "8249117793331643400421250155539008296413506091003072721582078890313407"
    "5333671575963724738316145960654011473422675335352698105685597874707193"
    "2426934723938037267953469671001286397007933817018297589002719264014779"
    "1093301688603229865571978531814896753002230183721420784350684633945463"
    "6266076193248843510536383667108780261298146636632094760114796626569565"
    "415549047447923471673995780073659239180363707645008",
    "9081412834795553001082053341636149710899746741805643250698622508162481"
    "2757857992575979440934687821237077965077417009086508430199860942145838"
    "8784882177106663022073953743909326410170867657447616086686891699382397"
    "9597831922038265921394586191537362376710605537371781581038873901710686"
    "1166374563798203293488605037880984410966747619198666373922329023390967"
    "3141094488432892460059106640714304595184942245991359196348552390643565"
    "9072266179708878242815458179581502175601955412576082640535181857091687"
    "62738315501898494726705392421906689286392" },
  { NULL, NULL, NULL },
};

int
main (int argc, char **argv)
{
  mpz_t op1, op2, x;
  mpz_t gcd, gcd2, s, t;
  mpz_t temp1, temp2;
  mp_size_t op1_size, op2_size, x_size;
  int i;
  int reps = 1000;
  gmp_randstate_ptr rands;
  mpz_t bs;
  unsigned long bsi, size_range;
  const struct test_value *tv;
  
  tests_start ();
  rands = RANDS;

  mpz_init (bs);

  if (argc == 2)
    reps = atoi (argv[1]);

  mpz_init (op1);
  mpz_init (op2);
  mpz_init (x);
  mpz_init (gcd);
  mpz_init (gcd2);
  mpz_init (temp1);
  mpz_init (temp2);
  mpz_init (s);
  mpz_init (t);

  for (i = -1, tv = gcd_values; tv->g; tv++, i--)
    {
      mpz_set_str (gcd, tv->g, 10);
      mpz_set_str (op1, tv->a, 10);
      mpz_set_str (op2, tv->b, 10);

      mpz_gcd (gcd2, op1, op2);
      if (mpz_cmp (gcd, gcd2) != 0)
	dump_abort (i, op1, op2);
    }
  
  for (i = 0; i < reps; i++)
    {
      mpz_urandomb (bs, rands, 32);
      size_range = mpz_get_ui (bs) % 14 + 2; /* 0..21767 bit operands */

      mpz_urandomb (bs, rands, size_range);
      op1_size = mpz_get_ui (bs);
      mpz_rrandomb (op1, rands, op1_size);

      mpz_urandomb (bs, rands, size_range);
      op2_size = mpz_get_ui (bs);
      mpz_rrandomb (op2, rands, op2_size);

      mpz_urandomb (bs, rands, size_range);
      x_size = mpz_get_ui (bs);
      mpz_rrandomb (x, rands, x_size);

      mpz_urandomb (bs, rands, 2);
      bsi = mpz_get_ui (bs);
      if ((bsi & 1) != 0)
	mpz_neg (op1, op1);
      if ((bsi & 2) != 0)
	mpz_neg (op2, op2);

      /* printf ("%ld %ld\n", SIZ (op1), SIZ (op2)); */

      mpz_mul (op1, op1, x);
      mpz_mul (op2, op2, x);

      mpz_gcd (gcd, op1, op2);
      /* We know GCD will be at least X, since we multiplied both operands
	 with it.  */
      if (mpz_cmp (gcd, x) < 0 && mpz_sgn (op1) != 0 && mpz_sgn (op2) != 0)
	dump_abort (i, op1, op2);

      if (mpz_fits_ulong_p (op2))
	{
	  mpz_gcd_ui (gcd2, op1, mpz_get_ui (op2));
	  if (mpz_cmp (gcd, gcd2))
	    dump_abort (i, op1, op2);
	}

      mpz_gcdext (gcd2, s, t, op1, op2);
      if (mpz_cmp (gcd, gcd2))
	dump_abort (i, op1, op2);

      mpz_gcdext (gcd2, s, NULL, op1, op2);
      if (mpz_cmp (gcd, gcd2))
	dump_abort (i, op1, op2);

      mpz_mul (temp1, s, op1);
      mpz_mul (temp2, t, op2);
      mpz_add (gcd2, temp1, temp2);
      if (mpz_cmp (gcd, gcd2))
	dump_abort (i, op1, op2);
    }

  mpz_clear (bs);
  mpz_clear (op1);
  mpz_clear (op2);
  mpz_clear (x);
  mpz_clear (gcd);
  mpz_clear (gcd2);
  mpz_clear (temp1);
  mpz_clear (temp2);
  mpz_clear (s);
  mpz_clear (t);

  tests_end ();
  exit (0);
}

void
dump_abort (int testnr, mpz_t op1, mpz_t op2)
{
  fprintf (stderr, "ERROR in test %d\n", testnr);
  fprintf (stderr, "op1 = "); debug_mp (op1, -16);
  fprintf (stderr, "op2 = "); debug_mp (op2, -16);
  abort();
}

void
debug_mp (mpz_t x, int base)
{
  mpz_out_str (stderr, base, x); fputc ('\n', stderr);
}
