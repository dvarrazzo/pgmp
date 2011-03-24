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


.. function:: text(z)

.. function:: text(z, base)

    Convert the `!mpz` *z* into a string. The form :samp:`{z}::text` is
    equivalent to :samp:`text({z})`.

    *base* may vary from 2 to 62 or from −2 to −36.  For base in the range
    2..36, digits and lower-case letters are used; for −2..−36, digits and
    upper-case letters are used; for 37..62, digits, upper-case letters, and
    lower-case letters (in that significance order) are used. If *base* is not
    specified, 10 is assumed.

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
    but they may actually fail if the server is 32 bit and the argument
    doesn't fit into an `!int4`.

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


.. function:: sgn(z)
    
    Return +1 if *z* > 0, 0 if *z* = 0, and -1 if *z* < 0. 


.. function:: odd(z)
    
.. function:: even(z)

    Return `!true` if *z* is odd or even, respectively, else `!false`.


Division Operators and Functions
--------------------------------

For all the division-related operators :math:`n \oslash d`, :math:`q` and
:math:`r` will satisfy :math:`n = q \cdot d + r`, and :math:`r` will satisfy
:math:`0 \le |r| \lt |d|`.

.. note::
    Only the truncating division and reminder (`!/` and `!%`) have the correct
    precedence respect to addition, subtraction and multiplication.
    See `the PostgreSQL precedence table`__ for further details.

    .. __: http://www.postgresql.org/docs/9.0/static/sql-syntax-lexical.html#SQL-PRECEDENCE-TABLE


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

    `/?`        Divisible (1)                   `!21::mpz /? 7::mpz` `!true`

    `/!`        Exact division (2)              `!21::mpz /! 7::mpz` 3
    =========== =============================== ==================== =======

Notes:

(1)
    See also the function `divisible()`.

(2)
    The exact division operator (`!/!`) produces correct results only when it
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

    `>>?`    Divisible by :math:`2^n` (1)         `!64::mpz >>? 3`    `!true`
    ======== ==================================== =================== =======

(1)
    See also the function `divisible_2exp()`.


.. function:: tdiv_qr(n, d)

    Return a tuple containing quotient *q* and remainder *r* of the division,
    rounding towards 0.


.. function:: cdiv_qr(n, d)

    Return a tuple containing quotient *q* and remainder *r* of the division,
    rounding towards +infinity (ceil).


.. function:: fdiv_qr(n, d)

    Return a tuple containing quotient *q* and remainder *r* of the division,
    rounding towards -infinity (floor).



.. function:: divisible(n, d)

.. function:: divisible_2exp(n, b)

    Return `!true` if *n* is exactly divisible by *d*, or in the case of
    `!divisible_2exp()` by :math:`2^b`.

    :math:`n` is divisible by :math:`d` if there exists an integer :math:`q`
    satisfying :math:`n = q \cdot d`.  Unlike the other division operators,
    *d*\=0 is accepted and following the rule it can be seen that only 0
    is considered divisible by 0.

    The operators `!/?` and `!>>?` are aliases for `!divisible()` and
    `!divisible_2exp()`.


.. function:: congruent(n, c, d)

.. function:: congruent_2exp(n, c, b)

    Return `!true` if *n* is congruent to *c* modulo *d*, or in the case of
    `!congruent_2exp()` modulo :math:`2^b`.

    :math:`n` is congruent to :math:`c \mod d` if there exists an integer
    :math:`q` satisfying :math:`n = c + q \cdot d`. Unlike the other division
    operators, *d*\=0 is accepted and following the rule it can be seen that n
    and c are considered congruent mod 0 only when exactly equal.


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


Number Theoretic Functions
--------------------------

.. function:: probab_prime(n, reps)

    Determine whether *n* is prime. Return 2 if *n* is definitely prime,
    return 1 if *n* is probably prime (without being certain), or return 0 if
    *n* is definitely composite.

    This function does some trial divisions, then some `Miller-Rabin
    probabilistic primality tests`__. *reps* controls how many such tests are
    done, 5 to 10 is a reasonable number, more will reduce the chances of a
    composite being returned as “probably prime”.

    .. __: http://en.wikipedia.org/wiki/Miller%E2%80%93Rabin_primality_test

    Miller-Rabin and similar tests can be more properly called compositeness
    tests. Numbers which fail are known to be composite but those which pass
    might be prime or might be composite. Only a few composites pass, hence
    those which pass are considered probably prime.

    .. seealso:: `Primality test
        <http://en.wikipedia.org/wiki/Primality_test>`__


.. function:: nextprime(op)

    Return the next prime greater than *op*.

    This function uses a probabilistic algorithm to identify primes. For
    practical purposes it's adequate, the chance of a composite passing will
    be extremely small.


.. function:: gcd(a, b)

    Return the greatest common divisor of *a* and *b*. The result is
    always positive even if one or both input operands are negative.


.. function:: gcdext(a, b)

    Return the greatest common divisor *g* of *a* and *b*, and in addition
    coefficients *s* and *t* satisfying :math:`a \cdot s + b \cdot t = g`. The
    value *g* is always positive, even if one or both of *a* and *b* are
    negative. The values *s* and *t* are chosen such that :math:`|s| \le |b|
    \hspace{0em}` and :math:`|t| \le |a| \hspace{0em}`.

    ..
        The \hspace{} are there to avoid the vim rest syntax highlighter to
        get crazy.

    .. code-block:: sql

        =# select * from  gcdext(6, 15);
         g | s  | t
        ---+----+---
         3 | -2 | 1


.. function:: lcm(a, b)

    Return the least common multiple of *a* and *b*. The value returned is
    always positive, irrespective of the signs of *a* and *b*. The return
    will be zero if either *a* or *b* is zero.


.. function:: fac(op)

    Return *op*\!, the factorial of *op*.


.. function:: invert(a, b)

    Return the inverse of *a* modulo *b* if exists. The return value *r*
    will satisfy :math:`0 \le r \lt b`. If an inverse doesn't exist return
    `!NULL`.


.. function:: jacobi(a, b)

    Calculate the `Jacobi symbol`__ :math:`(\frac{a}{b})`. This is defined
    only for *b* odd.

    .. __: http://en.wikipedia.org/wiki/Jacobi_symbol


.. function:: legendre(a, p)

    Calculate the `Legendre symbol`__ :math:`(\frac{a}{p})`.  This is defined
    only for *p* an odd positive prime, and for such *p* it's identical to the
    Jacobi symbol.

    .. __: http://en.wikipedia.org/wiki/Legendre_symbol


.. function:: kronecker(a, b)

    Calculate the Jacobi symbol :math:`(\frac{a}{b})` with the Kronecker
    extension :math:`(\frac{a}{2})=(\frac{2}{a})` when *a* odd, or
    :math:`(\frac{a}{2})=0` when *a* even.

    .. seealso::
        Section 1.4.2, Henri Cohen, "A Course in Computational Algebraic
        Number Theory", Graduate Texts in Mathematics number 138,
        Springer-Verlag, 1993. http://www.math.u-bordeaux.fr/~cohen/


Logical and Bit Manipulation Functions
--------------------------------------

These functions behave as if twos complement arithmetic were used (although
sign-magnitude is the actual implementation). The least significant bit is
number 0.

.. table:: Logical Operators

    ======== ======================== =================================== ===================
    Operator Description              Example                             Return
    ======== ======================== =================================== ===================
    `!&`     Bitwise and              `!'0b10001'::mpz & '0b01001'::mpz`  `!'0b1'::mpz`
    `!|`     Bitwise inclusive-or     `!'0b10001'::mpz | '0b01001'::mpz`  `!'0b11001'::mpz`
    `!#`     Bitwise exclusive-or     `!'0b10001'::mpz # '0b01001'::mpz`  `!'0b11000'::mpz`
    ======== ======================== =================================== ===================


.. function:: com(op)

    Return the ones' complement of *op*.


.. function:: popcount(op)

    If op>=0, return the population count of *op*, which is the number of 1
    bits in the binary representation. If op<0, the number of 1s is infinite,
    and the return value is the largest possible, represented by
    `gmp_max_bitcnt()`.


.. function:: hamdist(op1, op2)

    If *op1* and *op2* are both >=0 or both <0, return the `Hamming
    distance`__
    between the two operands, which is the number of bit positions where *op1*
    and *op2* have different bit values. If one operand is >=0 and the other <0
    then the number of bits different is infinite, and the return value is the
    largest possible, represented by `gmp_max_bitcnt()`.

    .. __: http://en.wikipedia.org/wiki/Hamming_distance


.. function:: scan0(op, starting_bit)

.. function:: scan1(op, starting_bit)

    Scan *op*, starting from bit *starting_bit*, towards more significant
    bits, until the first 0 or 1 bit (respectively) is found. Return the index
    of the found bit.

    If the bit at *starting_bit* is already what's sought, then *starting_bit*
    is returned.

    If there's no bit found, then the largest possible bit count is returned
    (represented by `gmp_max_bitcnt()`). This will happen in `!mpz_scan0()`
    past the end of a negative number, or `!mpz_scan1()` past the end of a
    nonnegative number.


.. function:: setbit(op, bit_index)

    Return *op* with bit *bit_index* set.

.. function:: clrbit(op, bit_index)

    Return *op* with bit *bit_index* cleared.

.. function:: combit(op, bit_index)

    Return *op* with bit *bit_index* complemented.

.. function:: tstbit(op, bit_index)

    Test bit *bit_index* in *op* and return 0 or 1 accordingly.


.. function:: gmp_max_bitcnt()

    Return the maximum value possible for the bit count as accepted or
    returned by functions dealing with bits.

    The value is the maximum `!unsigned long` defined on the server, so it can
    be :math:`2^{32}-1` or :math:`2^{64}-1` according to the server platform.

