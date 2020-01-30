/* pmpz_bits -- bit manipulation functions
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


/* Function with a more generic signature are defined in pmpz.arith.c */


/* Macro to get and return mp_bitcnt_t
 *
 * the value is defined as unsigned long, so it doesn't fit into an int8 on 64
 * bit platform. We'll convert them to/from mpz in SQL.
 */

#define PGMP_GETARG_BITCNT(tgt,n) \
do { \
    mpz_t _tmp; \
    PGMP_GETARG_MPZ(_tmp, n); \
 \
    if (!(mpz_fits_ulong_p(_tmp))) { \
        ereport(ERROR, ( \
            errcode(ERRCODE_INVALID_PARAMETER_VALUE), \
            errmsg("argument doesn't fit into a bitcount type") )); \
    } \
 \
    tgt = mpz_get_ui(_tmp); \
} while (0)

#define PGMP_RETURN_BITCNT(n) \
do { \
    mpz_t _rv; \
    mpz_init_set_ui(_rv, n); \
    PGMP_RETURN_MPZ(_rv); \
} while (0)


/* Return the largest possible mp_bitcnt_t. Useful for testing the return
 * value of a few other bit manipulation functions as the value depends on the
 * server platform.
 */
PGMP_PG_FUNCTION(pgmp_max_bitcnt)
{
    mp_bitcnt_t     ret;

    ret = ~((mp_bitcnt_t)0);

    PGMP_RETURN_BITCNT(ret);
}

PGMP_PG_FUNCTION(pmpz_popcount)
{
    const mpz_t     z;
    mp_bitcnt_t     ret;

    PGMP_GETARG_MPZ(z, 0);
    ret = mpz_popcount(z);

    PGMP_RETURN_BITCNT(ret);
}

PGMP_PG_FUNCTION(pmpz_hamdist)
{
    const mpz_t     z1;
    const mpz_t     z2;
    mp_bitcnt_t     ret;

    PGMP_GETARG_MPZ(z1, 0);
    PGMP_GETARG_MPZ(z2, 1);
    ret = mpz_hamdist(z1, z2);

    PGMP_RETURN_BITCNT(ret);
}


#define PMPZ_SCAN(f) \
 \
PGMP_PG_FUNCTION(pmpz_ ## f) \
{ \
    const mpz_t     z; \
    mp_bitcnt_t     start; \
 \
    PGMP_GETARG_MPZ(z, 0); \
    PGMP_GETARG_BITCNT(start, 1); \
 \
    PGMP_RETURN_BITCNT(mpz_ ## f(z, start)); \
}

PMPZ_SCAN(scan0)
PMPZ_SCAN(scan1)


/* inplace bit fiddling operations */

#define PMPZ_BIT(f) \
 \
PGMP_PG_FUNCTION(pmpz_ ## f) \
{ \
    const mpz_t     z; \
    mp_bitcnt_t     idx; \
    mpz_t           ret; \
 \
    PGMP_GETARG_MPZ(z, 0); \
    PGMP_GETARG_BITCNT(idx, 1); \
 \
    mpz_init_set(ret, z); \
    mpz_ ## f(ret, idx); \
    PGMP_RETURN_MPZ(ret); \
}

PMPZ_BIT(setbit)
PMPZ_BIT(clrbit)

#if __GMP_MP_RELEASE >= 40200
PMPZ_BIT(combit)
#endif


PGMP_PG_FUNCTION(pmpz_tstbit)
{
    const mpz_t     z;
    mp_bitcnt_t     idx;
    int32           ret;

    PGMP_GETARG_MPZ(z, 0);
    PGMP_GETARG_BITCNT(idx, 1);

    ret = mpz_tstbit(z, idx);
    PG_RETURN_INT32(ret);
}

