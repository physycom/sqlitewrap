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

  return 0;
}
