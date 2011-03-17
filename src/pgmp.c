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

#include <gmp.h>
#include "postgres.h"
#include "fmgr.h"

#include "pgmp-impl.h"

PG_MODULE_MAGIC;

void    _PG_init(void);
void    _PG_fini(void);

static void     *_pgmp_alloc(size_t alloc_size);
static void     *_pgmp_realloc(void *ptr, size_t old_size, size_t new_size);
static void     _pgmp_free(void *ptr, size_t size);


/* A couple of constant limbs used to create constant mp? data
 * from the content of varlena data */
const mp_limb_t _pgmp_limb_0 = 0;
const mp_limb_t _pgmp_limb_1 = 1;


/*
 * Module initialization and cleanup
 */

void
_PG_init(void)
{
    /* A vow to the gods of the memory allocation */
    mp_set_memory_functions(
        _pgmp_alloc, _pgmp_realloc, _pgmp_free);
}

void
_PG_fini(void)
{
}


/*
 * GMP custom allocation functions using PostgreSQL memory management.
 *
 * In order to store data into the database, the structure must be contiguous
 * in memory. GMP instead allocated the limbs dynamically. This means that to
 * convert from mpz_p to the varlena a memcpy would be required.
 *
 * But we don't like memcpy... So we allocate enough space to add the varlena
 * header and we return an offsetted pointer to GMP, so that we can always
 * scrubble a varlena header in front of the limbs and just ask the database
 * to store the result.
 */

static void *
_pgmp_alloc(size_t size)
{
    return PGMP_MAX_HDRSIZE + (char *)palloc(size + PGMP_MAX_HDRSIZE);
}

static void *
_pgmp_realloc(void *ptr, size_t old_size, size_t new_size)
{
    return PGMP_MAX_HDRSIZE + (char *)repalloc(
        (char *)ptr - PGMP_MAX_HDRSIZE,
        new_size + PGMP_MAX_HDRSIZE);
}

static void
_pgmp_free(void *ptr, size_t size)
{
    pfree((char *)ptr - PGMP_MAX_HDRSIZE);
}


