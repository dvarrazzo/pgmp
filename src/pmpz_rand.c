/* pmpz_rand -- mpz random numbers
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
#include "utils/memutils.h"         /* for TopMemoryContext */


/* The state of the random number generator.
 *
 * Currently this variable is reset when the library is loaded: this means at
 * every session but would break if the library starts being preloaded. So,
 * TODO: check if there is a way to explicitly allocate this structure per
 * session.
 */
gmp_randstate_t     *pgmp_randstate;


/* Clear the random state if set
 *
 * This macro should be invoked with the TopMemoryContext set as current
 * memory context
 */
#define PGMP_CLEAR_RANDSTATE \
do { \
    if (pgmp_randstate) { \
        gmp_randclear(*pgmp_randstate); \
        pfree(pgmp_randstate); \
        pgmp_randstate = NULL; \
    } \
} while (0)


/* Exit with an error if the random state is not set */

#define PGMP_CHECK_RANDSTATE \
do { \
    if (!pgmp_randstate) { \
        ereport(ERROR, ( \
            errcode(ERRCODE_INVALID_PARAMETER_VALUE), \
            errmsg("random state not initialized") )); \
    } \
} while (0)


/*
 * Random state initialization
 */

#define PGMP_RANDINIT(f, INIT) \
 \
PGMP_PG_FUNCTION(pgmp_ ## f) \
{ \
    gmp_randstate_t     *state; \
    MemoryContext       oldctx; \
 \
    /* palloc and init of the global variable should happen */ \
    /* in the global memory context. */ \
    oldctx = MemoryContextSwitchTo(TopMemoryContext); \
 \
    state = palloc(sizeof(gmp_randstate_t)); \
    INIT(f); \
 \
    /* set the global variable to the initialized state */ \
    PGMP_CLEAR_RANDSTATE; \
    pgmp_randstate = state; \
 \
    MemoryContextSwitchTo(oldctx); \
 \
    PG_RETURN_NULL(); \
}


#define PGMP_RANDINIT_NOARG(f) gmp_ ## f (*state)

PGMP_RANDINIT(randinit_default, PGMP_RANDINIT_NOARG)

#if __GMP_MP_RELEASE >= 40200
PGMP_RANDINIT(randinit_mt,      PGMP_RANDINIT_NOARG)
#endif


#define PGMP_RANDINIT_ACE(f) \
do { \
    const mpz_t         a; \
    unsigned long       c; \
    mp_bitcnt_t         e; \
 \
    PGMP_GETARG_MPZ(a, 0); \
    PGMP_GETARG_ULONG(c, 1); \
    PGMP_GETARG_ULONG(e, 2); \
 \
    gmp_ ## f (*state, a, c, e); \
} while (0)

PGMP_RANDINIT(randinit_lc_2exp, PGMP_RANDINIT_ACE)


#define PGMP_RANDINIT_SIZE(f) \
do { \
    mp_bitcnt_t         size; \
 \
    PGMP_GETARG_ULONG(size, 0); \
 \
    if (!gmp_ ## f (*state, size)) { \
        ereport(ERROR, ( \
            errcode(ERRCODE_INVALID_PARAMETER_VALUE), \
            errmsg("failed to initialized random state with size %lu", \
                size) )); \
    } \
} while (0)

PGMP_RANDINIT(randinit_lc_2exp_size, PGMP_RANDINIT_SIZE)


PGMP_PG_FUNCTION(pgmp_randseed)
{
    const mpz_t         seed;
    MemoryContext       oldctx;

    PGMP_CHECK_RANDSTATE;
    PGMP_GETARG_MPZ(seed, 0);

    /* Switch to the global memory cx in case gmp_randseed allocates */
    oldctx = MemoryContextSwitchTo(TopMemoryContext);

    gmp_randseed(*pgmp_randstate, seed);

    MemoryContextSwitchTo(oldctx);

    PG_RETURN_NULL();
}


/*
 * Random numbers functions
 */

#define PMPZ_RAND_BITCNT(f) \
 \
PGMP_PG_FUNCTION(pmpz_ ## f) \
{ \
    unsigned long   n; \
    mpz_t           ret; \
 \
    PGMP_CHECK_RANDSTATE; \
 \
    PGMP_GETARG_ULONG(n, 0); \
 \
    mpz_init(ret); \
    mpz_ ## f (ret, *pgmp_randstate, n); \
 \
    PGMP_RETURN_MPZ(ret); \
}

PMPZ_RAND_BITCNT(urandomb)
PMPZ_RAND_BITCNT(rrandomb)


PGMP_PG_FUNCTION(pmpz_urandomm)
{
    const mpz_t     n;
    mpz_t           ret;

    PGMP_CHECK_RANDSTATE;

    PGMP_GETARG_MPZ(n, 0);

    mpz_init(ret);
    mpz_urandomm(ret, *pgmp_randstate, n);

    PGMP_RETURN_MPZ(ret);
}

