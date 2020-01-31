/* pmpq_agg -- mpq aggregation functions
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

#include "pmpq.h"
#include "pgmp-impl.h"

#include "fmgr.h"


/* Convert an inplace accumulator into a pmpq structure.
 *
 * This function is strict, so don't care about NULLs
 */
PGMP_PG_FUNCTION(_pmpq_from_agg)
{
    mpq_t       *a;

    a = (mpq_t *)PG_GETARG_POINTER(0);
    PGMP_RETURN_MPQ(*a);
}


/* Macro to create an accumulation function from a gmp operator.
 *
 * This function can't be strict because the internal state is not compatible
 * with the base type.
 */
#define PMPQ_AGG(op, BLOCK, rel) \
 \
PGMP_PG_FUNCTION(_pmpq_agg_ ## op) \
{ \
    mpq_t           *a; \
    const mpq_t     q; \
    MemoryContext   oldctx; \
    MemoryContext   aggctx; \
 \
    if (UNLIKELY(!AggCheckCallContext(fcinfo, &aggctx))) \
    { \
        ereport(ERROR, \
            (errcode(ERRCODE_DATA_EXCEPTION), \
            errmsg("_mpq_agg_" #op " can only be called in accumulation"))); \
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
    PGMP_GETARG_MPQ(q, 1); \
 \
    oldctx = MemoryContextSwitchTo(aggctx); \
 \
    if (LIKELY(!PG_ARGISNULL(0))) { \
        a = (mpq_t *)PG_GETARG_POINTER(0); \
        BLOCK(op, rel); \
    } \
    else {                      /* uninitialized */ \
        a = (mpq_t *)palloc(sizeof(mpq_t)); \
        mpq_init(*a); \
        mpq_set(*a, q); \
    } \
 \
    MemoryContextSwitchTo(oldctx); \
 \
    PG_RETURN_POINTER(a); \
}


#define PMPQ_AGG_OP(op, rel) \
    mpq_ ## op (*a, *a, q)

PMPQ_AGG(add, PMPQ_AGG_OP, 0)
PMPQ_AGG(mul, PMPQ_AGG_OP, 0)


#define PMPQ_AGG_REL(op, rel) \
do { \
    if (mpq_cmp(*a, q) rel 0) { \
        mpq_set(*a, q); \
    } \
} while (0)

PMPQ_AGG(min, PMPQ_AGG_REL, >)
PMPQ_AGG(max, PMPQ_AGG_REL, <)

