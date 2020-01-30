/* pmpq_arith -- mpq arithmetic functions
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
#include "pmpz.h"
#include "pgmp-impl.h"

#include "fmgr.h"
#include "access/hash.h"            /* for hash_any */


/*
 * Unary operators
 */

PGMP_PG_FUNCTION(pmpq_uplus)
{
    const pmpq      *pq1;
    pmpq            *res;

    pq1 = PGMP_GETARG_PMPQ(0);

    res = (pmpq *)palloc(VARSIZE(pq1));
    memcpy(res, pq1, VARSIZE(pq1));

    PG_RETURN_POINTER(res);
}

#define PMPQ_UN(op, CHECK) \
 \
PGMP_PG_FUNCTION(pmpq_ ## op) \
{ \
    const mpq_t     q; \
    mpq_t           qf; \
 \
    PGMP_GETARG_MPQ(q, 0); \
    CHECK(q); \
 \
    mpq_init(qf); \
    mpq_ ## op (qf, q); \
 \
    PGMP_RETURN_MPQ(qf); \
}

PMPQ_UN(neg, PMPQ_NO_CHECK)
PMPQ_UN(abs, PMPQ_NO_CHECK)
PMPQ_UN(inv, PMPQ_CHECK_DIV0)


/*
 * Binary operators
 */

/* Template to generate binary operators */

#define PMPQ_OP(op, CHECK2) \
 \
PGMP_PG_FUNCTION(pmpq_ ## op) \
{ \
    const mpq_t     q1; \
    const mpq_t     q2; \
    mpq_t           qf; \
 \
    PGMP_GETARG_MPQ(q1, 0); \
    PGMP_GETARG_MPQ(q2, 1); \
    CHECK2(q2); \
 \
    mpq_init(qf); \
    mpq_ ## op (qf, q1, q2); \
 \
    PGMP_RETURN_MPQ(qf); \
}

PMPQ_OP(add, PMPQ_NO_CHECK)
PMPQ_OP(sub, PMPQ_NO_CHECK)
PMPQ_OP(mul, PMPQ_NO_CHECK)
PMPQ_OP(div, PMPQ_CHECK_DIV0)


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


/*
 * Comparison operators
 */

PGMP_PG_FUNCTION(pmpq_cmp)
{
    const mpq_t     q1;
    const mpq_t     q2;

    PGMP_GETARG_MPQ(q1, 0);
    PGMP_GETARG_MPQ(q2, 1);

    PG_RETURN_INT32(mpq_cmp(q1, q2));
}


#define PMPQ_CMP_EQ(op, rel) \
 \
PGMP_PG_FUNCTION(pmpq_ ## op) \
{ \
    const mpq_t     q1; \
    const mpq_t     q2; \
 \
    PGMP_GETARG_MPQ(q1, 0); \
    PGMP_GETARG_MPQ(q2, 1); \
 \
    PG_RETURN_BOOL(mpq_equal(q1, q2) rel 0); \
}

PMPQ_CMP_EQ(eq, !=)     /* note that the operators are reversed */
PMPQ_CMP_EQ(ne, ==)


#define PMPQ_CMP(op, rel) \
 \
PGMP_PG_FUNCTION(pmpq_ ## op) \
{ \
    const mpq_t     q1; \
    const mpq_t     q2; \
 \
    PGMP_GETARG_MPQ(q1, 0); \
    PGMP_GETARG_MPQ(q2, 1); \
 \
    PG_RETURN_BOOL(mpq_cmp(q1, q2) rel 0); \
}

PMPQ_CMP(gt, >)
PMPQ_CMP(ge, >=)
PMPQ_CMP(lt, <)
PMPQ_CMP(le, <=)


/* The hash of an integer mpq is the same of the same number as mpz.
 * This allows cross-type hash joins with mpz and builtins.
 */
PGMP_PG_FUNCTION(pmpq_hash)
{
    const mpq_t     q;
    Datum           nhash;

    PGMP_GETARG_MPQ(q, 0);

    nhash = pmpz_get_hash(mpq_numref(q));

    if (mpz_cmp_si(mpq_denref(q), 1L) == 0) {
        return nhash;
    }

    PG_RETURN_INT32(
        DatumGetInt32(nhash) ^ hash_any(
            (unsigned char *)LIMBS(mpq_denref(q)),
            NLIMBS(mpq_denref(q)) * sizeof(mp_limb_t)));
}


/* limit_den */

static void limit_den(mpq_ptr q_out, mpq_srcptr q_in, mpz_srcptr max_den);

PGMP_PG_FUNCTION(pmpq_limit_den)
{
    const mpq_t     q_in;
    const mpz_t     max_den;
    mpq_t           q_out;

    PGMP_GETARG_MPQ(q_in, 0);
    if (PG_NARGS() >= 2) {
        PGMP_GETARG_MPZ(max_den, 1);
    }
    else {
        mpz_init_set_si((mpz_ptr)max_den, 1000000);
    }

    if (mpz_cmp_si(max_den, 1) < 0)
    {
        ereport(ERROR, ( \
            errcode(ERRCODE_INVALID_PARAMETER_VALUE), \
            errmsg("max_den should be at least 1"))); \
    }

    mpq_init(q_out);
    limit_den(q_out, q_in, max_den);

    PGMP_RETURN_MPQ(q_out);
}

/*
 * Set q_out to the closest fraction to q_in with denominator at most max_den
 *
 * Ported from Python library: see
 * https://hg.python.org/cpython/file/v2.7/Lib/fractions.py#l206
 * for implementation notes.
 */
static void
limit_den(mpq_ptr q_out, mpq_srcptr q_in, mpz_srcptr max_den)
{
    mpz_t p0, q0, p1, q1;
    mpz_t n, d;
    mpz_t a, q2;
    mpz_t k;
    mpq_t b1, b2;
    mpq_t ab1, ab2;

    if (mpz_cmp(mpq_denref(q_in), max_den) <= 0) {
        mpq_set(q_out, q_in);
        return;
    }

    /* p0, q0, p1, q1 = 0, 1, 1, 0 */
    mpz_init_set_si(p0, 0);
    mpz_init_set_si(q0, 1);
    mpz_init_set_si(p1, 1);
    mpz_init_set_si(q1, 0);

    /* n, d = self._numerator, self._denominator */
    mpz_init_set(n, mpq_numref(q_in));
    mpz_init_set(d, mpq_denref(q_in));

    mpz_init(a);
    mpz_init(q2);

    for (;;) {
        /* a = n // d */
        mpz_tdiv_q(a, n, d);

        /* q2 = q0+a*q1 */
        mpz_set(q2, q0);
        mpz_addmul(q2, a, q1);

        if (mpz_cmp(q2, max_den) > 0) { break; }

        /* p0, q0, p1, q1 = p1, q1, p0+a*p1, q2 */
        mpz_swap(p0, p1);
        mpz_addmul(p1, a, p0);
        mpz_swap(q0, q1);
        mpz_swap(q1, q2);

        /* n, d = d, n-a*d */
        mpz_swap(n, d);
        mpz_submul(d, a, n);
    }

    /* k = (max_denominator - q0) // q1 */
    mpz_init(k);
    mpz_sub(k, max_den, q0);
    mpz_tdiv_q(k, k, q1);

    /* bound1 = Fraction(p0+k*p1, q0+k*q1) */
    mpq_init(b1);
    mpz_addmul(p0, k, p1);
    mpz_set(mpq_numref(b1), p0);
    mpz_addmul(q0, k, q1);
    mpz_set(mpq_denref(b1), q0);
    mpq_canonicalize(b1);

    /* bound2 = Fraction(p1, q1) */
    mpq_init(b2);
    mpz_set(mpq_numref(b2), p1);
    mpz_set(mpq_denref(b2), q1);
    mpq_canonicalize(b2);

    /* if abs(bound2 - self) <= abs(bound1 - self): */
    mpq_init(ab1);
    mpq_sub(ab1, b1, q_in);
    mpq_abs(ab1, ab1);

    mpq_init(ab2);
    mpq_sub(ab2, b2, q_in);
    mpq_abs(ab2, ab2);

    if (mpq_cmp(ab2, ab1) <= 0) {
        /* return bound2 */
        mpq_set(q_out, b2);
    }
    else {
        /* return bound1 */
        mpq_set(q_out, b1);
    }
}


