PostgreSQL Multiple Precision Arithmetic
========================================

`!pgmp` is a PostgreSQL_ extension to add the GMP_ data types and functions
directly into the database.

.. _PostgreSQL: http://www.postgresql.org
.. _GMP: http://www.gmplib.org

The extension adds the types `mpz` (arbitrary size integers) and `mpq`
(arbitrary precision rationals) to PostgreSQL and exposes to the database all
the functions available in the GMP library for these data types, allowing:

- higher performance arithmetic on integers respect to the `!numeric` data
  type

- a rational data type for absolute precision storage and arithmetic

- using specialized functions to deal with prime numbers, random numbers,
  factorization directly into the database.


References
----------

- Homepage: http://pgmp.projects.postgresql.org/
- Discussion group: http://groups.google.com/group/postgresql-gmp
- Source repository: https://github.com/dvarrazzo/pgmp/
- Bug tracking: https://github.com/dvarrazzo/pgmp/issues
- PgFoundry project: http://pgfoundry.org/projects/pgmp/


Documentation
-------------

.. toctree::
   :maxdepth: 2

   install
   mpz
   mpq
   misc


Indices and tables
------------------

  * :ref:`genindex`
  * :ref:`search`

..
  * :ref:`modindex`

..
    To Do List
    ----------

    .. todolist::

