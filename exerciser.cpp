#include "exerciser.h"

void exercise(connection *C) {
  add_player(C, 100, 60, "Michael", "Baldwin", 20, 20, 10, 10, 5.3, 5.3);
  add_team(C, "team_baldwin", 10, 3, 20, 0);
  add_state(C, "N/A");
  add_color(C, "perry-winkle");
  query1(C, 1, 10, 30, 0, 0, 40, 0, 0, 6, 0, 0, 5, 0, 0.0, 10.0, 0, 0.0, 10.0);
  query2(C, "Gold");
  query3(C, "FloridaState");
  query4(C, "FL", "Maroon");
  query5(C, 8);
}
