#include <stdio.h>
#include "gmp.h"

main (argc, argv)
     int argc;
     char **argv;
{
  mpz_t n;

  mpz_init (n);
  mpz_set_str (n, argv[1], 0);
  return ! mpz_probab_prime_p (n, 10);
}
