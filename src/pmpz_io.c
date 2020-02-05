/* pmpz_io -- mpz Input/Output functions
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
#include "utils/builtins.h"     /* for numeric_out */

#include <math.h>               /* for isinf, isnan */


/*
 * Input/Output functions
 */

PGMP_PG_FUNCTION(pmpz_in)
{
    char    *str;
    mpz_t   z;

    str = PG_GETARG_CSTRING(0);

    if (0 != mpz_init_set_str(z, str, 0))
    {
        const char *ell;
        const int maxchars = 50;
        ell = (strlen(str) > maxchars) ? "..." : "";

        ereport(ERROR, (
            errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
            errmsg("invalid input for mpz: \"%.*s%s\"",
                maxchars, str, ell)));
    }

    PGMP_RETURN_MPZ(z);
}

PGMP_PG_FUNCTION(pmpz_in_base)
{
    int     base;
    char    *str;
    mpz_t   z;

    base = PG_GETARG_INT32(1);

    if (!(base == 0 || (2 <= base && base <= PGMP_MAXBASE_IO)))
    {
        ereport(ERROR, (
            errcode(ERRCODE_INVALID_PARAMETER_VALUE),
            errmsg("invalid base for mpz input: %d", base),
            errhint("base should be between 2 and %d", PGMP_MAXBASE_IO)));
    }

    str = TextDatumGetCString(PG_GETARG_POINTER(0));

    if (0 != mpz_init_set_str(z, str, base))
    {
        const char *ell;
        const int maxchars = 50;
        ell = (strlen(str) > maxchars) ? "..." : "";

        ereport(ERROR, (
            errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
            errmsg("invalid input for mpz base %d: \"%.*s%s\"",
                base, 50, str, ell)));
    }

    PGMP_RETURN_MPZ(z);
}

PGMP_PG_FUNCTION(pmpz_out)
{
    const mpz_t     z;
    char            *buf;

    PGMP_GETARG_MPZ(z, 0);

    /* We must allocate the output buffer ourselves because the buffer
     * returned by mpz_get_str actually starts a few bytes before (because of
     * the custom GMP allocator); Postgres will try to free the pointer we
     * return in printtup() so with the offsetted pointer a segfault is
     * granted. */
    buf = palloc(mpz_sizeinbase(z, 10) + 2);        /* add sign and null */
    PG_RETURN_CSTRING(mpz_get_str(buf, 10, z));
}

PGMP_PG_FUNCTION(pmpz_out_base)
{
    const mpz_t     z;
    int             base;
    char            *buf;

    PGMP_GETARG_MPZ(z, 0);
    base = PG_GETARG_INT32(1);

    if (!((-36 <= base && base <= -2) ||
        (2 <= base && base <= PGMP_MAXBASE_IO)))
    {
        ereport(ERROR, (
            errcode(ERRCODE_INVALID_PARAMETER_VALUE),
            errmsg("invalid base for mpz output: %d", base),
            errhint("base should be between -36 and -2 or between 2 and %d",
                PGMP_MAXBASE_IO)));
    }

    /* Allocate the output buffer manually - see pmpz_out to know why */
    buf = palloc(mpz_sizeinbase(z, ABS(base)) + 2);     /* add sign and null */
    PG_RETURN_CSTRING(mpz_get_str(buf, base, z));
}


/*
 * Cast functions
 */

static Datum _pmpz_from_long(long in);
static Datum _pmpz_from_double(double in);

PGMP_PG_FUNCTION(pmpz_from_int2)
{
    int16 in = PG_GETARG_INT16(0);
    return _pmpz_from_long(in);
}

PGMP_PG_FUNCTION(pmpz_from_int4)
{
    int32 in = PG_GETARG_INT32(0);
    return _pmpz_from_long(in);
}

PGMP_PG_FUNCTION(pmpz_from_int8)
{
    int64   in = PG_GETARG_INT64(0);

#if PGMP_LONG_64

    return _pmpz_from_long(in);

#elif PGMP_LONG_32

    int         neg = 0;
    uint32      lo;
    uint32      hi;
    mpz_t       z;

    if (LIKELY(in != INT64_MIN))
    {
        if (in < 0) {
            neg = 1;
            in = -in;
        }

        lo = in & 0xFFFFFFFFUL;
        hi = in >> 32;

        if (hi) {
            mpz_init_set_ui(z, hi);
            mpz_mul_2exp(z, z, 32);
            mpz_add_ui(z, z, lo);
        }
        else {
            mpz_init_set_ui(z, lo);
        }

        if (neg) {
            mpz_neg(z, z);
        }
    }
    else {
        /* this would overflow the long */
        mpz_init_set_si(z, 1L);
        mpz_mul_2exp(z, z, 63);
        mpz_neg(z, z);
    }

    PGMP_RETURN_MPZ(z);

#endif
}


static Datum
_pmpz_from_long(long in)
{
    mpz_t   z;

    mpz_init_set_si(z, in);

    PGMP_RETURN_MPZ(z);
}


PGMP_PG_FUNCTION(pmpz_from_float4)
{
    float4 in = PG_GETARG_FLOAT4(0);
    return _pmpz_from_double(in);
}

PGMP_PG_FUNCTION(pmpz_from_float8)
{
    float8 in = PG_GETARG_FLOAT8(0);
    return _pmpz_from_double(in);
}

static Datum
_pmpz_from_double(double in)
{
    mpz_t   z;

    if (isinf(in) || isnan(in)) {
        ereport(ERROR, (
            errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
            errmsg("can't convert float value to mpz: \"%f\"", in)));
    }

    mpz_init_set_d(z, in);

    PGMP_RETURN_MPZ(z);
}


PGMP_PG_FUNCTION(pmpz_from_numeric)
{
    char    *str;
    char    *p;
    mpz_t   z;

    /* convert the numeric into string. */
    str = DatumGetCString(DirectFunctionCall1(numeric_out,
        PG_GETARG_DATUM(0)));

    /* truncate the string if it contains a decimal dot */
    if ((p = strchr(str, '.'))) { *p = '\0'; }

    if (0 != mpz_init_set_str(z, str, 10))
    {
        /* here str may have been cropped, but I expect this error
         * only triggered by NaN, so not in case of regular number */
        ereport(ERROR, (
            errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
            errmsg("can't convert numeric value to mpz: \"%s\"", str)));
    }

    PGMP_RETURN_MPZ(z);
}


PGMP_PG_FUNCTION(pmpz_to_int2)
{
    const mpz_t     z;
    int16           out;

    PGMP_GETARG_MPZ(z, 0);

    if (!mpz_fits_sshort_p(z)) {
        ereport(ERROR, (
            errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
            errmsg("numeric value too big to be converted to int2 data type")));
    }

    out = mpz_get_si(z);
    PG_RETURN_INT16(out);
}

PGMP_PG_FUNCTION(pmpz_to_int4)
{
    const mpz_t     z;
    int32           out;

    PGMP_GETARG_MPZ(z, 0);

    if (!mpz_fits_sint_p(z)) {
        ereport(ERROR, (
            errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
            errmsg("numeric value too big to be converted to int4 data type")));
    }

    out = mpz_get_si(z);
    PG_RETURN_INT32(out);
}

PGMP_PG_FUNCTION(pmpz_to_int8)
{
    const mpz_t     z;
    int64           ret = 0;

    PGMP_GETARG_MPZ(z, 0);

    if (0 != pmpz_get_int64(z, &ret)) {
        ereport(ERROR, (
            errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
            errmsg("numeric value too big to be converted to int8 data type")));
    }

    PG_RETURN_INT64(ret);
}

/* Convert an mpz into and int64.
 *
 * return 0 in case of success, else a nonzero value
 */
int
pmpz_get_int64(mpz_srcptr z, int64 *out)
{

#if PGMP_LONG_64

    if (mpz_fits_slong_p(z)) {
        *out = mpz_get_si(z);
        return 0;
    }

#elif PGMP_LONG_32

    switch (SIZ(z)) {
        case 0:
            *out = 0LL;
            return 0;
            break;
        case 1:
            *out = (int64)(LIMBS(z)[0]);
            return 0;
            break;
        case -1:
            *out = -(int64)(LIMBS(z)[0]);
            return 0;
            break;
        case 2:
            if (LIMBS(z)[1] < 0x80000000L) {
                *out = (int64)(LIMBS(z)[1]) << 32
                    | (int64)(LIMBS(z)[0]);
                return 0;
            }
            break;
        case -2:
            if (LIMBS(z)[1] < 0x80000000L) {
                *out = -((int64)(LIMBS(z)[1]) << 32
                    | (int64)(LIMBS(z)[0]));
                return 0;
            }
            else if (LIMBS(z)[0] == 0 && LIMBS(z)[1] == 0x80000000L) {
                *out = -0x8000000000000000LL;
                return 0;
            }
            break;
    }

#endif

    return -1;
}


PGMP_PG_FUNCTION(pmpz_to_float4)
{
    const mpz_t     z;
    double          out;

    PGMP_GETARG_MPZ(z, 0);
    out = mpz_get_d(z);
    PG_RETURN_FLOAT4((float4)out);
}

PGMP_PG_FUNCTION(pmpz_to_float8)
{
    const mpz_t     z;
    double          out;

    PGMP_GETARG_MPZ(z, 0);
    out = mpz_get_d(z);
    PG_RETURN_FLOAT8((float8)out);
}

