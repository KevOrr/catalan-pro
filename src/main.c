#include <stdio.h>
#include <gmp.h>

#include <string.h>
#include <stdlib.h>
#include "catalan_bignum.h"

#define EXITERROR() error_at_line(errno, errno, __FILE__, __LINE__, "pid %llu", (long long unsigned)getpid())
const char *argv0;

void usage(FILE* f, int err) {
    fprintf(f,
            "USAGE: %s INDEX\n\n"
            "Calculates the INDEXth Catalan number, using multiple threads.\n",
            argv0);
    exit(err);
}

int main(int argc, const char *argv[]) {
    argv0 = argv[0];

    if (argc != 2)
        usage(stderr, 1);

    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
        usage(stdout, 0);

    mpz_t n;
    if (mpz_init_set_str(n, argv[1], 10) != 0) {
        fprintf(stderr, "Could not parse %s as an integer\n", argv[1]);
        exit(1);
    } else if (mpz_sgn(n) < 0) {
        fputs("INDEX must be a non-negative integer\n", stderr);
        exit(1);
    }

    // Do multithreading stuff here...
    //int core_count = sysconf(_SC_NPROCESSORS_ONLN);
    mpz_t lower;
    mpz_init_set_ui(lower, 2);
    mpz_t upper;
    mpz_init_set(upper, n);
    mpz_add_ui(upper, upper, 1);

    mpq_t result;
    mpq_init(result);

    calculate_catalan_part(result, lower, upper, n);

    gmp_printf("%Qd", result);
    printf("\n");
}
