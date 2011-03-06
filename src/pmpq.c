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


/*
 * Create a new pmpq structure from the content of a mpq
 */
pmpq *
pmpq_from_mpq(mpq_srcptr q)
{
    pmpq *res;
    int nsize = SIZ(mpq_numref(q));

    if (0 != nsize)
    {
        int nalloc = ABS(nsize);
        int dsize = SIZ(mpq_denref(q));

        res = (pmpq *)palloc(
            PMPQ_HDRSIZE + (nalloc + dsize) * sizeof(mp_limb_t));
        res->num_size = nsize;
        res->den_size = dsize;

        SET_VARSIZE(res,
            PMPQ_HDRSIZE + (nalloc + dsize) * sizeof(mp_limb_t));
        memcpy(&(res->data), LIMBS(mpq_numref(q)),
            nalloc * sizeof(mp_limb_t));
        memcpy(&(res->data) + nalloc, LIMBS(mpq_denref(q)),
            dsize * sizeof(mp_limb_t));
    }
    else
    {
        res = (pmpq *)palloc0(sizeof(pmpq));
        SET_VARSIZE(res, sizeof(pmpq));
    }

    return res;
}


/* Two constant to address to create the representation of 0 */
static const mp_limb_t limb0 = 0;
static const mp_limb_t limb1 = 1;

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
    mpq_ptr wq = (mpq_ptr)q;

    if (pq->num_size != 0) {
        int nalloc = ABS(pq->num_size);

        /* We have data from numer and denom into the datum */
        ALLOC(mpq_numref(wq)) = nalloc;
        SIZ(mpq_numref(wq)) = pq->num_size;
        LIMBS(mpq_numref(wq)) = (mp_limb_t *)pq->data;

        ALLOC(mpq_denref(wq)) = pq->den_size;
        SIZ(mpq_denref(wq)) = pq->den_size;
        LIMBS(mpq_denref(wq)) = (mp_limb_t *)pq->data + nalloc;
    }
    else {
        /* in the datum there is not 1/0,
         * so let's just refer to some static const */
        ALLOC(mpq_numref(wq)) = 1;
        SIZ(mpq_numref(wq)) = 0;
        LIMBS(mpq_numref(wq)) = (mp_limb_t *)(&limb0);

        ALLOC(mpq_denref(wq)) = 1;
        SIZ(mpq_denref(wq)) = 1;
        LIMBS(mpq_denref(wq)) = (mp_limb_t *)(&limb1);
    }
}

