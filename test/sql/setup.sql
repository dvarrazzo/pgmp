SET client_min_messages = warning;
\set ECHO none
-- The above turn off echoing so that expected file
-- does not depend on contents of the setup file.

-- Setup the extension on PostgreSQL 9.1


CREATE EXTENSION pgmp;

\set ECHO all
RESET client_min_messages;

