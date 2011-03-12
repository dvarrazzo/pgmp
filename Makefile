# pgmp -- pgxs-based makefile
#
# Copyright (C) 2011 Daniele Varrazzo
#
# This file is part of the PostgreSQL GMP Module
#
# The PostgreSQL GMP Module is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 3 of the License,
# or (at your option) any later version.
#
# The PostgreSQL GMP Module is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
# General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with the PostgreSQL GMP Module.  If not, see
# http://www.gnu.org/licenses/.

USE_PGXS=1

SHLIB_LINK=-lgmp
REGRESS_OPTS=--user postgres

MODULE_big = pgmp
OBJS = src/pgmp.o src/pgmp_test.o \
	src/pmpz.o src/pmpza.o src/pmpz_io.o src/pmpz_arith.o \
	src/pmpq.o src/pmpq_io.o src/pmpq_arith.o

DATA_built = pgmp.sql
DATA = uninstall_pgmp.sql
REGRESS = mpz mpq
EXTRA_CLEAN = pgmp.sql.in

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

pgmp.sql.in: pgmp.pysql
	tools/unmix.py < $< > $@

