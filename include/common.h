#ifndef _COMMON_H_
#define _COMMON_H_

#include <gmp.h>

#define EXITERROR() error_at_line(errno, errno, __FILE__, __LINE__, "pid %llu", (long long unsigned)getpid())
const char *argv0;

struct args {
    mpq_t result;
    mpz_t lower;
    mpz_t upper;
    mpz_t n;
};


#endif
