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

#if PG_VERSION_NUM < 90000

#include "postgres.h"       /* for MemoryContext */
#include "fmgr.h"           /* for FunctionCallInfo */

#define AGG_CONTEXT_AGGREGATE   1       /* regular aggregate */
#define AGG_CONTEXT_WINDOW      2       /* window function */

int AggCheckCallContext(FunctionCallInfo fcinfo, MemoryContext *aggcontext);

#endif

