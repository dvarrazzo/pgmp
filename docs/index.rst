PostgreSQL Multiple Precision Arithmetic
========================================

pgmp_ is a PostgreSQL_ extension to add the GMP_ data types and functions
directly into the database.

.. _pgmp: https://dvarrazzo.github.io/pgmp/
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

Here you can find a more detailed `list of goals <goals.html>`__ for this
extension.

See the `performance <performance.html>`__ page for comparison tests between
GMP data types and PostgreSQL builtins.

`[Ab]using PostgreSQL to calculate pi <pi.html>`__, a pgmp demo showing how to
generate millions of digits of :math:`\pi` using PL/pgSQL and Python to run
concurrent backends.

The library is released under
`LGPL License <http://www.opensource.org/licenses/lgpl-3.0>`__.


References
----------

- Homepage: https://dvarrazzo.github.io/pgmp/
- Project page: https://github.com/dvarrazzo/pgmp/
- Download: http://pgxn.org/dist/pgmp/


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

