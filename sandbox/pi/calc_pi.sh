#!/bin/bash

# A script to calculate pi using parallel backends
# See calc_pi.py for a more advanced version.
#
# Copyright (C) 2011-2020 Daniele Varrazzo

export DBARGS=regression
export NDIGITS=$1

psql -q -c "drop table if exists pi;" $DBARGS
psql -q -c "create table pi (mult int4, arccot mpz);" $DBARGS

psql -Atq -c "insert into pi values (183, arccot(239, 10::mpz ^ ($NDIGITS + 10))) returning 1;" $DBARGS >&2 &
psql -Atq -c "insert into pi values (32, arccot(1023, 10::mpz ^ ($NDIGITS + 10))) returning 2;" $DBARGS >&2 &
psql -Atq -c "insert into pi values (-68, arccot(5832, 10::mpz ^ ($NDIGITS + 10))) returning 3;" $DBARGS >&2 &
wait;
psql -Atq -c "insert into pi values (12, arccot(110443, 10::mpz ^ ($NDIGITS + 10))) returning 4;" $DBARGS >&2 &
psql -Atq -c "insert into pi values (-12, arccot(4841182, 10::mpz ^ ($NDIGITS + 10))) returning 5;" $DBARGS >&2 &
psql -Atq -c "insert into pi values (-100, arccot(6826318, 10::mpz ^ ($NDIGITS + 10))) returning 6;" $DBARGS >&2 &
wait;

psql -At -c "select 4 * sum(mult * arccot) / (10::mpz ^ 10) from pi;" $DBARGS

