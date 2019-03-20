#include "exerciser.h"

void exercise(connection *C) {
  add_player(C, 10, 60, "Lionel", "Messi", 38, 20, 10, 10, 5.3, 5.3);
  add_team(C, "barcelona", 10, 3, 20, 0);
  add_state(C, "Spain");
  add_color(C, "red-blue");
  query1(C, 1, 35, 40, 0, 0, 40, 0, 0, 6, 0, 0, 5, 0, 0.0, 10.0, 0, 0.0, 10.0);

  query2(C, "LightBlue");
  query3(C, "Duke");
  query4(C, "NC", "LightBlue");
  query5(C, 8);
}
