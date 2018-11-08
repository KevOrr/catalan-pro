#ifndef _CATALAN_BIGNUM_H
#define _CATALAN_BIGNUM_H

#include <stdio.h>
#include <gmp.h>

void calculate_catalan_part(mpq_t result, const mpz_t lower, const mpz_t upper, const mpz_t n);

#endif
