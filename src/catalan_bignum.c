#include <stdio.h>
#include <gmp.h>

void calculate_catalan_part(mpq_t result, const mpz_t lower, const mpz_t upper, const mpz_t n) {
    mpq_set_ui(result, 1, 1);

    mpz_t num;
    mpz_init(num);
    mpq_t term, denom;
    mpq_inits(term, denom, NULL);

    mpz_t k;
    for (mpz_init_set(k, lower); mpz_cmp(k, upper) < 0; mpz_add_ui(k, k, 1)) {
        mpz_set(num, n);
        mpz_add(num, num, k);

        mpq_set_z(denom, k);

        mpq_set_z(term, num);
        mpq_div(term, term, denom);

        // mpq_canonicalize(term); // Profile this. It may be slowing down or speeding things up
        mpq_mul(result, result, term);
    }

    mpz_clears(num, NULL);
    mpq_clears(term, denom, NULL);
}
