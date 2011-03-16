.. PostgreSQL Multiple Precision Arithmetic documentation master file, created by
   sphinx-quickstart on Thu Mar 10 01:09:43 2011.

PostgreSQL Multiple Precision Arithmetic
========================================

pgmp is a PostgreSQL_ extension to add the GMP_ data types and functions
directly into the database.

The extension is currently under active development and the file format is not
guaranteed to be stable yet. The target for the first release is to support
integers (`mpz`) and rational (`mpq`) data types.

.. _PostgreSQL: http://www.postgresql.org
.. _GMP: http://www.gmplib.org

How people could have possibly lived without million digits numbers
and rationals into their databases is something I really can't
understand...

- Homepage: http://pgmp.projects.postgresql.org/
- Source repository: https://github.com/dvarrazzo/pgmp/
- Discussion group: http://groups.google.com/group/postgresql-gmp
- PgFoundry project: http://pgfoundry.org/projects/pgmp/


Status
------

The extension is under active development and not stable yet: we can't
guarantee the disk format will not change.

Documentation
-------------

.. toctree::
   :maxdepth: 2

   mpz


.. todolist::


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

