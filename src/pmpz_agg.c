/* pmpz_agg -- mpz aggregation functions
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


/* Convert an inplace accumulator into a pmpz structure.
 *
 * This function is strict, so don't care about NULLs
 */
PGMP_PG_FUNCTION(_pmpz_from_agg)
{
    mpz_t       *a;

    a = (mpz_t *)PG_GETARG_POINTER(0);
    PGMP_RETURN_MPZ(*a);
}


/* Macro to create an accumulation function from a gmp operator.
 *
 * This function can't be strict because the internal state is not compatible
 * with the base type.
 */
#define PMPZ_AGG(op, BLOCK, rel) \
 \
PGMP_PG_FUNCTION(_pmpz_agg_ ## op) \
{ \
    mpz_t           *a; \
    const mpz_t     z; \
    MemoryContext   oldctx; \
    MemoryContext   aggctx; \
 \
    if (UNLIKELY(!AggCheckCallContext(fcinfo, &aggctx))) \
    { \
        ereport(ERROR, \
            (errcode(ERRCODE_DATA_EXCEPTION), \
            errmsg("_mpz_agg_" #op " can only be called in accumulation"))); \
    } \
 \
    if (PG_ARGISNULL(1)) { \
        if (PG_ARGISNULL(0)) { \
            PG_RETURN_NULL(); \
        } \
        else { \
            PG_RETURN_POINTER(PG_GETARG_POINTER(0)); \
        } \
    } \
 \
    PGMP_GETARG_MPZ(z, 1); \
 \
    oldctx = MemoryContextSwitchTo(aggctx); \
 \
    if (LIKELY(!PG_ARGISNULL(0))) { \
        a = (mpz_t *)PG_GETARG_POINTER(0); \
        BLOCK(op, rel); \
    } \
    else {                      /* uninitialized */ \
        a = (mpz_t *)palloc(sizeof(mpz_t)); \
        mpz_init_set(*a, z); \
    } \
 \
    MemoryContextSwitchTo(oldctx); \
 \
    PG_RETURN_POINTER(a); \
}


#define PMPZ_AGG_OP(op, rel) \
    mpz_ ## op (*a, *a, z)

PMPZ_AGG(add, PMPZ_AGG_OP, 0)
PMPZ_AGG(mul, PMPZ_AGG_OP, 0)
PMPZ_AGG(and, PMPZ_AGG_OP, 0)
PMPZ_AGG(ior, PMPZ_AGG_OP, 0)
PMPZ_AGG(xor, PMPZ_AGG_OP, 0)


#define PMPZ_AGG_REL(op, rel) \
do { \
    if (mpz_cmp(*a, z) rel 0) { \
        mpz_set(*a, z); \
    } \
} while (0)

PMPZ_AGG(min, PMPZ_AGG_REL, >)
PMPZ_AGG(max, PMPZ_AGG_REL, <)

