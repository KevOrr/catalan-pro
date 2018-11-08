#include <stdio.h>
#include <sys/sysinfo.h>
#include <string.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>

#include <pthread.h>
#include <gmp.h>

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

struct args
{
    mpq_t result;
    mpz_t lower;
    mpz_t upper;
    mpz_t n;
};

void* catalan_wrapper(void * arg);
void mpz_min(mpz_t dest, const mpz_t a, const mpz_t b);

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


/*Test Code
    mpz_t lower;
    mpz_init_set_ui(lower, 2);
    mpz_t upper;
    mpz_init_set(upper, n);
    mpz_add_ui(upper, upper, 1);

    mpq_t result;
    mpq_init(result);

    calculate_catalan_part(result, lower, upper, n);

    gmp_printf("%Qd", result);
    printf("\n");*/


    // Do multithreading stuff here...
    int num_cores = get_nprocs_conf();
    if (num_cores < 0)
        EXITERROR();
    //int num_cores = get_nprocs();
    //printf("Number of configured cores: %d\n", num_conf_cores);
    //printf("Number of cores: %d\n", num_cores);


    mpz_t chunk_size;
    mpz_init(chunk_size);
    mpz_sub_ui(chunk_size, n, 1);
    mpz_cdiv_q_ui(chunk_size, chunk_size, num_cores);

    mpz_t temp;
    mpz_init(temp);

    mpz_t lower, upper;
    mpz_init(lower);
    mpz_set_ui(upper, 2);

    struct args results[num_cores];
    pthread_t threads[num_cores];

    for(int i = 0; i < num_cores; i++)
    {
        mpz_set(lower, upper);
        mpz_add(upper, upper, chunk_size);
        mpz_add_ui(temp, n, 1);
        mpz_min(upper, temp, upper);

        mpz_init_set(results[i].n, n);
        mpz_init_set(results[i].lower, lower);
        mpz_init_set(results[i].upper, upper);
        mpq_init(results[i].result);

        pthread_create(&threads[i], NULL, catalan_wrapper, (void*)&results[i]);
    }

    for (int i=0; i<num_cores; i++) {
        if (pthread_join(threads[i], NULL) != 0)
            EXITERROR();

        /* gmp_printf("[%Zd .. %Zd) => %Qd\n", results[i].lower, results[i].upper, results[i].result); */
        mpz_clear(results[i].lower);
        mpz_clear(results[i].upper);
    }

    for (int i=1; i<num_cores; i++)
        mpq_mul(results[0].result, results[0].result, results[i].result);

    gmp_printf("%Qd\n", results[0].result);

}

void* catalan_wrapper(void * arg)
{
    struct args* arg_ptr = arg;
    calculate_catalan_part(arg_ptr->result, arg_ptr->lower, arg_ptr->upper, arg_ptr->n);
    return arg;
}

void mpz_min(mpz_t dest, const mpz_t a, const mpz_t b)
{
    if(mpz_cmp(a,b) < 0)
        mpz_set(dest, a);
    else
        mpz_set(dest, b);
}
