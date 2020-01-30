/* A test program to study the mpz_t structure.
 *
 * Copyright (C) 2011-2020 Daniele Varrazzo
 */

#include <stdio.h>
#include <gmp.h>

int
main(int argc, char **argv)
{
    mpz_t z1, z2;

    mpz_init_set_ui(z1, ~((unsigned long int)0));
    mpz_init(z2);
    mpz_add_ui(z2, z1, 1);

    mpz_out_str(stdout, 10, z2);
    printf("\n");

    return 0;
}
