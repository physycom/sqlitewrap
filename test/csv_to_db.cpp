// Copyright 2017 - Alessandro Fabbri

// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt


/*
SQLITE C++ DB GENERATOR
Command line tool to create a sqlite db from:
- csv
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>

#include <sqlitewrap.hpp>

using namespace std;

#define NULL_CALLBACK    NULL

string some_external_function(char * val)
{ 
  return string(val) + " A. M."; 
}

enum{
  WRONG_CMDLINE = 100
};

int main(int argc, char **argv)
{
  if( argc != 2 )
  {
    cerr << "Usage: " << argv[0] << " path/to/csv" << endl;
    exit(WRONG_CMDLINE);
  }

  string input = argv[1];
  string output = input + ".db";
  string line;
  vector<string> header, tokens;
  ifstream ifs(input);

  // create the db
  sqlitedb db(output);

  // get header from csv's first line
  getline(ifs, line);
  boost::split(header, line, boost::is_any_of(","), boost::token_compress_off);
  db.add_table("csv", header);
  
  // add records to db by parsing csv line by line
  while(getline(ifs, line))
  {
    boost::split(tokens, line, boost::is_any_of(","), boost::token_compress_off);
    db.add_entry("csv", tokens);
  }

  // make a query
  db.query<void>("csv", [](void *, int col_num, char **values, char **headers)
  {
    cout << "------------------------" << endl; 
    for(int i=0; i<col_num; ++i) cout << headers[i] << " = " << values[i] << endl;
    return 0;
  });

  // query that counts the records
  int record_counter = 0;
  db.query("csv", [](void *cnt, int, char **, char **)
  {
    *(int *)cnt += 1;
    return 0;
  }, &record_counter);
  cout << "------------------------" << endl; 
  cout << "Query returns " << record_counter << " records." << endl;

  // query that stores the results
  vector<string> names;
  db.query("csv", [](void *container, int col_num, char **values, char **headers)
  {
    for(int i=0; i<col_num; ++i) if( string(headers[i]) == "NAME" ) (*(vector<string> *)container).push_back(values[i]);
    return 0;
  }, &names);
  cout << "------------------------" << endl; 
  cout << "Names : ";
  for(auto n : names) cout << n << " ";
  cout << endl;

  // query to update values
  db.custom_query("csv", "SELECT * FROM csv WHERE NAME = 'Paul'", [](void *db, int col_num, char **values, char **headers)
  {
    string prefix = "UPDATE csv SET NAME = '";
    string postfix = "' WHERE ";
    for(int i=0; i<col_num; ++i)
    {
      if( string(headers[i]) != "NAME" ) postfix += string(headers[i]) + " = '" + values[i] + "'" + (i==col_num-1?";":" AND ");
      else prefix += some_external_function(values[i]);
    }
    string cmd = prefix + postfix;

    ((sqlitedb*)db)->custom_query<void>("csv", cmd, NULL_CALLBACK);

    return 0;
  }, &db);

  return 0;
}
