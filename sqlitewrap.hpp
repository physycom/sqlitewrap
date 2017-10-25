/*
SQLITE C++ DB WRAPPER
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
#include <sqlite3.h>

#define NULL_CALLBACK    NULL

enum
{
  SQLERR_DBERR   = 1,
  SQLERR_GENERIC = 2
};

class table_schema
{
public:
  std::vector<std::string> headers, types;

  void insert(std::string h, std::string t = "TEXT NOT NULL")
  {
    headers.push_back(h);
    types.push_back(t);
  }
};

class sqlitedb
{
public:
  sqlite3 *db;
  char *err_msg = 0;

  table_schema table;

  sqlitedb(const std::string &db_name)
  {
    if( sqlite3_open(db_name.c_str(), &db) != SQLITE_OK )
    {
      std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
      sqlite3_close(db);
      exit(SQLERR_DBERR);
    }
  }

  ~sqlitedb()
  {
    sqlite3_close(db);
  }

  void add_table(const std::string &table_name, std::vector<std::string> &header)
  {
    sqlite3_free(err_msg);

    std::string cmd;
    cmd = "CREATE TABLE " + table_name + "(\n";
    for(int i=0; i<(int)header.size(); ++i) cmd += header[i] + " TEXT KEY NOT NULL" + ((i==header.size()-1)?"":",") + "\n";
    cmd += ");";

    if( sqlite3_exec(db, cmd.c_str(), NULL_CALLBACK, 0, &err_msg) != SQLITE_OK )
    {
      std::cerr << "SQL_ERROR: " << err_msg << std::endl;
    }
  }

  void add_table(const std::string &table_name, std::vector<std::string> &header, std::vector<std::string> &types)
  {
    sqlite3_free(err_msg);

    std::string cmd;
    cmd = "CREATE TABLE " + table_name + "(\n";
    for (int i = 0; i<(int)header.size(); ++i) cmd += header[i] + " " + types[i] + ((i == header.size() - 1) ? "" : ",") + "\n";
    cmd += ");";

    if (sqlite3_exec(db, cmd.c_str(), NULL_CALLBACK, 0, &err_msg) != SQLITE_OK)
    {
      std::cerr << "SQL_ERROR: " << err_msg << std::endl;
    }
  }

  void add_table(const std::string &table_name)
  {
    add_table(table_name, table.headers, table.types);
  }

  void add_entry(const std::string &table_name, std::vector<std::string> values)
  {
    sqlite3_free(err_msg);

    std::string cmd;
    cmd = "INSERT INTO " + table_name + " VALUES (";
    for(int i=0; i<(int)values.size(); ++i) cmd += "'" + values[i] + "'" + ((i==values.size()-1)?"":", ");
    cmd += ");";

    if( sqlite3_exec(db, cmd.c_str(), NULL_CALLBACK, NULL, &err_msg) != SQLITE_OK )
    {
      std::cerr << "SQL_ERROR: " << err_msg << std::endl;
    }
  }

  template<typename T> void query(const std::string &table_name, int (*cb)(void *, int, char **, char **), T* ptr = nullptr)
  {
    sqlite3_free(err_msg);

    std::string cmd = "SELECT * FROM " + std::string(table_name) + ";";

    if( sqlite3_exec(db, cmd.c_str(), cb, (void *) ptr, &err_msg) != SQLITE_OK )
    {
      std::cerr << "SQL_ERROR: " << err_msg << std::endl;
    }
  }

  template<typename T> void custom_query(const std::string &table_name, const std::string &cmd, int(*cb)(void *, int, char **, char **), T* ptr = nullptr)
  {
    sqlite3_free(err_msg);

    if (sqlite3_exec(db, cmd.c_str(), cb, (void *)ptr, &err_msg) != SQLITE_OK)
    {
      std::cerr << "SQL_ERROR: " << err_msg << std::endl;
    }
  }

  void enable_pragma_statement()
  {
    sqlite3_exec(db, "PRAGMA synchronous=OFF",     NULL_CALLBACK, NULL, &err_msg);
    sqlite3_exec(db, "PRAGMA count_changes=OFF",   NULL_CALLBACK, NULL, &err_msg);
    sqlite3_exec(db, "PRAGMA journal_mode=MEMORY", NULL_CALLBACK, NULL, &err_msg);
    sqlite3_exec(db, "PRAGMA temp_store=MEMORY",   NULL_CALLBACK, NULL, &err_msg);
  }

  void begin_transaction()
  {
    sqlite3_free(err_msg);

    std::string cmd = "BEGIN TRANSACTION";

    if (sqlite3_exec(db, cmd.c_str(), NULL_CALLBACK, NULL, &err_msg) != SQLITE_OK)
    {
      std::cerr << "SQL_ERROR: " << err_msg << std::endl;
    }
  }

  void commit_transaction()
  {
    sqlite3_free(err_msg);

    std::string cmd = "COMMIT TRANSACTION";

    if (sqlite3_exec(db, cmd.c_str(), NULL_CALLBACK, NULL, &err_msg) != SQLITE_OK)
    {
      std::cerr << "SQL_ERROR: " << err_msg << std::endl;
    }
  }
};
