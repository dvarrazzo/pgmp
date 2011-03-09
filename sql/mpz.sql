--
--  Test mpz datatype
--

--
-- first, define the datatype.  Turn off echoing so that expected file
-- does not depend on contents of pgmp.sql.
--
SET client_min_messages = warning;
\set ECHO none
\i pgmp.sql
\t
\a
\set ECHO all
RESET client_min_messages;


--
-- mpz input and output functions
--

SELECT '0'::mpz;
SELECT '1'::mpz;
SELECT '-1'::mpz;
SELECT '10'::mpz;
SELECT '-10'::mpz;

SELECT '000001'::mpz;       -- padding zeros
SELECT '-000001'::mpz;

SELECT '4294967295'::mpz;   -- limbs boundaries
SELECT '4294967296'::mpz;
SELECT '-4294967296'::mpz;
SELECT '-4294967297'::mpz;
SELECT '18446744073709551614'::mpz;
SELECT '18446744073709551615'::mpz;
SELECT '18446744073709551616'::mpz;
SELECT '18446744073709551617'::mpz;
SELECT '-18446744073709551615'::mpz;
SELECT '-18446744073709551616'::mpz;
SELECT '-18446744073709551617'::mpz;
SELECT '-18446744073709551618'::mpz;

SELECT '12345678901234567890123456789012345678901234567890123456789012345678901234567890'::mpz;

-- other bases
SELECT '0x10'::mpz, '010'::mpz, '0b10'::mpz;
SELECT mpz('10'), mpz('10', 16), mpz('10', 2), mpz('10', 62);
SELECT mpz('10', 1);
SELECT mpz('10', 63);

SELECT text(10::mpz);
SELECT text(255::mpz, 16);
SELECT text((36 * 36 - 1)::mpz, 36);
SELECT text((62 * 62 - 1)::mpz, 62);
SELECT text((36 * 36 - 1)::mpz, -36);
SELECT text(10::mpz, -37);
SELECT text(10::mpz, 63);

-- limited error
SELECT ('xx' || repeat('1234567890', 10))::mpz;
SELECT mpz('xx' || repeat('1234567890', 10), 42);
--
-- mpz cast
--

SELECT 0::smallint::mpz, (-32768)::smallint::mpz, 32767::smallint::mpz;
SELECT 0::integer::mpz, (-2147483648)::integer::mpz, 2147483647::integer::mpz;
SELECT 0::bigint::mpz, (-9223372036854775808)::bigint::mpz, 9223372036854775807::bigint::mpz;
SELECT 0::numeric::mpz, (-12345678901234567890)::numeric::mpz, 12345678901234567890::numeric::mpz;

SELECT 0::mpz, 1::mpz, (-1)::mpz;       -- automatic casts
SELECT 1000000::mpz, (-1000000)::mpz;
SELECT 1000000000::mpz, (-1000000000)::mpz;
SELECT 1000000000000000::mpz, (-1000000000000000)::mpz;
SELECT 1000000000000000000000000000000::mpz, (-1000000000000000000000000000000)::mpz;

SELECT -1::mpz;       -- these take the unary minus to work
SELECT -1000000::mpz;
SELECT -1000000000::mpz;
SELECT -1000000000000000::mpz;
SELECT -1000000000000000000000000000000::mpz;

SELECT 2147483647::mpz::integer;
SELECT -2147483647::mpz::integer;
SELECT 2147483648::mpz::integer;
SELECT -2147483648::mpz::integer;
SELECT 32767::mpz::smallint;
SELECT -32767::mpz::smallint;
SELECT 32768::mpz::smallint;
SELECT -32768::mpz::smallint;
SELECT 9223372036854775807::mpz::bigint;
SELECT -9223372036854775807::mpz::bigint;
SELECT 9223372036854775808::mpz::bigint;
SELECT -9223372036854775808::mpz::bigint;
SELECT 2147483648::mpz::bigint;
SELECT -2147483648::mpz::bigint;
SELECT (65536::mpz)::bigint;
SELECT (65536::mpz*65536::mpz)::bigint;
SELECT (65536::mpz*65536::mpz*65536::mpz)::bigint;
SELECT (65536::mpz*65536::mpz*65536::mpz*65536::mpz/2::mpz-1::mpz)::bigint;
SELECT (65536::mpz*65536::mpz*65536::mpz*65536::mpz/2::mpz)::bigint;
SELECT (-65536::mpz)::bigint;
SELECT (-65536::mpz*65536::mpz)::bigint;
SELECT (-65536::mpz*65536::mpz*65536::mpz)::bigint;
SELECT (-65536::mpz*65536::mpz*65536::mpz*65536::mpz/2::mpz+1::mpz)::bigint;
SELECT (-65536::mpz*65536::mpz*65536::mpz*65536::mpz/2::mpz)::bigint;


--
-- mpz arithmetic
--

SELECT -('0'::mpz), +('0'::mpz), -('1'::mpz), +('1'::mpz);
SELECT -('12345678901234567890'::mpz), +('12345678901234567890'::mpz);

SELECT '1'::mpz + '2'::mpz;
SELECT '2'::mpz + '-4'::mpz;
SELECT regexp_matches((
        ('1' || repeat('0', 1000))::mpz +
        ('2' || repeat('0', 1000))::mpz)::text,
    '^3(0000000000){100}$') IS NOT NULL;

SELECT '3'::mpz - '2'::mpz;
SELECT '3'::mpz - '5'::mpz;
SELECT regexp_matches((
        ('5' || repeat('0', 1000))::mpz -
        ('2' || repeat('0', 1000))::mpz)::text,
    '^3(0000000000){100}$') IS NOT NULL;

SELECT '3'::mpz * '2'::mpz;
SELECT '3'::mpz * '-5'::mpz;
SELECT regexp_matches((
        ('2' || repeat('0', 1000))::mpz *
        ('3' || repeat('0', 1000))::mpz)::text,
    '^6(00000000000000000000){100}$') IS NOT NULL;

-- PostgreSQL should apply the conventional precedence to operators
-- with the same name of the builtin operators.
SELECT '2'::mpz + '6'::mpz * '7'::mpz;  -- cit.


SELECT  '7'::mpz /  '3'::mpz;
SELECT '-7'::mpz /  '3'::mpz;
SELECT  '7'::mpz / '-3'::mpz;
SELECT '-7'::mpz / '-3'::mpz;
SELECT  '7'::mpz %  '3'::mpz;
SELECT '-7'::mpz %  '3'::mpz;
SELECT  '7'::mpz % '-3'::mpz;
SELECT '-7'::mpz % '-3'::mpz;

SELECT  '7'::mpz +/  '3'::mpz;
SELECT '-7'::mpz +/  '3'::mpz;
SELECT  '7'::mpz +/ '-3'::mpz;
SELECT '-7'::mpz +/ '-3'::mpz;
SELECT  '7'::mpz +%  '3'::mpz;
SELECT '-7'::mpz +%  '3'::mpz;
SELECT  '7'::mpz +% '-3'::mpz;
SELECT '-7'::mpz +% '-3'::mpz;

SELECT  '7'::mpz -/  '3'::mpz;
SELECT '-7'::mpz -/  '3'::mpz;
SELECT  '7'::mpz -/ '-3'::mpz;
SELECT '-7'::mpz -/ '-3'::mpz;
SELECT  '7'::mpz -%  '3'::mpz;
SELECT '-7'::mpz -%  '3'::mpz;
SELECT  '7'::mpz -% '-3'::mpz;
SELECT '-7'::mpz -% '-3'::mpz;

SELECT  '7'::mpz /  '0'::mpz;
SELECT  '7'::mpz %  '0'::mpz;
SELECT  '7'::mpz +/  '0'::mpz;
SELECT  '7'::mpz +%  '0'::mpz;
SELECT  '7'::mpz -/  '0'::mpz;
SELECT  '7'::mpz -%  '0'::mpz;

SELECT  '10000000000'::mpz << 10;
SELECT  '10000000000'::mpz << 0;
SELECT  '10000000000'::mpz << -1;

SELECT  '1027'::mpz >>   3;
SELECT '-1027'::mpz >>   3;
SELECT  '1027'::mpz >>  -3;
SELECT  '1027'::mpz %>>  3;
SELECT '-1027'::mpz %>>  3;
SELECT  '1027'::mpz %>> -3;

SELECT  '1027'::mpz +>>   3;
SELECT '-1027'::mpz +>>   3;
SELECT  '1027'::mpz +>>  -3;
SELECT  '1027'::mpz +%>>  3;
SELECT '-1027'::mpz +%>>  3;
SELECT  '1027'::mpz +%>> -3;

SELECT  '1027'::mpz ->>   3;
SELECT '-1027'::mpz ->>   3;
SELECT  '1027'::mpz ->>  -3;
SELECT  '1027'::mpz -%>>  3;
SELECT '-1027'::mpz -%>>  3;
SELECT  '1027'::mpz -%>> -3;


--
-- mpz ordering operators
--

select 1000::mpz =   999::mpz;
select 1000::mpz =  1000::mpz;
select 1000::mpz =  1001::mpz;
select 1000::mpz <>  999::mpz;
select 1000::mpz <> 1000::mpz;
select 1000::mpz <> 1001::mpz;
select 1000::mpz !=  999::mpz;
select 1000::mpz != 1000::mpz;
select 1000::mpz != 1001::mpz;
select 1000::mpz <   999::mpz;
select 1000::mpz <  1000::mpz;
select 1000::mpz <  1001::mpz;
select 1000::mpz <=  999::mpz;
select 1000::mpz <= 1000::mpz;
select 1000::mpz <= 1001::mpz;
select 1000::mpz >   999::mpz;
select 1000::mpz >  1000::mpz;
select 1000::mpz >  1001::mpz;
select 1000::mpz >=  999::mpz;
select 1000::mpz >= 1000::mpz;
select 1000::mpz >= 1001::mpz;

select mpz_cmp(1000::mpz,  999::mpz);
select mpz_cmp(1000::mpz, 1000::mpz);
select mpz_cmp(1000::mpz, 1001::mpz);

-- Can create a btree index
create table test_mpz_idx (z mpz);
create index test_mpz_idx_idx on test_mpz_idx (z);


--
-- mpz aggregation
--

CREATE TABLE mpzagg(z mpz);

SELECT sum(z) FROM mpzagg;      -- NULL sum

INSERT INTO mpzagg SELECT generate_series(1, 100);
INSERT INTO mpzagg VALUES (NULL);

SELECT sum(z) FROM mpzagg;
SELECT prod(z) FROM mpzagg;


--
-- test functions
--

SELECT mpz_test_dataset(5, 20);
SELECT numeric_test_dataset(5, '123456'::numeric);
