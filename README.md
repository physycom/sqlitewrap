# sqlitewrap

Introductory repo to SQLite.

### TUTORIAL
The folder tutorial contains a sample script `tutorial.sql` which displays the most basic feature of DB handling. To run it just type 
```
$ sqlite3 test.db < tutorial.sql
```
under UNIX, or
```
> sqlite3 test.db ".read tutorial.sql"
```
for everything else.

### C++ WRAPPER
The single header `sqlitewrap.hpp` contains a simple porting to `C++`, which requires `sqlite3` library, to handle SQL database.
