/* Test mpz_export.

Copyright 2002 Free Software Foundation, Inc.

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


void
check_data (void)
{
  static const struct {
    const char  *src;
    size_t      want_count;
    int         order;
    size_t      size;
    int         endian;
    int         nail;
    char        want_data[64];

  } data[] = {

    { "0", 0,1, 1,1, 0 },
    { "0", 0,1, 2,1, 0 },
    { "0", 0,1, 3,1, 0 },

    { "0x12345678", 4,1,  1,1, 0, { 0x12, 0x34, 0x56, 0x78 } },
    { "0x12345678", 1,1,  4,1, 0, { 0x12, 0x34, 0x56, 0x78 } },
    { "0x12345678", 1,-1, 4,1, 0, { 0x12, 0x34, 0x56, 0x78 } },

    { "0x12345678", 4,-1, 1,-1, 0, { 0x78, 0x56, 0x34, 0x12 } },
    { "0x12345678", 1,1,  4,-1, 0, { 0x78, 0x56, 0x34, 0x12 } },
    { "0x12345678", 1,-1, 4,-1, 0, { 0x78, 0x56, 0x34, 0x12 } },

    { "0x15", 5,1,  1,1, 7, { 0x01, 0x00, 0x01, 0x00, 0x01 } },

    { "0x1FFFFFFFFFFF", 3,1,  2,1,   1, { 0x7F,0xFF, 0x7F,0xFF, 0x7F,0xFF } },
    { "0x1FFFFFFFFFFF", 3,1,  2,-1,  1, { 0xFF,0x7F, 0xFF,0x7F, 0xFF,0x7F } },
    { "0x7",            3,1,  2,1,  15, { 0x00,0x01, 0x00,0x01, 0x00,0x01 } },
    { "0x7",            3,1,  2,-1, 15, { 0x01,0x00, 0x01,0x00, 0x01,0x00 } },

    { "0x24", 3,1,  2,1,  14, { 0x00,0x02, 0x00,0x01, 0x00,0x00 } },
    { "0x24", 3,1,  2,-1, 14, { 0x02,0x00, 0x01,0x00, 0x00,0x00 } },
    { "0x24", 3,-1, 2,-1, 14, { 0x00,0x00, 0x01,0x00, 0x02,0x00 } },
    { "0x24", 3,-1, 2,1,  14, { 0x00,0x00, 0x00,0x01, 0x00,0x02 } },

    { "0x123456789ABC", 3,1,  2,1,  0, { 0x12,0x34, 0x56,0x78, 0x9A,0xBC } },
    { "0x123456789ABC", 3,-1, 2,1,  0, { 0x9A,0xBC, 0x56,0x78, 0x12,0x34 } },
    { "0x123456789ABC", 3,1,  2,-1, 0, { 0x34,0x12, 0x78,0x56, 0xBC,0x9A } },
    { "0x123456789ABC", 3,-1, 2,-1, 0, { 0xBC,0x9A, 0x78,0x56, 0x34,0x12 } },

    { "0x112233445566778899AABBCC", 3,1,  4,1,  0,
      { 0x11,0x22,0x33,0x44, 0x55,0x66,0x77,0x88, 0x99,0xAA,0xBB,0xCC } },
    { "0x112233445566778899AABBCC", 3,-1, 4,1,  0,
      { 0x99,0xAA,0xBB,0xCC, 0x55,0x66,0x77,0x88, 0x11,0x22,0x33,0x44 } },
    { "0x112233445566778899AABBCC", 3,1,  4,-1, 0,
      { 0x44,0x33,0x22,0x11, 0x88,0x77,0x66,0x55, 0xCC,0xBB,0xAA,0x99 } },
    { "0x112233445566778899AABBCC", 3,-1, 4,-1, 0,
      { 0xCC,0xBB,0xAA,0x99, 0x88,0x77,0x66,0x55, 0x44,0x33,0x22,0x11 } },

    { "0x100120023003400450056006700780089009A00AB00BC00C", 3,1,  8,1,  0,
      { 0x10,0x01,0x20,0x02,0x30,0x03,0x40,0x04,
        0x50,0x05,0x60,0x06,0x70,0x07,0x80,0x08,
        0x90,0x09,0xA0,0x0A,0xB0,0x0B,0xC0,0x0C } },
    { "0x100120023003400450056006700780089009A00AB00BC00C", 3,-1, 8,1,  0,
      { 0x90,0x09,0xA0,0x0A,0xB0,0x0B,0xC0,0x0C,
        0x50,0x05,0x60,0x06,0x70,0x07,0x80,0x08,
        0x10,0x01,0x20,0x02,0x30,0x03,0x40,0x04 } },
    { "0x100120023003400450056006700780089009A00AB00BC00C", 3,1,  8,-1, 0,
      { 0x04,0x40,0x03,0x30,0x02,0x20,0x01,0x10,
        0x08,0x80,0x07,0x70,0x06,0x60,0x05,0x50,
        0x0C,0xC0,0x0B,0xB0,0x0A,0xA0,0x09,0x90 } },
    { "0x100120023003400450056006700780089009A00AB00BC00C", 3,-1, 8,-1, 0,
      { 0x0C,0xC0,0x0B,0xB0,0x0A,0xA0,0x09,0x90,
        0x08,0x80,0x07,0x70,0x06,0x60,0x05,0x50,
        0x04,0x40,0x03,0x30,0x02,0x20,0x01,0x10 } },

    { "0x155555555555555555555555", 3,1,  4,1,  1,
      { 0x55,0x55,0x55,0x55, 0x2A,0xAA,0xAA,0xAA, 0x55,0x55,0x55,0x55 } },
    { "0x155555555555555555555555", 3,-1,  4,1,  1,
      { 0x55,0x55,0x55,0x55, 0x2A,0xAA,0xAA,0xAA, 0x55,0x55,0x55,0x55 } },
    { "0x155555555555555555555555", 3,1,  4,-1,  1,
      { 0x55,0x55,0x55,0x55, 0xAA,0xAA,0xAA,0x2A, 0x55,0x55,0x55,0x55 } },
    { "0x155555555555555555555555", 3,-1,  4,-1,  1,
      { 0x55,0x55,0x55,0x55, 0xAA,0xAA,0xAA,0x2A, 0x55,0x55,0x55,0x55 } },
  };

  char    buf[sizeof(data[0].src) + sizeof (mp_limb_t) + 128];
  char    *got_data;
  void    *ret;
  size_t  align, got_count, j;
  int     i, error = 0;
  mpz_t   src;

  mpz_init (src);

  for (i = 0; i < numberof (data); i++)
    {
      for (align = 0; align < sizeof (mp_limb_t); align++)
        {
          mpz_set_str_or_abort (src, data[i].src, 0);
          MPZ_CHECK_FORMAT (src);
          got_data = buf + align;

          ASSERT_ALWAYS (data[i].want_count * data[i].size + align
                         <= sizeof (buf));

          memset (got_data, '\0', data[i].want_count * data[i].size);
          ret = mpz_export (got_data, &got_count, data[i].order,
                            data[i].size, data[i].endian, data[i].nail, src);

          if (ret != got_data)
            {
              printf ("return doesn't equal given pointer\n");
              error = 1;
            }
          if (got_count != data[i].want_count)
            {
              printf ("wrong count\n");
              error = 1;
            }
          if (memcmp (got_data, data[i].want_data, got_count * data[i].size) != 0)
            {
              printf ("wrong result data\n");
              error = 1;
            }
          if (error)
            {
              printf ("    at data[%d]  align=%d\n", i, (int) align);
              printf ("    src \"%s\"\n", data[i].src);
              mpz_trace ("    src", src);
              printf ("    order=%d  size=%u endian=%d nail=%u\n",
                      data[i].order,
                      data[i].size, data[i].endian, data[i].nail);
              printf ("    want count %u\n", data[i].want_count);
              printf ("    got count  %u\n", got_count);
              printf ("    want");
              for (j = 0; j < data[i].want_count*data[i].size; j++)
                printf (" 0x%02X,", (unsigned) (unsigned char) data[i].want_data[j]);
              printf ("\n");
              printf ("    got ");
              for (j = 0; j < got_count*data[i].size; j++)
                printf (" 0x%02X,", (unsigned) (unsigned char) got_data[j]);
              printf ("\n");
              abort ();
            }
        }
    }
  mpz_clear (src);
}


int
main (void)
{
  tests_start ();

  mp_trace_base = -16;
  check_data ();

  tests_end ();
  exit (0);
}
