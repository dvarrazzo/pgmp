--
--  Test mpz datatype
--

-- Compact output
\t
\a

SELECT gmp_version() > 10000;


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
SELECT mpz('10'), mpz('10', 16), mpz('10', 2);
SELECT mpz('10', 62);
SELECT mpz('10', 1);
SELECT mpz('10', 63);
SELECT mpz('10', 0), mpz('0x10', 0), mpz('010', 0), mpz('0b10', 0);

SELECT text(10::mpz);
SELECT text(10::mpz, 2);
SELECT text(10::mpz, -2);
SELECT text(255::mpz, 16);
SELECT text((36 * 36 - 1)::mpz, 36);
SELECT text((62 * 62 - 1)::mpz, 62);
SELECT text((36 * 36 - 1)::mpz, -36);
SELECT text(10::mpz, -37);
SELECT text(10::mpz, -1);
SELECT text(10::mpz, 0);
SELECT text(10::mpz, 1);
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
-- decimal are truncated
SELECT 123.10::numeric::mpz, 123.90::numeric::mpz;
SELECT (-123.10::numeric)::mpz, (-123.90::numeric)::mpz;
SELECT 'NaN'::numeric::mpz;

SELECT 0.0::float4::mpz, 123.15::float4::mpz, 123.95::float4::mpz;
SELECT (1e36::float4)::mpz BETWEEN pow(10::mpz,36) - pow(10::mpz,30) AND pow(10::mpz,36) + pow(10::mpz,30);
SELECT (-1e36::float4)::mpz BETWEEN -pow(10::mpz,36) - pow(10::mpz,30) AND -pow(10::mpz,36) + pow(10::mpz,30);
SELECT 'NaN'::float4::mpz;
SELECT 'Infinity'::float4::mpz;
SELECT '-Infinity'::float4::mpz;

SELECT 0.0::float8::mpz, 123.15::float8::mpz, 123.95::float8::mpz;
SELECT (1e307::float8)::mpz BETWEEN pow(10::mpz,307) - pow(10::mpz,307-15) AND pow(10::mpz,307) + pow(10::mpz,307-15);
SELECT (-1e307::float8)::mpz BETWEEN -pow(10::mpz,307) - pow(10::mpz,307-15) AND -pow(10::mpz,307) + pow(10::mpz,307-15);
SELECT 'NaN'::float8::mpz;
SELECT 'Infinity'::float8::mpz;
SELECT '-Infinity'::float8::mpz;

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

SELECT 32767::mpz::int2;
SELECT 32768::mpz::int2;
SELECT (-32768)::mpz::int2;
SELECT (-32769)::mpz::int2;
SELECT 2147483647::mpz::int4;
SELECT 2147483648::mpz::int4;
SELECT (-2147483648)::mpz::int4;
SELECT (-2147483649)::mpz::int4;
SELECT 9223372036854775807::mpz::int8;
SELECT 9223372036854775808::mpz::int8;
SELECT (-9223372036854775808)::mpz::int8;
SELECT (-9223372036854775809)::mpz::int8;
SELECT (2147483648)::mpz::int8;
SELECT (-2147483648)::mpz::int8;
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
SELECT (65536::mpz)::numeric;
SELECT (65536::mpz*65536::mpz)::numeric;
SELECT (65536::mpz*65536::mpz*65536::mpz)::numeric;
SELECT (65536::mpz*65536::mpz*65536::mpz*65536::mpz)::numeric;
SELECT (65536::mpz*65536::mpz*65536::mpz*65536::mpz-1::mpz)::numeric;
SELECT (-65536::mpz)::numeric;
SELECT (-65536::mpz*65536::mpz)::numeric;
SELECT (-65536::mpz*65536::mpz*65536::mpz)::numeric;
SELECT (-65536::mpz*65536::mpz*65536::mpz*65536::mpz)::numeric;
SELECT (-65536::mpz*65536::mpz*65536::mpz*65536::mpz+1::mpz)::numeric;

SELECT 0::mpz::float4, 123::mpz::float4, (-123::mpz)::float4;
SELECT pow(10::mpz, 30)::float4, (-pow(10::mpz, 30))::float4;
SELECT pow(10::mpz, 300)::float4, (-pow(10::mpz, 300))::float4;
SELECT 0::mpz::float8, 123::mpz::float8, (-123::mpz)::float8;
SELECT pow(10::mpz, 307)::float8, (-pow(10::mpz, 307))::float8;
SELECT pow(10::mpz, 407)::float8, (-pow(10::mpz, 407))::float8;

-- function-style casts
SELECT mpz('0'::varchar);
SELECT mpz('0'::int2);
SELECT mpz('0'::int4);
SELECT mpz('0'::int8);
SELECT mpz('0'::float4);
SELECT mpz('0'::float8);
SELECT mpz('0'::numeric);

SELECT text(0::mpz);
SELECT int2(0::mpz);
SELECT int4(0::mpz);
SELECT int8(0::mpz);
SELECT float4(0::mpz);
SELECT float8(0::mpz);


--
-- mpz arithmetic
--

SELECT -('0'::mpz), +('0'::mpz), -('1'::mpz), +('1'::mpz);
SELECT -('12345678901234567890'::mpz), +('12345678901234567890'::mpz);
SELECT abs('-1234567890'::mpz), abs('1234567890'::mpz);
SELECT sgn(0::mpz), sgn('-1234567890'::mpz), sgn('1234567890'::mpz);
SELECT even(10::mpz), even(11::mpz);
SELECT odd(10::mpz), odd(11::mpz);

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

SELECT  '21'::mpz /! '7'::mpz;

SELECT  '10000000000'::mpz << 10;
SELECT  '10000000000'::mpz << 0;
SELECT  '10000000000'::mpz << -1;

SELECT  '1027'::mpz >>   3;
SELECT '-1027'::mpz >>   3;
SELECT  '1027'::mpz >>  -3;
SELECT  '1027'::mpz %>   3;
SELECT '-1027'::mpz %>   3;
SELECT  '1027'::mpz %>  -3;

SELECT  '1027'::mpz +>>   3;
SELECT '-1027'::mpz +>>   3;
SELECT  '1027'::mpz +>>  -3;
SELECT  '1027'::mpz +%>   3;
SELECT '-1027'::mpz +%>   3;
SELECT  '1027'::mpz +%>  -3;

SELECT  '1027'::mpz ->>   3;
SELECT '-1027'::mpz ->>   3;
SELECT  '1027'::mpz ->>  -3;
SELECT  '1027'::mpz -%>   3;
SELECT '-1027'::mpz -%>   3;
SELECT  '1027'::mpz -%>  -3;

SELECT q, r from tdiv_qr( 7::mpz,  3::mpz);
SELECT q, r from tdiv_qr(-7::mpz,  3::mpz);
SELECT q, r from tdiv_qr( 7::mpz, -3::mpz);
SELECT q, r from tdiv_qr(-7::mpz, -3::mpz);
SELECT q, r from tdiv_qr( 7::mpz,  0::mpz);

SELECT q, r from cdiv_qr( 7::mpz,  3::mpz);
SELECT q, r from cdiv_qr(-7::mpz,  3::mpz);
SELECT q, r from cdiv_qr( 7::mpz, -3::mpz);
SELECT q, r from cdiv_qr(-7::mpz, -3::mpz);
SELECT q, r from cdiv_qr( 7::mpz,  0::mpz);

SELECT q, r from fdiv_qr( 7::mpz,  3::mpz);
SELECT q, r from fdiv_qr(-7::mpz,  3::mpz);
SELECT q, r from fdiv_qr( 7::mpz, -3::mpz);
SELECT q, r from fdiv_qr(-7::mpz, -3::mpz);
SELECT q, r from fdiv_qr( 7::mpz,  0::mpz);

SELECT divisible(10::mpz, 3::mpz);
SELECT divisible(12::mpz, 3::mpz);
SELECT divisible(10::mpz, 0::mpz);
SELECT divisible(0::mpz, 0::mpz);
SELECT divisible_2exp(63::mpz, 3);
SELECT divisible_2exp(64::mpz, 3);

SELECT 10::mpz /? 3::mpz;
SELECT 12::mpz /? 3::mpz;
SELECT 10::mpz /? 0::mpz;
SELECT 0::mpz /? 0::mpz;
SELECT 63::mpz >>? 3;
SELECT 64::mpz >>? 3;

SELECT congruent(12::mpz, 16::mpz, 5::mpz);
SELECT congruent(12::mpz, 17::mpz, 5::mpz);
SELECT congruent(12::mpz, 11::mpz, 0::mpz);
SELECT congruent(12::mpz, 12::mpz, 0::mpz);
SELECT congruent_2exp(18::mpz, 41::mpz, 3);
SELECT congruent_2exp(18::mpz, 42::mpz, 3);

-- power operator/functions

SELECT 2::mpz ^ 10;
SELECT 2::mpz ^ 0;
SELECT 2::mpz ^ -1;
SELECT pow(2::mpz, 10);
SELECT pow(2::mpz, 0);
SELECT pow(2::mpz, -1);
SELECT powm(3::mpz, 2::mpz, 9::mpz);
SELECT powm(3::mpz, 2::mpz, 8::mpz);
SELECT powm(3::mpz, -1::mpz, 8::mpz);
SELECT powm(3::mpz, 2::mpz, 0::mpz);

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

-- Can create btree and hash indexes
create table test_mpz_idx (z mpz);
insert into test_mpz_idx select generate_series(1, 10000);
create index test_mpz_btree_idx on test_mpz_idx using btree (z);
set client_min_messages = error;
create index test_mpz_hash_idx on test_mpz_idx using hash (z);
reset client_min_messages;

-- Hash is compatible with builtins
select mpz_hash(0) = hashint4(0);
select mpz_hash(32767::int2) = hashint2(32767::int2);
select mpz_hash((-32768)::int2) = hashint2((-32768)::int2);
select mpz_hash(2147483647) = hashint4(2147483647);
select mpz_hash(-2147483648) = hashint4(-2147483648);
select mpz_hash(9223372036854775807) = hashint8(9223372036854775807);
select mpz_hash(-9223372036854775808) = hashint8(-9223372036854775808);


--
-- mpz aggregation
--

CREATE TABLE mpzagg(z mpz);

SELECT sum(z) FROM mpzagg;      -- NULL sum

INSERT INTO mpzagg SELECT generate_series(1, 100);
INSERT INTO mpzagg VALUES (NULL);

SELECT sum(z) FROM mpzagg;
SELECT prod(z) FROM mpzagg;
SELECT min(z) FROM mpzagg;
SELECT max(z) FROM mpzagg;

-- check correct values when the sortop kicks in
CREATE INDEX mpzagg_idx ON mpzagg(z);
SELECT min(z) FROM mpzagg;
SELECT max(z) FROM mpzagg;

SELECT bit_and(z) FROM mpzagg;
SELECT bit_and(z) FROM mpzagg WHERE z % 2 = 1;
SELECT bit_or(z) FROM mpzagg;
SELECT bit_or(z) FROM mpzagg WHERE z % 2 = 0;
SELECT bit_or(z) FROM mpzagg WHERE z = 1 or z = 2;
SELECT bit_xor(z) FROM mpzagg;
SELECT bit_xor(z) FROM mpzagg WHERE z = 1 or z = 2;
SELECT bit_xor(z) FROM mpzagg WHERE z = 1 or z = 2 or z = 3;

-- check aggregates work in windows functions too
CREATE TABLE test_mpz_win(z mpz);
INSERT INTO test_mpz_win SELECT generate_series(1,500);
SELECT DISTINCT z % 5, prod(z) OVER (PARTITION BY z % 5) FROM test_mpz_win ORDER BY 1;


--
-- mpz functions tests
--

SELECT sqrt(25::mpz);
SELECT sqrt(('1' || repeat('0',100))::mpz);
SELECT sqrt(0::mpz);
SELECT sqrt(-1::mpz);

SELECT root(27::mpz, 3);
SELECT root(('1' || repeat('0',100))::mpz, 3);
SELECT root(0::mpz, 3);
SELECT root(27::mpz, 1);
SELECT root(27::mpz, 0);
SELECT root(-27::mpz, 3);
SELECT root(27::mpz, -1);

select * from rootrem(1000::mpz,2) as rootrem;
select * from rootrem(1000::mpz,9) as rootrem;
select * from rootrem(('1' || repeat('0',100))::mpz,2);
select * from rootrem(('1' || repeat('0',100))::mpz,5);
select root from rootrem(1000::mpz, 2);
select rem from rootrem(1000::mpz, 2);

select * from sqrtrem(1000::mpz) as rootrem;
select * from sqrtrem(('1' || repeat('0',100))::mpz);
select root from sqrtrem(1000::mpz);
select rem from sqrtrem(1000::mpz);

select perfect_power(26::mpz);
select perfect_power(27::mpz);
select perfect_power(65535::mpz);
select perfect_power(65536::mpz);
select perfect_power(-65536::mpz);
select perfect_power(-65535::mpz);
select perfect_power(('1' || repeat('0',100))::mpz);
select perfect_power(('1' || repeat('0',10000))::mpz);
select perfect_power(('1' || repeat('0',10001))::mpz);
select perfect_power(('1' || repeat('0',10000))::mpz+1::mpz);

select perfect_square(0::mpz);
select perfect_square(1::mpz);
select perfect_square(-1::mpz);
select perfect_square(26::mpz);
select perfect_square(27::mpz);
select perfect_square(16777215::mpz);
select perfect_square(16777216::mpz);
select perfect_square(('1' || repeat('0',10000))::mpz);
select perfect_square(('1' || repeat('0',10000))::mpz+1::mpz);

--
-- Number Theoretic Functions
--

SELECT probab_prime(5::mpz, 2);
SELECT probab_prime(10::mpz, 2);
SELECT probab_prime(17::mpz, 2);

SELECT nextprime(5::mpz);
SELECT nextprime(10::mpz);
SELECT nextprime(100::mpz);
SELECT nextprime(1000::mpz);
SELECT nextprime(0::mpz);
SELECT nextprime(-8::mpz);

SELECT gcd(3::mpz, 15::mpz);
SELECT gcd(17::mpz, 15::mpz);
SELECT gcd(12345::mpz, 54321::mpz);
SELECT gcd(10000000::mpz, 10000::mpz);

SELECT g, s, t FROM gcdext(6::mpz, 15::mpz);

SELECT lcm(3::mpz, 15::mpz);
SELECT lcm(17::mpz, 15::mpz);
SELECT lcm(12345::mpz, 54321::mpz);

SELECT invert(1::mpz,2::mpz);
SELECT invert(1::mpz,3::mpz);
SELECT invert(2::mpz,3::mpz);
SELECT invert(20::mpz,3::mpz);
SELECT invert(30::mpz,3::mpz);

select jacobi(2::mpz, 3::mpz);
select jacobi(5::mpz, 3::mpz);
select jacobi(5::mpz, 10::mpz);
select jacobi(5::mpz, 20::mpz);
select jacobi(5::mpz, 200::mpz);

select legendre(2::mpz, 3::mpz);
select legendre(5::mpz, 3::mpz);
select legendre(5::mpz, 10::mpz);
select legendre(5::mpz, 20::mpz);
select legendre(5::mpz, 200::mpz);

select kronecker(2::mpz, 3::mpz);
select kronecker(5::mpz, 3::mpz);
select kronecker(5::mpz, 10::mpz);
select kronecker(5::mpz, 20::mpz);
select kronecker(5::mpz, 200::mpz);

select remove(40::mpz, 5::mpz); 
select remove(43::mpz, 5::mpz); 
select remove(48::mpz, 6::mpz); 
select remove(48::mpz, 3::mpz); 

select fac(0);
select fac(1);
select fac(10);
select fac(100);
select fac(-1);

select bin(0::mpz, 0);
select bin(7::mpz, 2);
select bin(-2::mpz, 1);
select bin(2::mpz, -1);

select fib(0);
select fib(1);
select fib(10);
select fib(-1);

select fn, fnsub1 from fib2(0);
select fn, fnsub1 from fib2(1);
select fn, fnsub1 from fib2(2);
select fn, fnsub1 from fib2(10);
select fn, fnsub1 from fib2(-1);

select lucnum(0);
select lucnum(1);
select lucnum(10);
select lucnum(-1);

select ln, lnsub1 from lucnum2(0);
select ln, lnsub1 from lucnum2(1);
select ln, lnsub1 from lucnum2(2);
select ln, lnsub1 from lucnum2(10);
select ln, lnsub1 from lucnum2(-1);


--
-- Logic and bit fiddling functions and operators
--

SELECT text('0b10001'::mpz & '0b01001'::mpz, 2);
SELECT text('0b10001'::mpz | '0b01001'::mpz, 2);
SELECT text('0b10001'::mpz # '0b01001'::mpz, 2);
SELECT com(10::mpz);
SELECT popcount('0b101010'::mpz);
SELECT popcount(0::mpz);
SELECT popcount(-1::mpz) = gmp_max_bitcnt();
SELECT hamdist('0b101010'::mpz, '0b101100'::mpz);
SELECT hamdist(0::mpz, -1::mpz) = gmp_max_bitcnt();
SELECT scan0('0b110110'::mpz, 1);
SELECT scan0('0b110110'::mpz, 3);
SELECT scan0(-1::mpz, 2) = gmp_max_bitcnt();
SELECT scan0(0::mpz, -1);
SELECT scan0(0::mpz, (2^64)::numeric::mpz);
SELECT scan1('0b110110'::mpz, 1);
SELECT scan1('0b110110'::mpz, 3);
SELECT scan1(1::mpz, 2) = gmp_max_bitcnt();
SELECT scan1(0::mpz, -1);
SELECT scan1(0::mpz, (2^64)::numeric::mpz);
SELECT text(setbit('0b1010'::mpz, 0), 2);
SELECT text(setbit('0b1010'::mpz, 1), 2);
SELECT setbit(0::mpz, -1);
SELECT setbit(0::mpz, (2^64)::numeric::mpz);
SELECT text(clrbit('0b1010'::mpz, 0), 2);
SELECT text(clrbit('0b1010'::mpz, 1), 2);
SELECT clrbit(0::mpz, -1);
SELECT clrbit(0::mpz, (2^64)::numeric::mpz);
SELECT text(combit('0b1010'::mpz, 0), 2);
SELECT text(combit('0b1010'::mpz, 1), 2);
SELECT combit(0::mpz, -1);
SELECT combit(0::mpz, (2^64)::numeric::mpz);
SELECT tstbit('0b1010'::mpz, 0);
SELECT tstbit('0b1010'::mpz, 1);
SELECT tstbit(0::mpz, -1);
SELECT tstbit(0::mpz, (2^64)::numeric::mpz);


--
-- Random numbers
--

-- Errors
SELECT rrandomb(128);
SELECT urandomb(128);
SELECT randseed(123456::mpz);

-- Correct sequence
SELECT randinit();
SELECT urandomb(128);
SELECT urandomb(128);

-- Re-initialization
SELECT randinit();
SELECT urandomb(128);
SELECT urandomb(128);

SELECT randinit_mt();
SELECT urandomb(128);
SELECT urandomb(128);

SELECT randinit_lc_2exp(1103515245, 12345, 32);
SELECT urandomb(128);
SELECT urandomb(128);

SELECT randinit_lc_2exp_size(64);
SELECT urandomb(128);
SELECT urandomb(128);

-- A failed initialization leaves the state as it was before
SELECT randinit();
SELECT urandomb(128);
SELECT randinit_lc_2exp_size(8192);
SELECT urandomb(128);

-- Seeding
SELECT randinit();
SELECT randseed(123456::mpz);
SELECT urandomb(128);
SELECT urandomb(128);
SELECT randseed(123456::mpz);
SELECT urandomb(128);
SELECT urandomb(128);

SELECT randinit();
SELECT randseed(123456::mpz);
SELECT text(rrandomb(128), 2);
SELECT text(rrandomb(128), 2);
SELECT randseed(123456::mpz);
SELECT text(rrandomb(128), 2);
SELECT text(rrandomb(128), 2);

SELECT randinit();
SELECT randseed(123456::mpz);
SELECT urandomm(1000000::mpz);
SELECT urandomm(1000000::mpz);
SELECT randseed(123456::mpz);
SELECT urandomm(1000000::mpz);
SELECT urandomm(1000000::mpz);
