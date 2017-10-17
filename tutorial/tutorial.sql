.echo on
/* 
run this example with 
UNIX
$ sqlite3 test.db < tutorial.sql
PS1
> sqlite3 test.db ".read tutorial.sql"
*/

-- enable echo mode
.echo on

-- create tables structure
CREATE TABLE people( 
	ID INT PRIMARY KEY NOT NULL, 
	SURNAME TEXT NOT NULL, 
	NAME TEXT NOT NULL, 
	WORKPLACE TEXT
);
CREATE TABLE places( 
	ID INT PRIMARY KEY NOT NULL, 
	NAME TEXT NOT NULL, 
	LAT REAL NOT NULL,
	LON REAL NOT NULL
);
CREATE TABLE fake( 
	ID INT PRIMARY KEY NOT NULL
);

-- delete an existing table
DROP TABLE fake;

-- list tables and their structure

.tables

.schema people

.schema places

-- add a number of entries
INSERT INTO people (ID,SURNAME,NAME,WORKPLACE) VALUES (1, 'Einstein', 'Albert', 'Princeton');          -- header not mandatory
INSERT INTO people VALUES (2, 'Dirac', 'Paul', 'Cambridge');
INSERT INTO people VALUES (3, 'Feynman', 'Richard', 'MIT');
INSERT INTO people (ID,NAME,SURNAME) VALUES (4, 'Nobody', 'None');                                     -- but needed when some field is missing

INSERT INTO places VALUES (1, 'Cambridge', 52.2071403, 0.1202683 );
INSERT INTO places VALUES (2, 'Princeton', 40.3436821, -74.6534327 );
INSERT INTO places VALUES (3, 'MIT', 42.3606659, -71.0930377 );

UPDATE people SET NAME = 'Paul A. M.' WHERE NAME = 'Paul';

-- remove selected entry
DELETE FROM people WHERE name='Nobody';

-- setup nice display mode
.headers on
.mode column

-- display table contents

SELECT * FROM people;

SELECT * FROM places;

-- single table query
-- ( list all the record whose address doesn't start with 'B' )
SELECT * FROM people WHERE workplace NOT LIKE 'B%';          

-- ( list all the record for which -75 < lon < -70 )
SELECT * FROM places WHERE lon BETWEEN -75 AND -70;            

-- inner join (or cross search)
SELECT p.name, p.workplace, l.lat, l.lon FROM people p INNER JOIN places l on p.workplace = l.name;
	
-- create a new table from the result of a query
CREATE TABLE located AS SELECT p.name, p.workplace, l.lat, l.lon FROM people p INNER JOIN places l on p.workplace = l.name;        -- ( create a new table of records with the selected fields )

.tables 

.schema located

SELECT * FROM located;

-- create a new table from a csv file
-- e.g. test.csv
-- ID,SURNAME,NAME,WORKPLACE
-- 1,Einstein,Albert,Princeton
-- 2,Dirac,Paul,Cambridge
-- 3,Feynman,Richard,MIT

-- setting up for importing from csv
.mode csv
DROP TABLE IF EXISTS import_csv;
.import test.csv import_csv

.schema import_csv

SELECT * FROM import_csv;

