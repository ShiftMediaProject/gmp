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
#include <string.h>
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

    { "0x12345678", 4,1,  1,1, 0, { '\x12', '\x34', '\x56', '\x78' } },
    { "0x12345678", 1,1,  4,1, 0, { '\x12', '\x34', '\x56', '\x78' } },
    { "0x12345678", 1,-1, 4,1, 0, { '\x12', '\x34', '\x56', '\x78' } },

    { "0x12345678", 4,-1, 1,-1, 0, { '\x78', '\x56', '\x34', '\x12' } },
    { "0x12345678", 1,1,  4,-1, 0, { '\x78', '\x56', '\x34', '\x12' } },
    { "0x12345678", 1,-1, 4,-1, 0, { '\x78', '\x56', '\x34', '\x12' } },

    { "0x15", 5,1,  1,1, 7, { '\x01', '\x00', '\x01', '\x00', '\x01' } },

    { "0x1FFFFFFFFFFF", 3,1,  2,1,   1, {
        '\x7F','\xFF', '\x7F','\xFF', '\x7F','\xFF' } },
    { "0x1FFFFFFFFFFF", 3,1,  2,-1,  1, {
        '\xFF','\x7F', '\xFF','\x7F', '\xFF','\x7F' } },
    { "0x7",            3,1,  2,1,  15, {
        '\x00','\x01', '\x00','\x01', '\x00','\x01' } },
    { "0x7",            3,1,  2,-1, 15, {
        '\x01','\x00', '\x01','\x00', '\x01','\x00' } },

    { "0x24", 3,1,  2,1,  14, { '\x00','\x02', '\x00','\x01', '\x00','\x00' }},
    { "0x24", 3,1,  2,-1, 14, { '\x02','\x00', '\x01','\x00', '\x00','\x00' }},
    { "0x24", 3,-1, 2,-1, 14, { '\x00','\x00', '\x01','\x00', '\x02','\x00' }},
    { "0x24", 3,-1, 2,1,  14, { '\x00','\x00', '\x00','\x01', '\x00','\x02' }},

    { "0x123456789ABC", 3,1,  2,1,  0, {
        '\x12','\x34', '\x56','\x78', '\x9A','\xBC' } },
    { "0x123456789ABC", 3,-1, 2,1,  0, {
        '\x9A','\xBC', '\x56','\x78', '\x12','\x34' } },
    { "0x123456789ABC", 3,1,  2,-1, 0, {
        '\x34','\x12', '\x78','\x56', '\xBC','\x9A' } },
    { "0x123456789ABC", 3,-1, 2,-1, 0, {
        '\xBC','\x9A', '\x78','\x56', '\x34','\x12' } },

    { "0x112233445566778899AABBCC", 3,1,  4,1,  0,
      { '\x11','\x22','\x33','\x44',
        '\x55','\x66','\x77','\x88',
        '\x99','\xAA','\xBB','\xCC' } },
    { "0x112233445566778899AABBCC", 3,-1, 4,1,  0,
      { '\x99','\xAA','\xBB','\xCC',
        '\x55','\x66','\x77','\x88',
        '\x11','\x22','\x33','\x44' } },
    { "0x112233445566778899AABBCC", 3,1,  4,-1, 0,
      { '\x44','\x33','\x22','\x11',
        '\x88','\x77','\x66','\x55',
        '\xCC','\xBB','\xAA','\x99' } },
    { "0x112233445566778899AABBCC", 3,-1, 4,-1, 0,
      { '\xCC','\xBB','\xAA','\x99',
        '\x88','\x77','\x66','\x55',
        '\x44','\x33','\x22','\x11' } },

    { "0x100120023003400450056006700780089009A00AB00BC00C", 3,1,  8,1,  0,
      { '\x10','\x01','\x20','\x02','\x30','\x03','\x40','\x04',
        '\x50','\x05','\x60','\x06','\x70','\x07','\x80','\x08',
        '\x90','\x09','\xA0','\x0A','\xB0','\x0B','\xC0','\x0C' } },
    { "0x100120023003400450056006700780089009A00AB00BC00C", 3,-1, 8,1,  0,
      { '\x90','\x09','\xA0','\x0A','\xB0','\x0B','\xC0','\x0C',
        '\x50','\x05','\x60','\x06','\x70','\x07','\x80','\x08',
        '\x10','\x01','\x20','\x02','\x30','\x03','\x40','\x04' } },
    { "0x100120023003400450056006700780089009A00AB00BC00C", 3,1,  8,-1, 0,
      { '\x04','\x40','\x03','\x30','\x02','\x20','\x01','\x10',
        '\x08','\x80','\x07','\x70','\x06','\x60','\x05','\x50',
        '\x0C','\xC0','\x0B','\xB0','\x0A','\xA0','\x09','\x90' } },
    { "0x100120023003400450056006700780089009A00AB00BC00C", 3,-1, 8,-1, 0,
      { '\x0C','\xC0','\x0B','\xB0','\x0A','\xA0','\x09','\x90',
        '\x08','\x80','\x07','\x70','\x06','\x60','\x05','\x50',
        '\x04','\x40','\x03','\x30','\x02','\x20','\x01','\x10' } },

    { "0x155555555555555555555555", 3,1,  4,1,  1,
      { '\x55','\x55','\x55','\x55',
        '\x2A','\xAA','\xAA','\xAA',
        '\x55','\x55','\x55','\x55' } },
    { "0x155555555555555555555555", 3,-1,  4,1,  1,
      { '\x55','\x55','\x55','\x55',
        '\x2A','\xAA','\xAA','\xAA',
        '\x55','\x55','\x55','\x55' } },
    { "0x155555555555555555555555", 3,1,  4,-1,  1,
      { '\x55','\x55','\x55','\x55',
        '\xAA','\xAA','\xAA','\x2A',
        '\x55','\x55','\x55','\x55' } },
    { "0x155555555555555555555555", 3,-1,  4,-1,  1,
      { '\x55','\x55','\x55','\x55',
        '\xAA','\xAA','\xAA','\x2A',
        '\x55','\x55','\x55','\x55' } },
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
