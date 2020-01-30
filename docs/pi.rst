[Ab]using PostgreSQL to calculate :math:`\pi`
=============================================

Once the crime of binding the GMP_ multiple precision arithmetic library into
PostgreSQL_ has been perpetrated, using it to calculate one million digits of
:math:`\pi` is the least evil.

.. _GMP: https://www.gmplib.org/
.. _PostgreSQL: https://www.postgresql.org/

Calculation is performed using a `Machin-like formula`__, a generalization of
the Machin's formula:

.. math::

    \frac{\pi}{4} = 4 \; \tan^{-1}\frac{1}{5} - \tan^{-1}\frac{1}{239}

known as one of the most efficient ways to compute a large number of digits of
:math:`\pi`. Specifically we will use the Chien-Lih Hwang 1997 formula (further
details and references in the above link).

Calculation is performed using the `mpz` data type, thus using integers:
to calculate an approximation of :math:`\tan^{-1}\frac{1}{x}` to :math:`n`
digits, we use a Taylor series scaled to :math:`10^n`. Full details are
provided in this nice `LiteratePrograms article`__.

.. __: https://en.wikipedia.org/wiki/Machin-like_formula
.. __: https://web.archive.org/web/20111211140154/http://en.literateprograms.org/Pi_with_Machin's_formula_(Python)

It is possible to implement the `!arccot()` function using a
`common table expression`__ from PostgreSQL 8.4:

.. __: https://www.postgresql.org/docs/current/queries-with.html
.. _PL/pgSQL: https://www.postgresql.org/docs/current/plpgsql.html

.. code-block:: postgresql

    CREATE OR REPLACE FUNCTION arccot(x mpz, unity mpz)
    RETURNS mpz AS
    $$
    WITH RECURSIVE t(n, xp) AS (
        VALUES (3, $2 / -($1 ^ 3))
      UNION ALL
        SELECT n+2, xp / -($1 ^ 2)
        FROM t
        WHERE xp <> 0
    )
    SELECT sum(xp / n) + ($2 / $1) FROM t;
    $$
    LANGUAGE sql IMMUTABLE STRICT;

However using this method all the temporary terms in the calculation are kept
in memory, and the complex bookkeeping results in a non optimal use of the
CPU. The performance can be improved using `PL/pgSQL`_, which may be not the
fastest language around, but it's perfectly suitable for this task, as the
time spent in the function body is negligible respect to the time spent in the
arithmetic functions with huge operators.

.. code-block:: postgresql

    CREATE OR REPLACE FUNCTION arccot(x mpz, unity mpz)
    RETURNS mpz AS
    $$
    DECLARE
        xp mpz := unity / x;
        xp2 mpz := -(x ^ 2);
        acc mpz := xp;
        n mpz := 3;
        term mpz;
    BEGIN
        LOOP
            xp := xp / xp2;
            term := xp / n;
            EXIT WHEN term = 0;
            acc := acc + term;
            n := n + 2;
        END LOOP;
        RETURN acc;
    END
    $$
    LANGUAGE plpgsql IMMUTABLE STRICT;

And the Machin-like formula using it:

.. code-block:: postgresql

    CREATE OR REPLACE FUNCTION pi_hwang_97(ndigits int)
    RETURNS mpz AS
    $$
    DECLARE
        unity mpz = 10::mpz ^ (ndigits + 10);
    BEGIN
        RETURN 4 * (
            183 * arccot(239, unity)
            + 32 * arccot(1023, unity)
            - 68 * arccot(5832, unity)
            + 12 * arccot(110443, unity)
            - 12 * arccot(4841182, unity)
            - 100 * arccot(6826318, unity)
            ) / (10::mpz ^ 10);
    END
    $$
    LANGUAGE plpgsql IMMUTABLE STRICT;

This function produces 200,000 digits of :math:`\pi` in 30 sec on my laptop
(Intel i5 2.53GHz). As a comparison, the equivalent Python implementation
(using the Python built-in long integer implementation) takes almost 4 minutes.

The function above has the shortcoming of using only one of the 4 cores
available on my machine. But the formula itself is easy to parallelize as it
contains 6 independent terms: we can produce each term in a separate backend
process, storing the computation into a table (it's a database, after all!)
and composing the result as a last step. The following Python script
implements this idea:

.. code-block:: python

    import eventlet
    eventlet.patcher.monkey_patch()

    import sys
    import psycopg2

    dsn = 'dbname=regression'
    nprocs = 4
    ndigits = int(sys.argv[1])

    cnn = psycopg2.connect(dsn)
    cnn.set_isolation_level(0)
    cur = cnn.cursor()
    cur.execute("""
        drop table if exists pi;
        create table pi (mult int4, arccot mpz);
        """)

    def arccot((mult, arg)):
        cnn = psycopg2.connect(dsn)
        cnn.set_isolation_level(0)
        cur = cnn.cursor()
        cur.execute("""
            insert into pi
            values (%s, arccot(%s, 10::mpz ^ (%s + 10)));
            """, (mult, arg, ndigits))
        cnn.close()

    pool = eventlet.GreenPool(nprocs)
    list(pool.imap(arccot, [
        (183, 239), (32, 1023), (-68, 5832),
        (12, 110443), (-12, 4841182), (-100, 6826318)]))

    cur.execute("select 4 * sum(mult * arccot) / (10::mpz ^ 10) from pi;")
    print cur.fetchone()[0]

The script uses 4 cores concurrently to calculate the intermediate terms: it
produces 200,000 digits in about 13 seconds and 1 million digits in 8 minutes
and 30 seconds.

