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

.PHONY: docs

# You may have to customize this to run the test suite
# REGRESS_OPTS=--user postgres

PG_CONFIG=pg_config

SHLIB_LINK=-lgmp -lm

PGMP_VERSION=$(shell grep default_version pgmp.control | sed -e "s/default_version = '\(.*\)'/\1/")
PG91 = $(shell $(PG_CONFIG) --version | grep -qE " 8\.| 9\.0" && echo pre91 || echo 91)

EXTENSION=pgmp
MODULEDIR=pgmp
MODULE_big = pgmp
OBJS = src/pgmp.o src/pgmp_utils.o \
	src/pmpz.o src/pmpz_io.o src/pmpz_arith.o src/pmpz_agg.o \
	src/pmpz_roots.o src/pmpz_theor.o src/pmpz_bits.o src/pmpz_rand.o \
	src/pmpq.o src/pmpq_io.o src/pmpq_arith.o src/pmpq_agg.o

SRCFILES = $(wildcard src/*.[ch])
TESTFILES = $(wildcard sql/*.sql) $(wildcard expected/*.out)
DOCS = $(wildcard docs/*.rst) docs/conf.py docs/Makefile docs/_static/pgmp.css

PKGFILES = AUTHORS COPYING README Makefile \
	pgmp.control pgmp.pysql uninstall_pgmp.sql \
	$(SRCFILES) $(DOCS) $(TESTFILES) \
	$(wildcard tools/*.py)

ifeq ($(PG91),91)
INSTALLSCRIPT=pgmp--$(PGMP_VERSION).sql
UPGRADESCRIPT=pgmp--unpackaged--$(PGMP_VERSION).sql
DATA = $(INSTALLSCRIPT) $(UPGRADESCRIPT)
else
INSTALLSCRIPT=pgmp.sql
DATA = $(INSTALLSCRIPT) uninstall_pgmp.sql
endif

REGRESS = setup-$(PG91) mpz mpq
EXTRA_CLEAN = $(INSTALLSCRIPT) $(UPGRADESCRIPT)

PKGNAME = pgmp-$(PGMP_VERSION)
SRCPKG = dist/$(PKGNAME).tar.gz

USE_PGXS=1
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

# added to the targets defined in pgxs
all: $(INSTALLSCRIPT) $(UPGRADESCRIPT)

$(INSTALLSCRIPT): pgmp.pysql
	tools/unmix.py < $< > $@

$(UPGRADESCRIPT): $(INSTALLSCRIPT)
	tools/sql2extension.py --extname pgmp $< > $@

docs:
	$(MAKE) -C docs

sdist: $(SRCPKG)

$(SRCPKG): $(PKGFILES)
	ln -sf . $(PKGNAME)
	mkdir -p dist
	rm -rf $@
	tar czvf $@ $(addprefix $(PKGNAME)/,$^)
	rm $(PKGNAME)
