/*
SQLITE C++ DB GENERATOR
Command line tool to create a sqlite db from:
- csv
- json   (coming soon)
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
#include <sqlite3.h>

using namespace std;

#define NULL_CALLBACK    NULL

enum{
  SQLERR_DBERR   = 1,
  SQLERR_GENERIC = 2
};

class sqlitedb
{
  sqlite3 *db;
  char *err_msg = 0;
  
public:
  sqlitedb(const string &db_name)
  {
    if( sqlite3_open(db_name.c_str(), &db) != SQLITE_OK )
    {
      cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
      sqlite3_close(db);
      exit(SQLERR_DBERR);
    }
  }  

  ~sqlitedb()
  {
    sqlite3_close(db);
  }

  void add_table(const string &table_name, vector<string> &header)
  {
    sqlite3_free(err_msg);

    string cmd;
    cmd = "CREATE TABLE " + table_name + "(\n";
    for(size_t i=0; i<header.size(); ++i) cmd += header[i] + " TEXT KEY NOT NULL" + ((i==header.size()-1)?"":",") + "\n";
    cmd += ");";

    if( sqlite3_exec(db, cmd.c_str(), NULL_CALLBACK, 0, &err_msg) != SQLITE_OK )
    {
      cerr << "SQL_ERROR: " << err_msg << endl;
    }
  }

  void add_table(const string &table_name, vector<string> &header, vector<string> &types)
  {
    sqlite3_free(err_msg);

    string cmd;
    cmd = "CREATE TABLE " + table_name + "(\n";
    for (size_t i = 0; i<header.size(); ++i) cmd += header[i] + " " + types[i] + ((i == header.size() - 1) ? "" : ",") + "\n";
    cmd += ");";

    if (sqlite3_exec(db, cmd.c_str(), NULL_CALLBACK, 0, &err_msg) != SQLITE_OK)
    {
      cerr << "SQL_ERROR: " << err_msg << endl;
    }
  }

  void add_entry(const string &table_name, vector<string> &values)
  {
    sqlite3_free(err_msg);

    string cmd;
    cmd = "INSERT INTO " + table_name + " VALUES (";
    for(size_t i=0; i<values.size(); ++i) cmd += "'" + values[i] + "'" + ((i==values.size()-1)?"":", ");
    cmd += ");";

    if( sqlite3_exec(db, cmd.c_str(), NULL_CALLBACK, NULL, &err_msg) != SQLITE_OK )
    {
      cerr << "SQL_ERROR: " << err_msg << endl;
    } 
  }

  template<typename T> void query(const string &table_name, int (*cb)(void *, int, char **, char **), T* ptr = nullptr)
  {
    sqlite3_free(err_msg);

    string cmd = "SELECT * FROM " + string(table_name) + ";";
    
    if( sqlite3_exec(db, cmd.c_str(), cb, (void *) ptr, &err_msg) != SQLITE_OK )
    {
      cerr << "SQL_ERROR: " << err_msg << endl;
    }
  }

  template<typename T> void custom_query(const string &table_name, const string &cmd, int(*cb)(void *, int, char **, char **), T* ptr = nullptr)
  {
    sqlite3_free(err_msg);

    if (sqlite3_exec(db, cmd.c_str(), cb, (void *)ptr, &err_msg) != SQLITE_OK)
    {
      cerr << "SQL_ERROR: " << err_msg << endl;
    }
  }

  void enable_pragma_statement()
  {
    sqlite3_exec(db, "PRAGMA synchronous=OFF", NULL_CALLBACK, NULL, &err_msg);
    sqlite3_exec(db, "PRAGMA count_changes=OFF", NULL_CALLBACK, NULL, &err_msg);
    sqlite3_exec(db, "PRAGMA journal_mode=MEMORY", NULL_CALLBACK, NULL, &err_msg);
    sqlite3_exec(db, "PRAGMA temp_store=MEMORY", NULL_CALLBACK, NULL, &err_msg);
  }

  void begin_transaction()
  {
    sqlite3_free(err_msg);

    string cmd = "BEGIN TRANSACTION";

    if (sqlite3_exec(db, cmd.c_str(), NULL_CALLBACK, NULL, &err_msg) != SQLITE_OK)
    {
      cerr << "SQL_ERROR: " << err_msg << endl;
    }
  }

  void commit_transaction()
  {
    sqlite3_free(err_msg);

    string cmd = "COMMIT TRANSACTION";

    if (sqlite3_exec(db, cmd.c_str(), NULL_CALLBACK, NULL, &err_msg) != SQLITE_OK)
    {
      cerr << "SQL_ERROR: " << err_msg << endl;
    }
  }
};
