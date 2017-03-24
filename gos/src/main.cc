
// #include <gos/rule_engine.h>
// #include <gos/state.h>
#include <gos/app_engine.h>
#include <gos/view.h>

#include <cstdlib>


using gos::app_engine;
using gos::view::window;
// using gos::rule_engine;
// using gos::game_state;

int main(int argc, char * argv[])
{
  app_engine app("game'-._.of'._.survive", 800, 800);

  while (app.is_running()) {
    app.handle_events();
    app.update();
    app.draw();
  }

  return EXIT_SUCCESS;
}

