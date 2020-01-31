/* pmpz_arith -- mpz arithmetic functions
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
#include "access/hash.h"            /* for hash_any */
#include <access/htup_details.h>    /* for heap_form_tuple */
#if PG_VERSION_NUM >= 100000
#include <utils/fmgrprotos.h>       /* for hashint8 */
#endif


/*
 * Unary operators
 */

PGMP_PG_FUNCTION(pmpz_uplus)
{
    const pmpz      *pz1;
    pmpz            *res;

    pz1 = PGMP_GETARG_PMPZ(0);

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
    PGMP_GETARG_MPZ(z1, 0); \
    CHECK(z1); \
 \
    mpz_init(zf); \
    mpz_ ## op (zf, z1); \
 \
    PGMP_RETURN_MPZ(zf); \
}

PMPZ_UN(neg,    PMPZ_NO_CHECK)
PMPZ_UN(abs,    PMPZ_NO_CHECK)
PMPZ_UN(sqrt,   PMPZ_CHECK_NONEG)
PMPZ_UN(com,    PMPZ_NO_CHECK)


/*
 * Binary operators
 */

/* Operators defined (mpz, mpz) -> mpz.
 *
 * CHECK2 is a check performed on the 2nd argument.
 */

#define PMPZ_OP(op, CHECK2) \
 \
PGMP_PG_FUNCTION(pmpz_ ## op) \
{ \
    const mpz_t     z1; \
    const mpz_t     z2; \
    mpz_t           zf; \
 \
    PGMP_GETARG_MPZ(z1, 0); \
    PGMP_GETARG_MPZ(z2, 1); \
    CHECK2(z2); \
 \
    mpz_init(zf); \
    mpz_ ## op (zf, z1, z2); \
 \
    PGMP_RETURN_MPZ(zf); \
}

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
PMPZ_OP(gcd,        PMPZ_NO_CHECK)
PMPZ_OP(lcm,        PMPZ_NO_CHECK)
PMPZ_OP(remove,     PMPZ_NO_CHECK)      /* TODO: return value not returned */


/* Operators defined (mpz, mpz) -> (mpz, mpz). */

#define PMPZ_OP2(op, CHECK2) \
 \
PGMP_PG_FUNCTION(pmpz_ ## op) \
{ \
    const mpz_t     z1; \
    const mpz_t     z2; \
    mpz_t           zf1; \
    mpz_t           zf2; \
 \
    PGMP_GETARG_MPZ(z1, 0); \
    PGMP_GETARG_MPZ(z2, 1); \
    CHECK2(z2); \
 \
    mpz_init(zf1); \
    mpz_init(zf2); \
    mpz_ ## op (zf1, zf2, z1, z2); \
 \
    PGMP_RETURN_MPZ_MPZ(zf1, zf2); \
}

PMPZ_OP2(tdiv_qr,    PMPZ_CHECK_DIV0)
PMPZ_OP2(cdiv_qr,    PMPZ_CHECK_DIV0)
PMPZ_OP2(fdiv_qr,    PMPZ_CHECK_DIV0)


/* Functions defined on unsigned long */

#define PMPZ_OP_UL(op, CHECK1, CHECK2) \
 \
PGMP_PG_FUNCTION(pmpz_ ## op) \
{ \
    const mpz_t     z; \
    unsigned long   b; \
    mpz_t           zf; \
 \
    PGMP_GETARG_MPZ(z, 0); \
    CHECK1(z); \
    \
    PGMP_GETARG_ULONG(b, 1); \
    CHECK2(b); \
 \
    mpz_init(zf); \
    mpz_ ## op (zf, z, b); \
 \
    PGMP_RETURN_MPZ(zf); \
}

PMPZ_OP_UL(pow_ui,  PMPZ_NO_CHECK,      PMPZ_NO_CHECK)
PMPZ_OP_UL(root,    PMPZ_CHECK_NONEG,   PMPZ_CHECK_LONG_POS)
PMPZ_OP_UL(bin_ui,  PMPZ_NO_CHECK,      PMPZ_CHECK_LONG_NONEG)


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


/* Unary predicates */

#define PMPZ_PRED(pred) \
 \
PGMP_PG_FUNCTION(pmpz_ ## pred) \
{ \
    const mpz_t     op; \
 \
    PGMP_GETARG_MPZ(op, 0); \
 \
    PG_RETURN_BOOL(mpz_ ## pred ## _p(op)); \
}

PMPZ_PRED(even)
PMPZ_PRED(odd)
PMPZ_PRED(perfect_power)
PMPZ_PRED(perfect_square)


/*
 * Comparison operators
 */

PGMP_PG_FUNCTION(pmpz_cmp)
{
    const mpz_t     z1;
    const mpz_t     z2;

    PGMP_GETARG_MPZ(z1, 0);
    PGMP_GETARG_MPZ(z2, 1);

    PG_RETURN_INT32(mpz_cmp(z1, z2));
}


#define PMPZ_CMP(op, rel) \
 \
PGMP_PG_FUNCTION(pmpz_ ## op) \
{ \
    const mpz_t     z1; \
    const mpz_t     z2; \
 \
    PGMP_GETARG_MPZ(z1, 0); \
    PGMP_GETARG_MPZ(z2, 1); \
 \
    PG_RETURN_BOOL(mpz_cmp(z1, z2) rel 0); \
}

PMPZ_CMP(eq, ==)
PMPZ_CMP(ne, !=)
PMPZ_CMP(gt, >)
PMPZ_CMP(ge, >=)
PMPZ_CMP(lt, <)
PMPZ_CMP(le, <=)


/* The hash of an mpz fitting into a int64 is the same of the PG builtin.
 * This allows cross-type hash joins int2/int4/int8.
 */
PGMP_PG_FUNCTION(pmpz_hash)
{
    const mpz_t     z;

    PGMP_GETARG_MPZ(z, 0);
    return pmpz_get_hash(z);
}

Datum
pmpz_get_hash(mpz_srcptr z)
{
    int64           z64;

    if (0 == pmpz_get_int64(z, &z64)) {
        return DirectFunctionCall1(hashint8, Int64GetDatumFast(z64));
    }

    PG_RETURN_INT32(hash_any(
        (unsigned char *)LIMBS(z),
        NLIMBS(z) * sizeof(mp_limb_t)));
}


/*
 * Misc functions... each one has its own signature, sigh.
 */

PGMP_PG_FUNCTION(pmpz_sgn)
{
    const mpz_t     n;

    PGMP_GETARG_MPZ(n, 0);

    PG_RETURN_INT32(mpz_sgn(n));
}

PGMP_PG_FUNCTION(pmpz_divisible)
{
    const mpz_t     n;
    const mpz_t     d;

    PGMP_GETARG_MPZ(n, 0);
    PGMP_GETARG_MPZ(d, 1);

    /* GMP 4.1 doesn't guard for zero */
#if __GMP_MP_RELEASE < 40200
    if (UNLIKELY(MPZ_IS_ZERO(d))) {
        PG_RETURN_BOOL(MPZ_IS_ZERO(n));
    }
#endif

    PG_RETURN_BOOL(mpz_divisible_p(n, d));
}

PGMP_PG_FUNCTION(pmpz_divisible_2exp)
{
    const mpz_t     n;
    mp_bitcnt_t     b;

    PGMP_GETARG_MPZ(n, 0);
    PGMP_GETARG_ULONG(b, 1);

    PG_RETURN_BOOL(mpz_divisible_2exp_p(n, b));
}

PGMP_PG_FUNCTION(pmpz_congruent)
{
    const mpz_t     n;
    const mpz_t     c;
    const mpz_t     d;

    PGMP_GETARG_MPZ(n, 0);
    PGMP_GETARG_MPZ(c, 1);
    PGMP_GETARG_MPZ(d, 2);

    /* GMP 4.1 doesn't guard for zero */
#if __GMP_MP_RELEASE < 40200
    if (UNLIKELY(MPZ_IS_ZERO(d))) {
        PG_RETURN_BOOL(0 == mpz_cmp(n, c));
    }
#endif


    PG_RETURN_BOOL(mpz_congruent_p(n, c, d));
}

PGMP_PG_FUNCTION(pmpz_congruent_2exp)
{
    const mpz_t     n;
    const mpz_t     c;
    mp_bitcnt_t     b;

    PGMP_GETARG_MPZ(n, 0);
    PGMP_GETARG_MPZ(c, 1);
    PGMP_GETARG_ULONG(b, 2);

    PG_RETURN_BOOL(mpz_congruent_2exp_p(n, c, b));
}


PGMP_PG_FUNCTION(pmpz_powm)
{
    const mpz_t     base;
    const mpz_t     exp;
    const mpz_t     mod;
    mpz_t           zf;

    PGMP_GETARG_MPZ(base, 0);
    PGMP_GETARG_MPZ(exp, 1);
    PMPZ_CHECK_NONEG(exp);
    PGMP_GETARG_MPZ(mod, 2);
    PMPZ_CHECK_DIV0(mod);

    mpz_init(zf);
    mpz_powm(zf, base, exp, mod);

    PGMP_RETURN_MPZ(zf);
}

