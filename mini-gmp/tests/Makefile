# Note: Requires GNU make

srcdir=.
MINI_GMP_DIR=..

CC = gcc
EXTRA_CFLAGS = -O -Wall -g
CFLAGS = $(EXTRA_CFLAGS) -I$(MINI_GMP_DIR)
LDFLAGS =

LIBS = -lgmp -lm -lmcheck

CHECK_PROGRAMS = t-add t-sub t-mul t-invert t-div t-div_2exp \
	t-double t-gcd t-lcm \
	t-sqrt t-root t-powm t-logops t-bitops t-scan t-str \
	t-reuse

MISC_OBJS = hex-random.o mini-random.o mini-gmp.o

all:

clean:
	rm -f *.o $(CHECK_PROGRAMS)

%: %.c
.c:

# Keep object files
.PRECIOUS: %.o

%.o: %.c $(MINI_GMP_DIR)/mini-gmp.h hex-random.h mini-random.h
	$(CC) $(CFLAGS) -c $< -o $@

mini-gmp.o: $(MINI_GMP_DIR)/mini-gmp.c $(MINI_GMP_DIR)/mini-gmp.h
	$(CC) $(CFLAGS) -c $(MINI_GMP_DIR)/mini-gmp.c -o mini-gmp.o

%: %.o $(MISC_OBJS)
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@

# Missing tests include:
#   mpz_cmp_d, mpz_popcount, mpz_hamdist, mpz_ui_pow_ui

check: $(CHECK_PROGRAMS)
	$(srcdir)/run-tests $(CHECK_PROGRAMS)
