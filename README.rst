PostgreSQL Multiple Precision Arithmetic extension
==================================================

|travis|

.. |travis| image:: https://travis-ci.org/dvarrazzo/pgmp.svg?branch=master
    :target: https://travis-ci.org/dvarrazzo/pgmp
    :alt: Build status

pgmp is a PostgreSQL extension module to add support for the arbitrary
precision data types offered by the GMP library into the database.

The extension adds the types mpz_ (arbitrary size integers) and mpq_
(arbitrary precision rationals) to PostgreSQL and exposes to the
database all the functions available in the GMP library for these types,
providing:

- higher performance arithmetic on integers respect to the ``decimal``
  data type, using numbers only limited by the 1GB varlena maximum size;

- a rational data type for absolute precision storage and arithmetic;

- the use of specialized functions to deal with prime numbers, random
  numbers, factorization directly into the database.

The GMP data types can be stored into the database, used in mixed
arithmetic with other PostgreSQL numeric types and indexed using the
btree or hash methods.

Please refer to the documentation for installation and usage, either
online__ or in the ``docs/`` directory.

- Homepage: https://www.varrazzo.com/pgmp/
- Project page: https://github.com/dvarrazzo/pgmp/
- Download: https://pgxn.org/dist/pgmp/

.. _mpz: https://www.varrazzo.com/pgmp/mpz.html
.. _mpq: https://www.varrazzo.com/pgmp/mpq.html
.. __: https://www.varrazzo.com/pgmp/
