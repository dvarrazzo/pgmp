/* pgmp_test -- test functions
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
#include "funcapi.h"
#include "utils/numeric.h"

PG_FUNCTION_INFO_V1(mpz_test_dataset);
PG_FUNCTION_INFO_V1(numeric_test_dataset);

Datum mpz_test_dataset(PG_FUNCTION_ARGS);
Datum numeric_test_dataset(PG_FUNCTION_ARGS);


/*
 * Generate a sequence of mpz
 *
 * mpz_test_dataset(num, size)
 *
 * Parameters:
 * - num: items to generate
 * - size: number of digits in each item
 */
Datum
mpz_test_dataset(PG_FUNCTION_ARGS)
{
    FuncCallContext *funcctx;

    if (SRF_IS_FIRSTCALL())
    {
        MemoryContext oldctx;

        char        *buf;
        mpz_t       z;
        uint32      size;

        funcctx = SRF_FIRSTCALL_INIT();
        oldctx = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        /* Total calls */
        funcctx->max_calls = PG_GETARG_UINT32(0);

        /* Number to return */
        size = PG_GETARG_UINT32(1);
        buf = (char *)palloc(size + 1);
        memset(buf, '8', size);
        buf[size] = '\0';

        mpz_init_set_str(z, buf, 10);
        funcctx->user_fctx = (void *)pmpz_from_mpz(z);

        MemoryContextSwitchTo(oldctx);
    }

    funcctx = SRF_PERCALL_SETUP();

    if (funcctx->call_cntr < funcctx->max_calls)
    {
        /*
        pmpz        *res;
        res = (pmpz *)palloc(VARSIZE(funcctx->user_fctx));
        memcpy(res, funcctx->user_fctx, VARSIZE(funcctx->user_fctx));

        SRF_RETURN_NEXT(funcctx, (Datum)res);
        */
        SRF_RETURN_NEXT(funcctx, (Datum)(funcctx->user_fctx));
    }
    else
    {
        SRF_RETURN_DONE(funcctx);
    }

}


/*
 * Generate a sequence of numeric
 *
 * numeric_test_dataset(num, sample)
 *
 * Parameters:
 * - num: items to generate
 * - sample: the number to return repeatedly
 *
 *   TODO: is there an easy way to generate a numeric programmatically?
 */
Datum
numeric_test_dataset(PG_FUNCTION_ARGS)
{
    FuncCallContext *funcctx;

    if (SRF_IS_FIRSTCALL())
    {
        MemoryContext oldctx;

        funcctx = SRF_FIRSTCALL_INIT();
        oldctx = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        /* Total calls */
        funcctx->max_calls = PG_GETARG_UINT32(0);

        /* Number to return */
        funcctx->user_fctx = (void *)PG_GETARG_NUMERIC(1);

        MemoryContextSwitchTo(oldctx);
    }

    funcctx = SRF_PERCALL_SETUP();

    if (funcctx->call_cntr < funcctx->max_calls)
    {
        pmpz        *res;
        res = (pmpz *)palloc(VARSIZE(funcctx->user_fctx));
        memcpy(res, funcctx->user_fctx, VARSIZE(funcctx->user_fctx));

        SRF_RETURN_NEXT(funcctx, (Datum)res);
    }
    else
    {
        SRF_RETURN_DONE(funcctx);
    }

}

