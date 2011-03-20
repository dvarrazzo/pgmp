/* pmpz -- PostgreSQL data type for GMP mpz
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

#define PG_GETARG_PMPZ(x) \
    ((pmpz*)DatumGetPointer(PG_DETOAST_DATUM(PG_GETARG_DATUM(x))))
#define PG_RETURN_MPZ(z) \
    PG_RETURN_POINTER(pmpz_from_mpz(z))

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


/* Definition useful for internal use in mpz-related modules */

pmpz * pmpz_from_mpz(mpz_srcptr z);
void mpz_from_pmpz(mpz_srcptr z, const pmpz *pz);

#define MPZ_IS_ZERO(z) (SIZ(z) == 0)


/* Macros to be used in functions wrappers to limit the arguments domain */

#define PMPZ_NO_CHECK(arg)

#define PMPZ_CHECK_DIV0(arg) \
{ \
    if (UNLIKELY(MPZ_IS_ZERO(arg))) \
    { \
        ereport(ERROR, ( \
            errcode(ERRCODE_DIVISION_BY_ZERO), \
            errmsg("division by zero"))); \
    } \
} while (0)

#endif  /* __PMPZ_H__ */

