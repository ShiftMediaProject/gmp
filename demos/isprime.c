/* Return true if the command line argument is a (probable) prime.

Copyright (C) 1999, 2000 Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include <stdio.h>
#include "gmp.h"

main (int argc, char **argv)
{
  mpz_t n;

  mpz_init (n);
  mpz_set_str (n, argv[1], 0);
  return ! mpz_probab_prime_p (n, 10);
}
