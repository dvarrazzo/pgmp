/* pgmp-impl -- Implementation details
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

#ifndef __PGMP_IMPL_H__
#define __PGMP_IMPL_H__


/* Space to leave before the pointers returned by the GMP custom allocator.
 *
 * The space is used to store the varlena and structure header before the gmp
 * limbs, so it must be at least as big as the bigger of such headers.
 */
#define PGMP_MAX_HDRSIZE 8

#define MPZ_IS_ZERO(z) (SIZ(z) == 0)

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


/* Less noise in the creation of postgresql functions */
#define PGMP_PG_FUNCTION(name) \
\
PG_FUNCTION_INFO_V1(name); \
\
Datum name(PG_FUNCTION_ARGS); \
\
Datum name(PG_FUNCTION_ARGS)


#endif  /* __PGMP_IMPL_H__ */


