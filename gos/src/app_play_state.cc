
#include <gos/app_play_state.h>

#include <gos/state/cell.h>

#include <gos/util/logging.h>

#include <cmath>
#include <cstdlib>


namespace gos {

app_play_state app_play_state::_instance;

void app_play_state::initialize(app_engine * app) {
  GOS__LOG_DEBUG("app_play_state", "initialize()");
  _app           = app;
  _grid_extents  = app->settings().grid_extents;
  _grid_spacing  = app->win().view_extents().w / _grid_extents.w;
  _game_state    = new gos::state::game_state(_app);

  _active        = true;
  GOS__LOG_DEBUG("app_play_state", "initialize >");
}

void app_play_state::finalize() {
  _active = false;
  delete _game_state;
}

} // namespace gos
