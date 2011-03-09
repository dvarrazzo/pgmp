/* pmpq -- PostgreSQL data type for GMP mpq
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

#ifndef __PMPQ_H__
#define __PMPQ_H__

#include <gmp.h>
#include "postgres.h"

typedef struct
{
    char        vl_len_[4];     /* varlena header */
    int         num_size;       /* number of limbs in numerator, with sign */
    int         den_size;       /* number of limbs in denominator */
    mp_limb_t   data[1];        /* limbs */

} pmpq;

#define PMPQ_HDRSIZE   MAXALIGN(offsetof(pmpq,data))
#define PG_GETARG_PMPQ(x) \
    ((pmpq*)DatumGetPointer(PG_DETOAST_DATUM(PG_GETARG_DATUM(x))))
#define PG_RETURN_MPQ(q) \
    PG_RETURN_POINTER(pmpq_from_mpq(q))

pmpq * pmpq_from_mpq(mpq_srcptr q);
void mpq_from_pmpq(mpq_srcptr q, const pmpq *pq);


/* macro to report division by zero on denominator */

#define ERROR_IF_DENOM_ZERO(arg) \
 \
    { \
        if (UNLIKELY(MPZ_IS_ZERO(arg))) \
        { \
            ereport(ERROR, ( \
                errcode(ERRCODE_DIVISION_BY_ZERO), \
                errmsg("denominator can't be zero"))); \
        } \
    } while (0)

#endif  /* __PMPQ_H__ */

