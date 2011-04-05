/* pgmp_utils -- misc utility module
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

#include "pgmp_utils.h"

#if PG_VERSION_NUM < 90000

#include "nodes/nodes.h"            /* for IsA */
#include "nodes/execnodes.h"        /* for AggState */

/*
 * AggCheckCallContext - test if a SQL function is being called as an aggregate
 *
 * The function is available from PG 9.0. This allows compatibility with
 * previous versions.
 */
int
AggCheckCallContext(FunctionCallInfo fcinfo, MemoryContext *aggcontext)
{
    if (fcinfo->context && IsA(fcinfo->context, AggState))
    {
        if (aggcontext) {
            *aggcontext = ((AggState *) fcinfo->context)->aggcontext;
        }
        return AGG_CONTEXT_AGGREGATE;
    }

#if PG_VERSION_NUM >= 80400

    if (fcinfo->context && IsA(fcinfo->context, WindowAggState))
    {
        if (aggcontext) {
            /* different from PG 9.0: in PG 8.4 there is no aggcontext */
            *aggcontext = ((WindowAggState *) fcinfo->context)->wincontext;
        }
        return AGG_CONTEXT_WINDOW;
    }

#endif

    /* this is just to prevent "uninitialized variable" warnings */
    if (aggcontext) {
        *aggcontext = NULL;
    }

    return 0;
}
#endif

