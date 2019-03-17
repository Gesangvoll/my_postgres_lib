#include "exerciser.h"
#include <iostream>
#include <pqxx/pqxx>
#include <string>

using namespace std;
using namespace pqxx;
void read_data(string filename, work *txn) {}

int main(int argc, char *argv[]) {

  // Allocate & initialize a Postgres connection object
  connection *C;
  // Create a transaction
  work *txn;

  try {
    // Establish a connection to the database
    // Parameters: database name, user name, user password
    C = new connection("dbname=ACC_BBALL user=postgres password=passw0rd");
    if (C->is_open()) {
      cout << "Opened database successfully: " << C->dbname() << endl;
    } else {
      cout << "Can't open database" << endl;
      return 1;
    }
  } catch (const std::exception &e) {
    cerr << e.what() << std::endl;
    return 1;
  }

  // Drop tables if exist
  string drop_tables = "DROP TABLE IF EXISTS PLAYER, TEAM, STATE, COLOR;";
  try {
    txn = new work(*C);
    txn->exec(drop_tables);
    txn->commit();
  } catch (const exception &e) {
    cerr << e.what() << endl;
    txn->abort();
    return 1;
  }

  // Create PLAYER, TEAM, STATE, and COLOR tables in the ACC_BBALL
  // database
  string create_player_table("CREATE TABLE PLAYER("
                             "PLAYER_ID SERIAL PRIMARY KEY NOT NULL,"
                             "TEAM_ID INT,"
                             "UNIFORM_NUM INT,"
                             "FIRST_NAME VARCHAR (50),"
                             "LAST_NAME VARCHAR (50),"
                             "MPG INT,"
                             "PPG INT,"
                             "RPG INT,"
                             "APG INT,"
                             "SPG REAL,"
                             "BPG REAL);");
  string create_team_table("CREATE TABLE TEAM("
                           "TEAM_ID SERIAL PRIMARY KEY NOT NULL,"
                           "NAME VARCHAR (50),"
                           "STATE_ID INT,"
                           "COLOR_ID INT,"
                           "WINS INT,"
                           "LOSSES INT);");
  string create_state_table("CREATE TABLE STATE("
                            "STATE_ID SERIAL PRIMARY KEY NOT NULL,"
                            "NAME VARCHAR (10));");
  string create_color_table("CREATE TABLE COLOR("
                            "COLOR_ID SERIAL PRIMARY KEY NOT NULL,"
                            "NAME VARCHAR (50));");
  vector<string> create_tables;
  for (int i = 0; i < 4; i++) {
    switch (i) {
    case 0:
      create_tables.push_back(create_player_table);
      break;
    case 1:
      create_tables.push_back(create_team_table);
      break;
    case 2:
      create_tables.push_back(create_state_table);
      break;
    case 3:
      create_tables.push_back(create_color_table);
      break;
    }
  }
  for (int i = 0; i < 4; i++) {
    try {
      work *txn = new work(*C);
      txn->exec(create_tables[i]);
      txn->commit();
    } catch (const exception &e) {
      cerr << e.what() << endl;
      txn->abort();
      return 1;
    }
  }
  // txn->exec(create_player_table);
  // txn->exec(create_team_table);
  // txn->exec(create_state_table);
  // txn->exec(create_color_table);
  //      load each table with rows from the provided source txt files
  string files[] = {"player.txt", "team.txt", "state.txt", "color.txt"};
  for (int i = 0; i < 4; i++) {
    try {
      txn = new work(*C);
      read_data(files[i], txn);
      txn->commit();
    } catch (const exception &e) {
      cerr << e.what() << endl;
      txn->abort();
      return 1;
    }
  }
  exercise(C);

  // Close database connection
  C->disconnect();

  return 0;
}
