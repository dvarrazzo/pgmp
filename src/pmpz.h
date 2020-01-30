/* pmpz -- PostgreSQL data type for GMP mpz
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

#ifndef __PMPZ_H__
#define __PMPZ_H__

#include <gmp.h>
#include "postgres.h"

typedef struct
{
    char        vl_len_[4];     /* varlena header */
    unsigned    mdata;          /* version number, sign */
    mp_limb_t   data[1];        /* limbs */

} pmpz;

/* Must be not larger than PGMP_MAX_HDRSIZE */
#define PMPZ_HDRSIZE   MAXALIGN(offsetof(pmpz,data))


/* Macros to convert mpz arguments and return values */

#define PGMP_GETARG_PMPZ(n) \
    ((pmpz*)DatumGetPointer(PG_DETOAST_DATUM(PG_GETARG_DATUM(n))))

#define PGMP_GETARG_MPZ(z,n) \
    mpz_from_pmpz(z, PGMP_GETARG_PMPZ(n));

#define PGMP_RETURN_MPZ(z) \
    PG_RETURN_POINTER(pmpz_from_mpz(z))

#define PGMP_RETURN_MPZ_MPZ(z1,z2) \
do { \
    TupleDesc       _tupdesc; \
    Datum           _result[2]; \
    bool            _isnull[2] = {0,0}; \
 \
    if (get_call_result_type(fcinfo, NULL, &_tupdesc) != TYPEFUNC_COMPOSITE) \
        ereport(ERROR, \
            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), \
             errmsg("function returning composite called in context " \
                    "that cannot accept type composite"))); \
 \
    _tupdesc = BlessTupleDesc(_tupdesc); \
 \
    _result[0] = (Datum)pmpz_from_mpz(z1); \
    _result[1] = (Datum)pmpz_from_mpz(z2); \
 \
    return HeapTupleGetDatum(heap_form_tuple(_tupdesc, _result, _isnull)); \
} while (0)

#define PGMP_RETURN_MPZ_MPZ_MPZ(z1,z2,z3) \
do { \
    TupleDesc       _tupdesc; \
    Datum           _result[3]; \
    bool            _isnull[3] = {0,0,0}; \
 \
    if (get_call_result_type(fcinfo, NULL, &_tupdesc) != TYPEFUNC_COMPOSITE) \
        ereport(ERROR, \
            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), \
             errmsg("function returning composite called in context " \
                    "that cannot accept type composite"))); \
 \
    _tupdesc = BlessTupleDesc(_tupdesc); \
 \
    _result[0] = (Datum)pmpz_from_mpz(z1); \
    _result[1] = (Datum)pmpz_from_mpz(z2); \
    _result[2] = (Datum)pmpz_from_mpz(z3); \
 \
    return HeapTupleGetDatum(heap_form_tuple(_tupdesc, _result, _isnull)); \
} while (0)


/* Allow versioning of the data in the database.
 * Versions 0-7 allowed... hope to not change my mind more than 8 times */
#define PMPZ_VERSION_MASK   0x07
#define PMPZ_SIGN_MASK      0x80

#define PMPZ_VERSION(mz) (((mz)->mdata) & PMPZ_VERSION_MASK)
#define PMPZ_SET_VERSION(mdata,v) \
    (((mdata) & ~PMPZ_VERSION_MASK) | ((v) & PMPZ_VERSION_MASK))

#define PMPZ_SET_NEGATIVE(mdata) ((mdata) | PMPZ_SIGN_MASK)
#define PMPZ_SET_POSITIVE(mdata) ((mdata) & ~PMPZ_SIGN_MASK)
#define PMPZ_NEGATIVE(mz) (((mz)->mdata) & PMPZ_SIGN_MASK)


/* Definitions useful for internal use in mpz-related modules */

pmpz * pmpz_from_mpz(mpz_srcptr z);
void mpz_from_pmpz(mpz_srcptr z, const pmpz *pz);
int pmpz_get_int64(mpz_srcptr z, int64 *out);
Datum pmpz_get_hash(mpz_srcptr z);

#define MPZ_IS_ZERO(z) (SIZ(z) == 0)


/* Macros to be used in functions wrappers to limit the arguments domain */

#define PMPZ_NO_CHECK(arg)

#define PMPZ_CHECK_DIV0(arg) \
do { \
    if (UNLIKELY(MPZ_IS_ZERO(arg))) \
    { \
        ereport(ERROR, ( \
            errcode(ERRCODE_DIVISION_BY_ZERO), \
            errmsg("division by zero"))); \
    } \
} while (0)

#define PMPZ_CHECK_NONEG(arg) \
do { \
    if (UNLIKELY(SIZ(arg) < 0)) \
    { \
        ereport(ERROR, ( \
            errcode(ERRCODE_INVALID_PARAMETER_VALUE), \
            errmsg("argument can't be negative"))); \
    } \
} while (0)

#define PMPZ_CHECK_LONG_POS(arg) \
do { \
    if (UNLIKELY((arg) <= 0)) { \
        ereport(ERROR, ( \
            errcode(ERRCODE_INVALID_PARAMETER_VALUE), \
            errmsg("argument must be positive") )); \
    } \
} while (0)

#define PMPZ_CHECK_LONG_NONEG(arg) \
do { \
    if (UNLIKELY((arg) < 0)) { \
        ereport(ERROR, ( \
            errcode(ERRCODE_INVALID_PARAMETER_VALUE), \
            errmsg("argument can't be negative") )); \
    } \
} while (0)

#endif  /* __PMPZ_H__ */

