/* pgmp-impl -- Implementation details
 *
 * Copyright (C) 2011-2020 Daniele Varrazzo
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
 * https://www.gnu.org/licenses/.
 */

#ifndef __PGMP_IMPL_H__
#define __PGMP_IMPL_H__


#include <limits.h>         /* for LONG_MAX etc. */


/* Ensure we know what platform are we working on.
 *
 * GMP defines many structures and functions in term of long/ulong, while
 * Postgres always uses data types with an explicit number of bytes (int32,
 * int64 etc). Ensure we know what to do when passing arguments around.
 */
#if LONG_MAX == INT32_MAX
#define PGMP_LONG_32 1
#define PGMP_LONG_64 0
#endif

#if LONG_MAX == INT64_MAX
#define PGMP_LONG_32 0
#define PGMP_LONG_64 1
#endif

#if !(PGMP_LONG_32 || PGMP_LONG_64)
#error Expected platform where long is either 32 or 64 bits
#endif


/* Space to leave before the pointers returned by the GMP custom allocator.
 *
 * The space is used to store the varlena and structure header before the gmp
 * limbs, so it must be at least as big as the bigger of such headers.
 */
#define PGMP_MAX_HDRSIZE 8

/*
 * Macros equivalent to the ones defimed in gmp-impl.h
 */

#define ABS(x)      ((x) >= 0 ? (x) : -(x))
#define SIZ(z)      ((z)->_mp_size)
#define NLIMBS(z)   ABS((z)->_mp_size)
#define LIMBS(z)    ((z)->_mp_d)
#define ALLOC(v)    ((v)->_mp_alloc)

/* Allow direct user access to numerator and denominator of a mpq_t object.  */
#define mpq_numref(Q)   (&((Q)->_mp_num))
#define mpq_denref(Q)   (&((Q)->_mp_den))

/* __builtin_expect is in gcc 3.0, and not in 2.95. */
#if __GNUC__ >= 3
#define LIKELY(cond)    __builtin_expect ((cond) != 0, 1)
#define UNLIKELY(cond)  __builtin_expect ((cond) != 0, 0)
#else
#define LIKELY(cond)    (cond)
#define UNLIKELY(cond)  (cond)
#endif

/* Not available before GMP 5 */
#if (__GNU_MP_VERSION < 5)
typedef unsigned long int   mp_bitcnt_t;
#endif


/* Less noise in the creation of postgresql functions */
#define PGMP_PG_FUNCTION(name) \
\
PG_FUNCTION_INFO_V1(name); \
\
Datum name(PG_FUNCTION_ARGS); \
\
Datum name(PG_FUNCTION_ARGS)


/* Macros to get a long/ulong argument in wrappers.
 *
 * The argument SQL type should be int8. The macros may raise exception if the
 * input doesn't fit in a long/ulong as defined on the server.
 */
#if PGMP_LONG_32

#define PGMP_GETARG_LONG(tgt,narg) \
do { \
    int64 _tmp = PG_GETARG_INT64(narg); \
    if (!(LONG_MIN <= _tmp && _tmp <= LONG_MAX)) { \
        ereport(ERROR, ( \
            errcode(ERRCODE_INVALID_PARAMETER_VALUE), \
            errmsg("argument too large for a long: %lld", _tmp) )); \
    } \
    tgt = (long)_tmp; \
} while (0)

#define PGMP_GETARG_ULONG(tgt,narg) \
do { \
    int64 _tmp = PG_GETARG_INT64(narg); \
    if (_tmp > ULONG_MAX) { \
        ereport(ERROR, ( \
            errcode(ERRCODE_INVALID_PARAMETER_VALUE), \
            errmsg("argument too large for an unsigned long: %lld", _tmp) )); \
    } \
    if (_tmp < 0) { \
        ereport(ERROR, ( \
            errcode(ERRCODE_INVALID_PARAMETER_VALUE), \
            errmsg("argument can't be negative") )); \
    } \
    tgt = (unsigned long)_tmp; \
} while (0)

#elif PGMP_LONG_64

#define PGMP_GETARG_LONG(tgt,narg) \
    tgt = (long)PG_GETARG_INT64(narg);

#define PGMP_GETARG_ULONG(tgt,narg) \
do { \
    int64 _tmp = PG_GETARG_INT64(narg); \
    if (_tmp < 0) { \
        ereport(ERROR, ( \
            errcode(ERRCODE_INVALID_PARAMETER_VALUE), \
            errmsg("argument can't be negative") )); \
    } \
    tgt = (unsigned long)_tmp; \
} while (0)

#endif

/* Not available e.g. on GMP 4.1 */
#ifndef __GMP_MP_RELEASE
#define __GMP_MP_RELEASE ( \
    __GNU_MP_VERSION * 10000 + \
    __GNU_MP_VERSION_MINOR * 100 + \
    __GNU_MP_VERSION_PATCHLEVEL)
#endif

/* The text parsing functions have different range across versions */
#if __GMP_MP_RELEASE >= 40200
#define PGMP_MAXBASE_IO 62
#else
#define PGMP_MAXBASE_IO 36
#endif


#endif  /* __PGMP_IMPL_H__ */


