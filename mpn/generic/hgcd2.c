/* hgcd2.c

   THE FUNCTIONS IN THIS FILE ARE INTERNAL WITH MUTABLE INTERFACES.  IT IS ONLY
   SAFE TO REACH THEM THROUGH DOCUMENTED INTERFACES.  IN FACT, IT IS ALMOST
   GUARANTEED THAT THEY'LL CHANGE OR DISAPPEAR IN A FUTURE GNU MP RELEASE.

Copyright 1996, 1998, 2000-2004, 2008, 2012, 2019 Free Software Foundation,
Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of either:

  * the GNU Lesser General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your
    option) any later version.

or

  * the GNU General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any
    later version.

or both in parallel, as here.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received copies of the GNU General Public License and the
GNU Lesser General Public License along with the GNU MP Library.  If not,
see https://www.gnu.org/licenses/.  */

#include "gmp-impl.h"
#include "longlong.h"

#ifndef HGCD2_DIV1_METHOD
#define HGCD2_DIV1_METHOD 3
#endif

#ifndef HGCD2_DIV2_METHOD
#define HGCD2_DIV2_METHOD 2
#endif

#if GMP_NAIL_BITS != 0
#error Nails not implemented
#endif

#if HAVE_NATIVE_mpn_div_11

#define div1 mpn_div_11
/* Single-limb division optimized for small quotients.
   Returned value holds d0 = r, d1 = q. */
mp_double_limb_t div1 (mp_limb_t, mp_limb_t);

#elif HGCD2_DIV1_METHOD == 1

static inline mp_double_limb_t
div1 (mp_limb_t n0, mp_limb_t d0)
{
  mp_double_limb_t res;
  res.d1 = n0 / d0;
  res.d0 = n0 - res.d1 * d0;

  return res;
}

#elif HGCD2_DIV1_METHOD == 2

static mp_double_limb_t
div1 (mp_limb_t n0, mp_limb_t d0)
{
  mp_double_limb_t res;
  int ncnt, dcnt, cnt;
  mp_limb_t q;
  mp_limb_t mask;

  ASSERT (n0 >= d0);

  count_leading_zeros (ncnt, n0);
  count_leading_zeros (dcnt, d0);
  cnt = dcnt - ncnt;

  d0 <<= cnt;

  q = -(mp_limb_t) (n0 >= d0);
  n0 -= d0 & q;
  d0 >>= 1;
  q = -q;

  while (--cnt >= 0)
    {
      mask = -(mp_limb_t) (n0 >= d0);
      n0 -= d0 & mask;
      d0 >>= 1;
      q = (q << 1) - mask;
    }

  res.d0 = n0;
  res.d1 = q;
  return res;
}

#elif HGCD2_DIV1_METHOD == 3

static inline mp_double_limb_t
div1 (mp_limb_t n0, mp_limb_t d0)
{
  mp_double_limb_t res;
  if (UNLIKELY ((d0 >> (GMP_LIMB_BITS - 3)) != 0)
      || UNLIKELY (n0 >= (d0 << 3)))
    {
      res.d1 = n0 / d0;
      res.d0 = n0 - res.d1 * d0;
    }
  else
    {
      mp_limb_t q, mask;

      d0 <<= 2;

      mask = -(mp_limb_t) (n0 >= d0);
      n0 -= d0 & mask;
      q = 4 & mask;

      d0 >>= 1;
      mask = -(mp_limb_t) (n0 >= d0);
      n0 -= d0 & mask;
      q += 2 & mask;

      d0 >>= 1;
      mask = -(mp_limb_t) (n0 >= d0);
      n0 -= d0 & mask;
      q -= mask;

      res.d0 = n0;
      res.d1 = q;
    }
  return res;
}

#elif HGCD2_DIV1_METHOD == 4

/* Table quotients.  We extract the NBITS most significant bits of the
   numerator limb, and the corresponding bits from the divisor limb, and use
   these to form an index into the table.  We only get NBITS/2 bits of
   progress, and the table is very large.  The hit rate for the table with
   NBITS = 6 is about 80%, meaning we get a lot of poorly predicated branches.

   Possible improvements:

   * Don't identify the failure case early, by looking at the extracted divisor
     bits.  Instead do the table thing, and only invoke the slow path if the
     remainder is off after one conditional adjustment.  (This will require a
     table where the 0 entries are properly filled in.)

   * Perhaps extract the highest NBITS of the divisor instead of the same bits
     as from the numerator.  That would require another count_leading_zeros,
     and a post-multiply shift of the quotient.

   * Compress the table.  Its values are tiny now, and there are lots of zero
     entries (which are never used).

   * Round the table entries more cleverly?
*/

#define NBITS 6		/* Must be even. */
static const unsigned char tab[1 << (2 * NBITS - 1)] = {
0,0,0,0,0,0,0,0,3,3,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,3,3,3,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,3,3,3,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,3,3,3,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,4,3,3,3,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,4,3,3,3,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,4,3,3,3,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,4,3,3,3,3,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,4,4,3,3,3,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,4,4,3,3,3,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,4,4,3,3,3,3,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,4,4,3,3,3,3,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,4,4,4,3,3,3,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,5,4,4,3,3,3,3,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,5,4,4,3,3,3,3,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,5,4,4,3,3,3,3,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,5,4,4,4,3,3,3,3,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,5,4,4,4,3,3,3,3,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,5,5,4,4,3,3,3,3,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,5,5,4,4,3,3,3,3,3,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,5,5,4,4,4,3,3,3,3,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,5,5,4,4,4,3,3,3,3,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,6,5,4,4,4,3,3,3,3,3,2,2,2,2,2,2,2,2,2,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,6,5,5,4,4,3,3,3,3,3,2,2,2,2,2,2,2,2,2,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,6,5,5,4,4,4,3,3,3,3,2,2,2,2,2,2,2,2,2,2,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,6,5,5,4,4,4,3,3,3,3,3,2,2,2,2,2,2,2,2,2,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,6,5,5,4,4,4,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,6,5,5,4,4,4,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
0,0,0,0,0,0,0,0,6,6,5,5,4,4,4,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,
0,0,0,0,0,0,0,0,6,6,5,5,4,4,4,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
0,0,0,0,0,0,0,0,6,6,5,5,4,4,4,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
0,0,0,0,0,0,0,0,7,6,5,5,4,4,4,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0
};
static const unsigned char *tabp = tab - (1 << (NBITS - 1) << NBITS);

mp_double_limb_t
div1 (mp_limb_t n0, mp_limb_t d0)
{
  int ncnt;
  size_t nbi, dbi;
  mp_limb_t q0;
  mp_limb_t r0;
  mp_double_limb_t res;

  count_leading_zeros (ncnt, n0);
  nbi = n0 << ncnt >> (64 - NBITS);
  dbi = d0 << ncnt >> (64 - NBITS);

  if (UNLIKELY (dbi < (1 << NBITS/2)))
    {
      q0 = n0 / d0;
      r0 = n0 - q0 * d0;
    }
  else
    {
      mp_limb_t mask;
      q0 = tabp[(nbi << NBITS) + dbi];
      r0 = n0 - q0 * d0;
      mask = -(mp_limb_t) (r0 >= d0);
      q0 -= mask;
      r0 -= d0 & mask;
    }

  res.d1 = q0;
  res.d0 = r0;
  return res;
}


#elif HGCD2_DIV1_METHOD == 5

/* Table inverses of divisors.  We don't bother with suppressing the msb from
   the tables.  We index with the NBITS most significant divisor bits,
   including the always-set highest bit, but use addressing trickery via tabp
   to suppress it.  */

#ifndef NBITS
#define NBITS 8
#endif

#if NBITS == 7
static const unsigned char tab[64] = {
252,248,244,240,236,233,230,226,223,220,217,214,211,208,206,203,
200,198,195,193,191,188,186,184,182,180,178,176,174,172,170,168,
166,164,163,161,159,158,156,155,153,152,150,149,147,146,144,143,
142,140,139,138,137,136,134,133,132,131,130,129,128,127,126,125
};
static const unsigned char *tabp = tab - (1 << (NBITS - 1));
#elif NBITS == 8
static const unsigned short tab[128] = {
508,504,500,496,492,488,485,481,477,474,470,467,464,460,457,454,
451,447,444,441,438,435,432,430,427,424,421,418,416,413,410,408,
405,403,400,398,395,393,390,388,386,383,381,379,377,374,372,370,
368,366,364,362,360,358,356,354,352,350,348,346,344,342,340,339,
337,335,333,332,330,328,326,325,323,322,320,318,317,315,314,312,
311,309,308,306,305,303,302,300,299,298,296,295,293,292,291,289,
288,287,285,284,283,282,280,279,278,277,276,274,273,272,271,270,
269,267,266,265,264,263,262,261,260,259,258,257,256,255,254,253
};
static const unsigned short *tabp = tab - (1 << (NBITS - 1));
#elif NBITS == 9
static const unsigned short tab[256] = {
1020,1016,1012,1008,1004,1000,996,992,988,985,981,977,974,970,966,963,
959,956,952,949,945,942,938,935,932,928,925,922,919,915,912,909,
906,903,899,896,893,890,887,884,881,878,875,872,869,866,864,861,
858,855,852,849,847,844,841,838,836,833,830,828,825,822,820,817,
815,812,810,807,805,802,800,797,795,792,790,787,785,783,780,778,
776,773,771,769,767,764,762,760,758,755,753,751,749,747,744,742,
740,738,736,734,732,730,728,726,724,722,720,718,716,714,712,710,
708,706,704,702,700,698,696,695,693,691,689,687,685,684,682,680,
678,676,675,673,671,669,668,666,664,663,661,659,657,656,654,653,
651,649,648,646,644,643,641,640,638,636,635,633,632,630,629,627,
626,624,623,621,620,618,617,615,614,612,611,609,608,607,605,604,
602,601,600,598,597,595,594,593,591,590,589,587,586,585,583,582,
581,579,578,577,575,574,573,572,570,569,568,567,565,564,563,562,
560,559,558,557,556,554,553,552,551,550,549,547,546,545,544,543,
542,540,539,538,537,536,535,534,533,532,530,529,528,527,526,525,
524,523,522,521,520,519,518,517,516,515,514,513,512,511,510,509
};
static const unsigned short *tabp = tab - (1 << (NBITS - 1));
#else
#error No table for provided NBITS
#endif


mp_double_limb_t
div1 (mp_limb_t n0, mp_limb_t d0)
{
  int ncnt, dcnt;
  mp_limb_t q0;
  mp_limb_t r0;
  mp_double_limb_t res;

  count_leading_zeros (ncnt, n0);
  count_leading_zeros (dcnt, d0);

  if (UNLIKELY (dcnt - ncnt >= NBITS - 2))
    {
      q0 = n0 / d0;
      r0 = n0 - q0 * d0;
    }
  else
    {
      size_t dbi = d0 << dcnt >> (GMP_LIMB_BITS - NBITS);
      mp_limb_t qbits = tabp[dbi];
      mp_limb_t mask;
      q0 = ((n0 << ncnt) >> (NBITS + 1)) * qbits >> (GMP_LIMB_BITS - 1 + ncnt - dcnt);
      r0 = n0 - q0 * d0;
      mask = -(mp_limb_t) (r0 >= d0);
      q0 -= mask;
      r0 -= d0 & mask;
    }

  res.d1 = q0;
  res.d0 = r0;
  return res;
}

#else
#error Unknown HGCD2_DIV1_METHOD
#endif

#if HAVE_NATIVE_mpn_div_22

#define div2 mpn_div_22
/* Two-limb division optimized for small quotients.  */
mp_limb_t div2 (mp_ptr, mp_limb_t, mp_limb_t, mp_limb_t, mp_limb_t);

#elif HGCD2_DIV2_METHOD == 1

static mp_limb_t
div2 (mp_ptr rp,
      mp_limb_t n1, mp_limb_t n0,
      mp_limb_t d1, mp_limb_t d0)
{
  mp_double_limb_t rq = div1 (n1, d1);
  if (UNLIKELY (rq.d1 > d1))
    {
      mp_limb_t n2, q, t1, t0;
      int c;

      /* Normalize */
      count_leading_zeros (c, d1);
      ASSERT (c > 0);

      n2 = n1 >> (GMP_LIMB_BITS - c);
      n1 = (n1 << c) | (n0 >> (GMP_LIMB_BITS - c));
      n0 <<= c;
      d1 = (d1 << c) | (d0 >> (GMP_LIMB_BITS - c));
      d0 <<= c;

      udiv_qrnnd (q, n1, n2, n1, d1);
      umul_ppmm (t1, t0, q, d0);
      if (t1 > n1 || (t1 == n1 && t0 > n0))
	{
	  ASSERT (q > 0);
	  q--;
	  sub_ddmmss (t1, t0, t1, t0, d1, d0);
	}
      sub_ddmmss (n1, n0, n1, n0, t1, t0);

      /* Undo normalization */
      rp[0] = (n0 >> c) | (n1 << (GMP_LIMB_BITS - c));
      rp[1] = n1 >> c;

      return q;
    }
  else
    {
      mp_limb_t q, t1, t0;
      n1 = rq.d0;
      q = rq.d1;
      umul_ppmm (t1, t0, q, d0);
      if (UNLIKELY (t1 >= n1) && (t1 > n1 || t0 > n0))
	{
	  ASSERT (q > 0);
	  q--;
	  sub_ddmmss (t1, t0, t1, t0, d1, d0);
	}
      sub_ddmmss (rp[1], rp[0], n1, n0, t1, t0);
      return q;
    }
}

#elif HGCD2_DIV2_METHOD == 2

/* Bit-wise div2. Relies on fast count_leading_zeros. */
static mp_limb_t
div2 (mp_ptr rp,
      mp_limb_t n1, mp_limb_t n0,
      mp_limb_t d1, mp_limb_t d0)
{
  mp_limb_t q = 0;
  int ncnt;
  int dcnt;

  count_leading_zeros (ncnt, n1);
  count_leading_zeros (dcnt, d1);
  dcnt -= ncnt;

  d1 = (d1 << dcnt) + (d0 >> 1 >> (GMP_LIMB_BITS - 1 - dcnt));
  d0 <<= dcnt;

  do
    {
      mp_limb_t mask;
      q <<= 1;
      if (UNLIKELY (n1 == d1))
	mask = -(n0 >= d0);
      else
	mask = -(n1 > d1);

      q -= mask;

      sub_ddmmss (n1, n0, n1, n0, mask & d1, mask & d0);

      d0 = (d1 << (GMP_LIMB_BITS - 1)) | (d0 >> 1);
      d1 = d1 >> 1;
    }
  while (dcnt--);

  rp[0] = n0;
  rp[1] = n1;

  return q;
}
#else
#error Unknown HGCD2_DIV2_METHOD
#endif

/* Reduces a,b until |a-b| (almost) fits in one limb + 1 bit. Constructs
   matrix M. Returns 1 if we make progress, i.e. can perform at least
   one subtraction. Otherwise returns zero. */

/* FIXME: Possible optimizations:

   The div2 function starts with checking the most significant bit of
   the numerator. We can maintained normalized operands here, call
   hgcd with normalized operands only, which should make the code
   simpler and possibly faster.

   Experiment with table lookups on the most significant bits.

   This function is also a candidate for assembler implementation.
*/
int
mpn_hgcd2 (mp_limb_t ah, mp_limb_t al, mp_limb_t bh, mp_limb_t bl,
	   struct hgcd_matrix1 *M)
{
  mp_limb_t u00, u01, u10, u11;

  if (ah < 2 || bh < 2)
    return 0;

  if (ah > bh || (ah == bh && al > bl))
    {
      sub_ddmmss (ah, al, ah, al, bh, bl);
      if (ah < 2)
	return 0;

      u00 = u01 = u11 = 1;
      u10 = 0;
    }
  else
    {
      sub_ddmmss (bh, bl, bh, bl, ah, al);
      if (bh < 2)
	return 0;

      u00 = u10 = u11 = 1;
      u01 = 0;
    }

  if (ah < bh)
    goto subtract_a;

  for (;;)
    {
      ASSERT (ah >= bh);
      if (ah == bh)
	goto done;

      if (ah < (CNST_LIMB(1) << (GMP_LIMB_BITS / 2)))
	{
	  ah = (ah << (GMP_LIMB_BITS / 2) ) + (al >> (GMP_LIMB_BITS / 2));
	  bh = (bh << (GMP_LIMB_BITS / 2) ) + (bl >> (GMP_LIMB_BITS / 2));

	  break;
	}

      /* Subtract a -= q b, and multiply M from the right by (1 q ; 0
	 1), affecting the second column of M. */
      ASSERT (ah > bh);
      sub_ddmmss (ah, al, ah, al, bh, bl);

      if (ah < 2)
	goto done;

      if (ah <= bh)
	{
	  /* Use q = 1 */
	  u01 += u00;
	  u11 += u10;
	}
      else
	{
	  mp_limb_t r[2];
	  mp_limb_t q = div2 (r, ah, al, bh, bl);
	  al = r[0]; ah = r[1];
	  if (ah < 2)
	    {
	      /* A is too small, but q is correct. */
	      u01 += q * u00;
	      u11 += q * u10;
	      goto done;
	    }
	  q++;
	  u01 += q * u00;
	  u11 += q * u10;
	}
    subtract_a:
      ASSERT (bh >= ah);
      if (ah == bh)
	goto done;

      if (bh < (CNST_LIMB(1) << (GMP_LIMB_BITS / 2)))
	{
	  ah = (ah << (GMP_LIMB_BITS / 2) ) + (al >> (GMP_LIMB_BITS / 2));
	  bh = (bh << (GMP_LIMB_BITS / 2) ) + (bl >> (GMP_LIMB_BITS / 2));

	  goto subtract_a1;
	}

      /* Subtract b -= q a, and multiply M from the right by (1 0 ; q
	 1), affecting the first column of M. */
      sub_ddmmss (bh, bl, bh, bl, ah, al);

      if (bh < 2)
	goto done;

      if (bh <= ah)
	{
	  /* Use q = 1 */
	  u00 += u01;
	  u10 += u11;
	}
      else
	{
	  mp_limb_t r[2];
	  mp_limb_t q = div2 (r, bh, bl, ah, al);
	  bl = r[0]; bh = r[1];
	  if (bh < 2)
	    {
	      /* B is too small, but q is correct. */
	      u00 += q * u01;
	      u10 += q * u11;
	      goto done;
	    }
	  q++;
	  u00 += q * u01;
	  u10 += q * u11;
	}
    }

  /* NOTE: Since we discard the least significant half limb, we don't get a
     truly maximal M (corresponding to |a - b| < 2^{GMP_LIMB_BITS +1}). */
  /* Single precision loop */
  for (;;)
    {
      ASSERT (ah >= bh);

      ah -= bh;
      if (ah < (CNST_LIMB (1) << (GMP_LIMB_BITS / 2 + 1)))
	break;

      if (ah <= bh)
	{
	  /* Use q = 1 */
	  u01 += u00;
	  u11 += u10;
	}
      else
	{
	  mp_double_limb_t rq = div1 (ah, bh);
	  mp_limb_t q = rq.d1;
	  ah = rq.d0;

	  if (ah < (CNST_LIMB(1) << (GMP_LIMB_BITS / 2 + 1)))
	    {
	      /* A is too small, but q is correct. */
	      u01 += q * u00;
	      u11 += q * u10;
	      break;
	    }
	  q++;
	  u01 += q * u00;
	  u11 += q * u10;
	}
    subtract_a1:
      ASSERT (bh >= ah);

      bh -= ah;
      if (bh < (CNST_LIMB (1) << (GMP_LIMB_BITS / 2 + 1)))
	break;

      if (bh <= ah)
	{
	  /* Use q = 1 */
	  u00 += u01;
	  u10 += u11;
	}
      else
	{
	  mp_double_limb_t rq = div1 (bh, ah);
	  mp_limb_t q = rq.d1;
	  bh = rq.d0;

	  if (bh < (CNST_LIMB(1) << (GMP_LIMB_BITS / 2 + 1)))
	    {
	      /* B is too small, but q is correct. */
	      u00 += q * u01;
	      u10 += q * u11;
	      break;
	    }
	  q++;
	  u00 += q * u01;
	  u10 += q * u11;
	}
    }

 done:
  M->u[0][0] = u00; M->u[0][1] = u01;
  M->u[1][0] = u10; M->u[1][1] = u11;

  return 1;
}

/* Sets (r;b) = (a;b) M, with M = (u00, u01; u10, u11). Vector must
 * have space for n + 1 limbs. Uses three buffers to avoid a copy*/
mp_size_t
mpn_hgcd_mul_matrix1_vector (const struct hgcd_matrix1 *M,
			     mp_ptr rp, mp_srcptr ap, mp_ptr bp, mp_size_t n)
{
  mp_limb_t ah, bh;

  /* Compute (r,b) <-- (u00 a + u10 b, u01 a + u11 b) as

     r  = u00 * a
     r += u10 * b
     b *= u11
     b += u01 * a
  */

#if HAVE_NATIVE_mpn_addaddmul_1msb0
  ah = mpn_addaddmul_1msb0 (rp, ap, bp, n, M->u[0][0], M->u[1][0]);
  bh = mpn_addaddmul_1msb0 (bp, bp, ap, n, M->u[1][1], M->u[0][1]);
#else
  ah =     mpn_mul_1 (rp, ap, n, M->u[0][0]);
  ah += mpn_addmul_1 (rp, bp, n, M->u[1][0]);

  bh =     mpn_mul_1 (bp, bp, n, M->u[1][1]);
  bh += mpn_addmul_1 (bp, ap, n, M->u[0][1]);
#endif
  rp[n] = ah;
  bp[n] = bh;

  n += (ah | bh) > 0;
  return n;
}
