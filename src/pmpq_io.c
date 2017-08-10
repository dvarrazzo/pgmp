/* pmpq_io -- mpq Input/Output functions
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
#include "utils/builtins.h"     /* for numeric_out */

#include <string.h>


/*
 * Input/Output functions
 */

PGMP_PG_FUNCTION(pmpq_in)
{
    char    *str;
    mpq_t   q;

    str = PG_GETARG_CSTRING(0);

    mpq_init(q);
    if (0 != mpq_set_str(q, str, 0))
    {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                 errmsg("invalid input syntax for mpq: \"%s\"",
                        str)));
    }

    ERROR_IF_DENOM_ZERO(mpq_denref(q));

    mpq_canonicalize(q);
    PGMP_RETURN_MPQ(q);
}

PGMP_PG_FUNCTION(pmpq_in_base)
{
    int     base;
    char    *str;
    mpq_t   q;

    base = PG_GETARG_INT32(1);

    if (!(base == 0 || (2 <= base && base <= PGMP_MAXBASE_IO)))
    {
        ereport(ERROR, (
            errcode(ERRCODE_INVALID_PARAMETER_VALUE),
            errmsg("invalid base for mpq input: %d", base),
            errhint("base should be between 2 and %d", PGMP_MAXBASE_IO)));
    }

    str = TextDatumGetCString(PG_GETARG_POINTER(0));

    mpq_init(q);
    if (0 != mpq_set_str(q, str, base))
    {
        const char *ell;
        const int maxchars = 50;
        ell = (strlen(str) > maxchars) ? "..." : "";

        ereport(ERROR, (
            errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
            errmsg("invalid input for mpq base %d: \"%.*s%s\"",
                base, 50, str, ell)));
    }

    ERROR_IF_DENOM_ZERO(mpq_denref(q));

    mpq_canonicalize(q);
    PGMP_RETURN_MPQ(q);
}

PGMP_PG_FUNCTION(pmpq_out)
{
    const mpq_t     q;
    char            *buf;

    PGMP_GETARG_MPQ(q, 0);

    /* Allocate the output buffer manually - see mpmz_out to know why */
    buf = palloc(3             /* add sign, slash and null */
        + mpz_sizeinbase(mpq_numref(q), 10)
        + mpz_sizeinbase(mpq_denref(q), 10));

    PG_RETURN_CSTRING(mpq_get_str(buf, 10, q));
}

PGMP_PG_FUNCTION(pmpq_out_base)
{
    const mpq_t     q;
    int             base;
    char            *buf;

    PGMP_GETARG_MPQ(q, 0);
    base = PG_GETARG_INT32(1);

    if (!((-36 <= base && base <= -2) ||
        (2 <= base && base <= PGMP_MAXBASE_IO)))
    {
        ereport(ERROR, (
            errcode(ERRCODE_INVALID_PARAMETER_VALUE),
            errmsg("invalid base for mpq output: %d", base),
            errhint("base should be between -36 and -2 or between 2 and %d",
                PGMP_MAXBASE_IO)));
    }

    /* Allocate the output buffer manually - see mpmz_out to know why */
    buf = palloc(3             /* add sign, slash and null */
        + mpz_sizeinbase(mpq_numref(q), ABS(base))
        + mpz_sizeinbase(mpq_denref(q), ABS(base)));
    PG_RETURN_CSTRING(mpq_get_str(buf, base, q));
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

    mpz_init_set_si(mpq_numref(q), in);
    mpz_init_set_si(mpq_denref(q), 1L);

    PGMP_RETURN_MPQ(q);
}


static Datum _pmpq_from_double(double in);

PGMP_PG_FUNCTION(pmpq_from_float4)
{
    double in = (double)PG_GETARG_FLOAT4(0);
    return _pmpq_from_double(in);
}

PGMP_PG_FUNCTION(pmpq_from_float8)
{
    double in = PG_GETARG_FLOAT8(0);
    return _pmpq_from_double(in);
}

static Datum
_pmpq_from_double(double in)
{
    mpq_t   q;

    mpq_init(q);
    mpq_set_d(q, in);

    PGMP_RETURN_MPQ(q);
}


/* to convert from int8 we piggyback all the mess we've made for mpz */

Datum pmpz_from_int8(PG_FUNCTION_ARGS);

PGMP_PG_FUNCTION(pmpq_from_int8)
{
    mpq_t           q;
    mpz_t           tmp;

    mpz_from_pmpz(tmp,
        (pmpz *)DirectFunctionCall1(pmpz_from_int8,
            PG_GETARG_DATUM(0)));

    /* Make a copy of the num as MPQ will try to realloc on it */
    mpz_init_set(mpq_numref(q), tmp);
    mpz_init_set_si(mpq_denref(q), 1L);

    PGMP_RETURN_MPQ(q);
}


/* To convert from numeric we convert the numeric in str, then work on that */

PGMP_PG_FUNCTION(pmpq_from_numeric)
{
    mpq_t       q;
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

        if (0 != mpz_init_set_str(mpq_numref(q), sn, 10)) {
            goto error;
        }

        mpz_init_set_str(mpq_denref(q), sd, 10);
        mpq_canonicalize(q);
    }
    else {
        /* just an integer */
        if (0 != mpz_init_set_str(mpq_numref(q), sn, 10)) {
            goto error;
        }
        mpz_init_set_si(mpq_denref(q), 1L);
    }

    PGMP_RETURN_MPQ(q);

error:
    ereport(ERROR, (
        errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
        errmsg("can't convert numeric value to mpq: \"%s\"", sn)));

    PG_RETURN_NULL();
}

PGMP_PG_FUNCTION(pmpq_from_mpz)
{
    mpq_t           q;
    mpz_t           tmp;

    /* Make a copy of the num as MPQ will try to realloc on it */
    PGMP_GETARG_MPZ(tmp, 0);
    mpz_init_set(mpq_numref(q), tmp);
    mpz_init_set_si(mpq_denref(q), 1L);

    PGMP_RETURN_MPQ(q);
}

PGMP_PG_FUNCTION(pmpq_to_mpz)
{
    const mpq_t     q;
    mpz_t           z;

    PGMP_GETARG_MPQ(q, 0);

    mpz_init(z);
    mpz_set_q(z, q);

    PGMP_RETURN_MPZ(z);
}

#define PMPQ_TO_INT(type) \
 \
Datum pmpz_to_ ## type (PG_FUNCTION_ARGS); \
 \
PGMP_PG_FUNCTION(pmpq_to_ ## type) \
{ \
    const mpq_t     q; \
    mpz_t           z; \
 \
    PGMP_GETARG_MPQ(q, 0); \
 \
    mpz_init(z); \
    mpz_set_q(z, q); \
 \
    return DirectFunctionCall1(pmpz_to_ ## type, (Datum)pmpz_from_mpz(z)); \
}

PMPQ_TO_INT(int2)
PMPQ_TO_INT(int4)
PMPQ_TO_INT(int8)


PGMP_PG_FUNCTION(pmpq_to_float4)
{
    const mpq_t     q;
    PGMP_GETARG_MPQ(q, 0);

    PG_RETURN_FLOAT4((float4)mpq_get_d(q));
}

PGMP_PG_FUNCTION(pmpq_to_float8)
{
    const mpq_t     q;
    PGMP_GETARG_MPQ(q, 0);

    PG_RETURN_FLOAT8((float8)mpq_get_d(q));
}


PGMP_PG_FUNCTION(pmpq_to_numeric)
{
    const mpq_t     q;
    int32           typmod;
    long            scale;
    mpz_t           z;
    char            *buf;
    int             sbuf, snum;

    PGMP_GETARG_MPQ(q, 0);
    typmod = PG_GETARG_INT32(1);

    /* Parse precision and scale from the type modifier */
    if (typmod >= VARHDRSZ) {
        scale = (typmod - VARHDRSZ) & 0xffff;
    }
    else {
        scale = 15;
    }

    if (scale) {
        /* Convert q into a scaled z */
        char *cmult;
        mpz_t mult;

        /* create 10000... with as many 0s as the scale */
        cmult = (char *)palloc(scale + 2);
        memset(cmult + 1, '0', scale);
        cmult[0] = '1';
        cmult[scale + 1] = '\0';
        mpz_init_set_str(mult, cmult, 10);
        pfree(cmult);

        mpz_init(z);
        mpz_mul(z, mpq_numref(q), mult);
        sbuf = mpz_sizeinbase(z, 10);       /* size of the output buffer */
        mpz_tdiv_q(z, z, mpq_denref(q));
        snum = mpz_sizeinbase(z, 10);       /* size of the number */
    }
    else {
        /* Just truncate q into an integer */
        mpz_init(z);
        mpz_set_q(z, q);
        sbuf = snum = mpz_sizeinbase(z, 10);
    }

    /* If the numer is 0, everything is a special case: bail out */
    if (mpz_cmp_si(z, 0) == 0) {
        return DirectFunctionCall3(numeric_in,
            CStringGetDatum("0"),
            ObjectIdGetDatum(0),            /* unused 2nd value */
            Int32GetDatum(typmod));
    }

    /* convert z into a string */
    buf = palloc(sbuf + 3);                 /* add sign, point and null */
    mpz_get_str(buf, 10, z);
    if (scale) {
        char *end, *p;

        /* Left pad with 0s the number if smaller than the buffer */
        if (snum < sbuf) {
            char *num0 = buf + (buf[0] == '-'); /* start of the num w/o sign */
            memmove(num0 + (sbuf - snum), num0, snum + 1);
            memset(num0, '0', sbuf - snum);
        }

        end = buf + strlen(buf);

        /* Add the decimal point in the right place */
        memmove(end - scale + 1, end - scale, scale + 1);
        end[-scale] = '.';

        /* delete trailing 0s or they will be used to add extra precision */
        if (typmod < VARHDRSZ) {        /* scale was not specified */
            for (p = end; p > (end - scale) && *p == '0'; --p) {
                *p = '\0';
            }
            /* Don't leave a traliling point */
            if (*p == '.') {
                *p = '\0';
            }
        }
    }

    /* use numeric_in to build the value from the string and to apply the
     * typemod (which may result in overflow) */
    return DirectFunctionCall3(numeric_in,
        CStringGetDatum(buf),
        ObjectIdGetDatum(0),    /* unused 2nd value */
        Int32GetDatum(typmod));
}


/*
 * Constructor and accessors to num and den
 */

PGMP_PG_FUNCTION(pmpq_mpz_mpz)
{
    const mpz_t     num;
    const mpz_t     den;
    mpq_t           q;

    /* We must take a copy of num and den because they may be modified by
     * canonicalize */
    PGMP_GETARG_MPZ(num, 0);
    PGMP_GETARG_MPZ(den, 1);
    ERROR_IF_DENOM_ZERO(den);

    /* Put together the input and canonicalize */
    mpz_init_set(mpq_numref(q), num);
    mpz_init_set(mpq_denref(q), den);
    mpq_canonicalize(q);

    PGMP_RETURN_MPQ(q);
}

PGMP_PG_FUNCTION(pmpq_int4_int4)
{
    int32 num = PG_GETARG_INT32(0);
    int32 den = PG_GETARG_INT32(1);
    mpq_t           q;

    /* Put together the input and canonicalize */
    mpz_init_set_si(mpq_numref(q), (long)num);
    mpz_init_set_si(mpq_denref(q), (long)den);
    ERROR_IF_DENOM_ZERO(mpq_denref(q));
    mpq_canonicalize(q);

    PGMP_RETURN_MPQ(q);
}

PGMP_PG_FUNCTION(pmpq_numeric_numeric)
{
    char        *sn;
    char        *sd;
    mpq_t       q;

    sn = DatumGetCString(DirectFunctionCall1(numeric_out, PG_GETARG_DATUM(0)));
    if (0 != mpz_init_set_str(mpq_numref(q), sn, 10))
    {
        ereport(ERROR, (
            errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
            errmsg("can't handle numeric value at numerator: %s", sn),
            errhint("the mpq components should be integers")));
    }

    sd = DatumGetCString(DirectFunctionCall1(numeric_out, PG_GETARG_DATUM(1)));
    if (0 != mpz_init_set_str(mpq_denref(q), sd, 10))
    {
        ereport(ERROR, (
            errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
            errmsg("can't handle numeric value at denominator: %s", sd),
            errhint("the mpq components should be integers")));
    }

    ERROR_IF_DENOM_ZERO(mpq_denref(q));
    mpq_canonicalize(q);

    PGMP_RETURN_MPQ(q);
}

PGMP_PG_FUNCTION(pmpq_num)
{
    const mpq_t     q;
    mpz_t           z;

    PGMP_GETARG_MPQ(q, 0);

    mpz_init_set(z, mpq_numref(q));

    PGMP_RETURN_MPZ(z);
}

PGMP_PG_FUNCTION(pmpq_den)
{
    const mpq_t     q;
    mpz_t           z;

    PGMP_GETARG_MPQ(q, 0);

    mpz_init_set(z, mpq_denref(q));

    PGMP_RETURN_MPZ(z);
}


