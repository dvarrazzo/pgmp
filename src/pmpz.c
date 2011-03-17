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
#include "pgmp-impl.h"

#include "fmgr.h"


/* To be referred to to represent the zero */
extern const mp_limb_t _pgmp_limb_0;


/*
 * Create a pmpz structure from the content of a mpz.
 *
 * The function relies on the limbs being allocated using the GMP custom
 * allocator: such allocator leaves PGMP_MAX_HDRSIZE bytes *before* the
 * returned pointer. We scrubble that area prepending the pmpz header.
 */
pmpz *
pmpz_from_mpz(mpz_srcptr z)
{
    pmpz *res;
    int size = SIZ(z);

    res = (pmpz *)((char *)LIMBS(z) - PMPZ_HDRSIZE);

    if (LIKELY(0 != size))
    {
        size_t slimbs;
        int sign;

        if (size > 0) {
            slimbs = size * sizeof(mp_limb_t);
            sign = 0;
        }
        else {
            slimbs = -size * sizeof(mp_limb_t);
            sign = PMPZ_SIGN_MASK;
        }

        SET_VARSIZE(res, PMPZ_HDRSIZE + slimbs);
        res->mdata = sign;          /* implicit version: 0 */
    }
    else
    {
        /* In the zero representation there are no limbs */
        SET_VARSIZE(res, PMPZ_HDRSIZE);
        res->mdata = 0;             /* version: 0 */
    }

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
    int nlimbs;
    mpz_ptr wz;

    if (UNLIKELY(0 != (PMPZ_VERSION(pz)))) {
        ereport(ERROR, (
            errcode(ERRCODE_DATA_EXCEPTION),
            errmsg("unsupported mpz version: %d", PMPZ_VERSION(pz))));
    }

    /* discard the const qualifier */
    wz = (mpz_ptr)z;

    nlimbs = (VARSIZE(pz) - PMPZ_HDRSIZE) / sizeof(mp_limb_t);
    if (LIKELY(nlimbs != 0))
    {
        ALLOC(wz) = nlimbs;
        SIZ(wz) = PMPZ_NEGATIVE(pz) ? -nlimbs : nlimbs;
        LIMBS(wz) = (mp_limb_t *)pz->data;
    }
    else
    {
        /* in the datum there is just the varlena header
         * so let's just refer to some static const */
        ALLOC(wz) = 1;
        SIZ(wz) = 0;
        LIMBS(wz) = (mp_limb_t *)&_pgmp_limb_0;
    }
}

