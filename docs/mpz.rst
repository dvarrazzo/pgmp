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

