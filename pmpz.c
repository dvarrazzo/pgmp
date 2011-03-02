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

#include "pmpz.h"

#include "fmgr.h"


/*
 * Create a new pmpz structure from the content of a mpz
 */
pmpz *
pmpz_from_mpz(mpz_srcptr z)
{
    size_t  sres;
    size_t  nlimbs;
    size_t  alimbs;
    pmpz *res;

    nlimbs = NLIMBS(z);
    alimbs = nlimbs > 0 ? nlimbs : 1;

    sres = PMPZ_HDRSIZE + alimbs * sizeof(mp_limb_t);

    res = (pmpz *)palloc(sres);
    SET_VARSIZE(res, sres);
    res->size = SIZ(z);
    memcpy(&(res->data), LIMBS(z), alimbs * sizeof(mp_limb_t));

    return res;
}


/*
 * Initialize a mpz from the content of a datum
 *
 * NOTE: the function takes a pointer to a const and changes the structure.
 * This allows to define the structure as const in the calling function and
 * avoid the risk to change it inplace, which may corrupt the database data.
 *
 * The structure populated doesn't own the pointed data, so it must not be
 * changed in any way and must not be cleared.
 */
void
mpz_from_pmpz(mpz_srcptr z, const pmpz *pz)
{
    /* discard the const qualifier */
    mpz_ptr wz = (mpz_ptr)z;

    ALLOC(wz) = pz->size ? ABS(pz->size) : 1;
    SIZ(wz) = pz->size;
    LIMBS(wz) = (mp_limb_t *)pz->data;
}

