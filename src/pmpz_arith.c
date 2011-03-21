/* pmpz_arith -- mpz arithmetic functions
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


/*
 * Unary minus, plus
 */

PGMP_PG_FUNCTION(pmpz_uplus)
{
    const pmpz      *pz1;
    pmpz            *res;

    pz1 = PG_GETARG_PMPZ(0);

	res = (pmpz *)palloc(VARSIZE(pz1));
	memcpy(res, pz1, VARSIZE(pz1));

    PG_RETURN_POINTER(res);
}


/* Template to generate unary functions */

#define PMPZ_UN(op, CHECK) \
 \
PGMP_PG_FUNCTION(pmpz_ ## op) \
{ \
    const mpz_t     z1; \
    mpz_t           zf; \
 \
    mpz_from_pmpz(z1, PG_GETARG_PMPZ(0)); \
    CHECK(z1); \
 \
    mpz_init(zf); \
    mpz_ ## op (zf, z1); \
 \
    PG_RETURN_MPZ(zf); \
}

PMPZ_UN(neg,    PMPZ_NO_CHECK)
PMPZ_UN(abs,    PMPZ_NO_CHECK)
PMPZ_UN(sqrt,   PMPZ_CHECK_NONEG)
PMPZ_UN(nextprime,  PMPZ_NO_CHECK)
PMPZ_UN(com,    PMPZ_NO_CHECK)


/*
 * Binary operators
 */

/* Operators defined (mpz, mpz) -> mpz.
 *
 * CHECK2 is a check performed on the 2nd argument. Available checks are
 * defined below.
 */

#define PMPZ_OP(op, CHECK2) \
 \
PGMP_PG_FUNCTION(pmpz_ ## op) \
{ \
    const mpz_t     z1; \
    const mpz_t     z2; \
    mpz_t           zf; \
 \
    mpz_from_pmpz(z1, PG_GETARG_PMPZ(0)); \
    mpz_from_pmpz(z2, PG_GETARG_PMPZ(1)); \
    CHECK2(z2); \
 \
    mpz_init(zf); \
    mpz_ ## op (zf, z1, z2); \
 \
    PG_RETURN_MPZ(zf); \
}


/* Operators definitions */

PMPZ_OP(add,        PMPZ_NO_CHECK)
PMPZ_OP(sub,        PMPZ_NO_CHECK)
PMPZ_OP(mul,        PMPZ_NO_CHECK)
PMPZ_OP(tdiv_q,     PMPZ_CHECK_DIV0)
PMPZ_OP(tdiv_r,     PMPZ_CHECK_DIV0)
PMPZ_OP(cdiv_q,     PMPZ_CHECK_DIV0)
PMPZ_OP(cdiv_r,     PMPZ_CHECK_DIV0)
PMPZ_OP(fdiv_q,     PMPZ_CHECK_DIV0)
PMPZ_OP(fdiv_r,     PMPZ_CHECK_DIV0)
PMPZ_OP(divexact,   PMPZ_CHECK_DIV0)
PMPZ_OP(and,        PMPZ_NO_CHECK)
PMPZ_OP(ior,        PMPZ_NO_CHECK)
PMPZ_OP(xor,        PMPZ_NO_CHECK)


/* Functions defined on unsigned long */

#define PMPZ_OP_UL(op, CHECK1, CHECK2) \
 \
PGMP_PG_FUNCTION(pmpz_ ## op) \
{ \
    const mpz_t     z; \
    unsigned long   b; \
    mpz_t           zf; \
 \
    mpz_from_pmpz(z, PG_GETARG_PMPZ(0)); \
    CHECK1(z); \
    \
    PGMP_GETARG_ULONG(b, 1); \
    CHECK2(b); \
 \
    mpz_init(zf); \
    mpz_ ## op (zf, z, b); \
 \
    PG_RETURN_MPZ(zf); \
}

PMPZ_OP_UL(pow_ui,  PMPZ_NO_CHECK,      PMPZ_NO_CHECK)
PMPZ_OP_UL(root,    PMPZ_CHECK_NONEG,   PMPZ_CHECK_LONG_POS)


/* Functions defined on bit count
 *
 * mp_bitcnt_t is defined as unsigned long.
 */

#define PMPZ_OP_BITCNT PMPZ_OP_UL

PMPZ_OP_BITCNT(mul_2exp,        PMPZ_NO_CHECK,  PMPZ_NO_CHECK)
PMPZ_OP_BITCNT(tdiv_q_2exp,     PMPZ_NO_CHECK,  PMPZ_NO_CHECK)
PMPZ_OP_BITCNT(tdiv_r_2exp,     PMPZ_NO_CHECK,  PMPZ_NO_CHECK)
PMPZ_OP_BITCNT(cdiv_q_2exp,     PMPZ_NO_CHECK,  PMPZ_NO_CHECK)
PMPZ_OP_BITCNT(cdiv_r_2exp,     PMPZ_NO_CHECK,  PMPZ_NO_CHECK)
PMPZ_OP_BITCNT(fdiv_q_2exp,     PMPZ_NO_CHECK,  PMPZ_NO_CHECK)
PMPZ_OP_BITCNT(fdiv_r_2exp,     PMPZ_NO_CHECK,  PMPZ_NO_CHECK)


/*
 * Comparison operators
 */

PGMP_PG_FUNCTION(pmpz_cmp)
{
    const mpz_t     z1;
    const mpz_t     z2;

    mpz_from_pmpz(z1, PG_GETARG_PMPZ(0));
    mpz_from_pmpz(z2, PG_GETARG_PMPZ(1));

    PG_RETURN_INT32(mpz_cmp(z1, z2));
}


#define PMPZ_CMP(op, rel) \
 \
PGMP_PG_FUNCTION(pmpz_ ## op) \
{ \
    const mpz_t     z1; \
    const mpz_t     z2; \
 \
    mpz_from_pmpz(z1, PG_GETARG_PMPZ(0)); \
    mpz_from_pmpz(z2, PG_GETARG_PMPZ(1)); \
 \
    PG_RETURN_BOOL(mpz_cmp(z1, z2) rel 0); \
}

PMPZ_CMP(eq, ==)
PMPZ_CMP(ne, !=)
PMPZ_CMP(gt, >)
PMPZ_CMP(ge, >=)
PMPZ_CMP(lt, <)
PMPZ_CMP(le, <=)


/*
 * Unary predicates
 */

PGMP_PG_FUNCTION(pmpz_perfect_power)
{
    const mpz_t     z1;

    mpz_from_pmpz(z1, PG_GETARG_PMPZ(0));

    PG_RETURN_BOOL(mpz_perfect_power_p(z1));
}

PGMP_PG_FUNCTION(pmpz_perfect_square)
{
    const mpz_t     z1;

    mpz_from_pmpz(z1, PG_GETARG_PMPZ(0));

    PG_RETURN_BOOL(mpz_perfect_square_p(z1));
}

PGMP_PG_FUNCTION(pmpz_rootrem)
{
    const mpz_t     z1;
    mpz_t           zroot;
    mpz_t           zrem;
    unsigned long   n;

    mpz_from_pmpz(z1, PG_GETARG_PMPZ(0));
    PMPZ_CHECK_NONEG(z1);

    PGMP_GETARG_ULONG(n, 1);
    PMPZ_CHECK_LONG_POS(n);

    mpz_init(zroot);
    mpz_init(zrem);
    mpz_rootrem (zroot, zrem, z1, n);

    PG_RETURN_MPZ_MPZ(zroot, zrem);
}

PGMP_PG_FUNCTION(pmpz_sqrtrem)
{
    const mpz_t     z1;
    mpz_t           zroot;
    mpz_t           zrem;

    mpz_from_pmpz(z1, PG_GETARG_PMPZ(0));

    mpz_init(zroot);
    mpz_init(zrem);
    mpz_sqrtrem(zroot, zrem, z1);

    PG_RETURN_MPZ_MPZ(zroot, zrem);
}

