-- Some functions to calculate pi digits using mpz integers.
--
-- Reference:
-- https://web.archive.org/web/20111211140154/http://en.literateprograms.org/Pi_with_Machin's_formula_(Python)
--
-- Copyright (C) 2011-2020 Daniele Varrazzo

CREATE FUNCTION arccot(x mpz, unity mpz) RETURNS mpz
    LANGUAGE plpgsql IMMUTABLE STRICT
    AS $$
DECLARE
    xp mpz := unity / x;
    xp2 mpz := -(x ^ 2);
    acc mpz := xp;
    term mpz;
    n mpz := 3;
BEGIN
    LOOP
        xp := xp / xp2;
        term := xp / n;
        EXIT WHEN term = 0;
        acc := acc + term;
        n := n + 2;
    END LOOP;
    RETURN acc;
END
$$;


CREATE FUNCTION pi_machin(ndigits integer) RETURNS mpz
    LANGUAGE plpgsql IMMUTABLE STRICT
    AS $$
DECLARE
    unity mpz = 10::mpz ^ (ndigits + 10);
BEGIN
    RETURN 4 * (
        4 * arccot(5, unity)
        - arccot(239, unity)
        ) / (10::mpz ^ 10);
END
$$;


CREATE FUNCTION pi_hwang_97(ndigits integer) RETURNS mpz
    LANGUAGE plpgsql IMMUTABLE STRICT
    AS $$
DECLARE
    unity mpz = 10::mpz ^ (ndigits + 10);
BEGIN
    RETURN 4 * (
        183 * arccot(239, unity)
        + 32 * arccot(1023, unity)
        - 68 * arccot(5832, unity)
        + 12 * arccot(110443, unity)
        - 12 * arccot(4841182, unity)
        - 100 * arccot(6826318, unity)
        ) / (10::mpz ^ 10);
END
$$;


CREATE FUNCTION pi_hwang_03(ndigits integer) RETURNS mpz
    LANGUAGE plpgsql IMMUTABLE STRICT
    AS $$
DECLARE
    unity mpz = 10::mpz ^ (ndigits + 10);
BEGIN
    RETURN 4 * (
        183 * arccot(239, unity)
        + 32 * arccot(1023, unity)
        - 68 * arccot(5832, unity)
        + 12 * arccot(113021, unity)
        - 100 * arccot(6826318, unity)
        - 12 * arccot(33366019650, unity)
        + 12 * arccot(43599522992503626068::mpz, unity)
        ) / (10::mpz ^ 10);
END
$$;



