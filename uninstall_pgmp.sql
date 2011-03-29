/* pgmp -- Module uninstall SQL script
 *
 * Copyright (C) 2011 Daniele Varrazzo
 *
 * This file is part of the PostgreSQL GMP Module
 *
 * The PostgreSQL GMP Module is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3 of the License,
 * or (at your option) any later version.
 *
 * The PostgreSQL GMP Module is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the PostgreSQL GMP Module.  If not, see
 * http://www.gnu.org/licenses/.
 */

-- Adjust this setting to control where the objects get created.
SET search_path = public;

-- Drop the data types: this will rip off all the functions defined on them.
DROP TYPE mpz CASCADE;
DROP TYPE mpq CASCADE;

-- Drop the remaining functions.
DROP FUNCTION gmp_version();

DROP FUNCTION randinit();
DROP FUNCTION randinit_mt();
DROP FUNCTION randinit_lc_2exp_size(int8);

DROP FUNCTION numeric_test_dataset(int4, numeric);

