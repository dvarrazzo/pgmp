.. _goals:

Goals of the project
====================

If you wonder what is this project for, here is a loose set of goals and
achievements.

.. cssclass:: goals

Learning more about PostgreSQL internals
    The project has been an occasion to learn something more about PostgreSQL
    internals and more advanced topics: memory allocation in the server,
    definition of complete arithmetic data types and their operators. This
    goal has been completely fulfilled.

Providing and edge on other databases
    As far as I know no other database provides rational numbers nor the wide
    range of mathematical functions exposed by the GMP. Having them directly
    available into the database makes PostgreSQL a good choice in environments
    where these functions are of use, such as in mathematical and cryptography
    research.


Here instead is a list of *non-goals*:

.. cssclass:: goals

An extension to be used by everybody
    For almost every user the performance offered by the `!numeric` data type
    are perfectly acceptable and the extra performance provided by `!mpz` may
    not justify the use of an external library.

Replacing PostgreSQL builtin numerical types
    GMP data type are not constrained by the semantic specified by SQL, so
    they will hardly replace any standard data type. Furthermore the GMP (as
    well as the pgmp extension) is distributed with LGPL license, which is
    perfectly compatible with the PostgreSQL terms of distribution but likely
    to not be well accepted by the PostgreSQL developers who usually prefer
    BSD-style licenses.

