/* pmpq_arith -- mpq arithmetic functions
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

/*
 * Unary minus, plus
 */

PGMP_PG_FUNCTION(pmpq_uminus)
{
    const mpq_t     q1;
    mpq_t           qf;
    pmpq            *res;

    mpq_from_pmpq(q1, PG_GETARG_PMPQ(0));

    mpz_init_set(mpq_numref(qf), mpq_numref(q1));
    mpz_init_set(mpq_denref(qf), mpq_denref(q1));
    mpz_neg(mpq_numref(qf), mpq_numref(qf));

    res = pmpq_from_mpq(qf);
    PG_RETURN_POINTER(res);
}

PGMP_PG_FUNCTION(pmpq_uplus)
{
    const pmpq      *pq1;
    pmpq            *res;

    pq1 = PG_GETARG_PMPQ(0);

	res = (pmpq *)palloc(VARSIZE(pq1));
	memcpy(res, pq1, VARSIZE(pq1));

    PG_RETURN_POINTER(res);
}


