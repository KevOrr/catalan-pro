#include <vector>
#include <future>
#include <iostream>

#include <stddef.h> // necessary for some old versions of gmp https://gcc.gnu.org/gcc-4.9/porting_to.html
#include <stdio.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <error.h>
#include <errno.h>

#include <gmp.h>
#include <gmpxx.h>

#define EXITERROR() error_at_line(errno, errno, __FILE__, __LINE__, "pid %llu", (long long unsigned)getpid())

const char *argv0;

void usage(FILE *f, int err) {
    fprintf(f,
            "USAGE: %s INDEX\n\n"
            "Calculates the INDEXth Catalan number, using multiple threads.\n",
            argv0);
    exit(err);
}

mpq_class calculate_catalan_part(mpz_class lower, mpz_class upper, mpz_class n) {
    mpq_class result = 1;

    mpz_class k;
    for (mpz_class k = lower; k < upper; ++k)
        result *= (n + k) / (mpq_class)k;

    return result;
}

int main(int argc, const char *argv[]) {
    argv0 = argv[0];

    if (argc != 2)
        usage(stderr, 1);

    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
        usage(stdout, 0);

    mpz_class n;
    try {
        mpz_class temp(argv[1]);
        n = temp;
    } catch (std::invalid_argument &e) {
        fprintf(stderr, "Could not parse %s as an integer\n", argv[1]);
        exit(1);
    }

    if (n < 0) {
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
    mpz_class chunk_size = (n-1);
    mpz_cdiv_q_ui(chunk_size.get_mpz_t(), chunk_size.get_mpz_t(), num_cores);

    mpz_class lower;
    mpz_class upper = 2;

    // Thread handles go in `threads`
    std::vector<std::future<mpq_class>> futures;

    // Spawn threads
    for(int i = 0; i < num_cores; i++) {
        lower = upper;
        upper += chunk_size;
        upper = (n+1 < upper) ? n+1 : upper;
        // std::cout << lower << " " << upper << std::endl;

        futures.push_back(std::async(std::launch::async, calculate_catalan_part, lower, upper, n));
    }

    // Multiply results together
    mpq_class result = 1;
    for (int i=0; i<num_cores; i++)
    {
        mpq_class res = futures[i].get();
        // std::cout << res << std::endl;
        result *= res;
    }

    // Output the final result
    std::cout << result << std::endl;
}
