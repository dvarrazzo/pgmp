/* pmpq_io -- mpq Input/Output functions
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

#include "pmpq.h"
#include "pmpz.h"
#include "pgmp-impl.h"

#include "fmgr.h"

#include <string.h>


/*
 * Input/Output functions
 */

PGMP_PG_FUNCTION(pmpq_in)
{
    char    *str;
    mpq_t   q;
    pmpq    *res;

    str = PG_GETARG_CSTRING(0);

    mpq_init(q);
    if (0 != mpq_set_str(q, str, 10))
    {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                 errmsg("invalid input syntax for mpq: \"%s\"",
                        str)));
    }

    ERROR_IF_DENOM_ZERO(mpq_denref(q));

    mpq_canonicalize(q);
    res = pmpq_from_mpq(q);
    PG_RETURN_POINTER(res);
}

PGMP_PG_FUNCTION(pmpq_out)
{
    const pmpq      *pz;
    const mpq_t     q;
    char            *res;

    pz = PG_GETARG_PMPQ(0);
    mpq_from_pmpq(q, pz);

    res = mpq_get_str(NULL, 10, q);
    PG_RETURN_CSTRING(res);
}


/*
 * Cast functions
 */

static Datum _pmpq_from_long(long in);

PGMP_PG_FUNCTION(pmpq_from_int2)
{
    int16 in = PG_GETARG_INT16(0);
    return _pmpq_from_long(in);
}

PGMP_PG_FUNCTION(pmpq_from_int4)
{
    int32 in = PG_GETARG_INT32(0);
    return _pmpq_from_long(in);
}

static Datum
_pmpq_from_long(long in)
{
    mpq_t   q;
    pmpq    *res;

    mpz_init_set_si(mpq_numref(q), in);
    mpz_init_set_si(mpq_denref(q), 1L);

    res = pmpq_from_mpq(q);
    PG_RETURN_POINTER(res);
}


/* to convert from int8 we piggyback all the mess we've made for mpz */

Datum pmpz_from_int8(PG_FUNCTION_ARGS);

PGMP_PG_FUNCTION(pmpq_from_int8)
{
    mpq_t           q;
    pmpq            *res;

    mpz_from_pmpz(mpq_numref(q),
        (pmpz *)DirectFunctionCall1(pmpz_from_int8,
            PG_GETARG_DATUM(0)));

    mpz_init_set_si(mpq_denref(q), 1L);

    res = pmpq_from_mpq(q);
    PG_RETURN_POINTER(res);
}


/* To convert from numeric we convert the numeric in str, then work on that */

Datum numeric_out(PG_FUNCTION_ARGS);

PGMP_PG_FUNCTION(pmpq_from_numeric)
{
    mpq_t       q;
    pmpq        *res;
    char        *sn, *pn;

    sn = DatumGetCString(DirectFunctionCall1(numeric_out,
            PG_GETARG_DATUM(0)));

    if ((pn = strchr(sn, '.')))
    {
        char    *sd, *pd;

        /* Convert "123.45" into "12345" and produce "100" in the process. */
        pd = sd = (char *)palloc(strlen(sn));
        *pd++ = '1';
        while (pn[1])
        {
            pn[0] = pn[1];
            ++pn;
            *pd++ = '0';
        }
        *pd = *pn = '\0';

        if (0 != mpz_init_set_str(mpq_numref(q), sn, 10))
        {
            ereport(ERROR, (
                errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                errmsg("invalid input syntax for mpq: \"%s\"", sn)));
        }

        mpz_init_set_str(mpq_denref(q), sd, 10);
        mpq_canonicalize(q);
    }
    else {
        /* just an integer */
        if (0 != mpz_init_set_str(mpq_numref(q), sn, 10))
        {
            ereport(ERROR, (
                errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                errmsg("invalid input syntax for mpq: \"%s\"", sn)));
        }
        mpz_init_set_si(mpq_denref(q), 1L);
    }

    res = pmpq_from_mpq(q);
    PG_RETURN_POINTER(res);
}

PGMP_PG_FUNCTION(pmpq_from_mpz)
{
    mpq_t           q;
    pmpq            *res;

    mpz_from_pmpz(mpq_numref(q), PG_GETARG_PMPZ(0));
    mpz_init_set_si(mpq_denref(q), 1L);

    res = pmpq_from_mpq(q);
    PG_RETURN_POINTER(res);
}

