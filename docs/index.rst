.. PostgreSQL Multiple Precision Arithmetic documentation master file, created by
   sphinx-quickstart on Thu Mar 10 01:09:43 2011.

PostgreSQL Multiple Precision Arithmetic
========================================

This extension adds support in PostgreSQL_ for the data types and
functions avaliable in the GMP_ library for arbitrary precision
arithmetic.

.. _PostgreSQL: http://www.postgresql.org
.. _GMP: http://www.gmplib.org

The GMP structures are available as PostgreSQL data types and all the
functions exposed in the library are available from SQL.

How people could have possibly lived without general precision floating point
and rational numbers into their databases is something I really can't
understand...


Status
------

The extension is under active development and not stable yet: we can't
guarantee the disk format will not change.


Contents
--------

.. toctree::
   :maxdepth: 2

   mpz


.. todolist::


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

