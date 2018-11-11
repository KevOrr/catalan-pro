#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <error.h>
#include <errno.h>

#include <pthread.h>
#include <gmp.h>

#include "common.h"
#include "catalan_bignum.h"

void usage(FILE *f, int err) {
    fprintf(f,
            "USAGE: %s INDEX\n\n"
            "Calculates the INDEXth Catalan number, using multiple threads.\n",
            argv0);
    exit(err);
}

void* catalan_wrapper(struct args * arg) {
    calculate_catalan_part(arg->result, arg->lower, arg->upper, arg->n);
    return arg;
}

// Sets `dest` to the smaller of `a` or `b`
void mpz_min(mpz_t dest, const mpz_t a, const mpz_t b) {
    if(mpz_cmp(a,b) < 0)
        mpz_set(dest, a);
    else
        mpz_set(dest, b);
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


    // Split workload "equally" amongst all processors: 1 thread per cpu
    int num_cores = get_nprocs_conf();
    if (num_cores <= 0) {
        fputs("get_nprocs_conf() returned a non-positive result", stderr);
        EXITERROR();
    }

    // Calculate number of terms to go to each thread
    mpz_t chunk_size;
    mpz_init(chunk_size);
    mpz_sub_ui(chunk_size, n, 1);
    mpz_cdiv_q_ui(chunk_size, chunk_size, num_cores);

    mpz_t n_plus_1;
    mpz_init(n_plus_1);
    mpz_add_ui(n_plus_1, n, 1);

    mpz_t lower, upper;
    mpz_init(lower);
    mpz_init_set_ui(upper, 2);

    // Args (and results) go in `all_args`
    struct args all_args[num_cores];
    // Thread handles go in `threads`
    pthread_t threads[num_cores];

    // Spawn threads
    for(int i = 0; i < num_cores; i++) {
        mpz_set(lower, upper);
        mpz_add(upper, upper, chunk_size);
        mpz_min(upper, n_plus_1, upper);

        mpq_init(all_args[i].result);
        mpz_init_set(all_args[i].lower, lower);
        mpz_init_set(all_args[i].upper, upper);
        mpz_init_set(all_args[i].n, n);

        /* times[i] = clock(); */
        pthread_create(&threads[i], NULL, (void*(*)(void*))catalan_wrapper, (void*)&all_args[i]);
    }

    for (int i=0; i<num_cores; i++) {
        if (pthread_join(threads[i], NULL) != 0)
            EXITERROR();

        // We can deallocate these since we won't need them for the rest of the program
        mpz_clear(all_args[i].lower);
        mpz_clear(all_args[i].upper);
        mpz_clear(all_args[i].n);
    }

    // Multiply all the partial results together
    for (int i=1; i<num_cores; i++)
        mpq_mul(all_args[0].result, all_args[0].result, all_args[i].result);

    // Output the final result
    gmp_printf("%Qd\n", all_args[0].result);
}
