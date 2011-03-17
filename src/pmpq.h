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
    unsigned    mdata;          /* version, sign, limbs in numer */
    mp_limb_t   data[1];        /* limbs: numer, then denom */

} pmpq;

/* Postgres only allows 2^30 bytes in varlena. Because each limb is at least 4
 * bytes we need at most 2^28 bits to store the size. So we can use the
 * higher 4 bits for other stuff: we use 3 bits for the version and 1 for the
 * sign */

#define PMPQ_VERSION_MASK           0x70000000U
#define PMPQ_SIGN_MASK              0x80000000U
#define PMPQ_NUMER_SIZE_MASK        0x0FFFFFFFU

/* Must be not larger than PGMP_MAX_HDRSIZE */
#define PMPQ_HDRSIZE   MAXALIGN(offsetof(pmpq,data))

#define PMPQ_VERSION(mq) ((((mq)->mdata) & PMPQ_VERSION_MASK) >> 28)
#define PMPQ_SET_VERSION(mdata,v) \
    (((mdata) & ~PMPQ_VERSION_MASK) | (((v) << 28) & PMPQ_VERSION_MASK))

#define PMPQ_SET_NEGATIVE(mdata)    ((mdata) | PMPQ_SIGN_MASK)
#define PMPQ_SET_POSITIVE(mdata)    ((mdata) & ~PMPQ_SIGN_MASK)
#define PMPQ_NEGATIVE(mq)           (((mq)->mdata) & PMPQ_SIGN_MASK)

#define PMPQ_NLIMBS(mq) ((VARSIZE(mq) - PMPQ_HDRSIZE) / sizeof(mp_limb_t))
#define PMPQ_SIZE_NUMER(mq)   (((mq)->mdata) & PMPQ_NUMER_SIZE_MASK)
#define PMPQ_SIZE_DENOM(mq)   (PMPQ_NLIMBS(mq) - PMPQ_SIZE_NUMER(mq))
#define PMPQ_SET_SIZE_NUMER(mdata,s) \
    (((mdata) & ~PMPQ_NUMER_SIZE_MASK) | ((s) & PMPQ_NUMER_SIZE_MASK))

#define PG_GETARG_PMPQ(x) \
    ((pmpq*)DatumGetPointer(PG_DETOAST_DATUM(PG_GETARG_DATUM(x))))
#define PG_RETURN_MPQ(q) \
    PG_RETURN_POINTER(pmpq_from_mpq(q))

pmpq * pmpq_from_mpq(mpq_ptr q);
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

