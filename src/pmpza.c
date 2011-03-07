/* pmpza -- mpz aggregation
 *
 * Copyright (C) 2011 Daniele Varrazzo
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
 * http://www.gnu.org/licenses/.
 */

#include "pmpz.h"
#include "pgmp-impl.h"

#include "fmgr.h"


PG_FUNCTION_INFO_V1(pmpza_in);
PG_FUNCTION_INFO_V1(pmpza_out);

PG_FUNCTION_INFO_V1(_pmpz_from_pmpza);

Datum       pmpza_in(PG_FUNCTION_ARGS);
Datum       pmpza_out(PG_FUNCTION_ARGS);

Datum       _pmpz_from_pmpza(PG_FUNCTION_ARGS);

/*
 * Input/Output functions
 */

Datum
pmpza_in(PG_FUNCTION_ARGS)
{
    char    *str;
    mpz_t   *z;

    str = PG_GETARG_CSTRING(0);

    /* We return a null accumulator on blank input.
     * It can be recognized by having the limbs pointing to 0.
     *
     * We need this to allow a strict accumulation function and the
     * possibility to return null, as the accumulator type is not the same
     * type of the accumulated values.
     */
    if (str[0] != '\0')
    {
        z = (mpz_t *)palloc(sizeof(mpz_t));

        if (0 != mpz_init_set_str(*z, str, 10))
        {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                     errmsg("invalid input syntax for mpz: \"%s\"",
                            str)));
        }
    }
    else {
        z = (mpz_t *)palloc0(sizeof(mpz_t));
    }

    PG_RETURN_POINTER(z);
}

Datum
pmpza_out(PG_FUNCTION_ARGS)
{
    mpz_t       *z;
    char        *res;

    z = (mpz_t *)PG_GETARG_POINTER(0);

    res = (char *)palloc(mpz_sizeinbase(*z, 10) + 2);    /* add sign and 0 */
    mpz_get_str(res, 10, *z);

    PG_RETURN_CSTRING(res);
}


/*
 * Accumulation functions
 */

/* Convert an inplace accumulator into a pmpz structure */
Datum
_pmpz_from_pmpza(PG_FUNCTION_ARGS)
{
    mpz_t       *a;
    pmpz        *res;

    a = (mpz_t *)PG_GETARG_POINTER(0);

    if (LIKELY(LIMBS(*a))) {
        res = pmpz_from_mpz(*a);
        PG_RETURN_POINTER(res);
    }
    else {                      /* uninitialized */
        PG_RETURN_NULL();
    }
}


/* Macro to create an accumulation function from a gmp operator */

#define PMPZ_AGG(op) \
PG_FUNCTION_INFO_V1(_pmpz_agg_ ## op); \
 \
Datum       _pmpz_agg_ ## op (PG_FUNCTION_ARGS); \
 \
Datum \
_pmpz_agg_ ## op (PG_FUNCTION_ARGS) \
{ \
    mpz_t           *a; \
    const mpz_t     z; \
    MemoryContext   oldctx; \
    MemoryContext   aggctx; \
 \
    /* TODO: make compatible with PG < 9 */ \
    if (UNLIKELY(!AggCheckCallContext(fcinfo, &aggctx))) \
    { \
        ereport(ERROR, \
            (errcode(ERRCODE_DATA_EXCEPTION), \
            errmsg("_pmpz_agg_" #op " can only be called in accumulation"))); \
 \
        PG_RETURN_NULL();       /* unused, to avoid a warning */ \
    } \
 \
    a = (mpz_t *)PG_GETARG_POINTER(0); \
    mpz_from_pmpz(z, PG_GETARG_PMPZ(1)); \
 \
    oldctx = MemoryContextSwitchTo(aggctx); \
    if (LIKELY(LIMBS(*a))) { \
        mpz_ ## op (*a, *a, z); \
    } \
    else {                      /* uninitialized */ \
        mpz_init_set(*a, z); \
    } \
    MemoryContextSwitchTo(oldctx); \
 \
    PG_RETURN_POINTER(a); \
}

PMPZ_AGG(add)

