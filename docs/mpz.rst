`mpz` data type
===============

The `mpz` data type can store integer numbers of arbitrary size, up to the
maximum memory allowed by PostgreSQL data types (about 1GB).

Every PostgreSQL integer data type can be converted to `mpz`::

    SELECT 10000::mpz;

`mpz` can also be initialized from a string: the number can have a prefix
``0x`` or ``0X`` for hexadecimal numbers, ``0b`` or ``0B`` for binary, ``0``
for octal; with no prefix the base will be decimal. ::

    =# select '0x10'::mpz as hex, '10'::mpz as dec , '010'::mpz as oct, '0b10'::mpz as bin;
     hex | dec | oct | bin 
    -----+-----+-----+-----
     16  | 10  | 8   | 2

.. todo:: I should write a psql pygmets lexer

A specific base can be specified in input using :samp:`mpz({str}, {base})` and
in output using :samp:`text({mpz}, {base})`.

In input, *base* may vary from 2 to 62, or if base is 0, then the leading
characters are used to establish the base.  For bases up to 36, case is
ignored; upper-case and lower-case letters have the same value. For bases 37
to 62, upper-case letter represent the usual 10..35 while lower-case letter
represent 36..61.

.. todo:: Handle base 0 in mpz(str, int)

In output, *base* argument may vary from 2 to 62 or from −2 to −36.  For base
in the range 2..36, digits and lower-case letters are used; for −2..−36,
digits and upper-case letters are used; for 37..62, digits, upper-case
letters, and lower-case letters (in that significance order) are used.

.. todo::
    Not sure if handling 0 correctly here too, or if it should handled at all


`mpz` operators
---------------

`mpz` operators can work either on `mpz` arguments or they can take
integer arguments that will be implicitly comverted.  A few operators may
use optimized algorithms when one of the arguments is an integer.

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

Little to explain here I guess.


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
    =========== =============================== =================== =======

.. table:: Division operators for powers of 2

    ======== ==================================== ================= =======
    Operator Description                          Example           Return
    ======== ==================================== ================= =======
    `!>>`    Quotient of division by :math:`2^n`  1027::mpz >> 3    128

             Rounding towards zero                1027::mpz >> 3    -128

    `!%>>`   Remainder of division by :math:`2^n` 1027::mpz %>> 3   3

             Rounding towards zero                1027::mpz %>> 3   -3

    `!+>>`   Quotient of division by :math:`2^n`  1027::mpz +>> 3   129

             Rounding towards +infinity           1027::mpz +>> 3   -128

    `!+%>>`  Remainder of division by :math:`2^n` 1027::mpz +%>> 3  -5

             Rounding towards +infinity           1027::mpz +%>> 3  -3

    `!->>`   Quotient of division by :math:`2^n`  1027::mpz ->> 3   128

             Rounding towards -infinity           1027::mpz ->> 3   -129

    `!-%>>`  Remainder of division by :math:`2^n` 1027::mpz -%>> 3  3

             Rounding towards -infinity           1027::mpz -%>> 3  5
    ======== ==================================== ================= =======

In all cases :math:`q` and :math:`r` will satisfy :math:`n = q * d + r`, and
:math:`r` will satisfy :math:`0 <= abs(r) < abs(d)`.

.. note::
    Only the truncating division and reminder (`!/` and `!%`) have the correct
    precedence respect to addition, subtraction and multiplication.
    See `the PostgreSQL precedence table`__ for further details.

    .. __: http://www.postgresql.org/docs/9.0/static/sql-syntax-lexical.html#SQL-PRECEDENCE-TABLE


.. todo:: integer fast path

.. todo::
    fast path on int64 for 64 bit backends? Maybe introduce a long data type?

