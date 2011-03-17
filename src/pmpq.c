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

#include "pmpq.h"
#include "pgmp-impl.h"

#include "fmgr.h"


/* To be referred to to represent the zero */
extern const mp_limb_t _pgmp_limb_0;
extern const mp_limb_t _pgmp_limb_1;


/*
 * Create a pmpq structure from the content of a mpq
 *
 * The function is not const as the numerator will be realloc'd to make room
 * to the denom limbs after it. For this reason this function must never
 * receive directly data read from the database.
 */
pmpq *
pmpq_from_mpq(mpq_ptr q)
{
    pmpq        *res;
    mpz_ptr     num     = mpq_numref(q);
    mpz_ptr     den     = mpq_denref(q);
    int         nsize   = SIZ(num);

    if (LIKELY(0 != nsize))
    {
        /* Make enough room after the numer to store the denom limbs */
        int     nalloc      = ABS(nsize);
        int     dsize       = SIZ(mpq_denref(q));

        LIMBS(num) = _mpz_realloc(num, nalloc + dsize);
        res = (pmpq *)((char *)LIMBS(num) - PMPQ_HDRSIZE);

        /* copy the denom after the numer */
        memcpy(res->data + nalloc, LIMBS(den), dsize * sizeof(mp_limb_t));

        /* Set the number of limbs and implicitly version 0, then the sign */
        SET_VARSIZE(res, PMPQ_HDRSIZE + (nalloc + dsize) * sizeof(mp_limb_t));
        res->mdata = PMPQ_SET_SIZE_NUMER(0, nalloc);
        if (nsize < 0) { res->mdata = PMPQ_SET_NEGATIVE(res->mdata); }
    }
    else
    {
        res = (pmpq *)((char *)LIMBS(num) - PMPQ_HDRSIZE);
        SET_VARSIZE(res, PMPQ_HDRSIZE);
        res->mdata = 0;
    }

    return res;
}


/*
 * Initialize a mpq from the content of a datum
 *
 * NOTE: the function takes a pointer to a const and changes the structure.
 * This allows to define the structure as const in the calling function and
 * avoid the risk to change it inplace, which may corrupt the database data.
 *
 * The structure populated doesn't own the pointed data, so it must not be
 * changed in any way and must not be cleared.
 */
void
mpq_from_pmpq(mpq_srcptr q, const pmpq *pq)
{
    /* discard the const qualifier */
    mpq_ptr     wq      = (mpq_ptr)q;
    mpz_ptr     num     = mpq_numref(wq);
    mpz_ptr     den     = mpq_denref(wq);

    if (0 != PMPQ_NLIMBS(pq))
    {
        /* We have data from numer and denom into the datum */
        ALLOC(num) = SIZ(num) = PMPQ_SIZE_NUMER(pq);
        LIMBS(num) = (mp_limb_t *)pq->data;
        if (PMPQ_NEGATIVE(pq)) { SIZ(num) = -SIZ(num); }

        ALLOC(den) = SIZ(den) = PMPQ_SIZE_DENOM(pq);
        LIMBS(den) = (mp_limb_t *)pq->data + ALLOC(num);
    }
    else {
        /* in the datum there is not 1/0,
         * so let's just refer to some static const */
        ALLOC(num) = 1;
        SIZ(num) = 0;
        LIMBS(num) = (mp_limb_t *)(&_pgmp_limb_0);

        ALLOC(den) = 1;
        SIZ(den) = 1;
        LIMBS(den) = (mp_limb_t *)(&_pgmp_limb_1);
    }
}

