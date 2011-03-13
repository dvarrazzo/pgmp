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

.. function:: text(mpz)

.. function:: text(mpz, base)

    Convert an *mpz* into a string. The form :samp:`{mpz}::text` is
    equivalent to :samp:`text({mpz})`.

    *base* may vary from 2 to 62 or from −2 to −36.  For base in the range
    2..36, digits and lower-case letters are used; for −2..−36, digits and
    upper-case letters are used; for 37..62, digits, upper-case letters, and
    lower-case letters (in that significance order) are used. If *base* is not
    specified, 10 is assumed.

.. todo::
    Not sure if handling 0 correctly here too, or if it should handled at all

.. todo:: write a wrap() function


`!mpz` operators
----------------

These operators can either work on `!mpz` arguments or take an integer
argument that will be implicitly converted.  Operators taking a :math:`2^n`
argument always use an integer as right argument.

A few operators may use optimized algorithms when one of the arguments is an
integer.

.. table:: Arithmetic operators

    =========== =============================== =================== ===========
    Operator    Description                     Example             Return
    =========== =============================== =================== ===========
    `!-`        Unary minus                     \- 5::mpz           -5
    `!+`        Unary plus                      \+ 5::mpz           5
    `!+`        Addition                        2::mpz + 3::mpz     5
    `!-`        Subtraction                     2::mpz - 3::mpz     -1
    `!*`        Multiplication                  7::mpz * 3::mpz     21
    `!<<`       Multiplication by :math:`2^n`   3::mpz << 2         12
    =========== =============================== =================== ===========


.. table:: Division operators

    =========== =============================== =================== =======
    Operator    Description                     Example             Return
    =========== =============================== =================== =======
    `!/`        Division quotient               7::mpz / 3::mpz     2

                Rounding towards zero           -7::mpz / 3::mpz    -2

    `!%`        Division reminder               7::mpz % 3::mpz     1

                Rounding towards zero           -7::mpz % 3::mpz    -1

    `+/`        Division quotient               7::mpz +/ 3::mpz    3

                Rounding towards +infinity      -7::mpz +/ 3::mpz   -2

    `+%`        Division reminder               7::mpz +% 3::mpz    -2

                Rounding towards +infinity      -7::mpz +% 3::mpz   -1

    `!-/`       Division quotient               7::mpz -/ 3::mpz    2

                Rounding towards -infinity      -7::mpz -/ 3::mpz   -3

    `!-%`       Division reminder               7::mpz -% 3::mpz    1

                Rounding towards -infinity      -7::mpz -% 3::mpz   2

    `/!`        Exact division (1)              21::mpz /! 7::mpz   3 
    =========== =============================== =================== =======

Notes:

(1)
    The exact division operator (`!/!`) produce correct results only when it
    is known in advance that :math:`d` divides :math:`n`.  The operator is
    much faster than the other division operators, and is the best choice when
    exact division is known to occur, for example reducing a rational to
    lowest terms.


.. table:: Division operators for powers of 2

    ======== ==================================== ================= =======
    Operator Description                          Example           Return
    ======== ==================================== ================= =======
    `!>>`    Quotient of division by :math:`2^n`  1027::mpz >> 3    128

             Rounding towards zero                -1027::mpz >> 3   -128

    `!%>`    Remainder of division by :math:`2^n` 1027::mpz %>  3   3

             Rounding towards zero                -1027::mpz %>  3  -3

    `!+>>`   Quotient of division by :math:`2^n`  1027::mpz +>> 3   129

             Rounding towards +infinity           -1027::mpz +>> 3  -128

    `!+%>`   Remainder of division by :math:`2^n` 1027::mpz +%>  3  -5

             Rounding towards +infinity           -1027::mpz +%>  3 -3

    `!->>`   Quotient of division by :math:`2^n`  1027::mpz ->> 3   128

             Rounding towards -infinity           -1027::mpz ->> 3  -129

    `!-%>`   Remainder of division by :math:`2^n` 1027::mpz -%>  3  3

             Rounding towards -infinity           -1027::mpz -%>  3 5
    ======== ==================================== ================= =======

For all the division-related operators :math:`n \oslash d`, :math:`q` and
:math:`r` will satisfy :math:`n = q \cdot d + r`, and :math:`r` will satisfy
:math:`0 \le abs(r) \lt abs(d)`.

.. note::
    Only the truncating division and reminder (`!/` and `!%`) have the correct
    precedence respect to addition, subtraction and multiplication.
    See `the PostgreSQL precedence table`__ for further details.

    .. __: http://www.postgresql.org/docs/9.0/static/sql-syntax-lexical.html#SQL-PRECEDENCE-TABLE


.. todo:: integer fast path

.. todo::
    fast path on int64 for 64 bit backends? Maybe introduce a long data type?


`!mpz` functions
----------------

.. function:: abs(mpz)

    Return the absolute value of *mpz*.

