/* pgmp -- PostgreSQL GMP module
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

#ifndef __PGMP_H__
#define __PGMP_H__

#include <gmp.h>
#include "postgres.h"

typedef struct
{
    char        vl_len_[4];     /* varlena header */
    int         size;           /* number of limbs */
    mp_limb_t   data[1];        /* limbs */

} pmpz;

#define PMPZ_HDRSIZE   MAXALIGN(offsetof(pmpz,data))
#define PG_GETARG_PMPZ(x)   ((pmpz*)DatumGetPointer(PG_DETOAST_DATUM(PG_GETARG_DATUM(x))))


/*
 * Macros equivalent to the ones defimed in gmp-impl.h
 */

#define ABS(x)      ((x) >= 0 ? (x) : -(x))
#define SIZ(z)      ((z)->_mp_size)
#define NLIMBS(z)   ABS((z)->_mp_size)
#define LIMBS(z)    ((z)->_mp_d)
#define ALLOC(v)    ((v)->_mp_alloc)

#endif  /* __PGMP_H__ */
