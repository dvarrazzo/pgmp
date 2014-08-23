`!mpq` data type
================

The `!mpq` data type can store rational numbers whose denominator and
numerator have arbitrary size.

Rational numbers are converted in canonical form on input (meaning that the
denominator and the numerator have no common factors) and all the
operators will return a number in canonical form.

PostgreSQL integer types (`!int16`, `!int32`, `!int64`), `!numeric` and `mpz`
can be converted to `!mpq` without loss of precision and without surprise.
Floating point types (`!float4`, `!float8`) are converted without loss as
well... but with some surprise, as many fractions with finite decimal
expansion have no finite expansion in binary.

.. code-block:: psql

    =# select 10.1::numeric::mpq as "numeric",
    -#        10.1::float4::mpq as "single",
    -#        10.1::float8::mpq as "double";
     numeric |     single     |              double
    ---------+----------------+----------------------------------
     101/10  | 5295309/524288 | 5685794529555251/562949953421312


`!mpq` values can be converted to integer types (both PostgreSQL's and
`!mpz`): the result will be truncated. Conversion to `!float4` and `!float8`
will round the values to the precision allowed by the types (in case of
overflow the value will be *Infinity*). Conversion to `!numeric` will perform
a rounding to the precision set for the target type.

.. code-block:: psql

    =# select mpq('4/3')::integer as "integer",
    -#        mpq('4/3')::float4 as "single",
    -#        mpq('4/3')::decimal(10,3) as "decimal";
     integer | single  | decimal
    ---------+---------+---------
           1 | 1.33333 |   1.333


`!mpq` values can be compared using the regular PostgreSQL comparison
operators. Indexes on `!mpq` columns can be created using the *btree* or the
*hash* method.


`!mpq` textual input/output
---------------------------

.. function:: mpq(text)
              mpq(text, base)

    Convert a textual representation into an `!mpq` number. The form
    :samp:`{text}::mpq` is equivalent to :samp:`mpq({text})`.

    The string can be an integer like ``41`` or a fraction like ``41/152``.
    The fraction will be converted in canonical form, so common factors
    between denominator and numerator will be removed.

    The numerator and optional denominator are parsed the same as in `mpz`.
    White space is allowed in the string, and is simply ignored. The base can
    vary from 2 to 62, or if base is 0 then the leading characters are used:
    ``0x`` or ``0X`` for hex, ``0b`` or ``0B`` for binary, ``0`` for octal, or
    decimal otherwise. Note that this is done separately for the numerator and
    denominator, so for instance ``0xEF/100`` is 239/100, whereas
    ``0xEF/0x100`` is 239/256.

    .. note:: The maximum base accepted by GMP 4.1 is 36, not 62.


.. function:: text(q)
              text(q, base)

    Convert the `!mpq` *q* into a string. The form :samp:`{q}::text` is
    equivalent to :samp:`text({q})`.

    The string will be of the form :samp:`{num}/{den}`, or if the denominator
    is 1 then just :samp:`{num}`.

    *base* may vary from 2 to 62 or from -2 to -36.  For base in the range
    2..36, digits and lower-case letters are used; for -2..-36, digits and
    upper-case letters are used; for 37..62, digits, upper-case letters, and
    lower-case letters (in that significance order) are used. If *base* is not
    specified, 10 is assumed.

    .. note:: The maximum base accepted by GMP 4.1 is 36, not 62.


`!mpq` conversions
------------------

.. function:: mpq(num, den)

    Return an `!mpq` from its numerator and denominator.

    .. note::
        The function signature accepts `!mpz` values. PostgreSQL integers are
        implicitly converted to `!mpz` so invoking the function as
        ``mpq(30,17)`` will work as expected. However if the numbers become
        too big for an `int8` they will be interpreted by PostgreSQL as
        `!numeric` and, because the cast from `!numeric` to `!mpz` is not
        implicit, the call will fail. Forcing a cast to `!mpz` (*e.g.*
        ``mpq(30::mpz,17::mpz)``) will work for numbers of every size.


.. function:: num(q)
              den(q)

    Return the numerator or the denominator of *q* as `!mpz`.


Arithmetic Operators and Functions
----------------------------------

All the arithmetic operators and functions return their their output in
canonical form.

.. table:: Arithmetic operators

    =========== =============================== =========================== ===========
    Operator    Description                     Example                     Return
    =========== =============================== =========================== ===========
    `!-`        Unary minus                     `!- '4/3'::mpq`             -4/3
    `!+`        Unary plus                      `!+ '4/3'::mpq`             4/3
    `!+`        Addition                        `!'2/3'::mpq + '5/6'::mpq`  3/2
    `!-`        Subtraction                     `!'1/3'::mpq - '5/6'::mpq`  -1/2
    `!*`        Multiplication                  `!'2/3'::mpq * '5/6'::mpq`  5/9
    `!/`        Division                        `!'2/3'::mpq / '5/6'::mpq`  4/5
    `!<<`       Multiplication by :math:`2^n`   `!'2/3'::mpq << 3`          16/3
    `!>>`       Division by :math:`2^n`         `!'2/3'::mpq >> 3`          1/12
    =========== =============================== =========================== ===========


.. function:: abs(q)

    Return the absolute value of *q*.


.. function:: inv(q)

    Return 1/*q*.

.. function:: limit_den(q, max_den=1000000)

    Return the closest rational to *q* with denominator at most *max_den*.

    The function is useful for finding rational approximations to a given
    floating-point number:

    .. code-block:: psql

        =# select limit_den(pi(), 10);
        22/7

    or for recovering a rational number that's represented as a float:

    .. code-block:: psql

        =# select mpq(cos(pi()/3));
        4503599627370497/9007199254740992
        =# select limit_den(cos(pi()/3));
        1/2
        =# select limit_den(10.1::float4);
        101/10

    This function is not part of the GMP library: it is ported instead `from
    the Python library`__.

    .. __: http://docs.python.org/library/fractions.html#fractions.Fraction.limit_denominator

Aggregation functions
---------------------

.. function:: sum(q)

    Return the sum of *q* across all input values.

.. function:: prod(q)

    Return the product of *q* across all input values.

.. function:: max(q)

    Return the maximum value of *q* across all input values.

.. function:: min(q)

    Return the minimum value of *q* across all input values.

