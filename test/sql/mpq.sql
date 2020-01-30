--
--  Test mpq datatype
--

-- Compact output
\t
\a


--
-- mpq input and output functions
--

SELECT '0'::mpq;
SELECT '1'::mpq;
SELECT '-1'::mpq;
SELECT '10'::mpq;
SELECT '-10'::mpq;

SELECT '4294967295'::mpq;   -- limbs boundaries
SELECT '4294967296'::mpq;
SELECT '-4294967296'::mpq;
SELECT '-4294967297'::mpq;
SELECT '18446744073709551614'::mpq;
SELECT '18446744073709551615'::mpq;
SELECT '18446744073709551616'::mpq;
SELECT '18446744073709551617'::mpq;
SELECT '-18446744073709551615'::mpq;
SELECT '-18446744073709551616'::mpq;
SELECT '-18446744073709551617'::mpq;
SELECT '-18446744073709551618'::mpq;
SELECT '12345678901234567890123456789012345678901234567890123456789012345678901234567890'::mpq;
SELECT '-12345678901234567890123456789012345678901234567890123456789012345678901234567890'::mpq;

SELECT '1/4294967295'::mpq;   -- limbs boundaries on denom
SELECT '1/4294967296'::mpq;
SELECT '-1/4294967296'::mpq;
SELECT '-1/4294967297'::mpq;
SELECT '1/18446744073709551614'::mpq;
SELECT '1/18446744073709551615'::mpq;
SELECT '1/18446744073709551616'::mpq;
SELECT '1/18446744073709551617'::mpq;
SELECT '-1/18446744073709551615'::mpq;
SELECT '-1/18446744073709551616'::mpq;
SELECT '-1/18446744073709551617'::mpq;
SELECT '-1/18446744073709551618'::mpq;
SELECT '1/12345678901234567890123456789012345678901234567890123456789012345678901234567890'::mpq;
SELECT '-1/12345678901234567890123456789012345678901234567890123456789012345678901234567890'::mpq;

SELECT '1/1'::mpq;
SELECT '2/3'::mpq;
SELECT '640/30'::mpq;
SELECT '-640/30'::mpq;

SELECT '18446744073709551616/18446744073709551616'::mpq;
SELECT '12345678901234567890123456789012345678901234567890123456789012345678901234567890/'
       '88888888888888888888888888888888888888888888888888888888888888888888888888888888'::mpq;

SELECT '1/0'::mpq;

SELECT mpq('1/1');
SELECT mpq('2/3');
SELECT mpq('640/30');
SELECT mpq('-640/30');
SELECT mpq('0xEF/100');
SELECT mpq('0xEF/0x100');

SELECT mpq('10/30', 10);
SELECT mpq('EF/100', 16);
SELECT mpq('0xEF/100', 0);

SELECT mpq('z', 36), mpq('Z', 36);
SELECT mpq('z', 62), mpq('Z', 62);
SELECT mpq('1', 1);
SELECT mpq('1', -10);
SELECT mpq('1', 63);

SELECT text('239'::mpq);
SELECT text('-239'::mpq);
SELECT text('239/256'::mpq);
SELECT text('239'::mpq, 16);
SELECT text('239/256'::mpq, 10);
SELECT text('239/256'::mpq, 16);
SELECT text('239/256'::mpq, 0);
SELECT text('239/256'::mpq, 1);
SELECT text('239/256'::mpq, 2);
SELECT text('239/256'::mpq, 36);
SELECT text('239/256'::mpq, 62);
SELECT text('239/256'::mpq, 63);
SELECT text('239/256'::mpq, -1);
SELECT text('239/256'::mpq, -2);
SELECT text('239/256'::mpq, -36);
SELECT text('239/256'::mpq, -37);


--
-- mpq cast
--

SELECT 0::smallint::mpq, (-32768)::smallint::mpq, 32767::smallint::mpq;
SELECT 0::integer::mpq, (-2147483648)::integer::mpq, 2147483647::integer::mpq;
SELECT 0::bigint::mpq, (-9223372036854775808)::bigint::mpq, 9223372036854775807::bigint::mpq;
SELECT 0::numeric::mpq, (-12345678901234567890)::numeric::mpq, 12345678901234567890::numeric::mpq;
SELECT 0::mpz::mpq, (-12345678901234567890)::mpz::mpq, 12345678901234567890::mpz::mpq;
SELECT 0.0::float4::mpq, (-12345.25)::float4::mpq, 12345.25::float4::mpq;
SELECT 0.0::float8::mpq, (-123456789012.25)::float8::mpq, 123456789012.25::float8::mpq;
SELECT 0.1::float4::mpq;    -- don't know if it's portable
SELECT 0.1::float8::mpq;
SELECT 0.0::numeric::mpq, (-1234567890.12345)::numeric::mpq, 1234567890.12345::numeric::mpq;

SELECT 0::mpq, 1::mpq, (-1)::mpq;       -- automatic casts
SELECT 1000000::mpq, (-1000000)::mpq;
SELECT 1000000000::mpq, (-1000000000)::mpq;
SELECT 1000000000000000::mpq, (-1000000000000000)::mpq;
SELECT 1000000000000000000000000000000::mpq, (-1000000000000000000000000000000)::mpq;
SELECT 0.0::mpq, (-1234567890.12345)::mpq, 1234567890.12345::mpq;
SELECT 'NaN'::decimal::mpq;

SELECT -1::mpq;       -- these take the unary minus to work
SELECT -1000000::mpq;
SELECT -1000000000::mpq;
SELECT -1000000000000000::mpq;
SELECT -1000000000000000000000000000000::mpq;

SELECT 123.10::mpq::mpz, (-123.10)::mpq::mpz;
SELECT 123.90::mpq::mpz, (-123.90)::mpq::mpz;
SELECT 123.10::mpq::int2, (-123.10)::mpq::int2;
SELECT 123.10::mpq::int4, (-123.10)::mpq::int4;
SELECT 123.10::mpq::int8, (-123.10)::mpq::int8;
SELECT 32767::mpq::int2;
SELECT 32768::mpq::int2;
SELECT (-32768)::mpq::int2;
SELECT (-32769)::mpq::int2;
SELECT 2147483647::mpq::int4;
SELECT 2147483648::mpq::int4;
SELECT (-2147483648)::mpq::int4;
SELECT (-2147483649)::mpq::int4;
SELECT 9223372036854775807::mpq::int8;
SELECT 9223372036854775808::mpq::int8;
SELECT (-9223372036854775808)::mpq::int8;
SELECT (-9223372036854775809)::mpq::int8;
SELECT 123.10::mpq::float4, (-123.10)::mpq::float4;
SELECT 123.10::mpq::float8, (-123.10)::mpq::float8;
SELECT pow(10::mpz,400)::mpq::float4;       -- +inf
SELECT (-pow(10::mpz,400))::mpq::float4;    -- -inf
SELECT mpq(1,pow(10::mpz,400))::float4;     -- underflow
SELECT pow(10::mpz,400)::mpq::float8;
SELECT (-pow(10::mpz,400))::mpq::float8;
SELECT mpq(1,pow(10::mpz,400))::float8;
SELECT 1::mpq::numeric;
SELECT 123.456::mpq::numeric;
SELECT 123.456::mpq::numeric(10);
SELECT 123.456::mpq::numeric(10,2);
SELECT mpq(4,3)::numeric;
SELECT mpq(4,3)::numeric(10);
SELECT mpq(4,3)::numeric(10,5);
SELECT mpq(40000,3)::numeric(10,5);
SELECT mpq(-40000,3)::numeric(10,5);
SELECT mpq(400000,3)::numeric(10,5);

-- function-style casts
SELECT mpq('0'::varchar);
SELECT mpq('0'::int2);
SELECT mpq('0'::int4);
SELECT mpq('0'::int8);
SELECT mpq('0'::float4);
SELECT mpq('0'::float8);
SELECT mpq('0'::numeric);
SELECT mpq('0'::mpz);

SELECT text(0::mpq);
SELECT int2(0::mpq);
SELECT int4(0::mpq);
SELECT int8(0::mpq);
SELECT float4(0::mpq);
SELECT float8(0::mpq);
SELECT mpz('0'::mpq);

-- tricky cases of cast to numeric
select (x::mpz::mpq / 100)::decimal      from generate_series(-2, 2) x;
select (x::mpz::mpq / 100)::decimal(6,0) from generate_series(-2, 2) x;
select (x::mpz::mpq / 100)::decimal(6,1) from generate_series(-2, 2) x;
select (x::mpz::mpq / 100)::decimal(6,2) from generate_series(-2, 2) x;

SELECT mpq(10, 4), mpq(10, -4);
SELECT mpq(10, 0);
-- fails if mpq(int, int) or similar are availiable
SELECT mpq(4000000000000000000,3);
-- TODO: this shoud work.
-- currently not accepting it for ambiguous type promotion problems,
-- but this could change in the future if we find how to fix the above problem
SELECT mpq(47563485764385764395874365986384, 874539847539845639485769837553465);
-- Enable these checks if the above is solved.
-- SELECT mpq(1230::numeric, 123::numeric);
-- SELECT mpq(123.45::numeric, 1::numeric);
-- SELECT mpq(1::numeric, 123.45::numeric);
-- SELECT mpq(123::numeric, 0::numeric);
SELECT mpq(47563485764385764395874365986384::mpz, 874539847539845639485769837553465::mpz);
SELECT mpq('10'::mpz, '0'::mpz);
SELECT num('4/5'::mpq);
SELECT den('4/5'::mpq);


--
-- mpq arithmetic
--

SELECT -('0'::mpq), +('0'::mpq), -('1'::mpq), +('1'::mpq), -('-1'::mpq), +('-1'::mpq);
SELECT -('1234567890123456/7890'::mpq), +('1234567890123456/7890'::mpq);
SELECT '4/5'::mpq + '6/8'::mpq;
SELECT '4/5'::mpq - '6/8'::mpq;
SELECT '4/5'::mpq * '6/8'::mpq;
SELECT '4/5'::mpq / '6/8'::mpq;
SELECT '4/5'::mpq / '0'::mpq;

SELECT '4/5'::mpq << 4;
SELECT '4/5'::mpq << -1;
SELECT '4/5'::mpq >> 4;
SELECT '4/5'::mpq >> -1;


--
-- mpq unary function
--
SELECT abs(mpq(1,3));
SELECT abs(mpq(-1,3));
SELECT abs(mpq(1,-3));
SELECT abs(mpq(-1,-3));
SELECT inv(mpq(1,3));
SELECT inv(mpq(-1,3));
SELECT inv(mpq(3,1));
SELECT inv(mpq(-3,1));
SELECT inv(0::mpq);

SELECT limit_den(3.141592653589793, 10);
SELECT limit_den(3.141592653589793, 100);
SELECT limit_den(3.141592653589793, 1000000);
SELECT limit_den(3.141592653589793);
SELECT limit_den('4321/8765', 10000);
SELECT limit_den(3.141592653589793, 10000);
SELECT limit_den(-3.141592653589793, 10000);
SELECT limit_den(3.141592653589793, 113);
SELECT limit_den(3.141592653589793, 112);
SELECT limit_den('201/200', 100);
SELECT limit_den('201/200', 101);
SELECT limit_den(0, 10000);


--
-- mpq ordering operators
--

select 1000::mpq =   999::mpq;
select 1000::mpq =  1000::mpq;
select 1000::mpq =  1001::mpq;
select 1000::mpq <>  999::mpq;
select 1000::mpq <> 1000::mpq;
select 1000::mpq <> 1001::mpq;
select 1000::mpq !=  999::mpq;
select 1000::mpq != 1000::mpq;
select 1000::mpq != 1001::mpq;
select 1000::mpq <   999::mpq;
select 1000::mpq <  1000::mpq;
select 1000::mpq <  1001::mpq;
select 1000::mpq <=  999::mpq;
select 1000::mpq <= 1000::mpq;
select 1000::mpq <= 1001::mpq;
select 1000::mpq >   999::mpq;
select 1000::mpq >  1000::mpq;
select 1000::mpq >  1001::mpq;
select 1000::mpq >=  999::mpq;
select 1000::mpq >= 1000::mpq;
select 1000::mpq >= 1001::mpq;

select mpq_cmp(1000::mpq,  999::mpq);
select mpq_cmp(1000::mpq, 1000::mpq);
select mpq_cmp(1000::mpq, 1001::mpq);

-- Can create btree and hash indexes
create table test_mpq_idx (q mpq);
insert into test_mpq_idx select generate_series(1, 10000);
create index test_mpq_btree_idx on test_mpq_idx using btree (q);
set client_min_messages = error;
create index test_mpq_hash_idx on test_mpq_idx using hash (q);
reset client_min_messages;

-- Hash is compatible with mpz
select mpq_hash(0) = mpz_hash(0);
select mpq_hash(1000) = mpz_hash(1000);
select mpq_hash(-1000) = mpz_hash(-1000);
select mpq_hash('123456789012345678901234567890123456789012345678901234567890')
     = mpz_hash('123456789012345678901234567890123456789012345678901234567890');

-- den is used in hash
select mpq_hash(2) <> mpq_hash('2/3');
select mpq_hash('2/3') <> mpq_hash('2/5');


--
-- mpq aggregation
--

CREATE TABLE mpqagg(q mpq);

SELECT sum(q) FROM mpqagg;      -- NULL sum

INSERT INTO mpqagg SELECT mpq(x+1, x) from generate_series(1, 100) x;
INSERT INTO mpqagg VALUES (NULL);

SELECT sum(q) FROM mpqagg;
SELECT prod(q) FROM mpqagg;
SELECT min(q) FROM mpqagg;
SELECT max(q) FROM mpqagg;

-- check correct values when the sortop kicks in
CREATE INDEX mpqagg_idx ON mpqagg(q);
SELECT min(q) FROM mpqagg;
SELECT max(q) FROM mpqagg;

-- check aggregates work in windows functions too
CREATE TABLE test_mpq_win(q mpq);
INSERT INTO test_mpq_win SELECT mpq(1::mpz, i::mpz) from generate_series(1,500) i;
SELECT DISTINCT den(q) % 5, prod(q) OVER (PARTITION BY den(q) % 5) FROM test_mpq_win ORDER BY 1;
