PostgreSQL Multiple Precision Arithmetic extension
==================================================

pgmp is a PostgreSQL extension module to add support for the arbitrary
precision data types offered by the GMP library into the database.

The extension adds the types `mpz` (arbitrary size integers) and `mpq`
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

The extension is compatible with PostgreSQL versions from 8.4 and
packaged as a SQL extension in 9.1. The package includes comprehensive
documentation and regression tests.

Please refer to the documentation for installation and usage, either
online or in the ``docs/`` directory.

- Homepage: https://dvarrazzo.github.io/pgmp/
- Project page: https://github.com/dvarrazzo/pgmp/
