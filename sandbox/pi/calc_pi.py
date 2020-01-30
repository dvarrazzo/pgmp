#!/usr/bin/env python
"""
A script to calculate pi using parallel PostgreSQL processes.

Copyright (C) 2011-2020 Daniele Varrazzo
"""

import eventlet
eventlet.patcher.monkey_patch()

import sys
import psycopg2

def main():
    dsn = 'dbname=regression'
    nprocs = 4
    ndigits = int(sys.argv[1])

    def arccot((mult, arg)):
        print >>sys.stderr, 'start', arg
        cnn = psycopg2.connect(dsn)
        cnn.set_isolation_level(0)
        cur = cnn.cursor()
        cur.execute("""
            insert into pi
            values (%s, arccot(%s, 10::mpz ^ (%s + 10)))
            returning %s;
            """, (mult, arg, ndigits, arg))
        rv = cur.fetchone()[0]
        cnn.close()
        print >>sys.stderr, 'end', arg

    cnn = psycopg2.connect(dsn)
    cnn.set_isolation_level(0)
    cur = cnn.cursor()
    cur.execute("""
        drop table if exists pi;
        create table pi (mult int4, arccot mpz);
        """)

    pool = eventlet.GreenPool(nprocs)
    list(pool.imap(arccot, [
        (183, 239),
        (32, 1023),
        (-68, 5832),
        (12, 110443),
        (-12, 4841182),
        (-100, 6826318), ]))

    cur.execute("select 4 * sum(mult * arccot) / (10::mpz ^ 10) from pi;")
    print cur.fetchone()[0]

if __name__ == '__main__':
    main()

