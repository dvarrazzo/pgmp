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

    PGMP_GETARG_MPQ(q1, 0);

    mpz_init_set(mpq_numref(qf), mpq_numref(q1));
    mpz_init_set(mpq_denref(qf), mpq_denref(q1));
    mpz_neg(mpq_numref(qf), mpq_numref(qf));

    PGMP_RETURN_MPQ(qf);
}

PGMP_PG_FUNCTION(pmpq_uplus)
{
    const pmpq      *pq1;
    pmpq            *res;

    pq1 = PGMP_GETARG_PMPQ(0);

	res = (pmpq *)palloc(VARSIZE(pq1));
	memcpy(res, pq1, VARSIZE(pq1));

    PG_RETURN_POINTER(res);
}


/*
 * Binary operators
 */

/* Template to generate regular binary operators */

#define PMPQ_OP(op) \
 \
PGMP_PG_FUNCTION(pmpq_ ## op) \
{ \
    const mpq_t     q1; \
    const mpq_t     q2; \
    mpq_t           qf; \
 \
    PGMP_GETARG_MPQ(q1, 0); \
    PGMP_GETARG_MPQ(q2, 1); \
 \
    mpq_init(qf); \
    mpq_ ## op (qf, q1, q2); \
 \
    PGMP_RETURN_MPQ(qf); \
}

PMPQ_OP(add)
PMPQ_OP(sub)
PMPQ_OP(mul)


/* Template to generate binary operators that may divide by zero */

#define PMPQ_OP_DIV(op) \
 \
PGMP_PG_FUNCTION(pmpq_ ## op) \
{ \
    const mpq_t     q1; \
    const mpq_t     q2; \
    mpq_t           qf; \
 \
    PGMP_GETARG_MPQ(q2, 1); \
    if (UNLIKELY(MPZ_IS_ZERO(mpq_numref(q2)))) \
    { \
        ereport(ERROR, ( \
            errcode(ERRCODE_DIVISION_BY_ZERO), \
            errmsg("division by zero"))); \
    } \
 \
    PGMP_GETARG_MPQ(q1, 0); \
 \
    mpq_init(qf); \
    mpq_ ## op (qf, q1, q2); \
 \
    PGMP_RETURN_MPQ(qf); \
}

PMPQ_OP_DIV(div)


/* Functions defined on bit count */

#define PMPQ_BIT(op) \
 \
PGMP_PG_FUNCTION(pmpq_ ## op) \
{ \
    const mpq_t     q; \
    unsigned long   b; \
    mpq_t           qf; \
 \
    PGMP_GETARG_MPQ(q, 0); \
    PGMP_GETARG_ULONG(b, 1); \
 \
    mpq_init(qf); \
    mpq_ ## op (qf, q, b); \
 \
    PGMP_RETURN_MPQ(qf); \
}


PMPQ_BIT(mul_2exp)
PMPQ_BIT(div_2exp)

