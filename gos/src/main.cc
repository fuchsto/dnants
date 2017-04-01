
#include <gos/app_engine.h>
#include <gos/app_play_state.h>

#include <cstdlib>


using gos::app_engine;
using gos::app_play_state;
using gos::view::window;

int main(int argc, char * argv[])
{
  app_engine app(
    "game'-._.of'._.survive",
    900, 900);

  app.settings().grid_extents   = { 30, 30 };
  app.settings().rounds_per_sec = 4;
  app.settings().init_team_size = 30;

  app.run(app_play_state::get());

  while (app.is_running()) {
    app.handle_events();
    app.update();
    app.draw();
  }

  return EXIT_SUCCESS;
}

