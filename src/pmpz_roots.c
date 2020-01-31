/* pmpz_roots -- root extraction functions
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


/* Functions with a more generic signature are defined in pmpz.arith.c */

#if __GMP_MP_RELEASE >= 40200

PGMP_PG_FUNCTION(pmpz_rootrem)
{
    const mpz_t     z1;
    mpz_t           zroot;
    mpz_t           zrem;
    unsigned long   n;

    PGMP_GETARG_MPZ(z1, 0);
    PMPZ_CHECK_NONEG(z1);

    PGMP_GETARG_ULONG(n, 1);
    PMPZ_CHECK_LONG_POS(n);

    mpz_init(zroot);
    mpz_init(zrem);
    mpz_rootrem (zroot, zrem, z1, n);

    PGMP_RETURN_MPZ_MPZ(zroot, zrem);
}

#endif

PGMP_PG_FUNCTION(pmpz_sqrtrem)
{
    const mpz_t     z1;
    mpz_t           zroot;
    mpz_t           zrem;

    PGMP_GETARG_MPZ(z1, 0);

    mpz_init(zroot);
    mpz_init(zrem);
    mpz_sqrtrem(zroot, zrem, z1);

    PGMP_RETURN_MPZ_MPZ(zroot, zrem);
}


