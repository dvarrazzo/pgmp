Miscellaneous functions
=======================

.. function:: gmp_version()

    Return the version of the GMP library currently used as an integer.
    
    For example GMP 4.3.2 is reported as 40302.


.. function:: gmp_max_bitcnt()

    Return the maximum value possible for the bit count as accepted or
    returned by functions dealing with bits.

    The value is the maximum `!unsigned long` defined on the server, so it can
    be :math:`2^{32}-1` or :math:`2^{64}-1` according to the server platform.


