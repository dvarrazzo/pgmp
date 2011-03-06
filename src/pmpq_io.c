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
#include "pgmp-impl.h"

#include "fmgr.h"


PG_FUNCTION_INFO_V1(pmpq_in);
PG_FUNCTION_INFO_V1(pmpq_out);

Datum       pmpq_in(PG_FUNCTION_ARGS);
Datum       pmpq_out(PG_FUNCTION_ARGS);


/*
 * Input/Output functions
 */

Datum
pmpq_in(PG_FUNCTION_ARGS)
{
    char    *str;
    mpq_t   q;
    pmpq    *res;

    str = PG_GETARG_CSTRING(0);

    /* TODO: the input 1/0 causes an exception to be raised and is trapped
     * by the database. Is there a way to trap it ourselves? */
    mpq_init(q);
    if (0 != mpq_set_str(q, str, 10))
    {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                 errmsg("invalid input syntax for mpq: \"%s\"",
                        str)));
    }

    mpq_canonicalize(q);
    res = pmpq_from_mpq(q);
    PG_RETURN_POINTER(res);
}

Datum
pmpq_out(PG_FUNCTION_ARGS)
{
    const pmpq      *pz;
    const mpq_t     q;
    char            *res;

    pz = PG_GETARG_PMPQ(0);
    mpq_from_pmpq(q, pz);

    res = mpq_get_str(NULL, 10, q);
    PG_RETURN_CSTRING(res);
}

