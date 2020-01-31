/* pmpz_theor -- number theoretic functions
 *
 * Copyright (C) 2011-2020 Daniele Varrazzo
 *
 * This file is part of the PostgreSQL GMP Module
 *
 * The PostgreSQL GMP Module is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3 of the License,
 * or (at your option) any later version.
 *
 * The PostgreSQL GMP Module is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the PostgreSQL GMP Module.  If not, see
 * https://www.gnu.org/licenses/.
 */

#include "pmpz.h"
#include "pgmp-impl.h"

#include "fmgr.h"
#include "funcapi.h"
#include <access/htup_details.h>    /* for heap_form_tuple */


/* Function with a more generic signature are defined in pmpz.arith.c */

PGMP_PG_FUNCTION(pmpz_probab_prime_p)
{
    const mpz_t     z1;
    int             reps;

    PGMP_GETARG_MPZ(z1, 0);
    reps = PG_GETARG_INT32(1);

    PG_RETURN_INT32(mpz_probab_prime_p(z1, reps));
}

PGMP_PG_FUNCTION(pmpz_nextprime)
{
    const mpz_t     z1;
    mpz_t           zf;

    PGMP_GETARG_MPZ(z1, 0);

    mpz_init(zf);

#if __GMP_MP_RELEASE < 40300
    if (UNLIKELY(mpz_sgn(z1) < 0)) {
        mpz_set_ui(zf, 2);
    }
    else
#endif
    {
        mpz_nextprime(zf, z1);
    }

    PGMP_RETURN_MPZ(zf);
}


PGMP_PG_FUNCTION(pmpz_gcdext)
{
    const mpz_t     z1;
    const mpz_t     z2;
    mpz_t           zf;
    mpz_t           zs;
    mpz_t           zt;

    PGMP_GETARG_MPZ(z1, 0);
    PGMP_GETARG_MPZ(z2, 1);

    mpz_init(zf);
    mpz_init(zs);
    mpz_init(zt);
    mpz_gcdext(zf, zs, zt, z1, z2);

    PGMP_RETURN_MPZ_MPZ_MPZ(zf, zs, zt);
}

PGMP_PG_FUNCTION(pmpz_invert)
{
    const mpz_t     z1;
    const mpz_t     z2;
    mpz_t           zf;
    int             ret;

    PGMP_GETARG_MPZ(z1, 0);
    PGMP_GETARG_MPZ(z2, 1);

    mpz_init(zf);
    ret = mpz_invert(zf, z1, z2);

    if (ret != 0) {
        PGMP_RETURN_MPZ(zf);
    }
    else {
        PG_RETURN_NULL();
    }
}


#define PMPZ_INT32(f) \
 \
PGMP_PG_FUNCTION(pmpz_ ## f) \
{ \
    const mpz_t     z1; \
    const mpz_t     z2; \
 \
    PGMP_GETARG_MPZ(z1, 0); \
    PGMP_GETARG_MPZ(z2, 1); \
 \
    PG_RETURN_INT32(mpz_ ## f (z1, z2)); \
}

PMPZ_INT32(jacobi)
PMPZ_INT32(legendre)
PMPZ_INT32(kronecker)


#define PMPZ_ULONG(f) \
 \
PGMP_PG_FUNCTION(pmpz_ ## f) \
{ \
    unsigned long   op; \
    mpz_t           ret; \
 \
    PGMP_GETARG_ULONG(op, 0); \
 \
    mpz_init(ret); \
    mpz_ ## f (ret, op); \
 \
    PGMP_RETURN_MPZ(ret); \
}

PMPZ_ULONG(fac_ui)
PMPZ_ULONG(fib_ui)
PMPZ_ULONG(lucnum_ui)


#define PMPZ_ULONG_MPZ2(f) \
 \
PGMP_PG_FUNCTION(pmpz_ ## f) \
{ \
    unsigned long   op; \
    mpz_t           ret1; \
    mpz_t           ret2; \
 \
    PGMP_GETARG_ULONG(op, 0); \
 \
    mpz_init(ret1); \
    mpz_init(ret2); \
    mpz_ ## f (ret1, ret2, op); \
 \
    PGMP_RETURN_MPZ_MPZ(ret1, ret2); \
}

PMPZ_ULONG_MPZ2(fib2_ui)
PMPZ_ULONG_MPZ2(lucnum2_ui)

