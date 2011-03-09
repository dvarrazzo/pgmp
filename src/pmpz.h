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
    int         size;           /* number of limbs */
    mp_limb_t   data[1];        /* limbs */

} pmpz;

#define PMPZ_HDRSIZE   MAXALIGN(offsetof(pmpz,data))
#define PG_GETARG_PMPZ(x) \
    ((pmpz*)DatumGetPointer(PG_DETOAST_DATUM(PG_GETARG_DATUM(x))))
#define PG_RETURN_MPZ(z) \
    PG_RETURN_POINTER(pmpz_from_mpz(z))

pmpz * pmpz_from_mpz(mpz_srcptr z);
void mpz_from_pmpz(mpz_srcptr z, const pmpz *pz);

#endif  /* __PMPZ_H__ */

