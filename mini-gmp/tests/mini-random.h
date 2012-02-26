/*

Copyright 2011, Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.  */

#include "mini-gmp.h"
#include "hex-random.h"

void mini_urandomb (mpz_t r, unsigned long bits);
void mini_rrandomb (mpz_t r, unsigned long bits);
void mini_random_op (enum hex_random_op op,  unsigned long maxbits,
		     mpz_t a, mpz_t b, mpz_t r);
void mini_random_op4 (enum hex_random_op op, unsigned long maxbits,
		      mpz_t a, mpz_t b, mpz_t c, mpz_t e);
void mini_random_scan_op (enum hex_random_op op, unsigned long maxbits,
			  mpz_t a, mp_bitcnt_t *b, mp_bitcnt_t *r);
void mini_random_bit_op (enum hex_random_op op, unsigned long maxbits,
			 mpz_t a, mp_bitcnt_t *b, mpz_t r);
