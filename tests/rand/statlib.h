/* statlib.h */

/* This file requires the following header files: gmp.h */

void
mpf_freqt (mpf_t Kp,
	   mpf_t Km,
	   mpf_t X[],
	   const unsigned long int n);
unsigned long int
mpz_freqt (mpf_t V,
	   mpz_t X[],
	   unsigned int imax,
	   const unsigned long int n);

/* Low level functions. */
void
ks (mpf_t Kp,
    mpf_t Km,
    mpf_t X[],
    void (P) (mpf_t, mpf_t),
    const unsigned long int n);

void
ks_table (mpf_t p, mpf_t val, const unsigned int n);

void
x2_table (double t[],
	  unsigned int v);

/* eof statlib.h */
