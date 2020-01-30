#!/usr/bin/env python
"""Script to perform comparisons between pmpz and other data types.

Copyright (C) 2011-2020 - Daniele Varrazzo
"""
import sys

import psycopg2

import logging
logger = logging.getLogger()
logging.basicConfig(
    level=logging.INFO,
    stream=sys.stderr,
    format="%(asctime)s %(levelname)s %(message)s",
    datefmt="%Y-%m-%d %H:%M:%S")


class SkipTest(Exception):
    """The test can't be performed for some reason."""

class Benchmark(object):
    """Base class for a test.

    Tests should subclass and create methods called test_whatever. There can
    also be methods setup_whatever that will be invoked just once (the test
    can be repeated if -r is not 1).
    """
    # Subclass may change these details that will appear in the plots.
    title = "Benchmark"
    xlabel = "x axis"
    ylabel = "y axis"

    def __init__(self, opt):
        self.opt = opt

    def run(self):
        opt = self.opt
        self.conn = psycopg2.connect(opt.dsn)
        self.conn.set_isolation_level(0)

        tests = []
        for k in dir(self):
            if k.startswith('test_'):
                tests.append((k[5:], getattr(self, k)))
        tests.sort()

        print "title:", self.title
        print "xlabel:", self.xlabel
        print "ylabel:", self.ylabel

        for n in opt.nsamples:
            for s in opt.size:
                for name, f in tests:
                    # test initialization
                    setup = getattr(self, 'setup_' + name, None)
                    if setup:
                        logger.info("setup %s: n=%d s=%d", name, n, s)
                        try:
                            setup(n, s)
                        except SkipTest, e:
                            logger.info("skipping %s (n=%d s=%d): %s",
                                name, n, s, e)
                            continue

                    # test run
                    results = []
                    for i in xrange(opt.repeats):
                        logger.info("test %s (n=%d s=%s) run %d of %d",
                            name, n, s, i+1, opt.repeats)
                        results.append(f(n, s))
                        logger.info("result: %s", results[-1])

                    result = self.best_value(results)
                    print self.__class__.__name__, name, n, s, result

    def best_value(self, results):
        """Take the best values from a list of results."""
        return min(results)


class SumRational(Benchmark):
    """Test the time used to perform sum(x) for mpq and decimal data types.

    The type represent the same values.
    """
    _title = "Time for sum() for values with scale %s"
    xlabel = "Numbers size (in decimal digits)"
    ylabel = "Time (in millis)"

    @property
    def title(self): return self._title % self.opt.scale

    def setup_numeric(self, n, s):
        self._setup(n, s, "test_sum_rat_numeric",
            "create table test_sum_rat_numeric (n numeric(%s,%s));"
                % (s, self.opt.scale))

    def test_numeric(self, n, s):
        return self._test("test_sum_rat_numeric")

    def setup_mpq(self, n, s):
        self._setup(n, s, "test_sum_rat_mpq",
            "create table test_sum_rat_mpq (n mpq);")

    def test_mpq(self, n, s):
        return self._test("test_sum_rat_mpq")

    def _setup(self, n, s, table, query):
        cur = self.conn.cursor()
        cur.execute("drop table if exists %s;" % table)
        cur.execute(query)
        cur.execute("""
            select randinit();
            select randseed(31415926);

            insert into %s
            select urandomm(%%(max)s::mpz)::mpq / %%(scale)s
            from generate_series(1, %%(n)s);
            """ % table, {
            'max': 10 ** s,
            'scale': 10 ** self.opt.scale,
            'n': n})

        cur.execute("vacuum analyze %s;" % table)

    def _test(self, table):
        cur = self.conn.cursor()
        cur.execute("explain analyze select sum(n) from %s;" % table)
        recs = cur.fetchall()
        return float(recs[-1][0].split()[-2])


class SumInteger(Benchmark):
    """Test the time used to perform sum(n) for different data types.

    n is read from a table.
    """
    title = "Time to calculate sum() on a table"
    xlabel = "Numbers size (in decimal digits)"
    ylabel = "Time (in millis)"

    def setup_mpq(self, n, s):
        self._setup(n, s, "test_sum_mpq",
            "create table test_sum_mpq (n mpq);")

    def setup_mpz(self, n, s):
        self._setup(n, s, "test_sum_mpz",
            "create table test_sum_mpz (n mpz);")

    def setup_numeric(self, n, s):
        self._setup(n, s, "test_sum_numeric",
            "create table test_sum_numeric (n numeric);")

    def setup_int8(self, n, s):
        if s > 18:
            raise SkipTest("skipping test with %d digits" % s)

        self._setup(n, s, "test_sum_int8",
            "create table test_sum_int8 (n int8);")

    def setup_int4(self, n, s):
        if s > 9:
            raise SkipTest("skipping test with %d digits" % s)

        self._setup(n, s, "test_sum_int4",
            "create table test_sum_int4 (n int4);")

    def _setup(self, n, s, table, query):
        cur = self.conn.cursor()
        cur.execute("drop table if exists %s;" % table)
        cur.execute(query)
        cur.execute("""
            select randinit();
            select randseed(31415926);

            insert into %s
            select urandomm(%%(max)s::mpz)s
            from generate_series(1, %%(n)s);
            """ % table,
            { 'max': 10 ** s, 'n': n})

        cur.execute("vacuum analyze %s;" % table)

    def test_mpq(self, n, s):
        return self._test('test_sum_mpq')

    def test_mpz(self, n, s):
        return self._test('test_sum_mpz')

    def test_numeric(self, n, s):
        return self._test('test_sum_numeric')

    def test_int8(self, n, s):
        return self._test('test_sum_int8')

    def test_int4(self, n, s):
        return self._test('test_sum_int4')

    def _test(self, table):
        cur = self.conn.cursor()
        cur.execute("explain analyze select sum(n) from %s;" % table)
        recs = cur.fetchall()
        return float(recs[-1][0].split()[-2])


class Arith(Benchmark):
    """Perform an operation sum(a + b * c / d) on a table.
    """
    title = "Performance on arithmetic operations"
    xlabel = "Numbers size (in decimal digits)"
    ylabel = "Time (in millis)"

    def setup_mpq(self, n, s):
        self._setup(n, s, "test_arith_mpq", """
            create table test_arith_mpq
                (a mpq, b mpq, c mpq, d mpq);""")

    def setup_mpz(self, n, s):
        self._setup(n, s, "test_arith_mpz", """
            create table test_arith_mpz
                (a mpz, b mpz, c mpz, d mpz);""")

    def setup_numeric(self, n, s):
        self._setup(n, s, "test_arith_numeric", """
            create table test_arith_numeric (
                a numeric(%s), b numeric(%s), c numeric(%s), d numeric(%s));
                """ % ((s,s,s,s+1)))

    def setup_int8(self, n, s):
        if s > 9:
            raise SkipTest("skipping test with %d digits" % s)

        self._setup(n, s, "test_arith_int8", """
            create table test_arith_int8
                (a int8, b int8, c int8, d int8);""")

    def setup_int4(self, n, s):
        if s > 4:
            raise SkipTest("skipping test with %d digits" % s)

        self._setup(n, s, "test_arith_int4", """
            create table test_arith_int4
                (a int4, b int4, c int4, d int4);""")

    def test_mpq(self, n, s):
        return self._test("test_arith_mpq")

    def test_mpz(self, n, s):
        return self._test("test_arith_mpz")

    def test_numeric(self, n, s):
        return self._test("test_arith_numeric")

    def test_int8(self, n, s):
        return self._test("test_arith_int8")

    def test_int4(self, n, s):
        return self._test("test_arith_int4")

    def _setup(self, n, s, table, query):
        cur = self.conn.cursor()
        cur.execute("drop table if exists %s;" % table)
        cur.execute(query)
        cur.execute("""
            select randinit();
            select randseed(31415926);

            insert into %s
            select
                urandomm(%%(max)s::mpz), urandomm(%%(max)s::mpz),
                urandomm(%%(max)s::mpz), urandomm(%%(max)s::mpz) + 1
            from generate_series(1, %%(n)s);
            """ % table, {
            'max': 10 ** s,
            'n': n})

        cur.execute("vacuum analyze %s;" % table)

    def _test(self, table):
        cur = self.conn.cursor()
        cur.execute("""explain analyze
            select sum(a + b * c / d)
            from %s;""" % table)
        recs = cur.fetchall()
        return float(recs[-1][0].split()[-2])


class Factorial(Benchmark):
    """Measure the speed to calculate the factorial of n"""
    title = "Time to calculate n!"
    xlabel = "Input number"
    ylabel = "Time (in millis)"

    def setup_mpz(self, n, s):
        self._setup('mpz', 'mpz_mul')

    def setup_numeric(self, n, s):
        self._setup('numeric', 'numeric_mul')

    def _setup(self, type, f):
        cur = self.conn.cursor()
        cur.execute("drop aggregate if exists m(%s);" % type)
        cur.execute("create aggregate m(%s) (sfunc=%s, stype=%s);"
            % (type, f, type))

    def test_mpz(self, n, s):
        return self._test('mpz', s)

    def test_numeric(self, n, s):
        return self._test('numeric', s)

    def _test(self, type, s):
        cur = self.conn.cursor()
        cur.execute("""
            explain analyze
            select m(n::%s)
            from generate_series(1,%s) n;
            """ % (type, s))
        recs = cur.fetchall()
        return float(recs[-1][0].split()[-2])


class TableSize(Benchmark):
    """Measure the size of a table on disk with mpz and decimal columns.
    """
    title = "Size of a table on disk"
    xlabel = "Numbers size (in decimal digits)"
    ylabel = "Size (in pages)"

    def setup_int8(self, n, s):
        if s > 18:
            raise SkipTest("skipping test with %d digits" % s)

    def setup_int4(self, n, s):
        if s > 9:
            raise SkipTest("skipping test with %d digits" % s)

    def test_mpq(self, n, s):
        return self._test(n, s, "test_size_mpq",
            """
            create table test_size_mpq (n mpq);
            insert into test_size_mpq
                select urandomm(%(max)s::mpz)s
                from generate_series(1, %(n)s);
            """)

    def test_mpz(self, n, s):
        return self._test(n, s, "test_size_mpz",
            """
            create table test_size_mpz (n mpz);
            insert into test_size_mpz
                select urandomm(%(max)s::mpz)s
                from generate_series(1, %(n)s);
            """)

    def test_numeric(self, n, s):
        return self._test(n, s, "test_size_numeric",
            """
            create table test_size_numeric (n numeric);
            insert into test_size_numeric
                select urandomm(%(max)s::mpz)s
                from generate_series(1, %(n)s);
            """)

    def test_int8(self, n, s):
        return self._test(n, s, "test_size_int8",
            """
            create table test_size_int8 (n int8);
            insert into test_size_int8
                select urandomm(%(max)s::mpz)s
                from generate_series(1, %(n)s);
            """)

    def test_int4(self, n, s):
        return self._test(n, s, "test_size_int4",
            """
            create table test_size_int4 (n int4);
            insert into test_size_int4
                select urandomm(%(max)s::mpz)s
                from generate_series(1, %(n)s);
            """)

    def _test(self, n, s, table, query):
        cur = self.conn.cursor()
        cur.execute("""
            drop table if exists %s;
            select randinit();
            select randseed(31415926);
            """ % table)
        cur.execute(query, {'n': n, 'max': 10 ** s})
        cur.execute("vacuum analyze %s;" % table)
        cur.execute(
            "select relpages from pg_class where relname = %s;" ,
            (table, ))
        return cur.fetchone()[0]


def main():
    opt = parse_args()
    cls = globals()[opt.test_name]
    test = cls(opt)
    test.run()


def parse_args():
    # Find the tests available
    test_names = sorted(o.__name__ for o in globals().values()
        if isinstance(o, type)
        and issubclass(o, Benchmark) and o is not Benchmark)

    from optparse import OptionParser
    parser = OptionParser(usage="%prog [OPTIONS] TEST-NAME",
        description="choose a test from: %s" % ', '.join(test_names))
    parser.add_option('-n', '--nsamples', type=int, action='append',
        help="number of numbers to sum. specify once or more")
    parser.add_option('-s', '--size', type=int, action='append',
        help="size of numbers to sum. specify once or more")
    parser.add_option('-p', '--scale', type=int,
        help="scale of the tested numbers, if applicable")
    parser.add_option('-r', '--repeats', type=int, default=3,
        help="test repetitions (take the best value) [default: %default]")
    parser.add_option('--dsn', help="database to connect", default="")

    opt, args = parser.parse_args()
    if len(args) != 1:
        parser.error("please specify one test")

    opt.test_name = args[0]
    if opt.test_name not in test_names:
        parser.error("bad test name: '%s'" % opt.test_name)

    if not opt.nsamples or not opt.size:
        parser.error("please specify -n and -s at least once")

    return opt


if __name__ == '__main__':
    sys.exit(main())
