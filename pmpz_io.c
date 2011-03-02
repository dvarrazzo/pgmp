/* pmpz_io -- mpz Input/Output functions
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

#include "fmgr.h"


PG_FUNCTION_INFO_V1(pmpz_in);
PG_FUNCTION_INFO_V1(pmpz_out);

Datum       pmpz_in(PG_FUNCTION_ARGS);
Datum       pmpz_out(PG_FUNCTION_ARGS);


/*
 * Input/Output functions
 */

Datum
pmpz_in(PG_FUNCTION_ARGS)
{
    char    *str;
    mpz_t   z;
    pmpz    *res;

    str = PG_GETARG_CSTRING(0);

    /* TODO: make base variable */
    if (0 != mpz_init_set_str(z, str, 10))
    {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                 errmsg("invalid input syntax for mpz: \"%s\"",
                        str)));
    }

    res = pmpz_from_mpz(z);
    PG_RETURN_POINTER(res);
}

Datum
pmpz_out(PG_FUNCTION_ARGS)
{
    const pmpz      *pz;
    const mpz_t     z;
    char            *res;

    pz = PG_GETARG_PMPZ(0);
    mpz_from_pmpz(z, pz);

    /* TODO: make base variable */
    res = (char *)palloc(mpz_sizeinbase(z, 10) + 2);    /* add sign and 0 */
    gmp_sprintf(res, "%Zd", z);

    PG_RETURN_CSTRING(res);
}


