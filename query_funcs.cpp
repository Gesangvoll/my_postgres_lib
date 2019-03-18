#include "query_funcs.h"
#include <string>
using namespace std;

string &deal_with_single_quote(string &token) {
  int pos = token.find("'");
  if (pos != string::npos) {
    token.insert(pos, "'");
  }
  return token;
}

void add_player(connection *C, int team_id, int jersey_num, string first_name,
                string last_name, int mpg, int ppg, int rpg, int apg,
                double spg, double bpg) {
  string add_player_statement =
      "INSERT INTO PLAYER VALUES ( DEFAULT, '" + to_string(team_id) + "', '" +
      to_string(jersey_num) + "', '" + deal_with_single_quote(first_name) +
      "', '" + deal_with_single_quote(last_name) + "', '" + to_string(mpg) +
      "', '" + to_string(ppg) + "', '" + to_string(rpg) + "', '" +
      to_string(apg) + "', '" + to_string(spg) + "', '" + to_string(bpg) +
      "');";
  work txn(*C);
  txn.exec(add_player_statement);
  txn.commit();
}

void add_team(connection *C, string name, int state_id, int color_id, int wins,
              int losses) {
  string add_team_statement =
      "INSERT INTO TEAM VALUES ( DEFAULT, '" + deal_with_single_quote(name) +
      "', '" + to_string(state_id) + "', '" + to_string(color_id) + "', '" +
      to_string(wins) + "', '" + to_string(losses) + "');";
  work txn(*C);
  txn.exec(add_team_statement);
  txn.commit();
}

void add_state(connection *C, string name) {
  string add_state_statement = "INSERT INTO STATE VALUES ( DEFAULT, '" +
                               deal_with_single_quote(name) + "');";
  work txn(*C);
  txn.exec(add_state_statement);
  txn.commit();
}

void add_color(connection *C, string name) {
  string add_color_statement = "INSERT INTO COLOR VALUES ( DEFAULT, '" +
                               deal_with_single_quote(name) + "');";
  work txn(*C);
  txn.exec(add_color_statement);
  txn.commit();
}

void query1(connection *C, int use_mpg, int min_mpg, int max_mpg, int use_ppg,
            int min_ppg, int max_ppg, int use_rpg, int min_rpg, int max_rpg,
            int use_apg, int min_apg, int max_apg, int use_spg, double min_spg,
            double max_spg, int use_bpg, double min_bpg, double max_bpg) {
  string attributes[] = {"mpg", "ppg", "rpg", "apg", "spg", "bpg"};
  int use_array[] = {use_mpg, use_ppg, use_rpg, use_apg, use_spg, use_bpg};
  double min_array[] = {(double)min_mpg, (double)min_ppg, (double)min_rpg,
                        (double)min_apg, min_spg,         min_bpg};
  double max_array[] = {(double)max_mpg, (double)max_ppg, (double)max_rpg,
                        (double)max_rpg, max_spg,         max_bpg};
  string query_statement = "SELECT * FROM PLAYER";
  int index = -1;
  for (int i = 0; i < (sizeof(use_array) / sizeof(use_array[0])); i++) {
    if (use_array[i] == 1) {
      index = i;
      break;
    }
  }
  if (index != -1) {
    query_statement += " WHERE ";
    for (int i = index; i < (sizeof(use_array) / sizeof(use_array[0])); i++) {
      if (use_array[i] == 1) {
        query_statement = query_statement + "( " + attributes[i] +
                          " >= " + to_string(min_array[i]) + " AND " +
                          attributes[i] + " <= " + to_string(max_array[i]) +
                          ")";
      }
      if (i != 5 && use_array[i + 1] == 1) {
        query_statement += " AND ";
      }
    }
    query_statement += ";";
  } else {
    query_statement += ";";
  }
  nontransaction ntxn(*C);
  result r = ntxn.exec(query_statement);
  cout << "PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG "
          "SPG BPG"
       << endl;
  for (result::const_iterator c = r.begin(); c != r.end(); ++c) {
    cout << c[0].as<int>() << " " << c[1].as<int>() << " " << c[2].as<int>()
         << " " << c[3].as<string>() << " " << c[4].as<string>() << " "
         << c[5].as<int>() << " " << c[6].as<int>() << " " << c[7].as<int>()
         << " " << c[8].as<int>() << " " << c[9].as<float>() << " "
         << c[10].as<float>() << endl;
  }
  ntxn.commit();
}

void query2(connection *C, string team_color) {
  string query_statement =
      "SELECT TEAM.NAME FROM COLOR, TEAM WHERE COLOR.NAME = '" + team_color +
      "' AND COLOR.COLOR_ID = TEAM.COLOR_ID;";
  nontransaction ntxn(*C);
  result r = ntxn.exec(query_statement);
  cout << "Name" << endl;
  for (result::const_iterator c = r.begin(); c != r.end(); ++c) {
    cout << c[0].as<string>() << " " << endl;
  }
  ntxn.commit();
}

void query3(connection *C, string team_name) {
  string query_statement = "SELECT PLAYER.FIRST_NAME, PLAYER.LAST_NAME "
                           "FROM PLAYER, TEAM WHERE TEAM.NAME = '" +
                           team_name +
                           "' AND PLAYER.TEAM_ID = TEAM.TEAM_ID "
                           "ORDER BY PPG DESC;";
  nontransaction ntxn(*C);
  result r = ntxn.exec(query_statement);
  cout << "FIRST_NAME LAST_NAME" << endl;
  for (result::const_iterator c = r.begin(); c != r.end(); ++c) {
    cout << c[0].as<string>() << " " << c[1].as<string>() << endl;
  }
  ntxn.commit();
}

void query4(connection *C, string team_state, string team_color) {
  string query_statement =
      "SELECT PLAYER.FIRST_NAME, PLAYER.LAST_NAME, "
      "PLAYER.UNIFORM_NUM FROM PLAYER, TEAM, STATE, "
      "COLOR WHERE STATE.NAME = '" +
      team_state + "' AND COLOR.NAME = '" + team_color +
      "' AND PLAYER.TEAM_ID = TEAM.TEAM_ID AND TEAM.STATE_ID = STATE.STATE_ID "
      "AND TEAM.COLOR_ID = COLOR.COLOR_ID;";
  nontransaction ntxn(*C);
  result r = ntxn.exec(query_statement);
  cout << "FIRST_NAME LAST_NAME UNIFORM_NUM" << endl;
  for (result::const_iterator c = r.begin(); c != r.end(); ++c) {
    cout << c[0].as<string>() << " " << c[1].as<string>() << " "
         << c[2].as<string>() << endl;
  }
  ntxn.commit();
}

void query5(connection *C, int num_wins) {
  string query_statement =
      "SELECT PLAYER.FIRST_NAME, PLAYER.LAST_NAME, "
      "TEAM.NAME, TEAM.WINS FROM PLAYER, TEAM WHERE TEAM.WINS > " +
      to_string(num_wins) + " AND PLAYER.TEAM_ID = TEAM.TEAM_ID;";
  nontransaction ntxn(*C);
  result r = ntxn.exec(query_statement);
  cout << "FIRST_NAME LAST_NAME TEAM_NAME WINS" << endl;
  for (result::const_iterator c = r.begin(); c != r.end(); ++c) {
    cout << c[0].as<string>() << " " << c[1].as<string>() << " "
         << c[2].as<string>() << " " << c[3].as<int>() << endl;
  }
  ntxn.commit();
}
