#include "exerciser.h"
#include <fstream>
#include <iostream>
#include <pqxx/pqxx>
#include <string>
using namespace std;
using namespace pqxx;

string &deal_with_single_quote(string &token) {
  int pos = token.find("'");
  if (pos != string::npos) {
    token.insert(pos, "'");
  }
  return token;
}

void read_data(string filename, work *txn) {
  string table_name = filename.substr(0, filename.find('.'));
  ifstream ifs;
  ifs.exceptions(ifstream::badbit);
  try {
    ifs.open(filename);
  } catch (const ifstream::failure &e) {
    cerr << "Can not parse file: " << filename << e.what() << endl;
    exit(EXIT_FAILURE);
  }
  string line;
  while (getline(ifs, line)) {
    line.append(" ");
    int pos = line.find(" ");
    string token;
    vector<string> tokens;
    while (pos != string::npos) {
      token = line.substr(0, pos);
      tokens.push_back(token);
      line = line.substr(pos + 1, string::npos);
      pos = line.find(" ");
    }
    // Update the table
    string insert_statement;
    insert_statement = "INSERT INTO " + table_name + " VALUES ( DEFAULT, ";
    for (int i = 1; i < tokens.size() - 1; i++) {
      if (isalpha((int)tokens[i][0])) {
        deal_with_single_quote(tokens[i]);
        insert_statement.append("'").append(tokens[i]).append("'").append(", ");
      } else {
        insert_statement.append(tokens[i]).append(", ");
      }
    }
    if (isalpha((int)tokens[tokens.size() - 1][0])) {
      insert_statement.append("'")
          .append(tokens[tokens.size() - 1])
          .append("'")
          .append(");");
    } else {
      insert_statement.append(tokens[tokens.size() - 1]).append(");");
    }
    cout << insert_statement << endl;
    txn->exec(insert_statement);
  }
  ifs.close();
}

int main(int argc, char *argv[]) {

  // Allocate & initialize a Postgres connection object
  connection *C;
  // Create a transaction
  work *txn;

  try {
    // Establish a connection to the database
    // Parameters: database name, user name, user password
    C = new connection("dbname=acc_bball user=postgres password=passw0rd");
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

  create_tables.push_back(create_player_table);
  create_tables.push_back(create_team_table);
  create_tables.push_back(create_state_table);
  create_tables.push_back(create_color_table);

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

  // Load each table with rows from the provided source txt files
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
