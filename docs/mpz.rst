`!mpz` data type
================

The `!mpz` data type can store integer numbers of arbitrary size, up to the
maximum memory allowed by PostgreSQL data types (about 1GB).

Every PostgreSQL integer type (`!int16`, `!int32`, `!int64`) can be converted
to `!mpz`. Not integer types (`!float4`, `!float8`, `!numeric`) are truncated.
Cast from integer types are automatic, whereas non integer require explicit
cast (but are implicitly converted in assignment).

.. code-block:: sql

    SELECT mpz(10000);      -- Cast using mpz as a function
    SELECT 10000::mpz;      -- PostgreSQL-style cast

.. todo:: Not integer input

`!mpz` values can be compared using the regular PostgreSQL comparison
operators. Indexes on `!mpz` columns can be created using the *btree* method.


`!mpz` textual input/output
---------------------------

.. function:: mpz(text)
.. function:: mpz(text, base)

    Convert a textual representation into an `!mpz` number. The form
    :samp:`{text}::mpz` is equivalent to :samp:`mpz({text})`.

    White space is allowed in the string, and is simply ignored.

    The *base* may vary from 2 to 62, or if *base* is 0 or omitted, then the
    leading characters are used: ``0x`` and ``0X`` for hexadecimal, ``0b`` and
    ``0B`` for binary, ``0`` for octal, or decimal otherwise.

    For bases up to 36, case is ignored; upper-case and lower-case letters
    have the same value.  For bases 37 to 62, upper-case letter represent the
    usual 10..35 while lower-case letter represent 36..61.

    .. code-block:: sql

        =# SELECT '0x10'::mpz AS hex, '10'::mpz AS dec , '010'::mpz AS oct, '0b10'::mpz AS bin;
         hex | dec | oct | bin
        -----+-----+-----+-----
         16  | 10  | 8   | 2

.. todo:: I should write a psql pygmets lexer

.. todo:: Handle base 0 in mpz(str, int)

.. function:: text(z)

.. function:: text(z, base)

    Convert the `!mpz` *z* into a string. The form :samp:`{z}::text` is
    equivalent to :samp:`text({z})`.

    *base* may vary from 2 to 62 or from −2 to −36.  For base in the range
    2..36, digits and lower-case letters are used; for −2..−36, digits and
    upper-case letters are used; for 37..62, digits, upper-case letters, and
    lower-case letters (in that significance order) are used. If *base* is not
    specified, 10 is assumed.

.. todo::
    Not sure if handling 0 correctly here too, or if it should handled at all

.. todo:: write a wrap() function


Arithmetic Operators and Functions
----------------------------------

These operators can either work on `!mpz` arguments or take an integer
argument that will be implicitly converted.  Operators taking a :math:`2^n`
argument always use an integer as right argument.

A few operators may use optimized algorithms when one of the arguments is an
integer.

.. note::
    GMP defines many structures in terms of `!long` or `!unsigned long`, whose
    definitions may vary across platforms. PostgreSQL instead offers data
    types with a defined number of bytes (e.g. `!int4`, `!int8`). For this
    reason, functions taking an integer as argument are defined as `!int8`,
    but they may actually fail if the server is 32 bit and the argument don't
    fit into an `!int4`.

.. table:: Arithmetic operators

    =========== =============================== =================== ===========
    Operator    Description                     Example             Return
    =========== =============================== =================== ===========
    `!-`        Unary minus                     `!- 5::mpz`         -5
    `!+`        Unary plus                      `!+ 5::mpz`         5
    `!+`        Addition                        `!2::mpz + 3::mpz`  5
    `!-`        Subtraction                     `!2::mpz - 3::mpz`  -1
    `!*`        Multiplication                  `!7::mpz * 3::mpz`  21
    `!<<`       Multiplication by :math:`2^n`   `!3::mpz << 2`      12
    `!^`        Power (1)                       `!3::mpz ^ 2`       9
    =========== =============================== =================== ===========

Notes:

(1)
    See also the `exponentiation functions`_.


.. function:: abs(z)

    Return the absolute value of *z*.


Division Operators and Functions
--------------------------------

.. table:: Division operators

    =========== =============================== ==================== =======
    Operator    Description                     Example              Return
    =========== =============================== ==================== =======
    `!/`        Division quotient               `!7::mpz / 3::mpz`   2

                Rounding towards zero           `!-7::mpz / 3::mpz`  -2

    `!%`        Division reminder               `!7::mpz % 3::mpz`   1

                Rounding towards zero           `!-7::mpz % 3::mpz`  -1

    `+/`        Division quotient               `!7::mpz +/ 3::mpz`  3

                Rounding towards +infinity      `!-7::mpz +/ 3::mpz` -2

    `+%`        Division reminder               `!7::mpz +% 3::mpz`  -2

                Rounding towards +infinity      `!-7::mpz +% 3::mpz` -1

    `!-/`       Division quotient               `!7::mpz -/ 3::mpz`  2

                Rounding towards -infinity      `!-7::mpz -/ 3::mpz` -3

    `!-%`       Division reminder               `!7::mpz -% 3::mpz`  1

                Rounding towards -infinity      `!-7::mpz -% 3::mpz` 2

    `/!`        Exact division (1)              `!21::mpz /! 7::mpz` 3
    =========== =============================== ==================== =======

Notes:

(1)
    The exact division operator (`!/!`) produce correct results only when it
    is known in advance that :math:`d` divides :math:`n`.  The operator is
    much faster than the other division operators, and is the best choice when
    exact division is known to occur, for example reducing a rational to
    lowest terms.


.. table:: Division operators for powers of 2

    ======== ==================================== =================== =======
    Operator Description                          Example             Return
    ======== ==================================== =================== =======
    `!>>`    Quotient of division by :math:`2^n`  `!1027::mpz >> 3`   128

             Rounding towards zero                `!-1027::mpz >> 3`  -128

    `!%>`    Remainder of division by :math:`2^n` `!1027::mpz %> 3`   3

             Rounding towards zero                `!-1027::mpz %> 3`  -3

    `!+>>`   Quotient of division by :math:`2^n`  `!1027::mpz +>> 3`  129

             Rounding towards +infinity           `!-1027::mpz +>> 3` -128

    `!+%>`   Remainder of division by :math:`2^n` `!1027::mpz +%> 3`  -5

             Rounding towards +infinity           `!-1027::mpz +%> 3` -3

    `!->>`   Quotient of division by :math:`2^n`  `!1027::mpz ->> 3`  128

             Rounding towards -infinity           `!-1027::mpz ->> 3` -129

    `!-%>`   Remainder of division by :math:`2^n` `!1027::mpz -%> 3`  3

             Rounding towards -infinity           `!-1027::mpz -%> 3` 5
    ======== ==================================== =================== =======

For all the division-related operators :math:`n \oslash d`, :math:`q` and
:math:`r` will satisfy :math:`n = q \cdot d + r`, and :math:`r` will satisfy
:math:`0 \le |r| \lt |d|`.

.. note::
    Only the truncating division and reminder (`!/` and `!%`) have the correct
    precedence respect to addition, subtraction and multiplication.
    See `the PostgreSQL precedence table`__ for further details.

    .. __: http://www.postgresql.org/docs/9.0/static/sql-syntax-lexical.html#SQL-PRECEDENCE-TABLE


.. function:: divisible(n, d)

.. function:: divisible_2exp(n, b)

    Return `!true` if *n* is exactly divisible by *d*, or in the case of
    `!divisible_2exp()` by :math:`2^b`.

    :math:`n` is divisible by :math:`d` if there exists an integer :math:`q`
    satisfying :math:`n = q \cdot d`.  Unlike the other division operators,
    *d*\=0 is accepted and following the rule it can be seen that only 0
    is considered divisible by 0.


.. function:: congruent(n, c, d)

.. function:: congruent_2exp(n, c, b)

    Return `!true` if *n* is congruent to *c* modulo *d*, or in the case of
    `!congruent_2exp()` modulo :math:`2^b`.

    :math:`n` is congruent to :math:`c \mod d` if there exists an integer
    :math:`q` satisfying :math:`n = c + q \cdot d`. Unlike the other division
    operators, *d*\=0 is accepted and following the rule it can be seen that n
    and c are considered congruent mod 0 only when exactly equal.

    .. todo:: functions `divisible()` and `congruent()`


.. todo:: integer fast path

.. todo::
    fast path on int64 for 64 bit backends? Maybe introduce a long data type?


Exponentiation Functions
------------------------

.. function:: pow(base, exp)

    Return *base* raised to *exp*.

    *exp* is defined as `!int8` but must fit into a `!long` as defined on the
    server.

    The function is an alias for the `!^` operator.


.. function:: powm(base, exp, mod)

    Return (*base* raised to *exp*) modulo *mod*.

    Negative *exp* is supported if an inverse *base^-1* mod *mod* exists (see
    `invert()` function). If an inverse doesn't exist then a divide by zero is
    raised.


.. function:: powm_sec(base, exp, mod)

    Return (*base* raised to *exp*) modulo *mod*.

    It is required that *exp* > 0 and that *mod* is odd.

    This function is designed to take the same time and have the same cache
    access patterns for any two same-size arguments, assuming that function
    arguments are placed at the same position and that the machine state is
    identical upon function entry. This function is intended for cryptographic
    purposes, where resilience to side-channel attacks is desired.

.. todo:: `powm()`, `powm_sec()`.


Root Extraction Functions
-------------------------

.. function:: root(op, n)

    Return the truncated integer part of the *n*\th root of *op*.

    *n* is defined as `!int8` but must fit into a `!long` as defined on the
    server.

.. function:: rootrem(op, n)

    Return a tuple of 2 elements with the truncated integer part of the *n*\th
    root of *op* and the remainder (*i.e.* *op* - *root* ^ *n*).

    .. code-block:: sql

        =# select * from rootrem(28, 3);
         root | rem
        ------+-----
         3    | 1

.. function:: sqrt(op)

    Return the truncated integer part of the square root of *op*.

.. function:: sqrtrem(op)

    Return a tuple of 2 elements with the truncated integer part of the square
    root of *op* and the remainder (*i.e.* *op* - *root* \* *root*).

    .. code-block:: sql

        =# select * from sqrtrem(83);
         root | rem
        ------+-----
         9    | 2

.. function:: perfect_power(op)

    Return ``true`` if *op* is a perfect power, *i.e.*, if there exist
    integers :math:`a` and :math:`b`, with :math:`b>1`, such that *op* equals
    :math:`a^b`.

    Under this definition both 0 and 1 are considered to be perfect powers.
    Negative values of op are accepted, but of course can only be odd perfect
    powers.

.. function:: perfect_square(op)

    Return ``true`` if *op* is a perfect square, *i.e.*, if the square root of
    *op* is an integer. Under this definition both 0 and 1 are considered to
    be perfect squares.


Logical and Bit Manipulation Functions
--------------------------------------

.. table:: Logical Operators

    ======== ======================== =================================== ===================
    Operator Description              Example                             Return
    ======== ======================== =================================== ===================
    `!&`     Bitwise and              `!'0b10001'::mpz & '0b01001'::mpz`  `!'0b1'::mpz`
    `!|`     Bitwise inclusive-or     `!'0b10001'::mpz | '0b01001'::mpz`  `!'0b11001'::mpz`
    `!#`     Bitwise exclusive-or     `!'0b10001'::mpz # '0b01001'::mpz`  `!'0b11000'::mpz`
    `!-~`    One's complement         `!-~'0b10001'::mpz`                 `!'-0b10010'::mpz`
    ======== ======================== =================================== ===================
