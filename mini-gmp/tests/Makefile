# Note: Requires GNU make

CC = gcc
OPTFLAGS = -O
CFLAGS = $(OPTFLAGS) -Wall -g -I ..
LDFLAGS =

LIBS = -lgmp -lm -lmcheck

CHECK_PROGRAMS = t-add t-sub t-mul t-invert t-div t-div_2exp \
	t-double t-gcd t-lcm \
	t-sqrt t-powm t-logops t-bitops t-scan t-str

MISC_OBJS = hex-random.o mini-random.o mini-gmp.o

all:

clean:
	rm -f *.o $(CHECK_PROGRAMS)

%: %.c
.c:

# Keep object files
.PRECIOUS: %.o

%.o: %.c ../mini-gmp.h hex-random.h mini-random.h
	$(CC) $(CFLAGS) -c $< -o $@

mini-gmp.o: ../mini-gmp.c ../mini-gmp.h
	$(CC) $(CFLAGS) -c ../mini-gmp.c -o mini-gmp.o

%: %.o $(MISC_OBJS)
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@

# Missing tests include:
#   mpz_cmp_d, mpz_popcount, mpz_hamdist, mpz_ui_pow_ui

check: $(CHECK_PROGRAMS)
	./run-tests $(CHECK_PROGRAMS)
