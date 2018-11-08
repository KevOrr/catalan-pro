#include <stdio.h>
#include <gmp.h>
#include <sys/sysinfo.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
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
    const mpz_t lower;
    const mpz_t upper;
    const mpz_t n;
};

void* catalan_wrapper(void * arg);
void mpz_min(mpz_t dest, mpz_t a, mpz_t b);

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
    //int num_cores = get_nprocs();
    //printf("Number of configured cores: %d\n", num_conf_cores);
    //printf("Number of cores: %d\n", num_cores);


    mpz_t chunk_size;
    mpz_init(chunk_size);
    mpz_add_si(chunk_size, n, num_cores);
    mpz_add_si(chunk_size, chunk_size, -2);
    mpz_div_si(chunk_size, chunk_size, num_cores);

    struct args arg;
    mpz_init_set(arg.n,n);

    mpz_init_set_si(arg.upper, 2);
    mpz_init(arg.lower);
    mpq_init(arg.result);
    mpz_t temp;
    mpz_init(temp);

    for(int i = 0; i < num_cores; i++)
    {
        mpz_set(arg.lower, arg.upper);
        mpz_add(arg.upper, arg.upper, chunk_size);
        mpz_add_ui(temp, n, 1);
        mpz_min(arg.upper, temp, arg.upper);

    }
    
}

void* catalan_wrapper(void * arg)
{
    struct args* arg_ptr = arg;
    calculate_catalan_part(arg_ptr->result, arg_ptr->lower, arg_ptr->upper, arg_ptr->n);
    return NULL;
}

void mpz_min(mpz_t dest, mpz_t a, mpz_t b)
{
    if(mpz_cmp(a,b) < 0)
        mpz_set(dest, a);
    else
        mpz_set(dest, b);
}
