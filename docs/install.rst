`!pgmp` extension installation
==============================

Prerequisites
-------------

`!pgmp` is currently compatible with:

- PostgreSQL from version 9.0
- GMP from version 4.3.2 (untested with previous versions)


Building the library
--------------------

The library must be built and installed to be used with a database cluster:
once it is built, SQL installation scripts can be used to install the data
types and functions in one or more databases.

In order to build the library your system must have the server development
files (on Debian systems usually packaged as ``postgresql-server-dev``) and
regular UNIX development tools, such as :program:`make`. The
:program:`pg_config` program should be available in the :envvar:`PATH`. If
more than one PostgreSQL version is available on the system, the library will
be built against the version of the first :program:`pg_config` found in the
path.

To build and install the library:

.. code-block:: console

    $ make
    $ sudo make install

You can test the installation with:

.. code-block:: console

    $ make installcheck

(adjust the :envvar:`REGRESS_OPTS` variable to select a test database).


Installing the extension
------------------------

With PostgreSQL versions before 9.1, an install script called ``pgmp.sql`` is
installed in the directory :samp:`{$sharedir}/pgmp`: just run the script into
a database to install the provided types and functions. An uninstall script is
also provided in the same directory to remove the installed objects.

With PostgreSQL 9.1 the library is packaged as an extension: once built and
installed in the cluster, use the command:

.. code-block:: sql

    =# CREATE EXTENSION pgmp;

to install it in a database. If your database was migrated from a previous
PostgreSQL version, you can convert the `!pgmp` objects into a packaged
extension using the command:

.. code-block:: sql

    =# CREATE EXTENSION pgmp FROM unpackaged;

In order to uninstall the extension you can use the ``DROP EXTENSION``
command. Please refer to `the documentation`__ for further informations about
PostgreSQL extensions management.

.. __: http://developer.postgresql.org/pgdocs/postgres/extend-extensions.html


